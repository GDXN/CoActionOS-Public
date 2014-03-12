/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PCA9551
 *
 *
 * @{
 *
 */

#ifndef HWDL_PCA9551_H_
#define HWDL_PCA9551_H_

#include "hwdl.h"

int pca9551_open(const device_cfg_t * cfg);
int pca9551_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int pca9551_read(const device_cfg_t * cfg, device_transfer_t * rop);
int pca9551_write(const device_cfg_t * cfg, device_transfer_t * wop);
int pca9551_close(const device_cfg_t * cfg);




#endif /* HWDL_PCA9551_H_ */

/*! @} */
