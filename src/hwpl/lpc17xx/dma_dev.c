/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"

typedef struct {
	hwpl_callback_t callback;
	void * context;
} dma_local_t;

static dma_local_t dma_local[DMA_MAX_CHANNEL] HWPL_SYS_MEM;

LPC_GPDMACH_TypeDef * _hwpl_dma_get_channel(uint32_t chan){
	if ( chan < 8 ){
		return (LPC_GPDMACH_TypeDef *)(LPC_GPDMACH0_BASE + chan * 0x20);
	} else {
		return NULL;
	}
}

void _hwpl_dma_init(int mode){
	//Power up the DMA
	_hwpl_lpc_core_enable_pwr(PCGPDMA);
	//Enable the controller
	HWPL_SET_BIT(LPC_GPDMA->Config, 0);
	//Enable the interrupts
	_hwpl_core_priv_enable_irq((void*)DMA_IRQn);
}

void _hwpl_dma_exit(void){
	//Power up the DMA
	_hwpl_lpc_core_disable_pwr(PCGPDMA);
}

int _hwpl_dma_halttransfer(int chan){
	LPC_GPDMACH_TypeDef * channel_regs;
	//Get the channel registers
	if ( chan < 0 ){
		return -1;
	}

	if ( chan > DMA_MAX_CHANNEL ){
		return -1;
	}

	channel_regs = _hwpl_dma_get_channel(chan);
	channel_regs->CConfig = 0;
	return chan;
}


void _hwpl_core_dma_isr(void){
	int i;
	//Execute the channel callbacks if they are available
	for(i=0; i < DMA_MAX_CHANNEL; i++){
		if ( LPC_GPDMA->IntTCStat & (1<<i) ){ //If there is an interrupt on the channel, check for the callback
			LPC_GPDMA->IntTCClear = (1<<i);
			if ( dma_local[i].callback != NULL ){
				if( dma_local[i].callback(dma_local[i].context, NULL) == 0 ){
					dma_local[i].callback = NULL;
				}
			}
		}

		if ( LPC_GPDMA->IntErrStat & (1<<i) ){
			LPC_GPDMA->IntErrClr = (1<<i);
			if ( dma_local[i].callback != NULL ){
				if( dma_local[i].callback(dma_local[i].context, (void*)1) == 0 ){
					dma_local[i].callback = NULL;
				}
			}
		}
	}
}

int _hwpl_dma_transferlist(int operation,
		int chan,
		dma_lli_t * linked_list,
		hwpl_callback_t callback,
		void * context,
		uint32_t dest_periph,
		uint32_t src_periph
		){
	LPC_GPDMACH_TypeDef * channel_regs;

	//Get the channel registers
	channel_regs = _hwpl_dma_get_channel(chan);
	if (channel_regs == NULL ){
		//This is not a valid channel
		return -1;
	}

	if ( (LPC_GPDMA->EnbldChns & (1<<chan)) ){
		//Channel is busy
		return -1;
	}

	//Clear pending interrupts
	LPC_GPDMA->IntTCClear |= (1<<chan);
	LPC_GPDMA->IntErrClr |= (1<<chan);

	//Set up the transfer
	channel_regs->CSrcAddr = (uint32_t)linked_list->src;
	channel_regs->CDestAddr = (uint32_t)linked_list->dest;
	channel_regs->CControl = linked_list->ctrl;
	channel_regs->CLLI = (uint32_t)linked_list->next;

	//Set the callback value
	dma_local[chan].callback = callback;
	dma_local[chan].context = context;

	//Start the transfer
	channel_regs->CConfig = DMA_CFG_SRC_PERIPH(src_periph)|
			DMA_CFG_DEST_PERIPH(dest_periph)|
			DMA_CFG_TRANSFER_TYPE(operation)|
			DMA_CFG_ENABLE_IE|
			DMA_CFG_ENABLE_ITC|
			DMA_CFG_ENABLE_CHAN;

	return chan;
}

int _hwpl_dma_transfer(int operation,
		int chan,
		void * dest,
		const void * src,
		int ctrl,
		hwpl_callback_t cb,
		void * context,
		uint32_t dest_periph,
		uint32_t src_periph){
	dma_lli_t list;
	list.src = src;
	list.dest = dest;
	list.ctrl = ctrl;
	list.next = NULL;


	//If there is a callback, enable the interrupt
	if ( cb != 0 ){
		list.ctrl |= DMA_CTRL_ENABLE_TERMINAL_COUNT_INT;
	}

	return _hwpl_dma_transferlist(operation, chan, &list, cb, context, dest_periph, src_periph);
}

