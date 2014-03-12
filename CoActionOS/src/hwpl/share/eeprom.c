/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/eeprom.h"

#define I_EEPROM_TOTAL 0

//These functions are device specific
extern void _hwpl_eeprom_dev_power_on(int port);
extern void _hwpl_eeprom_dev_power_off(int port);
extern int _hwpl_eeprom_dev_powered_on(int port);
extern int _hwpl_eeprom_dev_read(const device_cfg_t * cfg, device_transfer_t * rop);
extern int _hwpl_eeprom_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const eeprom_ioctl_func_table[I_GLOBAL_TOTAL + I_EEPROM_TOTAL])(int, void*) = {
		hwpl_eeprom_getattr,
		hwpl_eeprom_setattr,
		hwpl_eeprom_setaction
};

int hwpl_eeprom_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_EEPROM_PORTS,
			_hwpl_eeprom_dev_powered_on,
			_hwpl_eeprom_dev_power_on);
}

int hwpl_eeprom_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_EEPROM_PORTS,
			_hwpl_eeprom_dev_powered_on,
			eeprom_ioctl_func_table,
			I_GLOBAL_TOTAL + I_EEPROM_TOTAL);
}




int hwpl_eeprom_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_read(cfg, rop,
			HWPL_EEPROM_PORTS,
			_hwpl_eeprom_dev_powered_on,
			_hwpl_eeprom_dev_read);
}


int hwpl_eeprom_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg, wop,
			HWPL_EEPROM_PORTS,
			_hwpl_eeprom_dev_powered_on,
			_hwpl_eeprom_dev_write);
}

int hwpl_eeprom_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_EEPROM_PORTS, _hwpl_eeprom_dev_powered_on, _hwpl_eeprom_dev_power_off);
}


