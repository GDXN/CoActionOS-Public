#include <stdlib.h>
#include <hwpl/i2c.h>
#include "util.h"
#include "I2c.hpp"

I2c::I2c(port_t port) : Pblock(port, HWPL_I2C_PORTS){}

int I2c::open(int flags){
	return hwpl_i2c_open((const device_cfg_t*)&(port_));
}

int I2c::close(){
	return hwpl_i2c_close((const device_cfg_t*)&(port_));

}

int I2c::getattr(i2c_attr_t * attr){
	return hwpl_i2c_getattr(port_, attr);
}

int I2c::setattr(const i2c_attr_t * attr){
	return hwpl_i2c_setattr(port_, (void*)attr);
}

int I2c::setup(const i2c_reqattr_t * req){
	return hwpl_i2c_setup(port_, (void*)req);
}

int I2c::geterr(void){
	return hwpl_i2c_geterr(port_, NULL);
}

int I2c::read(void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_i2c_read, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_READ_FLAG);
}

int I2c::write(const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_i2c_write, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_WRITE_FLAG);
}

