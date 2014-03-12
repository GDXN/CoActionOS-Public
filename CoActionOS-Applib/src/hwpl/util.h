/*
 * utili.h
 *
 *  Created on: Oct 13, 2012
 *      Author: tgil
 */

#ifndef UTILI_H_
#define UTILI_H_

#include <stdbool.h>
#include <device.h>
#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APPLIB_UTIL_READ_FLAG (1<<16)
#define APPLIB_UTIL_WRITE_FLAG (0)


#define APPLIB_AIO_FLAGS_BUSY (1<<16)


int applib_util_transfer(const device_cfg_t * cfg,
		int (*func)(const device_cfg_t *, device_transfer_t *),
		int loc,
		const void * buf,
		int nbyte,
		int flags);

int applib_util_aio_transfer(const device_cfg_t * cfg,
		int (*func)(const device_cfg_t *, device_transfer_t *),
		device_transfer_t * aio);


#ifdef __cplusplus
}
#endif

#endif /* UTILI_H_ */
