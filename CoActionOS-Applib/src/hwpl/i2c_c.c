/*
 * i2c_c.c
 *
 *  Created on: Oct 9, 2012
 *      Author: tgil
 */

#include <stdlib.h>
#include <hwpl/i2c.h>
#include <fcntl.h>

#include "i2c.h"
#include "util.h"


int i2c_open(port_t port){
	return hwpl_i2c_open((const device_cfg_t*)&(port));
}


int i2c_close(port_t port){
	return hwpl_i2c_close((const device_cfg_t*)&(port));
}

int i2c_getattr(port_t port, i2c_attr_t * attr){
	return hwpl_i2c_getattr(port, attr);
}

int i2c_setattr(port_t port, const i2c_attr_t * attr){
	return hwpl_i2c_setattr(port, (void*)attr);
}
int i2c_setup(port_t port, const i2c_reqattr_t * req){
	return hwpl_i2c_setup(port, (void*)req);
}

int i2c_geterr(port_t port){
	return hwpl_i2c_geterr(port, NULL);
}

int i2c_read(port_t port, int loc, void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_i2c_read, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_READ_FLAG);
}

int i2c_write(port_t port, int loc, const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_i2c_write, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_WRITE_FLAG);

}
