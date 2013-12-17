/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/pio.h"



//These functions are device specific
extern void _hwpl_pio_dev_power_on(int port);
extern void _hwpl_pio_dev_power_off(int port);
extern int _hwpl_pio_dev_powered_on(int port);
extern int _hwpl_pio_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const _hwpl_pio_ioctl_func_table[I_GLOBAL_TOTAL + I_PIO_TOTAL])(int, void*) = {
		hwpl_pio_getattr,
		hwpl_pio_setattr,
		hwpl_pio_setaction,
		hwpl_pio_setmask,
		hwpl_pio_clrmask,
		hwpl_pio_get,
		hwpl_pio_set
};

int hwpl_pio_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_PIO_PORTS,
			_hwpl_pio_dev_powered_on,
			_hwpl_pio_dev_power_on);
}

int hwpl_pio_ioctl(const device_cfg_t * cfg, int request, void * ctl){

	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_PIO_PORTS,
			_hwpl_pio_dev_powered_on,
			_hwpl_pio_ioctl_func_table,
			I_GLOBAL_TOTAL + I_PIO_TOTAL);
}

int hwpl_pio_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;

}


int hwpl_pio_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg,
			wop,
			HWPL_PIO_PORTS,
			_hwpl_pio_dev_powered_on,
			_hwpl_pio_dev_write);
}

int hwpl_pio_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_PIO_PORTS, _hwpl_pio_dev_powered_on, _hwpl_pio_dev_power_off);
}


