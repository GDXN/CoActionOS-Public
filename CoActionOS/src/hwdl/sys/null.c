/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <stddef.h>
#include "hwdl/sys.h"


int null_open(const device_cfg_t * cfg){
	return 0;
}

int null_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return 0;
}

int null_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return -1;
}

int null_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return wop->nbyte;
}

int null_close(const device_cfg_t * cfg){
	return 0;
}

