/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/qei.h"

//These functions are device specific
extern void _hwpl_qei_dev_power_on(int port);
extern void _hwpl_qei_dev_power_off(int port);
extern int _hwpl_qei_dev_powered_on(int port);


int (* const qei_ioctl_func_table[I_GLOBAL_TOTAL + I_QEI_TOTAL])(int, void*) = {
		hwpl_qei_getattr,
		hwpl_qei_setattr,
		hwpl_qei_setaction,
		hwpl_qei_get,
		hwpl_qei_getvelocity,
		hwpl_qei_getindex,
		hwpl_qei_reset
};

int hwpl_qei_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_QEI_PORTS,
			_hwpl_qei_dev_powered_on,
			_hwpl_qei_dev_power_on);
}

int hwpl_qei_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_QEI_PORTS,
			_hwpl_qei_dev_powered_on,
			qei_ioctl_func_table,
			I_GLOBAL_TOTAL + I_QEI_TOTAL);
}



int hwpl_qei_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;

}

int hwpl_qei_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;

}

int hwpl_qei_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_QEI_PORTS, _hwpl_qei_dev_powered_on, _hwpl_qei_dev_power_off);
}


