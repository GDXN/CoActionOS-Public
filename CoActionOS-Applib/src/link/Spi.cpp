#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "Spi.hpp"

#define NUM_PORTS HWPL_SPI_PORTS
#define PERIPH_NAME "spi"

int Spi::fd[NUM_PORTS];
bool Spi::isinitialized = false;


Spi::Spi(port_t port) : Pchar(port, NUM_PORTS, (int*)&fd, &isinitialized){}

int Spi::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Spi::getattr(spi_attr_t * attr){
	return ioctl(I_SPI_GETATTR, attr);
}

int Spi::setattr(const spi_attr_t * attr){
	return ioctl(I_SPI_SETATTR, attr);
}

int Spi::swap(int byte){
	return ioctl(I_SPI_SWAP, byte);
}

int Spi::setduplex(void * buf){
	return ioctl(I_SPI_SETDUPLEX, buf);
}


