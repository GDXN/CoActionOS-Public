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

#ifndef HWDL_UARTFIFO_H_
#define HWDL_UARTFIFO_H_

#include <stdbool.h>
#include "dev/uartfifo.h"
#include "hwdl.h"



/*! \details This stores the data for the state of the fifo buffer.
 *
 */
typedef struct {
	int head;
	int tail;
	bool overflow;
	device_transfer_t * rop;
	int len;
} uartfifo_state_t;

#ifdef __cplusplus
extern "C" {
#endif

int uartfifo_open(const device_cfg_t * cfg);
int uartfifo_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int uartfifo_read(const device_cfg_t * cfg, device_transfer_t * rop);
int uartfifo_write(const device_cfg_t * cfg, device_transfer_t * wop);
int uartfifo_close(const device_cfg_t * cfg);

#ifdef __cplusplus
}
#endif


#endif /* HWDL_UART_FIFO_H_ */


/*! @} */
