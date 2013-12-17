/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <fcntl.h>
#include "hwpl/pwm.h"
#include "hwpl/core.h"

#define READ_OVERFLOW (1<<0)
#define WRITE_OVERFLOW (1<<1)

static void update_pwm(int port, int chan, int duty);
static void exec_callback(int port, LPC_PWM_TypeDef * regs, void * data);

typedef struct HWPL_PACK {
	const uint32_t * volatile duty;
	volatile int pwm_nbyte_len;
	uint8_t chan;
	uint8_t pin_assign;
	uint8_t enabled_channels;
	uint8_t ref_count;
	hwpl_callback_t callback;
	void * context;
} pwm_local_t;

static pwm_local_t pwm_local[HWPL_PWM_PORTS] HWPL_SYS_MEM;

LPC_PWM_TypeDef * const pwm_regs_table[HWPL_PWM_PORTS] = HWPL_PWM_REGS;

void _hwpl_pwm_dev_power_on(int port){
	if ( pwm_local[port].ref_count == 0 ){
		switch(port){
#ifdef __lpc177x_8x
		case 0:
			_hwpl_lpc_core_enable_pwr(PCPWM0);
			_hwpl_core_priv_enable_irq((void*)PWM0_IRQn);
			break;
#endif
		case 1:
			_hwpl_lpc_core_enable_pwr(PCPWM1);
			_hwpl_core_priv_enable_irq((void*)PWM1_IRQn);
			break;
		}

	}
	pwm_local[port].ref_count++;
}

void _hwpl_pwm_dev_power_off(int port){
	if ( pwm_local[port].ref_count > 0 ){
		if ( pwm_local[port].ref_count == 1 ){
			switch(port){
#ifdef __lpc177x_8x
			case 0:
				_hwpl_core_priv_disable_irq((void*)(PWM0_IRQn));
				_hwpl_lpc_core_disable_pwr(PCPWM0);
				break;
#endif
			case 1:
				_hwpl_core_priv_disable_irq((void*)(PWM1_IRQn));
				_hwpl_lpc_core_disable_pwr(PCPWM1);
				break;

			}
		}
		pwm_local[port].ref_count--;
	}
}

int _hwpl_pwm_dev_powered_on(int port){
	switch(port){
#ifdef __lpc177x_8x
	case 0:
		return _hwpl_lpc_core_pwr_enabled(PCPWM0);
#endif
	case 1:
		return _hwpl_lpc_core_pwr_enabled(PCPWM1);
	}
	return 0;
}

int hwpl_pwm_getattr(int port, void * ctl){
	pwm_attr_t * ctlp;
	ctlp = (pwm_attr_t*)ctl;
	LPC_PWM_TypeDef * regs = pwm_regs_table[port];

#ifdef __lpc17xx
	if( regs == 0 ){
		errno = ENODEV;
		return -1;
	}
#endif

	ctlp->pin_assign = pwm_local[port].pin_assign;
	ctlp->enabled_channels = pwm_local[port].enabled_channels;
	ctlp->top = regs->MR0;
	ctlp->freq = _hwpl_core_getclock() / ( regs->PR + 1 );
	ctlp->flags = PWM_ATTR_FLAGS_ACTIVE_HIGH;

	return 0;
}

int hwpl_pwm_setattr(int port, void * ctl){
	//check the GPIO configuration
	uint32_t tmp;
	pwm_attr_t * ctl_ptr = (pwm_attr_t *)ctl;
	LPC_PWM_TypeDef * regs = pwm_regs_table[port];

#ifdef __lpc17xx
	if( regs == 0 ){
		errno = ENODEV;
		return -1;
	}
#endif

	if ( ctl_ptr->enabled_channels & ~0x3F ){
		errno = EINVAL;
		return -1 - offsetof(pwm_attr_t, enabled_channels);
	}

	if ( ctl_ptr->freq == 0 ){
		errno = EINVAL;
		return -1 - offsetof(pwm_attr_t, freq);
	}

	//Configure the GPIO

	switch(port){
#ifdef __lpc177x_8x

	case 0:
		if(1){
			switch( ctl_ptr->pin_assign ){
			case 0:
				if ( ctl_ptr->enabled_channels & (1<<0) ){
					_hwpl_core_set_pinsel_func(1, 2, CORE_PERIPH_PWM, 0);
				}
				if ( ctl_ptr->enabled_channels & (1<<1) ){
					_hwpl_core_set_pinsel_func(1, 3, CORE_PERIPH_PWM, 0);
				}
				if ( ctl_ptr->enabled_channels & (1<<2) ){
					_hwpl_core_set_pinsel_func(1, 5, CORE_PERIPH_PWM, 0);
				}
				if ( ctl_ptr->enabled_channels & (1<<3) ){
					_hwpl_core_set_pinsel_func(1, 6, CORE_PERIPH_PWM, 0);
				}
				if ( ctl_ptr->enabled_channels & (1<<4) ){
					_hwpl_core_set_pinsel_func(1, 7, CORE_PERIPH_PWM, 0);
				}
				if ( ctl_ptr->enabled_channels & (1<<5) ){
					_hwpl_core_set_pinsel_func(1, 11, CORE_PERIPH_PWM, 0);
				}
				break;
			case 1:
				if ( ctl_ptr->enabled_channels & (1<<0) ){
					_hwpl_core_set_pinsel_func(3, 16, CORE_PERIPH_PWM, 0);
				}
				if ( ctl_ptr->enabled_channels & (1<<1) ){
					_hwpl_core_set_pinsel_func(3, 17, CORE_PERIPH_PWM, 0);
				}
				if ( ctl_ptr->enabled_channels & (1<<2) ){
					_hwpl_core_set_pinsel_func(3, 18, CORE_PERIPH_PWM, 0);
				}
				if ( ctl_ptr->enabled_channels & (1<<3) ){
					_hwpl_core_set_pinsel_func(3, 19, CORE_PERIPH_PWM, 0);
				}
				if ( ctl_ptr->enabled_channels & (1<<4) ){
					_hwpl_core_set_pinsel_func(3, 20, CORE_PERIPH_PWM, 0);
				}
				if ( ctl_ptr->enabled_channels & (1<<5) ){
					_hwpl_core_set_pinsel_func(3, 21, CORE_PERIPH_PWM, 0);
				}
				break;
			}
		}
#endif
	case 1:
		if(1){
			switch( ctl_ptr->pin_assign ){
			case 0:
				if ( ctl_ptr->enabled_channels & (1<<0) ){
					_hwpl_core_set_pinsel_func(1, 18, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<1) ){
					_hwpl_core_set_pinsel_func(1, 20, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<2) ){
					_hwpl_core_set_pinsel_func(1, 21, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<3) ){
					_hwpl_core_set_pinsel_func(1, 23, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<4) ){
					_hwpl_core_set_pinsel_func(1, 24, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<5) ){
					_hwpl_core_set_pinsel_func(1, 26, CORE_PERIPH_PWM, 1);
				}
				break;
			case 1:
				if ( ctl_ptr->enabled_channels & (1<<0) ){
					_hwpl_core_set_pinsel_func(2, 0, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<1) ){
					_hwpl_core_set_pinsel_func(2, 1, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<2) ){
					_hwpl_core_set_pinsel_func(2, 2, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<3) ){
					_hwpl_core_set_pinsel_func(2, 3, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<4) ){
					_hwpl_core_set_pinsel_func(2, 4, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<5) ){
					_hwpl_core_set_pinsel_func(2, 5, CORE_PERIPH_PWM, 1);
				}
				break;
#ifdef __lpc177x_8x
			case 2:
				if ( ctl_ptr->enabled_channels & (1<<0) ){
					_hwpl_core_set_pinsel_func(3, 24, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<1) ){
					_hwpl_core_set_pinsel_func(3, 25, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<2) ){
					_hwpl_core_set_pinsel_func(3, 26, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<3) ){
					_hwpl_core_set_pinsel_func(3, 27, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<4) ){
					_hwpl_core_set_pinsel_func(3, 28, CORE_PERIPH_PWM, 1);
				}
				if ( ctl_ptr->enabled_channels & (1<<5) ){
					_hwpl_core_set_pinsel_func(3, 29, CORE_PERIPH_PWM, 1);
				}
				break;
#endif
			case HWPL_GPIO_CFG_USER:
				break;
			default:
				errno = EINVAL;
				return -1 - offsetof(pwm_attr_t, pin_assign);
			}
		}
	}


	tmp = _hwpl_core_getclock() / ctl_ptr->freq;
	if ( tmp > 0 ){
		tmp = tmp - 1;
	}

	regs->TCR = 0; //Disable the counter while the registers are being updated

	regs->PR = tmp;
	//Configure to reset on match0 in PWM Mode
	regs->MR0 = ctl_ptr->top;
	regs->LER |= (1<<0);
	regs->MCR = (1<<1); //enable the reset
	regs->TCR = (1<<3)|(1<<0); //Enable the counter in PWM mode
	regs->PCR = (ctl_ptr->enabled_channels & 0x3F) << 9;

	pwm_local[port].pin_assign = ctl_ptr->pin_assign;
	pwm_local[port].enabled_channels = ctl_ptr->enabled_channels;

	return 0;
}

int hwpl_pwm_setaction(int port, void * ctl){
	hwpl_action_t * action = (hwpl_action_t*)ctl;
	LPC_PWM_TypeDef * regs = pwm_regs_table[port];
	if( action->callback == 0 ){
		//cancel any ongoing operation
		if ( regs->MCR & (1<<0) ){ //If the interrupt is enabled--the pwm is busy
			exec_callback(port, regs, DEVICE_OP_CANCELLED);
		}
	}


	pwm_local[port].callback = action->callback;
	pwm_local[port].context = action->context;

	//need to decode the event
	return 0;
}

int hwpl_pwm_set(int port, void * ctl){
	pwm_reqattr_t * writep = (pwm_reqattr_t*)ctl;
	LPC_PWM_TypeDef * regs = pwm_regs_table[port];

#ifdef __lpc17xx
	if( regs == 0 ){
		errno = ENODEV;
		return -1;
	}
#endif

	if ( regs->MCR & (1<<0) ){ //If the interrupt is enabled--the pwm is busy
		//Device is busy and can't start a new write
		errno = EBUSY;
		return -1;
	}

	update_pwm(port, writep->channel, writep->duty);
	return 0;
}


int _hwpl_pwm_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	int port = DEVICE_GET_PORT(cfg);
	LPC_PWM_TypeDef * regs = pwm_regs_table[port];

#ifdef __lpc17xx
	if( regs == 0 ){
		errno = ENODEV;
		return -1;
	}
#endif

	if ( regs->MCR & (1<<0) ){ //If the interrupt is enabled--the pwm is busy
		errno = EAGAIN;
		return -1;
	}

	pwm_local[port].pwm_nbyte_len = wop->nbyte >> 2;
	pwm_local[port].duty = (const uint32_t *)wop->buf;
	regs->MCR |= (1<<0); //enable the interrupt
	pwm_local[port].chan = wop->loc;
	pwm_local[port].callback = wop->callback;
	pwm_local[port].context = wop->context;
	return 0;
}

void update_pwm(int port, int chan, int duty){
	LPC_PWM_TypeDef * regs = pwm_regs_table[port];

	switch(chan){
	case 0:
		regs->MR1 = duty;
		break;
	case 1:
		regs->MR2 = duty;
		break;
	case 2:
		regs->MR3 = duty;
		break;
	case 3:
		regs->MR4 = duty;
		break;
	case 4:
		regs->MR5 = duty;
		break;
	case 5:
		regs->MR6 = duty;
		break;
	}

	regs->LER |= (1<<(chan+1));
}

void exec_callback(int port, LPC_PWM_TypeDef * regs, void * data){
	//stop updating the duty cycle
	pwm_local[port].duty = NULL;

	//Disable the interrupt
	regs->MCR = (1<<1); //leave the reset on, but disable the interrupt

	//call the event handler
	if ( pwm_local[port].callback != NULL ){
		if( pwm_local[port].callback(pwm_local[port].context, data) == 0 ){
			pwm_local[port].callback = NULL;
		}
	}
}


static void _hwpl_core_pwm_isr(int port){
	//Clear the interrupt flag
	LPC_PWM_TypeDef * regs = pwm_regs_table[port];

	regs->IR |= (1<<0);

	if ( pwm_local[port].pwm_nbyte_len ){
		if ( pwm_local[port].duty != NULL ){
			update_pwm(port, pwm_local[port].chan, *pwm_local[port].duty++);
		}
		pwm_local[port].pwm_nbyte_len--;
	} else {
		exec_callback(port, regs, 0);
	}
}

#ifdef __lpc177x_8x
void _hwpl_core_pwm0_isr(void){
	_hwpl_core_pwm_isr(0);
}
#endif

//This will execute when MR0 overflows
void _hwpl_core_pwm1_isr(void){
	_hwpl_core_pwm_isr(1);
}

