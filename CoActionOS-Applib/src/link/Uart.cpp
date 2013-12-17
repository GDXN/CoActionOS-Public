
#include <unistd.h>
#include <fcntl.h>
#include "Uart.hpp"

#include <dev/uart.h>

#define NUM_PORTS 32
#define PERIPH_NAME "uart"

int Uart::fd[NUM_PORTS];
bool Uart::isinitialized = false;

Uart::Uart(port_t port) : Pchar(port, NUM_PORTS, (int*)&fd, &isinitialized) {}

int Uart::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Uart::getattr(uart_attr_t * attr){
	return ioctl(I_UART_GETATTR, attr);
}

int Uart::setattr(const uart_attr_t * attr){
	return ioctl(I_UART_SETATTR, attr);
}

int Uart::getbyte(char * c){
	return ioctl(I_UART_GETBYTE, c);
}

int Uart::flush(void){
	return ioctl(I_UART_FLUSH);
}

