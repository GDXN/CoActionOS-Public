/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup AT25DF
 * @{
 *

 */

/*! \file */

#ifndef HWDL_AT25DF_H_
#define HWDL_AT25DF_H_

#include <stdint.h>
#include "hwdl.h"
#include "dev/atmel/at25df.h"


/*! \details This defines the device specific volatile configuration.
 *
 */
typedef struct {
	const char * buf;
	int nbyte;
	uint8_t cmd[8];
	device_transfer_t op;
	hwpl_callback_t callback;
	void * context;
} at25df_state_t;

int at25df_open(const device_cfg_t * cfg);
int at25df_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int at25df_read(const device_cfg_t * cfg, device_transfer_t * rop);
int at25df_write(const device_cfg_t * cfg, device_transfer_t * wop);
int at25df_close(const device_cfg_t * cfg);


#endif /* AT25DF_H_ */


/*! @} */
