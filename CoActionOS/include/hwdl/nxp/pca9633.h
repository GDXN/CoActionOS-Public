/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PCA9633
 *
 *
 * @{
 *
 */

#ifndef HWDL_PCA9633_H_
#define HWDL_PCA9633_H_


#include "hwdl.h"


int pca9633_open(const device_cfg_t * cfg);
int pca9633_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int pca9633_read(const device_cfg_t * cfg, device_transfer_t * rop);
int pca9633_write(const device_cfg_t * cfg, device_transfer_t * wop);
int pca9633_close(const device_cfg_t * cfg);




#endif /* HWDL_PCA9633_H_ */

/*! @} */
