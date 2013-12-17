/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl/debug.h"
#include "hwpl/gpio.h"
#include "gpio_flags.h"


static void gpio_set_dir(int port, gpio_sample_t mask);
static void gpio_clr_dir(int port, gpio_sample_t mask);

GPIO_TypeDef * const hwpl_gpio_regs[HWPL_GPIO_PORTS] = HWPL_GPIO_REGS;

void hwpl_gpio_poweron(int port){
	RCC->APB2ENR |= (1<<(2 + port));
}


void gpio_set_dir(int port, gpio_sample_t mask){
	int i;
	uint32_t clrmask;
	DECLARE_REGS(regs, port);
	for(i=0; i < 8; i++){
		if( mask & (1<<i) ){
			configure_gpio_low(regs, i, MODE_CNF_OUTPUT_PUSHPULL_50MHZ);
		}
	}

	for(i=8; i < 16; i++){
		if ( mask & (1<<i) ){
			configure_gpio_high(regs, i, MODE_CNF_OUTPUT_PUSHPULL_50MHZ);
		}
	}
}

void gpio_clr_dir(int port, gpio_sample_t mask){
	int i;
	uint32_t clrmask;
	DECLARE_REGS(regs, port);
	for(i=0; i < 8; i++){
		if( mask & (1<<i) ){
			configure_gpio_low(regs, i, MODE_CNF_INPUT_FLOATING);
		}
	}

	for(i=8; i < 16; i++){
		if ( mask & (1<<i) ){
			configure_gpio_high(regs, i, MODE_CNF_INPUT_FLOATING);
		}
	}
}

static gpio_sample_t gpio_get_dir(int port){
	int i;
	gpio_sample_t dir;
	DECLARE_REGS(regs, port);
	dir = 0;
	for(i = 0; i < 8; i++){
		if( regs->CRL & (0x03<<(i*4)) != 0 ){
			dir |= (1<<i);
		}
	}

	for(i = 0; i < 8; i++){
		if( regs->CRL & (0x03<<(i*4)) != 0 ){
			dir |= (1<<(i+8));
		}
	}
	return dir;
}



int hwpl_gpio_getattr(int port, void * ctl){
	gpio_attr_t * ctlp = (gpio_attr_t*)ctl;
	//read the direction pin status
	ctlp->output = gpio_get_dir(port);
	ctlp->input = ~ctlp->output;
	return 0;
}

int hwpl_gpio_setattr(int port, void * ctl){
	gpio_attr_t * ctlp = (gpio_attr_t*)ctl;

	//set the input pins as input
	gpio_clr_dir(port, ctlp->input);

	//set output pins as output
	gpio_set_dir(port, ctlp->output);

	return 0;
}

int hwpl_gpio_setmask(int port, void * ctl){
	gpio_sample_t mask;
	DECLARE_REGS(regs, port);
	mask = (gpio_sample_t)(int)ctl;
	regs->BSRR = mask;
	return 0;
}

int hwpl_gpio_clrmask(int port, void * ctl){
	gpio_sample_t mask;
	DECLARE_REGS(regs, port);
	mask = (gpio_sample_t)(int)ctl;
	regs->BRR = mask;
	return 0;
}

int hwpl_gpio_get(int port, void * ctl){
	DECLARE_REGS(regs, port);
	return regs->IDR;
}

int hwpl_gpio_set(int port, void * ctl){
	DECLARE_REGS(regs, port);
	regs->ODR = (gpio_sample_t)(int)ctl;
	return 0;
}
