/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup STLM20
 * @{
 *
 *
 */

/*! \file  */

#ifndef HWDL_STLM20_H_
#define HWDL_STLM20_H_

#include "hwdl.h"
#include "dev/stlm20.h"

int stlm20_open(const device_cfg_t * cfg);
int stlm20_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int stlm20_read(const device_cfg_t * cfg, device_transfer_t * rop);
int stlm20_write(const device_cfg_t * cfg, device_transfer_t * wop);
int stlm20_close(const device_cfg_t * cfg);


#endif /* HWDL_STLM20_H_ */


/*! @} */
