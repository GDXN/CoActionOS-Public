/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/emc.h"


//These functions are device specific
extern void _hwpl_emc_dev_power_on(int port);
extern void _hwpl_emc_dev_power_off(int port);
extern int _hwpl_emc_dev_powered_on(int port);


int (* const emc_ioctl_func_table[I_GLOBAL_TOTAL + I_EMC_TOTAL])(int, void*) = {
		hwpl_emc_getattr,
		hwpl_emc_setattr,
		hwpl_emc_setaction
};

int hwpl_emc_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_EMC_PORTS,
			_hwpl_emc_dev_powered_on,
			_hwpl_emc_dev_power_on);
}

int hwpl_emc_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_EMC_PORTS,
			_hwpl_emc_dev_powered_on,
			emc_ioctl_func_table,
			I_GLOBAL_TOTAL + I_EMC_TOTAL);
}

int hwpl_emc_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_emc_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_emc_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_EMC_PORTS, _hwpl_emc_dev_powered_on, _hwpl_emc_dev_power_off);
}


