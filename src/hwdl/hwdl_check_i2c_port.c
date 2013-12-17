/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwdl.h"
#include "hwpl/i2c.h"



/* \details This function checks to see if the ADC
 * port is configured to use the channels specified
 * by the configuration.  If the ADC port is closed,
 * it is opened and configured using this function.
 * \return Zero if the port is ready to use; an
 * error code if it could not be configured.
 */
int hwdl_check_i2c_port(const device_cfg_t * cfgp){
	int err;
	i2c_attr_t i2c_cfg;


	err = hwpl_i2c_ioctl(cfgp, I_I2C_GETATTR, &i2c_cfg);

	if ( err == 0 ){ //port is already open
		//Check the port configuration

		if ( i2c_cfg.pin_assign != cfgp->pin_assign ){
			errno = EINVAL;
			return -1;
		}

		if ( i2c_cfg.bitrate > cfgp->bitrate ){
			errno = EINVAL;
			return -1;
		}

	} else {
		//Open the ADC port
		i2c_cfg.pin_assign = cfgp->pin_assign;
		i2c_cfg.bitrate = cfgp->bitrate;
		err = hwpl_i2c_open(cfgp);
		if ( err < 0 ){
			return err;
		}
		return hwpl_i2c_ioctl(cfgp, I_I2C_SETATTR, &i2c_cfg);
	}

	return 0;
}

