/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <fcntl.h>
#include "hwpl/dac.h"
#include "hwpl/debug.h"
#include "hwpl/core.h"
#include "hwpl/dma.h"


#define DAC_CTRL_DBUF_ENA (1<<1)
#define DAC_CTRL_CNT_ENA (1<<2)
#define DAC_CTRL_DMA_ENA (1<<3)

#define DAC_DMA_CHAN 5

typedef struct {
	hwpl_callback_t callback;
	void * context;
	volatile int len;
	void * volatile bufp;
	uint8_t enabled_channels;
	uint8_t ref_count;
} dac_local_t;

static dac_local_t dac_local HWPL_SYS_MEM;

static int do_transfer(void);
static int write_complete(void * context, const void * data);
static void exec_callback(void * data);


void _hwpl_dac_dev_power_on(int port){
	if ( dac_local.ref_count == 0 ){
		dac_local.ref_count++;
		memset(&dac_local, 0, sizeof(dac_local_t));
	}

	dac_local.ref_count++;
}

void _hwpl_dac_dev_power_off(int port){
	if ( dac_local.ref_count > 0 ){
		if ( dac_local.ref_count == 1 ){
		_hwpl_core_set_pinsel_func(0,26,CORE_PERIPH_GPIO,0);
		}
		dac_local.ref_count--;
	}
}

int _hwpl_dac_dev_powered_on(int port){
	return dac_local.ref_count != 0;
}

int hwpl_dac_getattr(int port, void * ctl){
	dac_attr_t * ctlp;
	ctlp = (dac_attr_t*)ctl;
	int clk_div;

	clk_div = LPC_DAC->CNTVAL;

	ctlp->pin_assign = 0; //always zero
	ctlp->enabled_channels = 1; //only has one channel
	ctlp->freq = hwpl_core_cpu_freq / (clk_div + 1);
	return 0;
}

int hwpl_dac_setattr(int port, void * ctl){
	dac_attr_t * ctlp;
	ctlp = (dac_attr_t*)ctl;
	int clkdiv;

	if ( ctlp->freq == 0 ){
		errno = EINVAL;
		return -1 - offsetof(dac_attr_t, freq);
	}

	if ( ctlp->freq > DAC_MAX_FREQ ){
		ctlp->freq = DAC_MAX_FREQ;
	}

	if ( ctlp->pin_assign != 0 ){
		errno = EINVAL;
		return -1 - offsetof(dac_attr_t, pin_assign);
	}

	clkdiv = hwpl_core_cpu_freq / ctlp->freq;
	if ( clkdiv > ((1<<16)-1) ){
		clkdiv = ((1<<16)-1);
	} else if ( clkdiv < 1 ){
		clkdiv = 1;
	}

	if ( ctlp->enabled_channels & 0x01 ){
		_hwpl_core_set_pinsel_func(0,26,CORE_PERIPH_DAC,0);
	} else {
		errno = EINVAL;
		return -1 - offsetof(dac_attr_t, enabled_channels);
	}

	_hwpl_dma_init(0);

	LPC_DAC->CNTVAL = clkdiv;
	return 0;
}

int hwpl_dac_setaction(int port, void * ctl){
	hwpl_action_t * action = (hwpl_action_t*)ctl;
	if( action->callback == 0 ){
		if ( LPC_GPDMA->EnbldChns & (1<<DAC_DMA_CHAN) ){
			exec_callback(DEVICE_OP_CANCELLED);
		}
	}
	dac_local.callback = action->callback;
	dac_local.context = action->context;
	return 0;
}

int hwpl_dac_get(int port, void * ctl){
	return LPC_DAC->CR;
}

int _hwpl_dac_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	//Check to see if the DAC is busy
	if ( wop->loc != 0 ){
		errno = EINVAL;
		return -1;
	}

	if ( LPC_GPDMA->EnbldChns & (1<<DAC_DMA_CHAN) ){
		errno = EAGAIN;
		return -1;
	}

	dac_local.bufp = (void * volatile)wop->buf;
	dac_local.len = (wop->nbyte) >> 2;
	do_transfer();
	LPC_DAC->CTRL = DAC_CTRL_CNT_ENA|DAC_CTRL_CNT_ENA|DAC_CTRL_DBUF_ENA;
	wop->nbyte = (wop->nbyte) & ~0x3;
	dac_local.callback = wop->callback;
	dac_local.context = wop->context;
	return 0;
}

int do_transfer(void){
	int page_size;
	int ctrl;
	int err;

	page_size = dac_local.len > (DMA_MAX_TRANSFER_SIZE >> 2) ? (DMA_MAX_TRANSFER_SIZE >> 2) : dac_local.len;

	ctrl = DMA_CTRL_TRANSFER_SIZE(page_size)|
			DMA_CTRL_SRC_WIDTH32|
			DMA_CTRL_DEST_WIDTH32|
			DMA_CTRL_ENABLE_TERMINAL_COUNT_INT|
			DMA_CTRL_SRC_INC;

	err = _hwpl_dma_transfer(DMA_MEM_TO_PERIPH,
			DAC_DMA_CHAN,
			(void*)&(LPC_DAC->CR),
			(void*)dac_local.bufp,
			ctrl,
			write_complete,
			NULL,
			DMA_REQ_DAC,
			0);

	if ( err < 0 ){
		return -1;
	}

	dac_local.len -= page_size;
	dac_local.bufp += (page_size << 2);
	return 0;
}

void exec_callback(void * data){
	dac_local.bufp = NULL;
	//call the signal callback
	LPC_DAC->CTRL = 0;
	if ( dac_local.callback != NULL ){
		if( dac_local.callback(dac_local.context, NULL) == 0 ){
			dac_local.callback = NULL;
		}
	}
}

int write_complete(void * context, const void * data){
	if ( dac_local.len ){
		do_transfer();
		return 1; //keep interrupt in place
	} else {
		exec_callback(0);
	}
	return 0;
}
