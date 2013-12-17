/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup MMA7660
 * @{
 */

/*! \file */

#include <errno.h>
#include "hwpl/debug.h"
#include "hwpl/i2c.h"
#include "hwdl/freescale/mma7660.h"

int mma7660_open(const device_cfg_t * cfg){
	int err;
	err = hwdl_check_i2c_port(cfg);
	if ( err < 0 ){
		return err;
	}

	return 0;
}

int mma7660_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_i2c_ioctl(cfg, request, ctl);
}

int mma7660_read(const device_cfg_t * cfg, device_transfer_t * rop){
	i2c_reqattr_t transfer;
	transfer.transfer = I2C_TRANSFER_NORMAL;
	transfer.slave_addr = cfg->pcfg.i2c.slave_addr;

	if ( hwpl_i2c_ioctl(cfg, I_I2C_SETUP, &transfer) ){
		return -1;
	}

	return hwpl_i2c_read(cfg, rop);
}

int mma7660_write(const device_cfg_t * cfg, device_transfer_t * wop){
	i2c_reqattr_t transfer;
	transfer.transfer = I2C_TRANSFER_NORMAL;
	transfer.slave_addr = cfg->pcfg.i2c.slave_addr;

	if ( hwpl_i2c_ioctl(cfg, I_I2C_SETUP, &transfer) ){
		return -1;
	}

	return hwpl_i2c_write(cfg, wop);
}

int mma7660_close(const device_cfg_t * cfg){
	return 0;
}


/*! @} */
