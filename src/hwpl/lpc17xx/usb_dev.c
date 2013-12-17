/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <fcntl.h>
#include "hwpl/usb.h"
#include "hwpl/pio.h"
#include "usb_flags.h"
#include "hwpl/core.h"

#include "hwpl/debug.h"


#ifdef USB_DEBUG
volatile int usbdev_stat;
#define USB_DEV_DEBUG(x) (usbdev_stat |= x)
#else
#define USB_DEV_DEBUG(x)
#endif

static uint32_t usb_irq_mask;
static usb_attr_t usb_ctl;
static void usb_connect(uint32_t con);
static inline void usb_cfg(int port, uint32_t cfg) HWPL_ALWAYS_INLINE;
static inline void usb_set_addr(int port, uint32_t addr) HWPL_ALWAYS_INLINE;
static inline void usb_reset_ep(int port, uint32_t endpoint_num) HWPL_ALWAYS_INLINE;
static inline void usb_enable_ep(int port, uint32_t endpoint_num) HWPL_ALWAYS_INLINE;
static inline void usb_disable_ep(int port, uint32_t endpoint_num) HWPL_ALWAYS_INLINE;
static inline void usb_set_stall_ep(int port, uint32_t endpoint_num) HWPL_ALWAYS_INLINE;
static inline void usb_clr_stall_ep(int port, uint32_t endpoint_num) HWPL_ALWAYS_INLINE;
static inline void usb_cfg_ep(int port, void * ep_desc) HWPL_ALWAYS_INLINE;
static inline void slow_ep_int(void) HWPL_ALWAYS_INLINE;

static void exec_readcallback(int ep, void * data);
static void exec_writecallback(int ep, void * data);


typedef struct {
	hwpl_callback_t write_callback[USB_LOGIC_EP_NUM];
	hwpl_callback_t read_callback[USB_LOGIC_EP_NUM];
	void * write_context[USB_LOGIC_EP_NUM];
	void * read_context[USB_LOGIC_EP_NUM];
	volatile int write_pending;
	volatile int read_ready;
	void (*event_handler)(usb_spec_event_t,int);
	uint8_t ref_count;
	uint8_t connected;
} usb_local_t;

static usb_local_t usb_local HWPL_SYS_MEM;

static void clear_callbacks(void);
void clear_callbacks(void){
	memset(usb_local.write_callback, 0, USB_LOGIC_EP_NUM * sizeof(void(*)(int)));
	memset(usb_local.read_callback, 0, USB_LOGIC_EP_NUM * sizeof(void(*)(int)));
}

#define EP_MSK_CTRL 0x0001 // Control Endpoint Logical Address Mask
#define EP_MSK_BULK 0xC924 // Bulk Endpoint Logical Address Mask
#define EP_MSK_INT  0x4492 // Interrupt Endpoint Logical Address Mask
#define EP_MSK_ISO  0x1248 // Isochronous Endpoint Logical Address Mask

static uint32_t calc_ep_addr (uint32_t endpoint_num);

//The following should be static
static void usb_sie_wr_cmd(uint32_t cmd) HWPL_NEVER_INLINE;
static uint32_t usb_sie_rd_dat(void) HWPL_NEVER_INLINE;
static void usb_sie_wr_cmd_dat(uint32_t cmd, uint32_t val);
static void usb_sie_wr_cmd_ep(uint32_t ep_num, uint32_t cmd);
static uint32_t usb_sie_rd_cmd_dat(uint32_t cmd);


void usb_sie_wr_cmd(uint32_t cmd){
	LPC_USB->DevIntClr = CCEMTY_INT | CDFULL_INT;
	LPC_USB->CmdCode = cmd;
	while ((LPC_USB->DevIntSt & CCEMTY_INT) == 0);
}

uint32_t usb_sie_rd_dat(void){
	while ((LPC_USB->DevIntSt & CDFULL_INT) == 0);
	return LPC_USB->CmdData;
}

void usb_sie_wr_cmd_dat (uint32_t cmd, uint32_t val){
	usb_sie_wr_cmd(cmd);
	usb_sie_wr_cmd(val);
}

void usb_sie_wr_cmd_ep (uint32_t endpoint_num, uint32_t cmd){
	usb_sie_wr_cmd( USB_SIE_CMD_SEL_EP(calc_ep_addr(endpoint_num)));
	usb_sie_wr_cmd(cmd);
}

uint32_t usb_sie_rd_cmd_dat (uint32_t cmd){
	uint32_t dat;
	dat = (cmd & ~0xFFFF) | 0x0200;
	usb_sie_wr_cmd(cmd);
	usb_sie_wr_cmd(dat);
	return usb_sie_rd_dat();
}

void _hwpl_usb_dev_power_on(int port){
	if ( usb_local.ref_count == 0 ){
		//Set callbacks to NULL
		usb_local.connected = 0;
		clear_callbacks();
		usb_local.event_handler = NULL;
		_hwpl_lpc_core_enable_pwr(PCUSB);
		LPC_USB->USBClkCtrl = 0x12; //turn on dev clk en and AHB clk en
		while( LPC_USB->USBClkCtrl != 0x12 ){}  //wait for clocks

	}
	usb_local.ref_count++;
}

void _hwpl_usb_dev_power_off(int port){
	if ( usb_local.ref_count > 0 ){
		if ( usb_local.ref_count == 1 ){
			_hwpl_core_priv_disable_irq((void*)(USB_IRQn));  //Enable the USB interrupt
			LPC_USB->USBClkCtrl = 0x0; //turn off dev clk en and AHB clk en
			while( LPC_USB->USBClkCtrl != 0 ){}
			_hwpl_lpc_core_disable_pwr(PCUSB);
		}
		usb_local.ref_count--;
	}
}

int _hwpl_usb_dev_powered_on(int port){
	if( _hwpl_lpc_core_pwr_enabled(PCUSB) ){
		return 1;
	}
	return 0;
}


int hwpl_usb_getattr(int port, void * ctl){
	memcpy(ctl, &usb_ctl, sizeof(usb_attr_t));
	return 0;
}

int hwpl_usb_setattr(int port, void * ctl){
	usb_attr_t * ctlp;

	ctlp = (usb_attr_t *)ctl;

#if __lpc177x_8x
	switch(ctlp->pin_assign){
	case 0:
		break;
	case 1:
		break;
	default:
		errno = EINVAL;
		return -1 - offsetof(usb_attr_t, pin_assign);
	}
#else
	if ( ctlp->pin_assign != 0 ){
		errno = EINVAL;
		return -1 - offsetof(usb_attr_t, pin_assign);
	}
#endif

	if (ctlp->mode != USB_ATTR_MODE_DEVICE){
		errno = EINVAL;
		return -1 - offsetof(usb_attr_t, mode);
	}

	//Start the USB clock
	_hwpl_core_setusbclock(ctlp->crystal_freq);

	memcpy(&usb_ctl, ctl, sizeof(usb_attr_t));

	usb_local.read_ready = 0;
	usb_local.write_pending = 0;

	//Configure the IO
#ifdef __lpc177x_8x
	LPC_USB->USBClkCtrl |= (1<<3); //enable portsel clock
	while( (LPC_USB->USBClkSt & (1<<3)) == 0 ){}
	LPC_USB->StCtrl &= ~0x03;
	if ( ctlp->pin_assign == 0 ){
		_hwpl_usb_cfg_pio(0, 0, 29, 30, 1, 30);
	} else {
		LPC_USB->StCtrl |= 0x03;
		_hwpl_usb_cfg_pio(0, 0, 31, 31, 1, 30); //USB2_D- is dedicated
	}
	LPC_USB->USBClkCtrl &= ~(1<<3); //disable portsel clock
	while( (LPC_USB->USBClkSt & (1<<3)) != 0 ){}
#else
	_hwpl_usb_cfg_pio(0, 0, 29, 30, 1, 30);
#endif

	usb_irq_mask = DEV_STAT_INT | EP_FAST_INT | EP_SLOW_INT;

	_hwpl_core_priv_enable_irq((void*)USB_IRQn);  //Enable the USB interrupt
	hwpl_usb_reset(0, NULL);
	usb_set_addr(0,0);
	return 0;
}

void usb_connect(uint32_t con){
	usb_sie_wr_cmd_dat(USB_SIE_CMD_SET_DEV_STAT, USB_SIE_DAT_WR_BYTE(con ? DEV_CON : 0));
}

int hwpl_usb_attach(int port, void * ctl){
	usb_connect(1);
	return 0;
}

int hwpl_usb_detach(int port, void * ctl){
	usb_connect(0);
	return 0;
}

int hwpl_usb_setaction(int port, void * ctl){
	usb_action_t * action = (usb_action_t*)ctl;
	int log_ep;


	if( action->channel & 0x80 ){
		if( action->callback == 0 ){
			exec_writecallback(action->channel & ~0x80, DEVICE_OP_CANCELLED);
		}
	} else {
		if( action->callback == 0 ){
			exec_readcallback(action->channel & ~0x80, DEVICE_OP_CANCELLED);
		}
	}



	log_ep = action->channel & ~0x80;
	if ( (log_ep < USB_LOGIC_EP_NUM)  ){
		if( action->event == USB_EVENT_DATA_READY ){
			_hwpl_core_priv_enable_interrupts(NULL);
			usb_local.read_callback[ log_ep ] = action->callback;
			usb_local.read_context[ log_ep ] = action->context;
			return 0;
		} else if( action->event == USB_EVENT_WRITE_COMPLETE ){
			usb_local.write_callback[ log_ep ] = action->callback;
			usb_local.write_context[ log_ep ] = action->context;
			return 0;
		}
	}

	errno = EINVAL;
	return -1;
}

int hwpl_usb_configure(int port, void * ctl){
	usb_cfg(port, (int)ctl);
	return 0;
}

int hwpl_usb_setaddr(int port, void * ctl){
	usb_set_addr(port, (uint32_t)ctl);
	return 0;
}

int hwpl_usb_resetep(int port, void * ctl){
	usb_reset_ep(port, (uint32_t)ctl);
	return 0;
}

int hwpl_usb_enableep(int port, void * ctl){
	usb_enable_ep(port, (uint32_t)ctl);
	return 0;
}

int hwpl_usb_disableep(int port, void * ctl){
	usb_disable_ep(port, (uint32_t)ctl);
	return 0;
}

int hwpl_usb_stallep(int port, void * ctl){
	int ep = (int)ctl;
	usb_set_stall_ep(port, ep);
	return 0;
}

int hwpl_usb_unstallep(int port, void * ctl){
	int ep = (int)ctl;
	usb_clr_stall_ep(port, ep);
	return 0;
}

int hwpl_usb_cfgep(int port, void * ctl){
	usb_cfg_ep(port, ctl);
	return 0;
}

int hwpl_usb_seteventhandler(int port, void * ctl){
	usb_local.event_handler = (void(*)(usb_spec_event_t,int))ctl;
	return 0;
}

int _hwpl_usb_dev_read(const device_cfg_t * cfg, device_transfer_t * rop){
	int ret;
	int loc = rop->loc;

	if ( loc > (USB_LOGIC_EP_NUM-1) ){
		errno = EINVAL;
		return -1;
	}

	if( usb_local.read_callback[loc] != NULL ){
		errno = EAGAIN;
		return -1;
	}

	//Synchronous read (only if data is ready) otherwise 0 is returned
	if ( usb_local.read_ready & (1<<loc) ){
		usb_local.read_ready &= ~(1<<loc);  //clear the read ready bit
		ret = hwpl_usb_rd_ep(0, loc, rop->buf);
	} else {
		rop->nbyte = 0;
		if ( !(rop->flags & O_NONBLOCK) ){
			//If this is a blocking call, set the callback and context
			usb_local.read_callback[loc] = rop->callback;
			usb_local.read_context[loc] = rop->context;
			ret = 0;
		} else {
			errno = EAGAIN;
			ret = -1;
		}
	}

	return ret;
}

int _hwpl_usb_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	//Asynchronous write
	int ep;
	int port;
	int loc = wop->loc;

	port = DEVICE_GET_PORT(cfg);

	ep = (loc & 0x7F);

	if ( ep > (USB_LOGIC_EP_NUM-1) ){
		errno = EINVAL;
		return -1;
	}

	if ( usb_local.write_callback[ep] != NULL ){
		errno = EAGAIN;
		return -1;
	}

	usb_local.write_pending |= (1<<ep);
	usb_local.write_callback[ep] = wop->callback;
	usb_local.write_context[ep] = wop->context;
	wop->nbyte = hwpl_usb_wr_ep(0, loc, wop->buf, wop->nbyte);

	if ( wop->nbyte < 0 ){
		usb_disable_ep( port, loc );
		usb_reset_ep( port, loc );
		usb_enable_ep( port, loc );
		usb_local.write_pending &= ~(1<<ep);
		return -2;
	}

	return 0;
}

uint32_t calc_ep_addr (uint32_t endpoint_num){
	uint32_t val;
	val = (endpoint_num & 0x0F) << 1;
	if (endpoint_num & 0x80){
		val += 1;
	}
	return (val);
}

int hwpl_usb_reset(int port, void * cfg){

	//Set max packet size of phy ep 0
	LPC_USB->EpInd = 0;
	LPC_USB->MaxPSize = USB_MAX_PACKET0;

	//set max packet size of phy ep 1
	LPC_USB->EpInd = 1;
	LPC_USB->MaxPSize = USB_MAX_PACKET0;

	while ((LPC_USB->DevIntSt & EP_RLZED_INT) == 0);

	LPC_USB->EpIntClr  = 0xFFFFFFFF;
	LPC_USB->EpIntEn   = 0xFFFFFFFF ^ USB_DMA_EP;
	LPC_USB->DevIntClr = 0xFFFFFFFF;
	LPC_USB->DevIntEn = usb_irq_mask;

	return 0;
}

void usb_wakeup(int port){
	usb_sie_wr_cmd_dat(USB_SIE_CMD_SET_DEV_STAT, USB_SIE_DAT_WR_BYTE(DEV_CON));
}

void usb_set_addr(int port, uint32_t addr){
	usb_sie_wr_cmd_dat(USB_SIE_CMD_SET_ADDR, USB_SIE_DAT_WR_BYTE(DEV_EN | addr)); /* Don't wait for next */
	usb_sie_wr_cmd_dat(USB_SIE_CMD_SET_ADDR, USB_SIE_DAT_WR_BYTE(DEV_EN | addr)); /*  Setup Status Phase */
}

void usb_cfg(int port, uint32_t cfg){
	usb_sie_wr_cmd_dat(USB_SIE_CMD_CFG_DEV, USB_SIE_DAT_WR_BYTE(cfg ? CONF_DVICE : 0));
}

void usb_cfg_ep(int port, void * ep_desc){
	uint32_t num;
	hwpl_usb_ep_desc_t * ep_ptr;
	ep_ptr = (hwpl_usb_ep_desc_t*)ep_desc;
	num = calc_ep_addr(ep_ptr->bEndpointAddress);
	LPC_USB->ReEp |= (1 << num);
	LPC_USB->EpInd = num;
	LPC_USB->MaxPSize = ep_ptr->wMaxPacketSize;
	while ((LPC_USB->DevIntSt & EP_RLZED_INT) == 0);
	LPC_USB->DevIntClr = EP_RLZED_INT;
}

void usb_enable_ep(int port, uint32_t endpoint_num){
	usb_sie_wr_cmd_dat(USB_SIE_CMD_SET_EP_STAT(calc_ep_addr(endpoint_num)), USB_SIE_DAT_WR_BYTE(0));
}

void usb_disable_ep(int port, uint32_t endpoint_num){
	usb_sie_wr_cmd_dat(USB_SIE_CMD_SET_EP_STAT(calc_ep_addr(endpoint_num)), USB_SIE_DAT_WR_BYTE(EP_STAT_DA));
}

void usb_reset_ep(int port, uint32_t endpoint_num){
	usb_sie_wr_cmd_dat(USB_SIE_CMD_SET_EP_STAT(calc_ep_addr(endpoint_num)), USB_SIE_DAT_WR_BYTE(0));
}

void usb_set_stall_ep(int port, uint32_t endpoint_num){
	usb_sie_wr_cmd_dat(USB_SIE_CMD_SET_EP_STAT(calc_ep_addr(endpoint_num)), USB_SIE_DAT_WR_BYTE(EP_STAT_ST));
}

void usb_clr_stall_ep(int port, uint32_t endpoint_num){
	usb_sie_wr_cmd_dat(USB_SIE_CMD_SET_EP_STAT(calc_ep_addr(endpoint_num)), USB_SIE_DAT_WR_BYTE(0));
}

int hwpl_usb_isconnected(int port, void * ctl){
	return usb_local.connected;
}

void usb_clr_ep_buf(int port, uint32_t endpoint_num){
	usb_sie_wr_cmd_ep(endpoint_num, USB_SIE_CMD_CLR_BUF);
}

int hwpl_usb_rd_ep(int port, uint32_t endpoint_num, void * dest){
	uint32_t n;
	uint32_t * ptr;
	uint32_t size;
	uint32_t mask;
	int i;

	LPC_USB->Ctrl = ((endpoint_num & 0x0F) << 2) | CTRL_RD_EN;

	//There needs to be a delay between the time USBCtrl is set and reading USBRxPLen due to the difference in clocks
	if ( _hwpl_core_getclock() > 48000000 ){
		for(i=0; i < _hwpl_core_getclock()/4000000; i++){
			asm volatile("nop");
		}
	}

	ptr = (uint32_t*)dest;
	n = 0;
	mask = ((EP_MSK_ISO >> endpoint_num) & 1);
	size = LPC_USB->RxPLen;

	while(LPC_USB->Ctrl & CTRL_RD_EN){
		*ptr++ = LPC_USB->RxData;
		n += 4;
	}

	if (mask == 0){   // Non-Isochronous Endpoint
		//Clear the buffer so more data can be received
		usb_sie_wr_cmd_ep(endpoint_num, USB_SIE_CMD_CLR_BUF);
	}

	return size & 0x3FF;
}


/*! \details
 */
int hwpl_usb_wr_ep(int port, uint32_t endpoint_num, const void * src, uint32_t size){
	uint32_t n;
	uint32_t * ptr = (uint32_t*)src;

	LPC_USB->Ctrl = ((endpoint_num & 0x0F) << 2) | CTRL_WR_EN;
	LPC_USB->TxPLen = size;

	n = 0;
	while( LPC_USB->Ctrl & CTRL_WR_EN ){
		if ( n < size ){
			LPC_USB->TxData = *ptr++;
		} else {
			LPC_USB->TxData = 0;
		}
		n+=4;
	}

	usb_sie_wr_cmd_ep(endpoint_num, USB_SIE_CMD_VALID_BUF);
	return size;
}

extern volatile int usbdev_stat;


/*! \details This function services the USB interrupt request.
 */
void _hwpl_core_usb_isr(void){
	uint32_t device_interrupt_status;
	uint32_t tmp;
	int i;

	USB_DEV_DEBUG(0x01);
	device_interrupt_status = LPC_USB->DevIntSt;     //Device interrupt status
	if (device_interrupt_status & ERR_INT){ //Error interrupt
		USB_DEV_DEBUG(0x02);
		tmp = usb_sie_rd_cmd_dat(USB_SIE_CMD_RD_ERR_STAT);
		if ( usb_local.event_handler ){
			usb_local.event_handler(USB_SPEC_EVENT_ERR, tmp);
		}
		LPC_USB->DevIntClr = ERR_INT;
	}

	if (device_interrupt_status & FRAME_INT){ //start of frame
		USB_DEV_DEBUG(0x04);
		if ( usb_local.event_handler ){
			usb_local.event_handler(USB_SPEC_EVENT_SOF, 0);
		}
		LPC_USB->DevIntClr = FRAME_INT;
	}

	if (device_interrupt_status & DEV_STAT_INT){ //Status interrupt (Reset, suspend/resume or connect)
		USB_DEV_DEBUG(0x08);
		_delay_us(100);
		tmp = usb_sie_rd_cmd_dat(USB_SIE_CMD_GET_DEV_STAT);
		if (tmp & DEV_RST){
			USB_DEV_DEBUG(0x10);
			//hwpl_usb_reset(0, NULL);
			usb_local.connected = 1;
			usb_local.write_pending = 0;
			usb_local.read_ready = 0;
			if ( usb_local.event_handler ){
				usb_local.event_handler(USB_SPEC_EVENT_RESET, 0);
			}
		}

		if ( tmp == 0x0D ){
			USB_DEV_DEBUG(0x20);
			usb_local.connected = 0;
			for(i = 1; i < USB_LOGIC_EP_NUM; i++){
				if( usb_local.read_callback[i] != NULL ){
					if( usb_local.read_callback[i](usb_local.read_context[i], (const void*)-1) == 0 ){
						usb_local.read_callback[i] = NULL;
					}
				}

				if( usb_local.write_callback[i] != NULL ){
					if( usb_local.write_callback[i](usb_local.write_context[i], (const void*)-1) == 0 ){
						usb_local.write_callback[i] = NULL;
					}
				}

			}
		}

		if ( usb_local.event_handler ){
			if (tmp & DEV_CON_CH){
				usb_local.event_handler(USB_SPEC_EVENT_POWER, tmp);
			}

			if (tmp & DEV_SUS_CH){
				if (tmp & DEV_SUS){
					usb_local.event_handler(USB_SPEC_EVENT_SUSPEND, tmp);
				} else {
					usb_local.event_handler(USB_SPEC_EVENT_RESUME, tmp);
				}
			}
		}

		LPC_USB->DevIntClr = DEV_STAT_INT|CCEMTY_INT|CDFULL_INT|RxENDPKT_INT|TxENDPKT_INT|EP_RLZED_INT;
		return;
	}

	if (device_interrupt_status & EP_SLOW_INT){ //Slow endpoint interrupt
		slow_ep_int();
	}

	LPC_USB->DevIntClr = CCEMTY_INT|CDFULL_INT|RxENDPKT_INT|TxENDPKT_INT|EP_RLZED_INT;
	return;
}

void slow_ep_int(void){
	uint32_t episr;
	uint32_t phy_ep, log_ep;
	uint32_t tmp;
	episr = LPC_USB->EpIntSt;

	USB_DEV_DEBUG(0x100);

	for (phy_ep = 0; phy_ep < USB_EP_NUM; phy_ep++){

		if (episr & (1 << phy_ep)){

			//Calculate the logical endpoint value (associated with the USB Spec)
			log_ep = phy_ep >> 1;

			//Clear the interrupt
			LPC_USB->EpIntClr = (1 << phy_ep);

			//Read the SIE command data
			while ((LPC_USB->DevIntSt & CDFULL_INT) == 0);
			tmp = LPC_USB->CmdData;

			//Check for endpoint 0
			if ((phy_ep & 1) == 0){ //These are the OUT endpoints

				//Check for a setup packet
				if ( (phy_ep == 0) && (tmp & EP_SEL_STP) ){
					USB_DEV_DEBUG(0x200);
					if (usb_local.read_callback[0]){
						if( usb_local.read_callback[0](usb_local.read_context, (const void*)USB_SETUP_EVENT) == 0 ){
							usb_local.read_callback[0] = NULL;
						}
					}
				} else {
					exec_readcallback(log_ep, (void*)USB_OUT_EVENT);

				}
			} else {  //These are the IN endpoints
				exec_writecallback(log_ep, (void*)USB_IN_EVENT);
			}
		}

	}
	LPC_USB->DevIntClr = EP_SLOW_INT;
}

static void exec_readcallback(int log_ep, void * data){
	usb_local.read_ready |= (1<<log_ep);
	if( usb_local.read_callback[log_ep] != NULL ){
		if( usb_local.read_callback[log_ep](usb_local.read_context[log_ep], data) == 0 ){
			usb_local.read_callback[log_ep] = NULL;
		}
	}
}

static void exec_writecallback(int log_ep, void * data){
	usb_local.write_pending &= ~(1<<log_ep);
	if( usb_local.write_callback[log_ep] != NULL ){
		if( usb_local.write_callback[log_ep](usb_local.write_context[log_ep], data) == 0 ){
			usb_local.write_callback[log_ep] = NULL;
		}
	}
}








