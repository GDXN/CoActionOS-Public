/*
 * c_spi.c
 *
 *  Created on: Sep 9, 2012
 *      Author: tgil
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "spi.h"
#include "util.h"

#define NUM_PORTS HWPL_SPI_PORTS
#define PERIPH_NAME "spi"

static int fd[HWPL_SPI_PORTS];
static bool isinitialized = false;

int spi_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int spi_close(port_t port){
	return phylib_util_close(port, fd);
}

int spi_getattr(port_t port, spi_attr_t * attr){
	return ioctl(fd[port], I_SPI_GETATTR, attr);
}

int spi_setattr(port_t port, spi_attr_t * attr){
	return ioctl(fd[port], I_SPI_SETATTR, attr);
}

int spi_swap(port_t port, int byte){
	return ioctl(fd[port], I_SPI_SWAP, byte);
}

int spi_setduplex(port_t port, void * buf){
	return ioctl(fd[port], I_SPI_SETDUPLEX, buf);
}

int spi_read(port_t port, void * buf, int nbyte){
	return read(fd[port], buf, nbyte);
}

int spi_write(port_t port, const void * buf, int nbyte){
	return write(fd[port], buf, nbyte);
}
