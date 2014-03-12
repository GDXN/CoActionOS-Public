/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/i2c.h"

//These functions are device specific
extern void _hwpl_i2c_dev_power_on(int port);
extern void _hwpl_i2c_dev_power_off(int port);
extern int _hwpl_i2c_dev_powered_on(int port);
extern int _hwpl_i2c_dev_read(const device_cfg_t * cfg, device_transfer_t * rop);
extern int _hwpl_i2c_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const i2c_ioctl_func_table[I_GLOBAL_TOTAL + I_I2C_TOTAL])(int, void*) = {
		hwpl_i2c_getattr,
		hwpl_i2c_setattr,
		hwpl_i2c_setaction,
		hwpl_i2c_setup,
		hwpl_i2c_geterr
};

int hwpl_i2c_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_I2C_PORTS,
			_hwpl_i2c_dev_powered_on,
			_hwpl_i2c_dev_power_on);
}

int hwpl_i2c_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_I2C_PORTS,
			_hwpl_i2c_dev_powered_on,
			i2c_ioctl_func_table,
			I_GLOBAL_TOTAL + I_I2C_TOTAL);
}



int hwpl_i2c_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_read(cfg, rop,
			HWPL_I2C_PORTS,
			_hwpl_i2c_dev_powered_on,
			_hwpl_i2c_dev_read);

}


int hwpl_i2c_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg, wop,
			HWPL_I2C_PORTS,
			_hwpl_i2c_dev_powered_on,
			_hwpl_i2c_dev_write);

}

int hwpl_i2c_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_I2C_PORTS, _hwpl_i2c_dev_powered_on, _hwpl_i2c_dev_power_off);
}



