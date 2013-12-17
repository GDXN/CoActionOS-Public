/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/core.h"

//These functions are device specific
extern void _hwpl_core_dev_power_on(int port);
extern void _hwpl_core_dev_power_off(int port);
extern int _hwpl_core_dev_powered_on(int port);


int (* const _hwpl_core_ioctl_func_table[I_GLOBAL_TOTAL + I_CORE_TOTAL])(int, void*) = {
		hwpl_core_getattr,
		hwpl_core_setattr,
		hwpl_core_setaction,
		hwpl_core_setpinfunc,
		hwpl_core_sleep,
		hwpl_core_reset,
		hwpl_core_invokebootloader,
		hwpl_core_setirqprio,
		hwpl_core_setclkout
};


int hwpl_core_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_CORE_PORTS,
			_hwpl_core_dev_powered_on,
			_hwpl_core_dev_power_on);
}

int hwpl_core_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_CORE_PORTS,
			_hwpl_core_dev_powered_on,
			_hwpl_core_ioctl_func_table,
			I_GLOBAL_TOTAL + I_CORE_TOTAL);
}



int hwpl_core_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}


int hwpl_core_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_core_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, 1, _hwpl_core_dev_powered_on, _hwpl_core_dev_power_off);
}


