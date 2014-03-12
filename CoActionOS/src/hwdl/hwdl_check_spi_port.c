/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwpl/debug.h"
#include "hwdl.h"
#include "hwpl/spi.h"

/* \details This function checks to see if a SPI port is opened
 * and configured in a way that is compatible with the
 * configuration specified.  If the SPI port is not
 * already open, this function will open it with
 * the configuration specified.
 * \return 0 if the port is ready to use or an error
 * code if the port is already open with
 * incompatible settings.
 */
int hwdl_check_spi_port(const device_cfg_t * cfgp){
	int err;
	spi_attr_t spi_cfg;

	//check to see if the SPI port is already configured
	err = hwpl_spi_ioctl(cfgp, I_SPI_GETATTR, &spi_cfg);
	if ( err == 0 ){

		if ( spi_cfg.pin_assign != cfgp->pin_assign ){
			errno = EINVAL;
			return -1;
		}

		if ( spi_cfg.width != cfgp->pcfg.spi.width ){
			errno = EINVAL;
			return -1;
		}

		if ( spi_cfg.mode != cfgp->pcfg.spi.mode ){
			errno = EINVAL;
			return -1;
		}

		if ( spi_cfg.format != cfgp->pcfg.spi.format ){
			errno = EINVAL;
			return -1;
		}

		if ( spi_cfg.bitrate > cfgp->bitrate ){
			errno = EINVAL;
			return -1;
		}

	} else {
		//Open the port with the desired configuration
		spi_cfg.pin_assign = cfgp->pin_assign;
		spi_cfg.width = cfgp->pcfg.spi.width;
		spi_cfg.mode = cfgp->pcfg.spi.mode;
		spi_cfg.format = cfgp->pcfg.spi.format;
		spi_cfg.bitrate = cfgp->bitrate;
		spi_cfg.master = SPI_ATTR_MASTER;
		err = hwpl_spi_open(cfgp);
		if ( err < 0 ){
			return err;
		}

		return hwpl_spi_ioctl(cfgp, I_SPI_SETATTR, &spi_cfg);
	}

	return 0;
}
