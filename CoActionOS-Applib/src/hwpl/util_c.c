/*
 * periph_c.c
 *
 *  Created on: Oct 13, 2012
 *      Author: tgil
 */

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <hwpl/core.h>

#include "util.h"


static int applib_util_opcomplete(void * context, const void * data);


int applib_util_opcomplete(void * context, const void * data){
	bool * done = context;
	*done = true;
	return 0;
}

int applib_util_transfer(const device_cfg_t * cfg,
		int (*func)(const device_cfg_t * cfg, device_transfer_t * rop),
		int loc,
		const void * buf,
		int nbyte,
		int flags){
	device_transfer_t op;
	volatile bool done;
	bool isread = ((flags & APPLIB_UTIL_READ_FLAG) == APPLIB_UTIL_READ_FLAG);
	int ret;

	if( nbyte == 0 ){
		return 0;
	}

	done = false;
	op.cbuf = buf;
	op.loc = loc;
	op.flags = flags | O_RDWR;
	op.nbyte = nbyte;
	op.context = (void*)&done;
	op.callback = applib_util_opcomplete;

	isread = false;

	ret = func(cfg, &op);

	if( ret == 0 ){

		while( done == false ){
			_hwpl_core_sleep(CORE_SLEEP);
		}

		if( (isread == true) && (op.nbyte == 0) ){
			op.nbyte = nbyte;
			return func(cfg, &op);
		}
		return op.nbyte;
	}
	return ret;

}

int applib_util_aio_complete(void * context, const void * data){
	device_transfer_t * aio = (device_transfer_t *)context;
	aio->flags &= ~APPLIB_AIO_FLAGS_BUSY;
	return 0;
}

int applib_util_aio_transfer(const device_cfg_t * cfg,
		int (*func)(const device_cfg_t *, device_transfer_t *),
		device_transfer_t * aio){
	int ret;

	aio->flags = O_RDWR | APPLIB_AIO_FLAGS_BUSY;
	aio->callback = applib_util_aio_complete;
	aio->context = aio;

	ret = func(cfg, aio);
	return ret;
}

