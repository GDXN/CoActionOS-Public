/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SI9986 Si9986 Motor Driver
 * @{
 *
 */

/*! \file  */

#ifndef HWDL_SI9986_H_
#define HWDL_SI9986_H_

#include "hwdl.h"
#include "dev/si9986.h"

int si9986_open(const device_cfg_t * cfg);
int si9986_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int si9986_read(const device_cfg_t * cfg, device_transfer_t * rop);
int si9986_write(const device_cfg_t * cfg, device_transfer_t * wop);
int si9986_close(const device_cfg_t * cfg);


#endif /* HWDL_SI9986_H_ */


/*! @} */
