/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <fcntl.h>
#include "hwpl/i2c.h"
#include "hwpl/pio.h"
#include "hwpl/debug.h"
#include "hwpl/core.h"

#define WRITE_OP 0
#define READ_OP 1

static LPC_I2C_TypeDef * const i2c_regs_table[HWPL_I2C_PORTS] = {
		LPC_I2C0,
		LPC_I2C1,
		LPC_I2C2
};

static inline LPC_I2C_TypeDef * i2c_get_regs(int port) HWPL_ALWAYS_INLINE;
LPC_I2C_TypeDef * i2c_get_regs(int port){
	return i2c_regs_table[port];
}

static void exec_callback(int port, void * data);

typedef union {
	uint16_t ptr16;
	uint8_t ptr8[2];
} i2c_ptr_t;

typedef struct {
	volatile char * data;
	volatile uint16_t size;
	volatile i2c_ptr_t ptr;
	char addr;
	uint8_t ref_count;
	volatile uint8_t state;
	volatile int err;
	int * ret;
	i2c_reqattr_t transfer;
	hwpl_callback_t callback;
	void * context;
} i2c_local_t;

i2c_attr_t i2c_local_attr[HWPL_I2C_PORTS] HWPL_SYS_MEM;
i2c_local_t i2c_local[HWPL_I2C_PORTS] HWPL_SYS_MEM;

#define AA (1<<2)
#define SI (1<<3)
#define STO (1<<4)
#define STA (1<<5)
#define I2EN (1<<6)

#define I2C_DONE_FLAG (1<<7)


static inline char i2c_tst_done(int port) HWPL_ALWAYS_INLINE;
char i2c_tst_done(int port){
	return i2c_local[port].state & I2C_DONE_FLAG;
}

static inline void i2c_clr_done(int port) HWPL_ALWAYS_INLINE;
void i2c_clr_done(int port){
	i2c_local[port].state  &= ~(I2C_DONE_FLAG);
}

static int i2c_transfer(int port, int op, device_transfer_t * dop);

#define i2c_slave_ack(port) (LPC_I2C[port].CONSET = I2CONSET_AA)
#define i2c_slave_nack(port) (LPC_I2C[port].CONCLR = I2CONCLR_AAC)
#define i2c_slave_clr_int(port) (LPC_I2C[port].CONCLR = I2CONCLR_SIC)


void _hwpl_i2c_dev_power_on(int port){
	if ( i2c_local[port].ref_count == 0 ){
		switch(port){
		case 0:
			_hwpl_lpc_core_enable_pwr(PCI2C0);
			break;
		case 1:
			_hwpl_lpc_core_enable_pwr(PCI2C1);
			break;
		case 2:
			_hwpl_lpc_core_enable_pwr(PCI2C2);
			break;
		}
		_hwpl_core_priv_enable_irq((void*)I2C0_IRQn + port);
		i2c_local[port].callback = NULL;
	}
	i2c_local[port].ref_count++;
}

void _hwpl_i2c_dev_power_off(int port){
	if ( i2c_local[port].ref_count > 0 ){
		if ( i2c_local[port].ref_count == 1 ){
			_hwpl_core_priv_disable_irq((void*)(I2C0_IRQn + port));
			switch(port){
			case 0:
				_hwpl_lpc_core_disable_pwr(PCI2C0);
				break;
			case 1:
				_hwpl_lpc_core_disable_pwr(PCI2C1);
				break;
			case 2:
				_hwpl_lpc_core_disable_pwr(PCI2C2);
				break;
			}
		}
		i2c_local[port].ref_count--;
	}
}

int _hwpl_i2c_dev_powered_on(int port){
	return ( i2c_local[port].ref_count != 0 );
}

int hwpl_i2c_getattr(int port, void * ctl){
	memcpy(ctl, &(i2c_local_attr[port]), sizeof(i2c_attr_t));
	return 0;
}

int hwpl_i2c_setattr(int port, void * ctl){
	LPC_I2C_TypeDef * i2c_regs;
	i2c_attr_t * ctl_ptr;
	int count;

	ctl_ptr = (i2c_attr_t *)ctl;

	//Check for a valid port
	i2c_regs = i2c_regs_table[port];

	if ( ctl_ptr->bitrate == 0 ){
		errno = EINVAL;
		return -1 - offsetof(i2c_attr_t, bitrate);
	}

	if ( ctl_ptr->pin_assign != HWPL_GPIO_CFG_USER ){

		switch(port){
		case 0:
			if (1){ //This makes auto-indent work properly with eclipse
				switch(ctl_ptr->pin_assign){
				case 0:
					if ( _hwpl_i2c_cfg_pio(0, 0, 28, 27) < 0 ){
						errno = EINVAL;
						return -1;
					}
					break;
				default:
					errno = EINVAL;
					return -1 - offsetof(i2c_attr_t, pin_assign);
				}
			}
			break;
		case 1:
			if (1){
				switch(ctl_ptr->pin_assign){
				case 0:
					if( _hwpl_i2c_cfg_pio(1, 0, 1, 0) < 0 ){
						errno = EINVAL;
						return -1;
					}
					break;
				case 1:
					if ( _hwpl_i2c_cfg_pio(1, 0, 20, 19) < 0 ){
						errno = EINVAL;
						return -1;
					}
					break;
				default:
					errno = EINVAL;
					return -1 - offsetof(i2c_attr_t, pin_assign);
				}
			}
			break;
		case 2:
			if (1){
				switch(ctl_ptr->pin_assign){
				case 0:
					_hwpl_i2c_cfg_pio(2, 0, 11, 10);
					break;
				default:
					errno = EINVAL;
					return -1 - offsetof(i2c_attr_t, pin_assign);
				}
			}
			break;
		default:
			errno = EINVAL;
			return -1 - offsetof(i2c_attr_t, pin_assign);
		}
	}

	i2c_local[port].state = I2C_DONE_FLAG; //Set the done flag (not busy)

	count = ((_hwpl_core_getclock()) / (ctl_ptr->bitrate * 2));
	if ( count > 0xFFFF ){
		count = 0xFFFF;
	}
	i2c_regs->SCLH = count;
	i2c_regs->SCLL = count;

	//Enable the I2C unit
	i2c_regs->CONSET = (I2EN);
	memcpy(&(i2c_local_attr[port]), ctl_ptr, sizeof(i2c_attr_t));
	i2c_local_attr[port].bitrate = _hwpl_core_getclock() / (2 * count);
	return 0;
}

int hwpl_i2c_setup(int port, void * ctl){
	i2c_reqattr_t * ctl_ptr;
	ctl_ptr = ctl;
	i2c_local[port].transfer.slave_addr = ctl_ptr->slave_addr;
	i2c_local[port].transfer.transfer = ctl_ptr->transfer;
	return 0;
}

int hwpl_i2c_geterr(int port, void * ctl){
	return i2c_local[port].err;
}

int hwpl_i2c_setaction(int port, void * ctl){
	hwpl_action_t * action = (hwpl_action_t*)ctl;
	if( action->callback == 0 ){
		if ( !(i2c_local[port].state & I2C_DONE_FLAG) ){
			exec_callback(port, DEVICE_OP_CANCELLED);
		}
	}

	i2c_local[port].callback = action->callback;
	i2c_local[port].context = action->context;
	return 0;
}

int _hwpl_i2c_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	int port = DEVICE_GET_PORT(cfg);;
	return i2c_transfer(port, WRITE_OP, wop);
}

int _hwpl_i2c_dev_read(const device_cfg_t * cfg, device_transfer_t * rop){
	int port = DEVICE_GET_PORT(cfg);
	return i2c_transfer(port, READ_OP, rop);
}

static void be_done(int port){
	i2c_local[port].state = I2C_DONE_FLAG;
	*(i2c_local[port].ret) = 0;
	if ( i2c_local[port].callback != NULL ){
		i2c_local[port].callback(i2c_local[port].context, (const void*)i2c_local[port].err);
		i2c_local[port].callback = NULL;
	}
}

static void _hwpl_i2c_isr(int port) {
	uint8_t stat_value;
	LPC_I2C_TypeDef * i2c_regs;
	// this handler deals with master read and master write only
	i2c_regs = i2c_get_regs(port);

	if ( i2c_regs == NULL ){
		be_done(port);
	}
	stat_value = i2c_regs->STAT;
	switch ( stat_value ){
	case 0x08: //Start Condition has been sent
	case 0x10: //Repeated Start condition
		i2c_local[port].err = 0;
		if ( i2c_local[port].state == I2C_STATE_NONE){
			i2c_regs->DAT = i2c_local[port].addr | 0x01; //Set the Read bit -- repeated start after write ptr
		} else {
			i2c_regs->DAT = i2c_local[port].addr;
		}

		i2c_regs->CONSET = AA;
		i2c_regs->CONCLR = STA;
		break;
	case 0x18: //SLA+W transmitted -- Ack Received
		i2c_regs->DAT = i2c_local[port].ptr.ptr8[1]; //Send the offset pointer (MSB of 16 or 8 bit)
		i2c_regs->CONSET = AA;
		break;
	case 0x28: //Data byte transmitted -- Ack Received
		if (( i2c_local[port].state == I2C_STATE_RD_16_OP )||
				( i2c_local[port].state == I2C_STATE_WR_16_OP )){
			i2c_regs->DAT = i2c_local[port].ptr.ptr8[0]; //Send the offset pointer (LSB)
			i2c_regs->CONSET = AA;
			if ( i2c_local[port].state == I2C_STATE_RD_16_OP ){
				i2c_local[port].state = I2C_STATE_RD_OP;
			}
			break;
		}

		if ( i2c_local[port].state == I2C_STATE_RD_OP ){
			i2c_regs->CONSET = AA|STA; //Restart (then send read command)
			i2c_local[port].state = I2C_STATE_NONE;
			break;
		}

		//Transmit data
		if ( i2c_local[port].size ){
			i2c_regs->DAT = *(i2c_local[port].data);
			i2c_local[port].data++;
			i2c_regs->CONSET = AA;
			i2c_local[port].size--;
		} else {
			i2c_regs->CONSET = STO|AA;
			i2c_local[port].state = I2C_STATE_NONE | I2C_DONE_FLAG;
		}
		break;
	case 0x20:
	case 0x30:
	case 0x48:
		//Receiver nack'd
		i2c_regs->CONSET = STO;
		i2c_local[port].size = 0;
		i2c_local[port].err = I2C_ERROR_ACK;
		i2c_local[port].state = I2C_DONE_FLAG;
		break;
	case 0x38:
		i2c_local[port].size = 0;
		i2c_regs->CONSET = STO;
		i2c_local[port].err = I2C_ERROR_ARBITRATION_LOST;
		i2c_local[port].state = I2C_DONE_FLAG;
		break;
	case 0x40: //SLA+R transmitted -- Ack received
		if ( i2c_local[port].size > 1 ){
			i2c_regs->CONSET = AA; //only ACK if more than one byte is coming
		} else {
			i2c_regs->CONCLR = AA;
		}
		break;
	case 0x50: //Data Byte received -- Ack returned
		//Receive Data
		if ( i2c_local[port].size ) i2c_local[port].size--;
		*(i2c_local[port].data) = (char)i2c_regs->DAT;
		i2c_local[port].data++;
		if ( i2c_local[port].size > 1 ){
			i2c_regs->CONSET = AA;
		} else {
			i2c_regs->CONCLR = AA;
		}
		break;
	case 0x58: //Data byte received -- Not Ack returned
		if ( i2c_local[port].size ) i2c_local[port].size--;
		*(i2c_local[port].data) = (char)i2c_regs->DAT;
		i2c_local[port].data++;

		i2c_regs->CONSET = STO;
		i2c_local[port].state = I2C_STATE_NONE | I2C_DONE_FLAG;
		break;
	case 0x00:
		i2c_local[port].err = I2C_ERROR_START;
		i2c_local[port].state = I2C_DONE_FLAG;
		i2c_regs->CONSET = STO;
		break;
	}

	i2c_regs->CONCLR = SI; //clear the interrupt flag

	if ( i2c_local[port].state & I2C_DONE_FLAG ){
		exec_callback(port, 0);
	}
}

void exec_callback(int port, void * data){
	i2c_local[port].state |= I2C_DONE_FLAG;
	if ( i2c_local[port].err != 0 ){
		*(i2c_local[port].ret) = -1;
	}

	if ( i2c_local[port].callback != NULL ){
		if( i2c_local[port].callback(i2c_local[port].context, data) == 0 ){
			i2c_local[port].callback = NULL;
		}
	}
}

int i2c_transfer(int port, int op, device_transfer_t * dop){
	LPC_I2C_TypeDef * i2c_regs;
	int size = dop->nbyte;
	i2c_regs = i2c_get_regs(port);


	if ( !(i2c_local[port].state & I2C_DONE_FLAG) ){
		errno = EAGAIN;
		return -1;
	}

	i2c_local[port].callback = dop->callback;
	i2c_local[port].context = dop->context;

	if ( dop->nbyte == 0 ){
		return 0;
	}

	i2c_clr_done(port); //clear the done flag
	i2c_local[port].addr = (i2c_local[port].transfer.slave_addr << 1); //a write operation
	i2c_local[port].data = (volatile void*)dop->buf;
	i2c_local[port].ret = &(dop->nbyte);

	switch(i2c_local[port].transfer.transfer){
	case I2C_TRANSFER_NORMAL:
		i2c_local[port].ptr.ptr8[1] = dop->loc;
		i2c_local[port].size = size;
		if ( op == WRITE_OP ){
			i2c_local[port].state = I2C_STATE_WR_OP;
		} else {
			i2c_local[port].state = I2C_STATE_RD_OP;
		}
		break;
	case I2C_TRANSFER_NORMAL_16:
		i2c_local[port].ptr.ptr16 = dop->loc;
		i2c_local[port].size = size;
		if ( op == WRITE_OP ){
			i2c_local[port].state = I2C_STATE_WR_16_OP;
		} else {
			i2c_local[port].state = I2C_STATE_RD_16_OP;
		}
		break;
	case I2C_TRANSFER_READ_ONLY:
		i2c_local[port].size = size;
		if ( op == READ_OP ){
			i2c_local[port].state = I2C_STATE_NONE;
		} else {
			errno = EINVAL;
			return -1;
		}
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	//Master transmitter mode -- write the pointer value
	i2c_regs->CONSET = STA;
	return 0;
}

void _hwpl_core_i2c0_isr(void){ _hwpl_i2c_isr(0); }
void _hwpl_core_i2c1_isr(void){ _hwpl_i2c_isr(1); }
void _hwpl_core_i2c2_isr(void){ _hwpl_i2c_isr(2); }



