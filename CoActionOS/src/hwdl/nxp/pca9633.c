/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwdl.h"
#include "hwpl/i2c.h"

#include "dev/nxp/pca9633.h"
#include "hwdl/nxp/pca9633.h"


int pca9633_open(const device_cfg_t * cfg){
	//check the i2c port for the proper configuration
	int err;
	err = hwdl_check_i2c_port(cfg);
	if ( err < 0 ){
		return err;
	}

	return 0;
}

int pca9633_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_i2c_ioctl(cfg, request, ctl);
}

int pca9633_read(const device_cfg_t * cfg, device_transfer_t * rop){
	i2c_reqattr_t setup;
	setup.slave_addr = cfg->pcfg.i2c.slave_addr;
	rop->loc |= PCA9633_PTR_AUTO_INCREMENT_ALL;
	setup.transfer = I2C_TRANSFER_NORMAL;
	hwpl_i2c_setup(cfg->periph.port, &setup);
	return hwpl_i2c_read(cfg, rop);
}

int pca9633_write(const device_cfg_t * cfg, device_transfer_t * wop){
	i2c_reqattr_t setup;
	setup.slave_addr = cfg->pcfg.i2c.slave_addr;
	wop->loc |= PCA9633_PTR_AUTO_INCREMENT_ALL;
	setup.transfer = I2C_TRANSFER_NORMAL;
	hwpl_i2c_setup(cfg->periph.port, &setup);
	return hwpl_i2c_write(cfg, wop);
}

int pca9633_close(const device_cfg_t * cfg){
	return hwpl_i2c_close(cfg);
}
