/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/dac.h"

//These functions are device specific
extern void _hwpl_dac_dev_power_on(int port);
extern void _hwpl_dac_dev_power_off(int port);
extern int _hwpl_dac_dev_powered_on(int port);
extern int _hwpl_dac_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);


int (* const dac_ioctl_func_table[I_GLOBAL_TOTAL + I_DAC_TOTAL])(int, void*) = {
		hwpl_dac_getattr,
		hwpl_dac_setattr,
		hwpl_dac_setaction,
		hwpl_dac_get
};

int hwpl_dac_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_DAC_PORTS,
			_hwpl_dac_dev_powered_on,
			_hwpl_dac_dev_power_on);
}

int hwpl_dac_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_DAC_PORTS,
			_hwpl_dac_dev_powered_on,
			dac_ioctl_func_table,
			I_GLOBAL_TOTAL + I_DAC_TOTAL);
}



int hwpl_dac_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}


int hwpl_dac_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg, wop,
			HWPL_DAC_PORTS,
			_hwpl_dac_dev_powered_on,
			_hwpl_dac_dev_write);

}

int hwpl_dac_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_DAC_PORTS, _hwpl_dac_dev_powered_on, _hwpl_dac_dev_power_off);
}


