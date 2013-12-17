/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "pio_flags.h"
#include "hwpl/eint.h"
#include "hwpl/core.h"

#include "hwpl/debug.h"

typedef struct {
	hwpl_callback_t callback;
	void * context;
	uint8_t ref_count;
} eint_local_t;

static eint_local_t eint_local[HWPL_EINT_PORTS] HWPL_SYS_MEM;
static void _hwpl_eint_enable_interrupt(int port);
static void _hwpl_eint_disable_interrupt(int port);

void _hwpl_eint_dev_power_on(int port){
	if ( eint_local[port].ref_count == 0 ){
		eint_local[port].ref_count++;
		_hwpl_eint_enable_interrupt(port);
	}
	eint_local[port].ref_count++;
}

void _hwpl_eint_dev_power_off(int port){
	if ( eint_local[port].ref_count > 0 ){
		if ( eint_local[port].ref_count == 1 ){
			_hwpl_eint_disable_interrupt(port);
		}
		eint_local[port].ref_count--;
	}
}

int _hwpl_eint_dev_powered_on(int port){
	return eint_local[port].ref_count;
}

int hwpl_eint_setaction(int port, void * ctl){
	hwpl_action_t * action = (hwpl_action_t*)ctl;
	eint_local[port].callback = action->callback;
	eint_local[port].context = action->context;

	EXTI->IMR &= ~(1<<port);
	EXTI->RTSR &= ~(1<<port);
	EXTI->FTSR &= ~(1<<port);
	switch(action->event){
	case EINT_ACTION_EVENT_UNCONFIGURED:
		return 0;
	case EINT_ACTION_EVENT_RISING:
		EXTI->RTSR |= (1<<port);
		break;
	case EINT_ACTION_EVENT_FALLING:
		EXTI->FTSR |= (1<<port);
		break;
	case EINT_ACTION_EVENT_BOTH:
		EXTI->RTSR |= (1<<port);
		EXTI->FTSR |= (1<<port);
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	EXTI->IMR |= (1<<port);
	return 0;
}

int hwpl_eint_getattr(int port, void * ctl){
	errno = ENOTSUP;
	return -1;
}

int hwpl_eint_setattr(int port, void * ctl){
	int index;
	uint16_t shift;
	eint_attr_t * attr = ctl;
	//set the PIO configuration
	if( attr->pin_assign >= HWPL_PIO_PORTS ){
		errno = EINVAL;
		return -1;
	}

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	index = port >> 2;
	shift = (port & 0x03) << 2;
	AFIO->EXTICR[index] &= ~(0xF << shift);
	AFIO->EXTICR[index] |= (attr->pin_assign << shift);
	RCC->APB2ENR &= ~RCC_APB2ENR_AFIOEN;

	return 0;
}

void _hwpl_eint_isr(int port){
	if ( eint_local[port].callback != NULL ){  //If there is a callback ready, then call it
		if( eint_local[port].callback(eint_local[port].context, 0) == 0 ){
			eint_local[port].callback = NULL;
		}
	}
	EXTI->PR |= (1<<port); //clear the pending flag
}

void _hwpl_core_eint0_isr(void){
	_hwpl_eint_isr(0);
}

void _hwpl_core_eint1_isr(void){
	_hwpl_eint_isr(1);
}

void _hwpl_core_eint2_isr(void){
	_hwpl_eint_isr(2);
}

void _hwpl_core_eint3_isr(void){
	_hwpl_eint_isr(3);
}

void _hwpl_core_eint4_isr(void){
	_hwpl_eint_isr(4);
}

void _hwpl_core_eint59_isr(void){
	//decode interrupt
	int i;
	for(i=5; i < 10; i++){
		if( EXTI->PR & (1<<i) ){
			_hwpl_eint_isr(i);
		}
	}
}


void _hwpl_core_eint1015_isr(void){
	int i;
	for(i=10; i < 16; i++){
		if( EXTI->PR & (1<<i) ){
			_hwpl_eint_isr(i);
		}
	}
}

void _hwpl_eint_enable_interrupt(int port){
	switch(port){
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		NVIC_EnableIRQ(EXTI0_IRQn + port);
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		NVIC_EnableIRQ(EXTI9_5_IRQn);
		break;
	default:
		NVIC_EnableIRQ(EXTI15_10_IRQn);
		break;
	}
}
void _hwpl_eint_disable_interrupt(int port){
	switch(port){
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		NVIC_DisableIRQ(EXTI0_IRQn + port);
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		NVIC_DisableIRQ(EXTI9_5_IRQn);
		break;
	default:
		NVIC_DisableIRQ(EXTI15_10_IRQn);
		break;
	}
}




