/*
 * i2c_c.c
 *
 *  Created on: Oct 9, 2012
 *      Author: tgil
 */

#include <unistd.h>
#include <fcntl.h>

#include "i2c.h"
#include "util.h"

#define NUM_PORTS HWPL_I2C_PORTS
#define PERIPH_NAME "i2c"

static int fd[NUM_PORTS];
static bool isinitialized = false;

int i2c_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int i2c_close(port_t port){
	return phylib_util_close(port, fd);
}

/*! \details This function gets the I2C attributes from the specified SPI port.
 *
 */
int i2c_getattr(port_t port, i2c_attr_t * attr){
	return ioctl(fd[port], I_I2C_GETATTR, attr);
}

int i2c_setattr(port_t port, const i2c_attr_t * attr){
	return ioctl(fd[port], I_I2C_SETATTR, attr);
}
int i2c_setup(port_t port, const i2c_reqattr_t * req){
	return ioctl(fd[port], I_I2C_SETUP, req);
}

int i2c_geterr(port_t port){
	return ioctl(fd[port], I_I2C_GETERR);
}

int i2c_read(port_t port, int loc, void * buf, int nbyte){
	lseek(fd[port], loc, SEEK_SET);
	return read(fd[port], buf, nbyte);
}

int i2c_write(port_t port, int loc, const void * buf, int nbyte){
	lseek(fd[port], loc, SEEK_SET);
	return write(fd[port], buf, nbyte);
}
