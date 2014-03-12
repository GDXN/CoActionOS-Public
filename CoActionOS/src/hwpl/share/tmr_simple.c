/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "local.h"
#include "hwpl/tmr.h"

//These functions are device specific
extern void _hwpl_tmr_dev_power_on(int port);
extern void _hwpl_tmr_dev_power_off(int port);
extern int _hwpl_tmr_dev_powered_on(int port);

static int invalid_request(int port, void * ctl){
	errno = ENOTSUP;
	return -1;
}

int (* const tmrsimple_ioctl_func_table[I_GLOBAL_TOTAL + I_TMR_TOTAL])(int, void*) = {
		hwpl_tmrsimple_getattr,
		hwpl_tmrsimple_setattr,
		hwpl_tmrsimple_setaction,
		hwpl_tmr_on,
		hwpl_tmr_off,
		invalid_request,
		invalid_request,
		invalid_request,
		invalid_request,
		hwpl_tmr_set,
		hwpl_tmr_get
};


int hwpl_tmrsimpleopen(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_TMR_PORTS,
			_hwpl_tmr_dev_powered_on,
			_hwpl_tmr_dev_power_on);
}

int hwpl_tmrsimple_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_TMR_PORTS,
			_hwpl_tmr_dev_powered_on,
			tmrsimple_ioctl_func_table,
			I_GLOBAL_TOTAL + I_TMR_TOTAL);
}

int hwpl_tmrsimple_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_tmrsimple_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_tmrsimple_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_TMR_PORTS, _hwpl_tmr_dev_powered_on, _hwpl_tmr_dev_power_off);
}


