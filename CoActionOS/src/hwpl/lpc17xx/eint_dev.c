/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwpl/eint.h"
#include "hwpl/core.h"

#include "hwpl/debug.h"

typedef struct {
	hwpl_callback_t callback;
	void * context;
	uint8_t ref_count;
} eint_local_t;
static eint_local_t eint_local[4] HWPL_SYS_MEM;


//static eint_action_event_t get_event(int port);
static int set_event(int port, eint_action_event_t event);
static void reset_eint_port(int port);
static void exec_callback(int port, void * data);


void _hwpl_eint_dev_power_on(int port){
	if ( eint_local[port].ref_count == 0 ){
		eint_local[port].ref_count++;
		hwpl_eint_setaction(port, NULL);
		reset_eint_port(port);
	}
	eint_local[port].ref_count++;
}

void _hwpl_eint_dev_power_off(int port){
	if ( eint_local[port].ref_count > 0 ){
		if ( eint_local[port].ref_count == 1 ){
			hwpl_eint_setaction(port, NULL);
			reset_eint_port(port);
		}
		eint_local[port].ref_count--;
	}
}

int _hwpl_eint_dev_powered_on(int port){
	return eint_local[port].ref_count;
}

int _hwpl_eint_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	int port;
	hwpl_action_t * action;
	port = cfg->periph.port;
	if ( eint_local[port].callback != 0 ){
		//The interrupt is on -- port is busy
		errno = EAGAIN;
		return -1;
	}
	if( wop->nbyte != sizeof(hwpl_action_t) ){
		errno = EINVAL;
		return -1;
	}

	action = (hwpl_action_t*)wop->buf;
	action->callback = wop->callback;
	action->context = wop->context;
	return hwpl_eint_setaction(port, action);
}

int hwpl_eint_setaction(int port, void * ctl){
	hwpl_action_t * action = (hwpl_action_t*)ctl;

	if( action->callback == 0 ){
		exec_callback(port, DEVICE_OP_CANCELLED);
	}

	eint_local[port].callback = action->callback;
	eint_local[port].context = action->context;
	set_event(port, action->event);
	return 0;
}

int hwpl_eint_getattr(int port, void * ctl){
	eint_attr_t * ctlp;
	ctlp = (eint_attr_t *)ctl;
	ctlp->pin_assign = 0;

#ifdef __lpc17xx
	switch(port){
	case 0:
		ctlp->value = ((LPC_GPIO2->PIN & (1<<10)) == (1<<10));
		break;
	case 1:
		ctlp->value = ((LPC_GPIO2->PIN & (1<<11)) == (1<<11));
		break;
	case 2:
		ctlp->value = ((LPC_GPIO2->PIN & (1<<12)) == (1<<12));
		break;
	case 3:
		ctlp->value = ((LPC_GPIO2->PIN & (1<<13)) == (1<<13));
		break;
	}
#endif

	return 0;
}

int hwpl_eint_setattr(int port, void * ctl){
	eint_attr_t * ctlp;

	ctlp = (eint_attr_t *)ctl;

	if ( ctlp->pin_assign != 0 ){
		errno = EINVAL;
		return -1 - offsetof(eint_attr_t, pin_assign);
	}

	reset_eint_port(port);

	switch(port){
	case 0:
		_hwpl_core_set_pinsel_func(2,10,CORE_PERIPH_EINT,0);
		break;
	case 1:
		_hwpl_core_set_pinsel_func(2,11,CORE_PERIPH_EINT,1);
		break;
	case 2:
		_hwpl_core_set_pinsel_func(2,12,CORE_PERIPH_EINT,2);
		break;
	case 3:
		_hwpl_core_set_pinsel_func(2,13,CORE_PERIPH_EINT,3);
		break;
	}

	return 0;
}

void reset_eint_port(int port){
	_hwpl_core_priv_disable_irq((void*)(EINT0_IRQn + port));

	eint_local[port].callback = 0;

	LPC_SC->EXTPOLAR &= ~(1<<port);
	LPC_SC->EXTMODE &= ~(1<<port);

	switch(port){
	case 0:
		_hwpl_core_set_pinsel_func(2,10,CORE_PERIPH_GPIO,2);
		break;
	case 1:
		_hwpl_core_set_pinsel_func(2,11,CORE_PERIPH_GPIO,2);
		break;
	case 2:
		_hwpl_core_set_pinsel_func(2,12,CORE_PERIPH_GPIO,2);
		break;
	case 3:
		_hwpl_core_set_pinsel_func(2,13,CORE_PERIPH_GPIO,2);
		break;
	}

	LPC_SC->EXTINT |= (1<<port); //Clear the interrupt flag
}

int set_event(int port, eint_action_event_t event){
	int err;
	err = 0;

	_hwpl_core_priv_disable_irq((void*)(EINT0_IRQn + port));

	LPC_SC->EXTPOLAR &= ~(1<<port);
	LPC_SC->EXTMODE &= ~(1<<port);
	switch(event){
	case EINT_ACTION_EVENT_RISING:
		LPC_SC->EXTPOLAR |= (1<<port);
		LPC_SC->EXTMODE |= (1<<port);
		break;
	case EINT_ACTION_EVENT_FALLING:
		LPC_SC->EXTMODE |= (1<<port);
		break;
	case EINT_ACTION_EVENT_HIGH:
		LPC_SC->EXTPOLAR |= (1<<port);
		break;
	case EINT_ACTION_EVENT_LOW:
		break;
	case EINT_ACTION_EVENT_BOTH:
	default:
		errno = EINVAL;
		err = -1;
		break;
	}
	if ( err == 0 ){
		LPC_SC->EXTINT |= (1<<port); //Clear the interrupt flag
	}

	_hwpl_core_priv_enable_irq((void*)EINT0_IRQn + port);
	return err;
}

/*
eint_action_event_t get_event(int port){
	uint32_t polar;
	uint32_t mode;

	polar = LPC_SC->EXTPOLAR;
	mode = LPC_SC->EXTMODE;

	if ( polar & (1<<port) ){
		//High or rising
		if ( mode & (1<<port) ){
			//edge sensitive
			return EINT_ACTION_EVENT_RISING;
		} else {
			return EINT_ACTION_EVENT_HIGH;
		}
	} else {
		//low or falling
		if ( mode & (1<<port) ){
			//edge sensitive
			return EINT_ACTION_EVENT_FALLING;
		} else {
			return EINT_ACTION_EVENT_LOW;
		}
	}
}
 */

void exec_callback(int port, void * data){
	if ( eint_local[port].callback != NULL ){  //If there is a callback ready, then call it
		if( eint_local[port].callback(eint_local[port].context, data) == 0 ){
			eint_local[port].callback = NULL;
		}
	}
}

//Interrupt Handling
void _hwpl_eint_isr(int port){
	exec_callback(port, 0);
	LPC_SC->EXTINT |= (1<<port); //Clear the interrupt flag
}

extern void _hwpl_core_pio_isr(void);

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
	//check for GPIO interrupts first
	if ( LPC_GPIOINT->IntStatus ){
		_hwpl_core_pio_isr();
	}

	if ( LPC_SC->EXTINT & (1<<3)){
		_hwpl_eint_isr(3);
	}
}
