/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/flash.h"


//These functions are device specific
extern void flash_dev_power_on(int port);
extern void flash_dev_power_off(int port);
extern int flash_dev_powered_on(int port);
extern int _hwpl_flash_dev_read(const device_cfg_t * cfg, device_transfer_t * rop);
extern int _hwpl_flash_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const flash_ioctl_func_table[I_GLOBAL_TOTAL + I_FLASH_TOTAL])(int, void*) = {
		hwpl_flash_getattr,
		hwpl_flash_setattr,
		hwpl_flash_setaction,
		hwpl_flash_eraseaddr,
		hwpl_flash_erasepage,
		hwpl_flash_getpage,
		hwpl_flash_getsize,
		hwpl_flash_getpageinfo,
		hwpl_flash_writepage
};

int hwpl_flash_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_FLASH_PORTS,
			flash_dev_powered_on,
			flash_dev_power_on);
}

int hwpl_flash_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_FLASH_PORTS,
			flash_dev_powered_on,
			flash_ioctl_func_table,
			I_GLOBAL_TOTAL + I_FLASH_TOTAL);
}




int hwpl_flash_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_read(cfg, rop,
			HWPL_FLASH_PORTS,
			flash_dev_powered_on,
			_hwpl_flash_dev_read);
}


int hwpl_flash_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_flash_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_FLASH_PORTS, flash_dev_powered_on, flash_dev_power_off);
}


