/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/spi.h"

//These functions are device specific
extern void _hwpl_ssp_dev_power_on(int port);
extern void _hwpl_ssp_dev_power_off(int port);
extern int _hwpl_ssp_dev_powered_on(int port);
extern int _hwpl_ssp_dev_read(const device_cfg_t * cfg, device_transfer_t * rop);
extern int _hwpl_ssp_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const ssp_ioctl_func_table[I_GLOBAL_TOTAL + I_SPI_TOTAL])(int, void*) = {
		hwpl_ssp_getattr,
		hwpl_ssp_setattr,
		hwpl_ssp_setaction,
		hwpl_ssp_swap,
		hwpl_ssp_setduplex,
};

int hwpl_ssp_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_SSP_PORTS,
			_hwpl_ssp_dev_powered_on,
			_hwpl_ssp_dev_power_on);
}

int hwpl_ssp_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_SSP_PORTS,
			_hwpl_ssp_dev_powered_on,
			ssp_ioctl_func_table,
			I_GLOBAL_TOTAL + I_SPI_TOTAL);
}



int hwpl_ssp_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_read(cfg, rop,
			HWPL_SSP_PORTS,
			_hwpl_ssp_dev_powered_on,
			_hwpl_ssp_dev_read);

}


int hwpl_ssp_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg, wop,
			HWPL_SSP_PORTS,
			_hwpl_ssp_dev_powered_on,
			_hwpl_ssp_dev_write);

}

int hwpl_ssp_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_SSP_PORTS, _hwpl_ssp_dev_powered_on, _hwpl_ssp_dev_power_off);
}


