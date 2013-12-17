/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "core_flags.h"
#include "hwpl/core.h"


extern void _hwpl_set_sleep_mode(int * level);
extern LPC_GPDMACH_TypeDef * _hwpl_dma_get_channel(uint32_t chan);

int _hwpl_core_sleep(core_sleep_t level){
	int tmp;
	LPC_GPDMACH_TypeDef * channel_regs;

#ifdef __lpc17xx
	//Wait for any DMA transactions on RAM to complete (AHB transactions are OK in idle but not deep sleep)
	for(tmp = 0; tmp < DMA_MAX_CHANNEL; tmp++){
		if ( LPC_GPDMA->EnbldChns & (1<<tmp) ){
			channel_regs = _hwpl_dma_get_channel(tmp);
			if ( level >= CORE_DEEPSLEEP ){
				if ( (channel_regs->CDestAddr <= 0x10001FFF) ||
						(channel_regs->CSrcAddr <= 0x10001FFF) ){

					//wait for DMA to complete
					while(  LPC_GPDMA->EnbldChns & (1<<tmp) ){
						//this will trip the WDT if it takes too long
					}
				}
			} else {
				//wait for DMA to complete
				while(  LPC_GPDMA->EnbldChns & (1<<tmp) ){
					//this will trip the WDT if it takes too long
				}
			}
		}
	}
#endif

	tmp = level;
	hwpl_core_privcall((core_privcall_t)_hwpl_set_sleep_mode, &level);
	if ( level < 0 ){
		return level;
	}


	//Wait for an interrupts
	__WFI();
	return 0;
}

