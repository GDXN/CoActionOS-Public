/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PCA9674
 *
 *
 * @{
 *
 */

#ifndef HWDL_PCA9674_H_
#define HWDL_PCA9674_H_

#include "hwdl.h"
#include "dev/nxp/pca9674.h"

int pca9674_open(const device_cfg_t * cfg);
int pca9674_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int pca9674_read(const device_cfg_t * cfg, device_transfer_t * rop);
int pca9674_write(const device_cfg_t * cfg, device_transfer_t * wop);
int pca9674_close(const device_cfg_t * cfg);




#endif /* HWDL_PCA9674_H_ */

/*! @} */
