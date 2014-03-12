/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwpl/debug.h"
#include "pio_flags.h"

typedef struct {
	hwpl_callback_t callback;
	void * context;
	uint8_t ref_count;
} pio_local_t;

static pio_local_t hwpl_pio_local[HWPL_GPIO_PORTS] HWPL_SYS_MEM;
static GPIO_TypeDef * const hwpl_pio_regs[HWPL_PIO_PORTS] = HWPL_GPIO_REGS;

int _hwpl_pio_dev_port_is_invalid(int port){
	if ( (uint32_t)port > HWPL_GPIO_PORTS ){
		return 1;
	}
	return 0;
}

void _hwpl_pio_dev_power_on(int port){
	if ( hwpl_pio_local[port].ref_count == 0 ){
		hwpl_pio_local[port].callback = NULL;
		RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN<<(port)); //turn the clock off to the port
	}
	hwpl_pio_local[port].ref_count++;

	hwpl_pio_poweron(port); //turn the clock on to the port
}

void _hwpl_pio_dev_power_off(int port){
	if ( hwpl_pio_local[port].ref_count > 0 ){
		if ( hwpl_pio_local[port].ref_count == 1 ){
			hwpl_pio_local[port].callback = NULL;
			RCC->APB2ENR &= ~(RCC_APB2ENR_IOPAEN<<(port)); //turn the clock off to the port
		}
		hwpl_pio_local[port].ref_count--;
	}
}

int _hwpl_pio_dev_powered_on(int port){
	if ( hwpl_pio_local[port].ref_count > 0 ){
		return 1;
	}
	return 0;
}

void hwpl_pio_poweron(int port){
	RCC->APB2ENR |= (1<<(2 + port));
}


int hwpl_pio_getattr(int port, void * ctl){
	errno = ENOTSUP;
	return -1;
}

int hwpl_pio_setattr(int port, void * ctl){
	pio_attr_t * attrp = ctl;
	DECLARE_REGS(regs, port);
	int pinmode;
	int pullup;
	int mode;
	int i;

	pinmode = 0;
	pullup = 0;
	//set the input pins as input
	if( attrp->mode & PIO_MODE_INPUT ){

		if( attrp->mode & PIO_MODE_PULLUP ){
			pullup = 1;
			pinmode = MODE_CNF_INPUT_PULLUP_DOWN;
		} else if( attrp->mode & PIO_MODE_PULLDOWN ){
			pinmode = MODE_CNF_INPUT_PULLUP_DOWN;
		} else if( attrp->mode & PIO_MODE_FLOAT ){
			pinmode = MODE_CNF_INPUT_ANALOG;

		}
	} else if( attrp->mode & PIO_MODE_OUTPUT ){
		if( attrp->mode & PIO_MODE_OUTPUT_50MHZ ){
			pinmode = MODE_CNF_OUTPUT_PUSHPULL_50MHZ;
		} else if( attrp->mode & PIO_MODE_OUTPUT_10MHZ ){
			pinmode = MODE_CNF_OUTPUT_PUSHPULL_10MHZ;
		} else {
			pinmode = MODE_CNF_OUTPUT_PUSHPULL_2MHZ;
		}

		if( attrp->mode & PIO_MODE_OPENDRAIN ){
			pinmode |= MODE_CNF_OUTPUT_OPENDRAIN;
		}
	} else {
		errno = EINVAL;
		return -1;
	}


	for(i=0; i < 16; i++){
		if( attrp->mask & (1<<i) ){
			if( i < 8 ){
				configure_pio_low(regs, i, pinmode);
			} else {
				configure_pio_high(regs, i, pinmode);
			}

			if( pullup ){
				regs->BSRR = (1<<i); //set output
			} else {
				regs->BRR = (1<<i); //reset output
			}
		}


	}

	return 0;
}

int hwpl_pio_setmask(int port, void * ctl){
	pio_sample_t mask;
	DECLARE_REGS(regs, port);
	mask = (pio_sample_t)(int)ctl;
	regs->BSRR = mask;
	return 0;
}

int hwpl_pio_clrmask(int port, void * ctl){
	pio_sample_t mask;
	DECLARE_REGS(regs, port);
	mask = (pio_sample_t)(int)ctl;
	regs->BRR = mask;
	return 0;
}

int hwpl_pio_get(int port, void * ctl){
	DECLARE_REGS(regs, port);
	return regs->IDR;
}

int hwpl_pio_set(int port, void * ctl){
	DECLARE_REGS(regs, port);
	regs->ODR = (pio_sample_t)(int)ctl;
	return 0;
}

int hwpl_pio_setaction(int port, void * ctl){

	return 0;
}

//The pio interrupts use the eint3 interrupt service routine -- this function should be called from there
void _hwpl_core_pio_isr(void){

}


