/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYS_NULL Null Device
 * @{
 *
 *
 * \ingroup SYS
 *
 * \details This device discards input and returns EOF when read.
 *
 *
 */

/*! \file  */

#ifndef HWDL_ZERO_H_
#define HWDL_ZERO_H_

#include "hwdl.h"


int zero_open(const device_cfg_t * cfg);
int zero_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int zero_read(const device_cfg_t * cfg, device_transfer_t * rop);
int zero_write(const device_cfg_t * cfg, device_transfer_t * wop);
int zero_close(const device_cfg_t * cfg);


#endif /* HWDL_FIFO_H_ */


/*! @} */
