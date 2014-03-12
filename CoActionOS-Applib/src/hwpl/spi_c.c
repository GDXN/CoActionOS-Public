
#include <stdlib.h>
#include <hwpl/spi.h>
#include <fcntl.h>
#include "spi.h"
#include "util.h"


int spi_open(port_t port){
	return hwpl_spi_close((const device_cfg_t*)&(port));
}

int spi_close(port_t port){
	return hwpl_spi_close((const device_cfg_t*)&(port));
}

int spi_getattr(port_t port, spi_attr_t * attr){
	return hwpl_spi_getattr(port, attr);
}

int spi_setattr(port_t port, spi_attr_t * attr){
	return hwpl_spi_setattr(port, attr);
}

int spi_swap(port_t port, int byte){
	return hwpl_spi_swap(port, (void*)byte);
}

int spi_setduplex(port_t port, void * buf){
	return hwpl_spi_setduplex(port, buf);
}

int spi_read(port_t port, void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_spi_read, 0, buf, nbyte, O_RDWR | APPLIB_UTIL_READ_FLAG);
}

int spi_write(port_t port, const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_spi_write, 0, buf, nbyte, O_RDWR | APPLIB_UTIL_WRITE_FLAG);
}
