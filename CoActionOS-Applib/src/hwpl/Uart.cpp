
#include <stdlib.h>
#include <hwpl/uart.h>
#include <hwdl/uartfifo.h>
#include "util.h"
#include "Uart.hpp"

const device_cfg_t * Uart::fifocfg[HWPL_UART_PORTS];

Uart::Uart(port_t port) : Pchar(port, HWPL_UART_PORTS, NULL, NULL){ flags = 0; }

int Uart::open(int flags){
	this->flags = flags; //set the flags (so that O_NONBLOCK can be saved)
	if( fifocfg[port_] != 0 ){
		return uartfifo_open(fifocfg[port_]);
	}
	return hwpl_uart_open((const device_cfg_t*)&(port_));
}

int Uart::close(){
	if( fifocfg[port_] != 0 ){
		return uartfifo_close(fifocfg[port_]);
	}
	return hwpl_uart_close((const device_cfg_t*)&(port_));
}

int Uart::getattr(uart_attr_t * attr){
	return hwpl_uart_getattr(port_, (void*)attr);
}

int Uart::setattr(const uart_attr_t * attr){
	if( fifocfg[port_] != 0 ){
		return uartfifo_ioctl(fifocfg[port_], I_UART_SETATTR, (void*)attr);
	}
	return hwpl_uart_setattr(port_, (void*)attr);
}

int Uart::getbyte(char * c){
	return hwpl_uart_getbyte(port_, c);
}

int Uart::flush(void){
	if( fifocfg[port_] != 0 ){
		return uartfifo_ioctl(fifocfg[port_], I_UART_FLUSH, NULL);
	}
	return hwpl_uart_flush(port_, NULL);
}


int Uart::read(void * buf, int nbyte){
	if( fifocfg[port_] != 0 ){
		return applib_util_transfer(fifocfg[port_], uartfifo_read, 0, buf, nbyte, flags | APPLIB_UTIL_READ_FLAG);
	}
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_uart_read, 0, buf, nbyte, flags | APPLIB_UTIL_READ_FLAG);
}

int Uart::write(const void * buf, int nbyte){
	if( fifocfg[port_] != 0 ){
		return applib_util_transfer(fifocfg[port_], uartfifo_write, 0, buf, nbyte, flags | APPLIB_UTIL_WRITE_FLAG);
	}
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_uart_write, 0, buf, nbyte, flags | APPLIB_UTIL_WRITE_FLAG);
}



