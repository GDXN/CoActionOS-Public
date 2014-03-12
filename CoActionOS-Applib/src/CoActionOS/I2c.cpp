#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "util.h"
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


int I2c::read(int loc, uint8_t & reg){
	int ret;
	ret = read(loc, &reg, 1);
	return ret;
}

int I2c::write(int loc, uint8_t reg){
	int ret;
	ret = write(loc, &reg, 1);
	return ret;
}


int I2c::set(int loc, int bit, bool high){
	int ret;
	uint8_t tmp;
	ret = read(loc, tmp);
	if( ret < 0 ){
		return ret;
	}
	if ( high == true ){
		tmp |= (1<<bit);
	} else {
		tmp &= ~(1<<bit);
	}
	return write(loc, tmp);
}
