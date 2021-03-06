/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <stddef.h>
#include "hwdl/sys.h"


int zero_open(const device_cfg_t * cfg){
	return 0;
}

int zero_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return 0;
}

int zero_read(const device_cfg_t * cfg, device_transfer_t * rop){
	memset(rop->buf, 0, rop->nbyte);
	return rop->nbyte;
}

int zero_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return wop->nbyte;
}

int zero_close(const device_cfg_t * cfg){
	return 0;
}

