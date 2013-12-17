/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwpl/debug.h"
#include "hwpl/pio.h"
#include "hwpl/core.h"

typedef struct {
	hwpl_callback_t callback;
	void * context;
	uint8_t ref_count;
} pio_local_t;

static pio_local_t _hwpl_pio0_local HWPL_SYS_MEM;
static pio_local_t _hwpl_pio2_local HWPL_SYS_MEM;

static int _hwpl_pio_set_event(int port, int event, int pin);
static void exec_callback0(void * data);
static void exec_callback2(void * data);



void _hwpl_pio_dev_power_on(int port){
	if ( port == 0 ){
		if ( _hwpl_pio0_local.ref_count == 0 ){
			_hwpl_pio0_local.callback = NULL;
		}
		_hwpl_pio0_local.ref_count++;
	} else if ( port == 2 ){
		if ( _hwpl_pio2_local.ref_count == 0 ){
			_hwpl_pio2_local.callback = NULL;
		}
		_hwpl_pio2_local.ref_count++;
	}
}

void _hwpl_pio_dev_power_off(int port){

	if ( port == 0 ){
		if ( _hwpl_pio0_local.ref_count > 0 ){
			if ( _hwpl_pio0_local.ref_count == 1 ){
				_hwpl_pio0_local.callback = NULL;
			}
			_hwpl_pio0_local.ref_count--;
		}
	} else if ( port == 2 ){
		if ( _hwpl_pio2_local.ref_count > 0 ){
			if ( _hwpl_pio2_local.ref_count == 1 ){
				_hwpl_pio2_local.callback = NULL;
			}
			_hwpl_pio2_local.ref_count--;
		}
	}
}

int _hwpl_pio_dev_powered_on(int port){
	if ( port == 0 ){
		if ( _hwpl_pio0_local.ref_count > 0 ){
			return 1;
		} else {
			return 0;
		}
	} else if ( port == 2 ){
		if ( _hwpl_pio2_local.ref_count > 0 ){
			return 1;
		} else {
			return 0;
		}
	} else {
		return 1;
	}
}

int _hwpl_pio_set_event(int port, int event, int pin){

	if ( port == 0 ){

		if ( (pin > 30) ||
				(pin == 12) ||
				(pin == 13) ||
				(pin == 14) ){
			return -1;
		}

		switch(event){
		case PIO_ACTION_EVENT_NONE:
			LPC_GPIOINT->IO0IntEnR &= ~(1<<pin);
			LPC_GPIOINT->IO0IntEnF &= ~(1<<pin);
			break;
		case PIO_ACTION_EVENT_RISING:
			LPC_GPIOINT->IO0IntEnR |= (1<<pin);
			LPC_GPIOINT->IO0IntEnF &= ~(1<<pin);
			break;
		case PIO_ACTION_EVENT_FALLING:
			LPC_GPIOINT->IO0IntEnR &= ~(1<<pin);
			LPC_GPIOINT->IO0IntEnF |= (1<<pin);
			break;
		case PIO_ACTION_EVENT_BOTH:
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
		case PIO_ACTION_EVENT_NONE:
			LPC_GPIOINT->IO2IntEnR &= ~(1<<pin);
			LPC_GPIOINT->IO2IntEnF &= ~(1<<pin);
			break;
		case PIO_ACTION_EVENT_RISING:
			LPC_GPIOINT->IO2IntEnR |= (1<<pin);
			LPC_GPIOINT->IO2IntEnF &= ~(1<<pin);
			break;
		case PIO_ACTION_EVENT_FALLING:
			LPC_GPIOINT->IO2IntEnR &= ~(1<<pin);
			LPC_GPIOINT->IO2IntEnF |= (1<<pin);
			break;
		case PIO_ACTION_EVENT_BOTH:
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


int _hwpl_pio_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	int port;
	hwpl_action_t * action;
	port = cfg->periph.port;

	if( wop->nbyte != sizeof(hwpl_action_t) ){
		errno = EINVAL;
		return -1;
	}

	action = (hwpl_action_t*)wop->buf;
	action->callback = wop->callback;
	action->context = wop->context;
	return hwpl_pio_setaction(port, action);
}

int hwpl_pio_setaction(int port, void * ctl){
	int err;
	hwpl_action_t * action = (hwpl_action_t*)ctl;

	if( action->callback == 0 ){
		if( port == 0 ){
			exec_callback0(DEVICE_OP_CANCELLED);
		} else if ( port == 2 ){
			exec_callback2(DEVICE_OP_CANCELLED);
		}
	}

	err = _hwpl_pio_set_event(port, action->event, action->channel);
	if ( err ){
		return err;
	}


#ifdef __lpc177x_8x

#else
	if ( port == 0 ){
		_hwpl_pio0_local.callback = action->callback;
		_hwpl_pio0_local.context = action->context;
	} else if ( port == 2 ){
		_hwpl_pio2_local.callback = action->callback;
		_hwpl_pio2_local.context = action->context;
	} else {
		errno = EINVAL;
		return -1;
	}

	//This is the interrupt for GPIO0 and GPIO2
	_hwpl_core_priv_enable_irq((void*)EINT3_IRQn);
#endif

	return 0;
}


int hwpl_pio_getattr(int port, void * ctl){
	//read the direction pin status
	errno = ENOTSUP;
	return -1;
}

int hwpl_pio_setattr(int port, void * ctl){
	int i;
	int mode;
	pio_attr_t * attr;
	attr = ctl;
#ifdef __lpc177x_8x
	__IO uint32_t * regs_iocon = (__IO uint32_t *)LPC_IOCON;
#else
	int shift;
	__IO uint32_t * regs_pin;
	__IO uint32_t * regs_od = &(LPC_PINCON->PINMODE_OD0);
#endif
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE + port;

	if( attr->mode & PIO_MODE_INPUT ){
		gpio_regs->DIR&=(~attr->mask);
	}

	if( attr->mode & (PIO_MODE_OUTPUT) ){
		//set output pins as output
		gpio_regs->DIR|=attr->mask;
	}

	if( attr->mode & PIO_MODE_DIRONLY ){
		return 0;
	}

	mode = 0;


#ifdef __lpc177x_8x

	if( attr->mode & PIO_MODE_PULLUP ){
		mode = 2;
	} else if( attr->mode & PIO_MODE_REPEATER ){
		mode = 3;
	} else if( attr->mode & PIO_MODE_FLOAT ){
		mode = 0;
	} else if ( attr->mode & PIO_MODE_PULLDOWN ){
		mode = 1;
	}

	for(i = 0; i < 32; i++){
		if ( (1<<i) & (attr->mask) ){
			regs_iocon[port] = 0;
			_hwpl_core_set_pinsel_func(port, i, CORE_PERIPH_PIO, port); //set the pin to use GPIO

			if( attr->mode & PIO_MODE_OPENDRAIN ){
				regs_iocon[port] |= (1<<10);
			}

			if( attr->mode & PIO_MODE_HYSTERESIS ){
				regs_iocon[port] |= (1<<5);
			}

			if( attr->mode & PIO_MODE_ANALOG ){
				regs_iocon[port] |= (1<<7);
			}

			regs_iocon[port] |= (mode<<3);



		}
	}

#else

	if( attr->mode & PIO_MODE_PULLUP ){
		mode = 0;
	} else if( attr->mode & PIO_MODE_REPEATER ){
		mode = 1;
	} else if( attr->mode & PIO_MODE_FLOAT ){
		mode = 2;
	} else if ( attr->mode & PIO_MODE_PULLDOWN ){
		mode = 3;
	}

	for(i = 0; i < 8*sizeof(pio_sample_t); i++){
		if ( (1<<i) & attr->mask ){

			_hwpl_core_set_pinsel_func(port, i, CORE_PERIPH_GPIO, 0); //set the pin to use GPIO

			if( attr->mode & PIO_MODE_OPENDRAIN ){
				regs_od[port] |= (1<<i);
			} else {
				regs_od[port] &= ~(1<<i);
			}

			regs_pin = &(&(LPC_PINCON->PINMODE0))[port*2 + i/16];
			shift = (i < 16 ? i*2 : ((i-16)*2));
			HWPL_CLR_MASK( *regs_pin, (3<<shift) );
			HWPL_SET_MASK( *regs_pin, (mode<<(shift) ) );
		}
	}
#endif

	return 0;
}

int hwpl_pio_setmask(int port, void * ctl){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].SET=(int)ctl;
	return 0;
}

int hwpl_pio_clrmask(int port, void * ctl){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].CLR=(int)ctl;
	return 0;
}

int hwpl_pio_get(int port, void * ctl){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	return gpio_regs[port].PIN;
}

int hwpl_pio_set(int port, void * ctl){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].PIN=(int)ctl;
	return 0;
}

void exec_callback0(void * data){
	uint32_t tmp;
	tmp = LPC_GPIOINT->IO0IntStatR | LPC_GPIOINT->IO0IntStatF;
	if ( _hwpl_pio0_local.callback != NULL ){
		if( _hwpl_pio0_local.callback(_hwpl_pio0_local.context, data) == 0 ){
			_hwpl_pio0_local.callback = NULL;
		}
	}
	LPC_GPIOINT->IO0IntClr = tmp;
}

void exec_callback2(void * data){
	uint32_t tmp;
	tmp = LPC_GPIOINT->IO2IntStatR | LPC_GPIOINT->IO2IntStatF;
	if ( _hwpl_pio2_local.callback != NULL ){
		if( _hwpl_pio2_local.callback(_hwpl_pio2_local.context, (const void*)tmp) == 0 ){
			_hwpl_pio2_local.callback = NULL;
		}
	}
	LPC_GPIOINT->IO0IntClr = tmp;
}

//The pio interrupts use the eint3 interrupt service routine -- this function should be called from there
void _hwpl_core_pio_isr(void){
	if ( LPC_GPIOINT->IntStatus & (1<<0) ){
		exec_callback0(0);
	}

	if ( LPC_GPIOINT->IntStatus & (1<<2) ){
		exec_callback2(0);
	}

}

