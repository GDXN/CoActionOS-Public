/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/tmr.h"

//These functions are device specific
extern void _hwpl_tmr_dev_power_on(int port);
extern void _hwpl_tmr_dev_power_off(int port);
extern int _hwpl_tmr_dev_powered_on(int port);
extern int _hwpl_tmr_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const tmr_ioctl_func_table[I_GLOBAL_TOTAL + I_TMR_TOTAL])(int, void*) = {
		hwpl_tmr_getattr,
		hwpl_tmr_setattr,
		hwpl_tmr_setaction,
		hwpl_tmr_on,
		hwpl_tmr_off,
		hwpl_tmr_setoc,
		hwpl_tmr_getoc,
		hwpl_tmr_setic,
		hwpl_tmr_getic,
		hwpl_tmr_set,
		hwpl_tmr_get
};


int hwpl_tmr_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_TMR_PORTS,
			_hwpl_tmr_dev_powered_on,
			_hwpl_tmr_dev_power_on);
}

int hwpl_tmr_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_TMR_PORTS,
			_hwpl_tmr_dev_powered_on,
			tmr_ioctl_func_table,
			I_GLOBAL_TOTAL + I_TMR_TOTAL);
}

int hwpl_tmr_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_tmr_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg,
			wop,
			HWPL_TMR_PORTS,
			_hwpl_tmr_dev_powered_on,
			_hwpl_tmr_dev_write);
}

int hwpl_tmr_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_TMR_PORTS, _hwpl_tmr_dev_powered_on, _hwpl_tmr_dev_power_off);
}


