/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup BMA150
 * @{
 */

/*! \file  */

#ifndef HWDL_BMA150_H_
#define HWDL_BMA150_H_

#include "hwdl.h"
#include "dev/bosch/bma150.h"

typedef struct {
	hwpl_callback_t callback;
	void * context;
} bma150_state_t;

int bma150_open(const device_cfg_t * cfg);
int bma150_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int bma150_read(const device_cfg_t * cfg, device_transfer_t * rop);
int bma150_write(const device_cfg_t * cfg, device_transfer_t * wop);
int bma150_close(const device_cfg_t * cfg);


#endif /* HWDL_BMA150_H_ */


/*! @} */
