
#include <stdlib.h>
#include <hwpl/spi.h>
#include "Spi.hpp"
#include "util.h"


Spi::Spi(port_t port) : Pchar(port, HWPL_SPI_PORTS, NULL, NULL){}

int Spi::open(int flags){
	return hwpl_spi_open((const device_cfg_t*)&(this->port_));
}

int Spi::close(){
	return hwpl_spi_close((const device_cfg_t*)&(this->port_));
}

int Spi::getattr(spi_attr_t * attr){
	return hwpl_spi_getattr(this->port_, attr);
}

int Spi::setattr(const spi_attr_t * attr){
	return hwpl_spi_setattr(this->port_, (void*)attr);
}

int Spi::swap(int byte){
	return hwpl_spi_swap(this->port_, (void*)byte);
}

int Spi::setduplex(void * buf){
	return hwpl_spi_setduplex(this->port_, buf);
}

int Spi::read(void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_spi_read, 0, buf, nbyte, O_RDWR | APPLIB_UTIL_READ_FLAG);
}

int Spi::write(const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_spi_write, 0, (void*)buf, nbyte, O_RDWR | APPLIB_UTIL_WRITE_FLAG);
}

