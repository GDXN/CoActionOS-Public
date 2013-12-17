/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \ingroup MCP4725
 * @{
 */

/*! \file  */

#include "hwdl.h"
#include "hwpl/i2c.h"
#include "dev/microchip/mcp4725.h"



int mcp4725_open(const device_cfg_t * cfg){
	if ( hwdl_check_i2c_port(cfg) < 0 ){
		return -1;
	}
	return 0;
}

int mcp4725_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_i2c_ioctl(cfg, request, ctl);
}

int mcp4725_read(const device_cfg_t * cfg, device_transfer_t * rop){
	i2c_reqattr_t setup;

	setup.transfer = I2C_TRANSFER_READ_ONLY;
	setup.slave_addr = cfg->pcfg.i2c.slave_addr;

	if ( hwpl_i2c_ioctl(cfg, I_I2C_SETUP, &setup) ){
		return -1;
	}

	return hwpl_i2c_read(cfg, rop);
}

int mcp4725_write(const device_cfg_t * cfg, device_transfer_t * wop){
	i2c_reqattr_t setup;
	setup.transfer = I2C_TRANSFER_NORMAL;
	setup.slave_addr = cfg->pcfg.i2c.slave_addr;

	if ( hwpl_i2c_ioctl(cfg, I_I2C_SETUP, &setup) ){
		return -1;
	}
	return hwpl_i2c_write(cfg, wop);
}

int mcp4725_close(const device_cfg_t * cfg){
	//! \todo Put this in low power mode
	return hwpl_i2c_close(cfg);
}


/*! @} */
