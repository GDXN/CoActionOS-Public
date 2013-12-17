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

#ifndef HWDL_DEVFIFO_H_
#define HWDL_DEVFIFO_H_

#include <stdbool.h>
#include "dev/devfifo.h"
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
} devfifo_state_t;

int devfifo_open(const device_cfg_t * cfg);
int devfifo_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int devfifo_read(const device_cfg_t * cfg, device_transfer_t * rop);
int devfifo_write(const device_cfg_t * cfg, device_transfer_t * wop);
int devfifo_close(const device_cfg_t * cfg);


#endif /* HWDL_UART_FIFO_H_ */


/*! @} */
