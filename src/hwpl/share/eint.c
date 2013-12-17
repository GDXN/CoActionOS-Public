/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/eint.h"


//These functions are device specific
extern void _hwpl_eint_dev_power_on(int port);
extern void _hwpl_eint_dev_power_off(int port);
extern int _hwpl_eint_dev_powered_on(int port);
extern int _hwpl_eint_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const eint_ioctl_func_table[I_GLOBAL_TOTAL + I_EINT_TOTAL])(int, void*) = {
		hwpl_eint_getattr,
		hwpl_eint_setattr,
		hwpl_eint_setaction
};

int hwpl_eint_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_EINT_PORTS,
			_hwpl_eint_dev_powered_on,
			_hwpl_eint_dev_power_on);
}

int hwpl_eint_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_EINT_PORTS,
			_hwpl_eint_dev_powered_on,
			eint_ioctl_func_table,
			I_GLOBAL_TOTAL + I_EINT_TOTAL);
}

int hwpl_eint_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_eint_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg,
			wop,
			HWPL_EINT_PORTS,
			_hwpl_eint_dev_powered_on,
			_hwpl_eint_dev_write);
}

int hwpl_eint_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_EINT_PORTS, _hwpl_eint_dev_powered_on, _hwpl_eint_dev_power_off);
}


