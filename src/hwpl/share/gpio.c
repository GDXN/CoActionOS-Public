/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/gpio.h"



//These functions are device specific
extern void _hwpl_gpio_dev_power_on(int port);
extern void _hwpl_gpio_dev_power_off(int port);
extern int _hwpl_gpio_dev_powered_on(int port);

int (* const gpio_ioctl_func_table[I_GLOBAL_TOTAL + I_GPIO_TOTAL])(int, void*) = {
		hwpl_gpio_getattr,
		hwpl_gpio_setattr,
		hwpl_gpio_setaction,
		hwpl_gpio_setmask,
		hwpl_gpio_clrmask,
		hwpl_gpio_get,
		hwpl_gpio_set,
		hwpl_gpio_setpinmode
};

int hwpl_gpio_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_GPIO_PORTS,
			_hwpl_gpio_dev_powered_on,
			_hwpl_gpio_dev_power_on);
}

int hwpl_gpio_ioctl(const device_cfg_t * cfg, int request, void * ctl){

	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_GPIO_PORTS,
			_hwpl_gpio_dev_powered_on,
			gpio_ioctl_func_table,
			I_GLOBAL_TOTAL + I_GPIO_TOTAL);
}

int hwpl_gpio_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;

}


int hwpl_gpio_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;

}

int hwpl_gpio_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_GPIO_PORTS, _hwpl_gpio_dev_powered_on, _hwpl_gpio_dev_power_off);
}


