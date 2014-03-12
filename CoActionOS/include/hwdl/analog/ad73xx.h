/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup AD73XX
 * @{
 */

/*! \file */

#ifndef HWDL_AD73XX_H_
#define HWDL_AD73XX_H_

#include "hwdl.h"
#include "dev/analog/ad73xx.h"

typedef struct {
	device_transfer_t rop;
	device_transfer_t * rop_orig;
	int bytes_read;
} ad73xx_state_t;

int ad73xx_open(const device_cfg_t * cfg);
int ad73xx_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int ad73xx_read(const device_cfg_t * cfg, device_transfer_t * rop);
int ad73xx_write(const device_cfg_t * cfg, device_transfer_t * wop);
int ad73xx_close(const device_cfg_t * cfg);



#endif //HWDL_AD73XX_H_

/*! @} */
