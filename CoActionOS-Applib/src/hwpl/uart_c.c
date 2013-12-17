
#include <stdlib.h>
#include <hwpl/uart.h>
#include <fcntl.h>
#include "uart.h"
#include "util.h"


int uart_open(port_t port){
	return hwpl_uart_open((const device_cfg_t*)&(port));
}

int uart_close(port_t port){
	return hwpl_uart_close((const device_cfg_t*)&(port));
}

int uart_getattr(port_t port, uart_attr_t * attr){
	return hwpl_uart_getattr(port, attr);
}

int uart_setattr(port_t port, const uart_attr_t * attr){
	return hwpl_uart_setattr(port, (void*)attr);
}

int uart_getbyte(port_t port, char * c){
	return hwpl_uart_getbyte(port, c);
}

int uart_flush(port_t port){
	return hwpl_uart_flush(port, NULL);
}

int uart_read(port_t port, void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_uart_read, 0, buf, nbyte, O_RDWR | APPLIB_UTIL_READ_FLAG);
}

int uart_write(port_t port, const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_uart_write, 0, buf, nbyte, O_RDWR | APPLIB_UTIL_WRITE_FLAG);
}
