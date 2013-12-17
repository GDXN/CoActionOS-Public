/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SI9986
 * @{
 *
 * \ingroup VISHAY
 *
 */

/*! \file  */

#include <errno.h>
#include "hwpl/pwm.h"
#include "hwdl.h"
#include "dev/vishay/si9986.h"

static int quick_duty_set(const device_cfg_t * cfg, uint8_t chan, float duty){
	pwm_duty_t pduty;
	pduty = (pwm_duty_t)( (float)duty * cfg->pcfg.pwm.top );
	pwm_reqattr_t do_write;
	do_write.channel = chan;
	do_write.duty = pduty;
	return hwpl_pwm_ioctl(cfg, I_PWM_SET, &do_write);
}

static void bound_duty(float * duty){
	if ( *duty > 1.0 ){
		*duty = 1.0;
	}

	if ( *duty < -1.0 ){
		*duty = -1.0;
	}
}


int si9986_ireq_brake(const device_cfg_t * cfg, void * ctl);
int si9986_ireq_hiz(const device_cfg_t * cfg, void * ctl);
int si9986_ireq_setbrake(const device_cfg_t * cfg, void * ctl);
int si9986_ireq_sethiz(const device_cfg_t * cfg, void * ctl);
int si9986_ireq_convert(const device_cfg_t * cfg, void * ctl);


int (* const si9986_ioctl_func_table[I_SI9986_TOTAL])(const device_cfg_t *, void*) = {
		si9986_ireq_brake,
		si9986_ireq_hiz,
		si9986_ireq_setbrake,
		si9986_ireq_sethiz,
		si9986_ireq_convert
};


int si9986_open(const device_cfg_t * cfg){
	//check to see if the PWM is opened and then configure
	//the appropriate channels
	int err;

	err = hwdl_check_pwm_port(cfg);
	if ( err < 0 ){
		return err;
	}

	return 0;
}

int si9986_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	int tmp;

	if( _IOCTL_IDENT(request) == SI9986_IOC_CHAR ){
		tmp = _IOCTL_NUM(request);
		if ( tmp < I_SI9986_TOTAL ){
			return si9986_ioctl_func_table[tmp](cfg, ctl);
		} else {
			errno = EINVAL;
			return -1;
		}
	} else {
		return hwpl_pwm_ioctl(cfg, request, ctl);
	}

}

int si9986_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_pwm_read(cfg, rop);
}

int si9986_write(const device_cfg_t * cfg, device_transfer_t * wop){
	int err;
	uint8_t hold_chan;
	uint8_t move_chan;

	if ( wop->loc & SI9986_LOC_FORWARD ){
		hold_chan = cfg->pcfg.pwm.channels[0];
		move_chan = cfg->pcfg.pwm.channels[1];
	} else {
		hold_chan = cfg->pcfg.pwm.channels[1];
		move_chan = cfg->pcfg.pwm.channels[0];
	}

	//Hold one channel in one place
	if ( wop->loc & SI9986_LOC_BRAKE ){
		err = quick_duty_set(cfg, hold_chan, 0);
	} else {
		err = quick_duty_set(cfg, hold_chan, cfg->pcfg.pwm.top);
	}

	if ( err < 0 ){
		return err;
	}

	wop->loc = move_chan;
	//move the other channel
	return hwpl_pwm_write(cfg, wop);
}

int si9986_close(const device_cfg_t * cfg){
	return hwpl_pwm_close(cfg);
}

int si9986_ireq_brake(const device_cfg_t * cfg, void * ctl){
	int err;
	err = quick_duty_set(cfg, cfg->pcfg.pwm.channels[0], 0);
	if ( err < 0 ){
		return err;
	}
	return quick_duty_set(cfg, cfg->pcfg.pwm.channels[1], 0);
}

int si9986_ireq_hiz(const device_cfg_t * cfg, void * ctl){
	int err;
	err = quick_duty_set(cfg, cfg->pcfg.pwm.channels[0], cfg->pcfg.pwm.top );
	if ( err < 0 ){
		return err;
	}
	return quick_duty_set(cfg, cfg->pcfg.pwm.channels[1], cfg->pcfg.pwm.top );
}

int si9986_ireq_setbrake(const device_cfg_t * cfg, void * ctl){
	float duty;
	int err;
	duty = ((float*)ctl)[0];
	bound_duty(&duty);
	if ( duty < 0 ){
		err = quick_duty_set(cfg, cfg->pcfg.pwm.channels[0], 0.0);
		if ( err < 0 ){
			return err;
		}
		err = quick_duty_set(cfg, cfg->pcfg.pwm.channels[1], (duty*-1.0) );
	} else {
		err = quick_duty_set(cfg, cfg->pcfg.pwm.channels[0], duty);
		if ( err < 0 ){
			return err;
		}
		err = quick_duty_set(cfg, cfg->pcfg.pwm.channels[1], 0.0);
	}
	return err;
}

int si9986_ireq_sethiz(const device_cfg_t * cfg, void * ctl){
	int err;
	float duty;
	duty = ((float*)ctl)[0];
	bound_duty(&duty);
	if ( duty < 0 ){
		err = quick_duty_set(cfg, cfg->pcfg.pwm.channels[0], 1.0);
		if ( err < 0 ){
			return err;
		}
		err = quick_duty_set(cfg, cfg->pcfg.pwm.channels[1], 1.0 - duty*-1);
	} else {
		err = quick_duty_set(cfg, cfg->pcfg.pwm.channels[0], 1.0 - duty);
		if ( err < 0 ){
			return err;
		}
		err = quick_duty_set(cfg, cfg->pcfg.pwm.channels[1], 1.0);
	}
	return err;

}

int si9986_ireq_convert(const device_cfg_t * cfg, void * ctl){
	pwm_duty_t d;
	si9986_convert_t * duty = (si9986_convert_t*)ctl;
	if ( duty->float_duty > 1.0 ){
		duty->float_duty = 1.0;
	}

	if ( duty->float_duty < 0.0 ){
		duty->float_duty = 0.0;
	}

	if ( duty->loc & SI9986_LOC_BRAKE ){
	d = duty->float_duty * cfg->pcfg.pwm.top;
	} else {
		d = (1.0 - duty->float_duty) * cfg->pcfg.pwm.top;
	}
	duty->duty = d;
	return 0;
}



/*! @} */
