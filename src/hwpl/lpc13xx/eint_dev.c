/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <stddef.h>
#include "hwpl/debug.h"
#include "hwpl/eint.h"
#include "hwpl/pio.h"
#include "hwpl/core.h"

typedef struct HWPL_PACK {
	hwpl_callback_t callback;
	void * context;
	uint8_t ref_count;
} eint_local_t;

static eint_local_t _hwpl_eint_local[HWPL_EINT_PORTS] HWPL_SYS_MEM;
static int _hwpl_eint_set_event(int port, int event);

void _hwpl_eint_dev_power_on(int port){
	if ( _hwpl_eint_local[port].ref_count == 0 ){
		_hwpl_eint_local[port].callback = NULL;
	}
	_hwpl_eint_local[port].ref_count++;
}

void _hwpl_eint_dev_power_off(int port){
	if ( _hwpl_eint_local[port].ref_count > 0 ){
		if ( _hwpl_eint_local[port].ref_count == 1 ){
			_hwpl_eint_local[port].callback = NULL;
		}
		_hwpl_eint_local[port].ref_count--;
	}
}

int _hwpl_eint_dev_powered_on(int port){
	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_GPIO;

	if ( _hwpl_eint_local[port].ref_count > 0 ){
		return 1;
	} else {
		return 0;
	}
}

int _hwpl_eint_set_event(int port, int event){
	uint32_t mask;
	volatile uint32_t * start_logic_edge;
	volatile uint32_t * start_logic_enable;
	volatile uint32_t * start_logic_clear;

	if( port > 31 ){
		port = port - 32;
		start_logic_edge = &(LPC_SYSCON->STARTAPRP1);
		start_logic_enable = &(LPC_SYSCON->STARTERP1);
		start_logic_clear = &(LPC_SYSCON->STARTRSRP1CLR);
	} else {
		start_logic_edge = &(LPC_SYSCON->STARTAPRP0);
		start_logic_enable = &(LPC_SYSCON->STARTERP0);
		start_logic_clear = &(LPC_SYSCON->STARTRSRP0CLR);
	}

	mask = (1<<port);


	//turn interrupt off
	*start_logic_enable &= ~(mask);

	switch(event){
	case EINT_ACTION_EVENT_UNCONFIGURED:
		return 0;
	case EINT_ACTION_EVENT_RISING:
		*start_logic_edge |= mask;
		break;
	case EINT_ACTION_EVENT_FALLING:
		*start_logic_edge &= ~mask;
		break;
	case EINT_ACTION_EVENT_BOTH:
	default:
		errno = EINVAL;
		return -1;
	}

	*start_logic_enable |= (mask);
	*start_logic_clear |= mask;
	return 0;
}

int _hwpl_eint_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	int port;
	hwpl_action_t * action;
	port = cfg->periph.port;

	if( wop->nbyte != sizeof(hwpl_action_t) ){
		errno = EINVAL;
		return -1;
	}

	action = wop->buf;
	action->callback = wop->callback;
	action->context = wop->context;
	return hwpl_eint_setaction(port, &action);
}


int hwpl_eint_setaction(int port, void * ctl){
	int err;
	hwpl_action_t * action = (hwpl_action_t*)ctl;

	err = _hwpl_eint_set_event(port, action->event);
	if ( err ){
		return err;
	}

	_hwpl_eint_local[port].callback = action->callback;
	_hwpl_eint_local[port].context = action->context;

	//set the interrupt on the wakeup pins
	_hwpl_core_priv_enable_irq((void*)(WAKEUP0_IRQn + port));

	return 0;
}


int hwpl_eint_getattr(int port, void * ctl){
	//read the direction pin status
	int pio_port, pinmask;
	pio_sample_t p;
	eint_attr_t * attr = (eint_attr_t*)ctl;
	pio_port = port / 12;
	pinmask = 1<< (port % 12);
	p = hwpl_pio_get(pio_port, 0);
	attr->pin_assign = 0;
	attr->value = ((p & (pinmask)) == pinmask);
	return 0;
}

int hwpl_eint_setattr(int port, void * ctl){
	return 0;
}

void _hwpl_core_eint_isr(int port){
	volatile uint32_t * start_logic_clear;
	uint32_t mask;

	//clear the interrupt flag
	if( port > 31 ){
		port = port - 32;
		start_logic_clear = &(LPC_SYSCON->STARTRSRP1CLR);
	} else {
		start_logic_clear = &(LPC_SYSCON->STARTRSRP0CLR);
	}

	mask = (1<<port);
	*start_logic_clear |= mask;

	if( _hwpl_eint_local[port].callback != 0 ){
		if( _hwpl_eint_local[port].callback(_hwpl_eint_local[port].context, 0) == 0 ){
			_hwpl_eint_local[port].callback = 0;
		}
	}

}


void _hwpl_core_eint0_isr(void){ _hwpl_core_eint_isr(0); }
void _hwpl_core_eint1_isr(void){ _hwpl_core_eint_isr(1); }
void _hwpl_core_eint2_isr(void){ _hwpl_core_eint_isr(2); }
void _hwpl_core_eint3_isr(void){ _hwpl_core_eint_isr(3); }
void _hwpl_core_eint4_isr(void){ _hwpl_core_eint_isr(4); }
void _hwpl_core_eint5_isr(void){ _hwpl_core_eint_isr(5); }
void _hwpl_core_eint6_isr(void){ _hwpl_core_eint_isr(6); }
void _hwpl_core_eint7_isr(void){ _hwpl_core_eint_isr(7); }
void _hwpl_core_eint8_isr(void){ _hwpl_core_eint_isr(8); }
void _hwpl_core_eint9_isr(void){ _hwpl_core_eint_isr(9); }
void _hwpl_core_eint10_isr(void){ _hwpl_core_eint_isr(10); }
void _hwpl_core_eint11_isr(void){ _hwpl_core_eint_isr(11); }

void _hwpl_core_eint12_isr(void){ _hwpl_core_eint_isr(12); }
void _hwpl_core_eint13_isr(void){ _hwpl_core_eint_isr(13); }
void _hwpl_core_eint14_isr(void){ _hwpl_core_eint_isr(14); }
void _hwpl_core_eint15_isr(void){ _hwpl_core_eint_isr(15); }
void _hwpl_core_eint16_isr(void){ _hwpl_core_eint_isr(16); }
void _hwpl_core_eint17_isr(void){ _hwpl_core_eint_isr(17); }
void _hwpl_core_eint18_isr(void){ _hwpl_core_eint_isr(18); }
void _hwpl_core_eint19_isr(void){ _hwpl_core_eint_isr(19); }
void _hwpl_core_eint20_isr(void){ _hwpl_core_eint_isr(20); }
void _hwpl_core_eint21_isr(void){ _hwpl_core_eint_isr(21); }
void _hwpl_core_eint22_isr(void){ _hwpl_core_eint_isr(22); }
void _hwpl_core_eint23_isr(void){ _hwpl_core_eint_isr(23); }

void _hwpl_core_eint24_isr(void){ _hwpl_core_eint_isr(24); }
void _hwpl_core_eint25_isr(void){ _hwpl_core_eint_isr(25); }
void _hwpl_core_eint26_isr(void){ _hwpl_core_eint_isr(26); }
void _hwpl_core_eint27_isr(void){ _hwpl_core_eint_isr(27); }
void _hwpl_core_eint28_isr(void){ _hwpl_core_eint_isr(28); }
void _hwpl_core_eint29_isr(void){ _hwpl_core_eint_isr(29); }
void _hwpl_core_eint30_isr(void){ _hwpl_core_eint_isr(30); }
void _hwpl_core_eint31_isr(void){ _hwpl_core_eint_isr(31); }
void _hwpl_core_eint32_isr(void){ _hwpl_core_eint_isr(32); }
void _hwpl_core_eint33_isr(void){ _hwpl_core_eint_isr(33); }
void _hwpl_core_eint34_isr(void){ _hwpl_core_eint_isr(34); }
void _hwpl_core_eint35_isr(void){ _hwpl_core_eint_isr(35); }

void _hwpl_core_eint36_isr(void){ _hwpl_core_eint_isr(36); }
void _hwpl_core_eint37_isr(void){ _hwpl_core_eint_isr(37); }
void _hwpl_core_eint38_isr(void){ _hwpl_core_eint_isr(38); }
void _hwpl_core_eint39_isr(void){ _hwpl_core_eint_isr(39); }


