#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "I2c.hpp"


#define NUM_PORTS HWPL_I2C_PORTS
#define PERIPH_NAME "i2c"

int I2c::fd[NUM_PORTS];
bool I2c::isinitialized = false;

I2c::I2c(port_t port) : Pblock(port, NUM_PORTS, (int*)&(fd), &isinitialized){}

int I2c::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int I2c::getattr(i2c_attr_t * attr){
	return ioctl(I_I2C_GETATTR, attr);
}

int I2c::setattr(const i2c_attr_t * attr){
	return ioctl(I_I2C_SETATTR, attr);
}

int I2c::setup(const i2c_reqattr_t * req){
	return ioctl(I_I2C_SETUP, req);
}

int I2c::geterr(void){
	return ioctl(I_I2C_GETERR);
}

