/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <stdbool.h>
#include "hwpl/core.h"
#include "hwpl/tmr.h"
#include "pio_flags.h"

#include "tmr_flags.h"

void _hwpl_tmr_isr(int port); //This is size optimized
//static void _hwpl_tmr_isr(int port); //This is speed optimized

#define DECLARE_TMR_REGS(regs, port) TIM_TypeDef * regs = _hwpl_tmr_regs_table[port]
TIM_TypeDef * const _hwpl_tmr_regs_table[HWPL_TMR_PORTS] = HWPL_TMR_REGS;

tmr_local_t hwpl_tmr_local[HWPL_TMR_PORTS];

static void tmr_clear_actions(int port);

void tmr_clear_actions(int port){
	int i;
	for(i=0; i < NUM_CHANNELS; i++){
		hwpl_tmr_local[port].callback[i] = NULL;
	}
}

int _hwpl_tmr_dev_port_is_invalid(int port){
	if ( (uint32_t)port >= HWPL_TMR_PORTS ){
		return 1;
	}
	return 0;
}

void _hwpl_tmr_dev_power_on(int port){
	if ( hwpl_tmr_local[port].ref_count == 0 ){
		tmr_clear_actions(port);
		switch(port){
		case 0:
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			NVIC_EnableIRQ(TIM2_IRQn);
			break;
		case 1:
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			NVIC_EnableIRQ(TIM3_IRQn);
			break;
#if HWPL_TMR_PORTS > 2
		case 2:
			RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
			NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
			break;
#endif

#if HWPL_TMR_PORTS > 3
		case 3:
			RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
			NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
			break;
#endif

#if HWPL_TMR_PORTS > 4
		case 4:
			RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
			NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
			break;
#endif

#if HWPL_TMR_PORTS > 5
		case 5:
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
			NVIC_EnableIRQ(TIM4_IRQn);
			break;
#endif
		}
	}
	hwpl_tmr_local[port].ref_count++;

}


void _hwpl_tmr_dev_power_off(int port){
	if ( hwpl_tmr_local[port].ref_count > 0 ){
		if ( hwpl_tmr_local[port].ref_count == 1 ){
			switch(port){
			case 0:
				RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
				NVIC_DisableIRQ(TIM2_IRQn);
				break;
			case 1:
				RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
				NVIC_DisableIRQ(TIM3_IRQn);
				break;
#if HWPL_TMR_PORTS > 2
			case 2:
				RCC->APB2ENR &= ~RCC_APB2ENR_TIM15EN;
				NVIC_DisableIRQ(TIM1_BRK_TIM15_IRQn);
				break;
#endif

#if HWPL_TMR_PORTS > 3
			case 3:
				RCC->APB2ENR &= ~RCC_APB2ENR_TIM16EN;
				NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
				break;
#endif

#if HWPL_TMR_PORTS > 4
			case 4:
				RCC->APB2ENR &= ~RCC_APB2ENR_TIM17EN;
				NVIC_DisableIRQ(TIM1_TRG_COM_TIM17_IRQn);
				break;
#endif

#if HWPL_TMR_PORTS > 5
			case 5:
				RCC->APB1ENR &= ~RCC_APB1ENR_TIM4EN;
				NVIC_DisableIRQ(TIM4_IRQn);
				break;
#endif
			}
		}
		hwpl_tmr_local[port].ref_count--;
	}
}

int _hwpl_tmr_dev_powered_on(int port){
	return ( hwpl_tmr_local[port].ref_count != 0);
}

int hwpl_tmr_getattr(int port, void * ctl){
	errno = ENOTSUP;
	return -1;
}

int hwpl_tmr_setattr(int port, void * ctl){
	tmr_attr_t * attr = ctl;
	uint16_t cr1;
	uint16_t cr2;
	uint16_t fdiv;
	DECLARE_TMR_REGS(regs, port);

	cr1 = 0;
	cr2 = 0;
	fdiv = 0;

	if( (attr->clksrc & TMR_CLKSRC_COUNTDOWN) != 0 ){
		cr1 |= TIM_CR1_DIR;
	}

	//set the clock source
	if( (attr->clksrc & TMR_CLKSRC_MASK) == TMR_CLKSRC_CPU ){
		//set the timer to use internal clock at attr->freq
		if( attr->freq == 0 ){
			errno = EINVAL;
			return -1;
		}
		fdiv = _hwpl_core_getclock() / attr->freq;
		if( fdiv > 0 ){
			fdiv--;
		}
	}

	//configure enabled output compare channels
	switch(port){
	case TIM2_PORT:
		break;
	case TIM3_PORT:
		break;
	case TIM15_PORT:
		break;
	case TIM16_PORT:
		if( attr->enabled_oc_chans & (1<<0) ){
			//enable PB8 for alternate function (TIM16_CH1)
			RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //turn the clock off to the port
			configure_pio_high(GPIOB, 8, MODE_CNF_ALTFUNCTION_PUSHPULL_50MHZ);

		}

		if( attr->enabled_oc_chans & (1<<1) ){
			//enable PB6 for TIM16_CH1N
			RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //turn the clock off to the port
			configure_pio_high(GPIOB, 6, MODE_CNF_ALTFUNCTION_PUSHPULL_50MHZ);
		}


		break;
	case TIM17_PORT:
		break;
	}

	regs->CR1 = cr1;
	regs->CR2 = cr2;
	regs->PSC = fdiv;

	return 0;

}


int hwpl_tmr_on(int port, void * ctl){
	DECLARE_TMR_REGS(regs, port);
	regs->CR1 |= TIM_CR1_CEN;
	return 0;
}

int hwpl_tmr_off(int port, void * ctl){
	DECLARE_TMR_REGS(regs, port);
	regs->CR1 &= ~TIM_CR1_CEN;
	return 0;
}

int _hwpl_tmr_dev_read(const device_cfg_t * cfg, device_transfer_t * rop){
	int port = DEVICE_GET_PORT(cfg);
	int chan = rop->loc;
	hwpl_tmr_local[port].callback[chan] = rop->callback;
	hwpl_tmr_local[port].context[chan] = rop->context;
	return 0;
}

int hwpl_tmr_set(int port, void * ctl){
	DECLARE_TMR_REGS(regs, port);
	regs->CNT = (uint32_t)ctl;
	return 0;
}

int hwpl_tmr_get(int port, void * ctl){
	DECLARE_TMR_REGS(regs, port);
	return regs->CNT;
}


int hwpl_tmr_setoc(int port, void * ctl){
	tmr_reqattr_t * req = ctl;
	uint32_t * ptr;
	DECLARE_TMR_REGS(regs, port);
	ptr = (uint32_t *)&(regs->CCR1);
	ptr[req->channel] = (req->value & 0xFFFF);
	return 0;
}

int hwpl_tmr_getoc(int port, void * ctl){
	tmr_reqattr_t * req = ctl;
	uint32_t * ptr;
	DECLARE_TMR_REGS(regs, port);
	ptr = (uint32_t *)&(regs->CCR1);
	return (ptr[req->channel] & 0xFFFF);
}


int hwpl_tmr_setic(int port, void * ctl){
	return hwpl_tmr_setoc(port, ctl);
}

int hwpl_tmr_getic(int port, void * ctl){
	return hwpl_tmr_getoc(port, ctl);
}

int hwpl_tmr_setaction(int port, void * ctl){
	tmr_action_t * action = ctl;
	DECLARE_TMR_REGS(regs, port);


	if( action->event == TMR_ACTION_EVENT_NONE ){
		regs->CCER = 0; //disable all actions
		return 0;
	}

	if( action->event & TMR_ACTION_EVENT_TOGGLEOC ){
		regs->CCMR1 = TIM_CCMR1_OC1M_0|TIM_CCMR1_OC1M_1;
		regs->CCER |= TIM_CCER_CC1E;
	}

	return 0;
}



void _hwpl_tmr_isr(int port){
	DECLARE_TMR_REGS(regs, port);
	uint32_t status;
	//if available execute the callback on the active channel
	int i;
	status = regs->SR;
	for(i=0; i < NUM_CHANNELS; i++){
		if( status & (TIM_SR_CC1IF<<i) ){
			if( hwpl_tmr_local[port].callback[i] != NULL ){
				if( hwpl_tmr_local[port].callback[i](hwpl_tmr_local[port].context[i], NULL) == 0 ){
					hwpl_tmr_local[port].callback[i] = NULL;
				}
			}
		}
	}

	if( status & (TIM_SR_UIF) ){
		if( hwpl_tmr_local[port].overflow_callback != NULL ){
			if( hwpl_tmr_local[port].overflow_callback(hwpl_tmr_local[port].overflow_context, NULL) == 0 ){
				hwpl_tmr_local[port].overflow_callback = NULL;
			}
		}
	}
}

void _hwpl_core_tmr2_isr(void){
	_hwpl_tmr_isr(TIM2_PORT);
}

void _hwpl_core_tmr3_isr(void){
	_hwpl_tmr_isr(TIM3_PORT);
}

#if HWPL_TMR_PORTS  > TIM15_PORT
//TIM15
void core_tmr1brk_isr(void){
	_hwpl_tmr_isr(TIM15_PORT);
}
#endif

#if HWPL_TMR_PORTS  > TIM16_PORT
//TIM16
void core_tmr1up_isr(void){
	_hwpl_tmr_isr(TIM16_PORT);
}
#endif

#if HWPL_TMR_PORTS  > TIM17_PORT
//TIM17
void core_tmr1trigcom_isr(void){
	_hwpl_tmr_isr(TIM17_PORT);
}
#endif

#if HWPL_TMR_PORTS  > TIM4_PORT
void core_tmr4_isr(void){
	_hwpl_tmr_isr(TIM4_PORT);
}
#endif

#if HWPL_TMR_PORTS  > TIM5_PORT
void core_tmr5_isr(void){
	_hwpl_tmr_isr(TIM5_PORT);
}
#endif

#if HWPL_TMR_PORTS  > TIM6_PORT
void core_tmr6_isr(void){
	_hwpl_tmr_isr(TIM6_PORT);
}
#endif

#if HWPL_TMR_PORTS  > TIM7_PORT
void core_tmr7_isr(void){
	_hwpl_tmr_isr(TIM7_PORT);
}
#endif



