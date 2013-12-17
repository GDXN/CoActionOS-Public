/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup AD5160
 * @{
 *
 */

/*! \file  */

#ifndef HWDL_AD5160_H_
#define HWDL_AD5160_H_

#include "hwdl.h"
#include "dev/analog/ad5160.h"

int ad5160_open(const device_cfg_t * cfg);
int ad5160_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int ad5160_read(const device_cfg_t * cfg, device_transfer_t * rop);
int ad5160_write(const device_cfg_t * cfg, device_transfer_t * wop);
int ad5160_close(const device_cfg_t * cfg);


#endif /* HWDL_AD5160_H_ */

/*! @} */
