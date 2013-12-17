/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYS_FIFO
 * @{
 *
 */

/*! \file  */

#ifndef HWDL_FIFO_H_
#define HWDL_FIFO_H_

#include <stdbool.h>
#include "hwdl.h"
#include "dev/fifo.h"

typedef struct {
	int head;
	int tail;
	device_transfer_t * rop;
	int rop_len;
	device_transfer_t * wop;
	int wop_len;
	int write_block;
} fifo_state_t;

int fifo_open(const device_cfg_t * cfg);
int fifo_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int fifo_read(const device_cfg_t * cfg, device_transfer_t * rop);
int fifo_write(const device_cfg_t * cfg, device_transfer_t * wop);
int fifo_close(const device_cfg_t * cfg);


#endif /* HWDL_FIFO_H_ */


/*! @} */
