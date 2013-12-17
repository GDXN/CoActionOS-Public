/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup MMA7660
 *
 * @{
 *
 */

/*! \file */
#ifndef HWDL_MMA7660_H_
#define HWDL_MMA7660_H_

#include "hwdl.h"
#include "dev/freescale/mma7660.h"

int mma7660_open(const device_cfg_t * cfg);
int mma7660_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int mma7660_read(const device_cfg_t * cfg, device_transfer_t * rop);
int mma7660_write(const device_cfg_t * cfg, device_transfer_t * wop);
int mma7660_close(const device_cfg_t * cfg);


#endif /* MMA7660_H_ */

/*! @} */
