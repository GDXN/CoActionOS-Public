/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYS_DEVFIFO
 * @{
 *
 *
 */

/*! \file  */

#ifndef HWDL_USBFIFO_H_
#define HWDL_USBFIFO_H_

#include <stdbool.h>
#include "dev/usbfifo.h"
#include "hwdl.h"
#include "hwpl/circ_buf.h"



/*! \details This stores the data for the state of the fifo buffer.
 *
 */
typedef struct {
	int head;
	int tail;
	bool overflow;
	device_transfer_t * rop;
	int len;
} usbfifo_state_t;

int usbfifo_open(const device_cfg_t * cfg);
int usbfifo_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int usbfifo_read(const device_cfg_t * cfg, device_transfer_t * rop);
int usbfifo_write(const device_cfg_t * cfg, device_transfer_t * wop);
int usbfifo_close(const device_cfg_t * cfg);


#endif /* HWDL_USBFIFO_H_ */


/*! @} */
