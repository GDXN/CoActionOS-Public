/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwpl/debug.h"
#include "gpio_flags.h"


typedef struct {
	hwpl_callback_t callback;
	void * context;
	uint8_t ref_count;
} gpio_local_t;

static gpio_local_t gpio_local[HWPL_GPIO_PORTS] HWPL_SYS_MEM;

int _hwpl_gpio_dev_port_is_invalid(int port){
	if ( (uint32_t)port > HWPL_GPIO_PORTS ){
		return 1;
	}
	return 0;
}

void _hwpl_gpio_dev_power_on(int port){
	if ( gpio_local[port].ref_count == 0 ){
		gpio_local[port].callback = NULL;
	}
	gpio_local[port].ref_count++;

	hwpl_gpio_poweron(port); //turn the clock on to the port
}

void _hwpl_gpio_dev_power_off(int port){
	if ( gpio_local[port].ref_count > 0 ){
		if ( gpio_local[port].ref_count == 1 ){
			gpio_local[port].callback = NULL;
			RCC->APB2ENR &= ~(1<<(RCC_APB2ENR_IOPAEN + port)); //turn the clock off to the port
		}
		gpio_local[port].ref_count--;
	}
}

int _hwpl_gpio_dev_powered_on(int port){
	if ( gpio_local[port].ref_count > 0 ){
		return 1;
	}
	return 0;
}

int gpio_set_event(int port, int event, int pin){
	errno = ENOTSUP;
	return -1;
}


int hwpl_gpio_setaction(int port, void * ctl){
	errno = ENOTSUP;
	return -1;
}

void gpio_set_pinmode(GPIO_TypeDef * regs, int pin, uint8_t mode, int open_drain){
	if( pin < 8 ){
		configure_gpio_low(regs, pin, mode);
	} else {
		configure_gpio_high(regs, pin, mode);
	}
}

int hwpl_gpio_setpinmode(int port, void * ctl){
	int i;
	int mode;
	gpio_pinmode_req_t * pinmode;
	pinmode = (gpio_pinmode_req_t*)ctl;
	int pull;
	DECLARE_REGS(regs, port);

	pull = 0;
	switch(pinmode->mode){
	case GPIO_PINMODE_PULLUP:
		mode = MODE_CNF_INPUT_PULLUP_DOWN;
		pull = 1;
		break;
	case GPIO_PINMODE_FLOAT:
		mode = MODE_CNF_INPUT_FLOATING;
		break;
	case GPIO_PINMODE_PULLDOWN:
		mode = MODE_CNF_INPUT_PULLUP_DOWN;
		pull = 1;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	for(i = 0; i < 8*sizeof(gpio_sample_t); i++){
		if ( (1<<i) & pinmode->mask ){
			gpio_set_pinmode(regs, i, mode, pinmode->opendrain);
			if( pull ){
				if( pinmode->mode == GPIO_PINMODE_PULLUP ){
					regs->BSRR = (1<<i);
				} else {
					regs->BRR = (1<<i);
				}
			}
		}
	}

	return 0;
}

//The gpio interrupts use the eint3 interrupt service routine -- this function should be called from there
void _hwpl_core_gpio_isr(void){

}

