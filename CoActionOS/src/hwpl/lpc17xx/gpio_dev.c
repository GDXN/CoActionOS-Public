/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwpl/debug.h"
#include "hwpl/gpio.h"
#include "gpio_flags.h"
#include "hwpl/core.h"


typedef struct {
	hwpl_callback_t callback;
	void * context;
	uint8_t ref_count;
} gpio_local_t;

static gpio_local_t gpio0_local HWPL_SYS_MEM;
static gpio_local_t gpio2_local HWPL_SYS_MEM;
static int gpio_set_event(int port, int event, int pin);


void _hwpl_gpio_dev_power_on(int port){

	if ( port == 0 ){
		if ( gpio0_local.ref_count == 0 ){
			gpio0_local.callback = NULL;
		}
		gpio0_local.ref_count++;
	} else if ( port == 2 ){
		if ( gpio2_local.ref_count == 0 ){
			gpio2_local.callback = NULL;
		}
		gpio2_local.ref_count++;
	}
}

void _hwpl_gpio_dev_power_off(int port){

	if ( port == 0 ){
		if ( gpio0_local.ref_count > 0 ){
			if ( gpio0_local.ref_count == 1 ){
				gpio0_local.callback = NULL;
			}
			gpio0_local.ref_count--;
		}
	} else if ( port == 2 ){
		if ( gpio2_local.ref_count > 0 ){
			if ( gpio2_local.ref_count == 1 ){
				gpio2_local.callback = NULL;
			}
			gpio2_local.ref_count--;
		}
	}
}

int _hwpl_gpio_dev_powered_on(int port){
	if ( port == 0 ){
		if ( gpio0_local.ref_count > 0 ){
			return 1;
		} else {
			return 0;
		}
	} else if ( port == 2 ){
		if ( gpio2_local.ref_count > 0 ){
			return 1;
		} else {
			return 0;
		}
	} else {
		return 1;
	}
}

int gpio_set_event(int port, int event, int pin){

	if ( port == 0 ){

		if ( (pin > 30) ||
				(pin == 12) ||
				(pin == 13) ||
				(pin == 14) ){
			return -1;
		}

		switch(event){
		case GPIO_ACTION_EVENT_NONE:
			LPC_GPIOINT->IO0IntEnR &= ~(1<<pin);
			LPC_GPIOINT->IO0IntEnF &= ~(1<<pin);
			break;
		case GPIO_ACTION_EVENT_RISING:
			LPC_GPIOINT->IO0IntEnR |= (1<<pin);
			LPC_GPIOINT->IO0IntEnF &= ~(1<<pin);
			break;
		case GPIO_ACTION_EVENT_FALLING:
			LPC_GPIOINT->IO0IntEnR &= ~(1<<pin);
			LPC_GPIOINT->IO0IntEnF |= (1<<pin);
			break;
		case GPIO_ACTION_EVENT_BOTH:
			LPC_GPIOINT->IO0IntEnR |= (1<<pin);
			LPC_GPIOINT->IO0IntEnF |= (1<<pin);
			break;
		default:
			errno = EINVAL;
			return -1;
		}

	} else if ( port == 2 ){

		if ( (pin > 13) ){
			errno = EINVAL;
			return -1;
		}

		switch(event){
		case GPIO_ACTION_EVENT_NONE:
			LPC_GPIOINT->IO2IntEnR &= ~(1<<pin);
			LPC_GPIOINT->IO2IntEnF &= ~(1<<pin);
			break;
		case GPIO_ACTION_EVENT_RISING:
			LPC_GPIOINT->IO2IntEnR |= (1<<pin);
			LPC_GPIOINT->IO2IntEnF &= ~(1<<pin);
			break;
		case GPIO_ACTION_EVENT_FALLING:
			LPC_GPIOINT->IO2IntEnR &= ~(1<<pin);
			LPC_GPIOINT->IO2IntEnF |= (1<<pin);
			break;
		case GPIO_ACTION_EVENT_BOTH:
			LPC_GPIOINT->IO2IntEnR |= (1<<pin);
			LPC_GPIOINT->IO2IntEnF |= (1<<pin);
			break;
		default:
			errno = EINVAL;
			return -1;
		}
	}
	return 0;
}


int hwpl_gpio_setaction(int port, void * ctl){
	int err;
	hwpl_action_t * action = (hwpl_action_t*)ctl;

	err = gpio_set_event(port, action->event, action->channel);
	if ( err ){
		return err;
	}

	if ( port == 0 ){
		gpio0_local.callback = action->callback;
		gpio0_local.context = action->context;
	} else if ( port == 2 ){
		gpio2_local.callback = action->callback;
		gpio2_local.context = action->context;
	} else {
		errno = EINVAL;
		return -1;
	}

	//This is the interrupt for GPIO0 and GPIO2
	_hwpl_core_priv_enable_irq((void*)EINT3_IRQn);

	return 0;
}

void gpio_set_pinmode(int port, int pin, int mode, int open_drain){
	switch(port){
	case 0:
		if ( open_drain ){
			LPC_PINCON->PINMODE_OD0 |= (1<<pin);
		} else {
			LPC_PINCON->PINMODE_OD0 &= ~(1<<pin);
		}
		break;
	case 1:
		if ( open_drain ){
			LPC_PINCON->PINMODE_OD1 |= (1<<pin);
		} else {
			LPC_PINCON->PINMODE_OD1 &= ~(1<<pin);
		}
		break;
	case 2:
		if ( open_drain ){
			LPC_PINCON->PINMODE_OD2 |= (1<<pin);
		} else {
			LPC_PINCON->PINMODE_OD2 &= ~(1<<pin);
		}
		break;
	case 3:
		if ( open_drain ){
			LPC_PINCON->PINMODE_OD3 |= (1<<pin);
		} else {
			LPC_PINCON->PINMODE_OD3 &= ~(1<<pin);
		}
		break;
	case 4:
		if ( open_drain ){
			LPC_PINCON->PINMODE_OD4 |= (1<<pin);
		} else {
			LPC_PINCON->PINMODE_OD4 &= ~(1<<pin);
		}
		break;
	}

	HWPL_CLR_MASK( ((uint32_t*)(&(LPC_PINCON->PINMODE0)))[port*2 + pin/16], (3<<(pin < 16 ? pin*2 : ((pin-16)*2))) );
	HWPL_SET_MASK( ((uint32_t*)(&(LPC_PINCON->PINMODE0)))[port*2 + pin/16], (mode<<(pin < 16 ? pin*2 : ((pin-16)*2)) ) );
}

int hwpl_gpio_setpinmode(int port, void * ctl){
	int i;
	int mode;
	gpio_pinmode_req_t * pinmode;
	pinmode = (gpio_pinmode_req_t*)ctl;

	switch(pinmode->mode){
	case GPIO_PINMODE_PULLUP:
		mode = 0;
		break;
	case GPIO_PINMODE_REPEATER:
		mode = 1;
		break;
	case GPIO_PINMODE_FLOAT:
		mode = 2;
		break;
	case GPIO_PINMODE_PULLDOWN:
		mode = 3;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	for(i = 0; i < 8*sizeof(gpio_sample_t); i++){
		if ( (1<<i) & pinmode->mask ){
			_hwpl_core_set_pinsel_func(port, i, CORE_PERIPH_GPIO, 0); //set the pin to use GPIO
			gpio_set_pinmode(port, i, mode, pinmode->opendrain);
		}
	}

	return 0;
}

//The gpio interrupts use the eint3 interrupt service routine -- this function should be called from there
void _hwpl_core_gpio_isr(void){
	uint32_t tmp;
	if ( LPC_GPIOINT->IntStatus & (1<<0) ){
		tmp = LPC_GPIOINT->IO0IntStatR | LPC_GPIOINT->IO0IntStatF;
		if ( gpio0_local.callback != NULL ){
			if( gpio0_local.callback(gpio0_local.context, (const void*)tmp) == 0 ){
				gpio0_local.callback = NULL;
			}
		}
		LPC_GPIOINT->IO0IntClr = tmp;
	}

	if ( LPC_GPIOINT->IntStatus & (1<<2) ){
		tmp = LPC_GPIOINT->IO2IntStatR | LPC_GPIOINT->IO2IntStatF;
		if ( gpio2_local.callback != NULL ){
			if( gpio2_local.callback(gpio2_local.context, (const void*)tmp) == 0 ){
				gpio2_local.callback = NULL;
			}
		}
		LPC_GPIOINT->IO0IntClr = tmp;
	}

}

