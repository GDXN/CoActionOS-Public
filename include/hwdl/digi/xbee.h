/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup XBEE XBee RF Transceiver
 * @{
 *
 *
 */

/*! \file  */

#ifndef HWDL_XBEE_H_
#define HWDL_XBEE_H_

#include "hwdl.h"

int xbee_open(const device_cfg_t * cfg);
int xbee_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int xbee_read(const device_cfg_t * cfg, device_transfer_t * rop);
int xbee_write(const device_cfg_t * cfg, device_transfer_t * wop);
int xbee_close(const device_cfg_t * cfg);


#endif /* HWDL_XBEE_H_ */


/*! @} */
