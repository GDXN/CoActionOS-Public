/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <stddef.h>
#include "hwdl/sys.h"


int urandom_open(const device_cfg_t * cfg){
	return 0;
}

int urandom_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return 0;
}

int urandom_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}

int urandom_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int urandom_close(const device_cfg_t * cfg){
	return 0;
}

