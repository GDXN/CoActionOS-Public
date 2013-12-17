/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/pwm.h"

//These functions are device specific
extern void _hwpl_pwm_dev_power_on(int port);
extern void _hwpl_pwm_dev_power_off(int port);
extern int _hwpl_pwm_dev_powered_on(int port);
extern int _hwpl_pwm_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);


int (* const pwm_ioctl_func_table[I_GLOBAL_TOTAL + I_PWM_TOTAL])(int, void*) = {
		hwpl_pwm_getattr,
		hwpl_pwm_setattr,
		hwpl_pwm_setaction,
		hwpl_pwm_set
};


int hwpl_pwm_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_PWM_PORTS,
			_hwpl_pwm_dev_powered_on,
			_hwpl_pwm_dev_power_on);
}

int hwpl_pwm_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_PWM_PORTS,
			_hwpl_pwm_dev_powered_on,
			pwm_ioctl_func_table,
			I_GLOBAL_TOTAL + I_PWM_TOTAL);
}

int hwpl_pwm_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;

}

int hwpl_pwm_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg, wop,
			HWPL_PWM_PORTS,
			_hwpl_pwm_dev_powered_on,
			_hwpl_pwm_dev_write);

}

int hwpl_pwm_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_PWM_PORTS, _hwpl_pwm_dev_powered_on, _hwpl_pwm_dev_power_off);
}


