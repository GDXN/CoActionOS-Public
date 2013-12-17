#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "uart.h"
#include "util.h"

#define NUM_PORTS HWPL_UART_PORTS
#define PERIPH_NAME "uart"

static int fd[NUM_PORTS];
static bool isinitialized = false;

int uart_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int uart_close(port_t port){
	return phylib_util_close(port, fd);
}

int uart_getattr(port_t port, uart_attr_t * attr){
	return ioctl(fd[port], I_UART_GETATTR, attr);
}

int uart_setattr(port_t port, const uart_attr_t * attr){
	return ioctl(fd[port], I_UART_SETATTR, attr);
}

int uart_getbyte(port_t port, char * c){
	return ioctl(fd[port], I_UART_GETBYTE, c);
}

int uart_flush(port_t port){
	return ioctl(fd[port], I_UART_FLUSH);
}

int uart_read(port_t port, void * buf, int nbyte){
	return read(fd[port], buf, nbyte);
}

int uart_write(port_t port, const void * buf, int nbyte){
	return write(fd[port], buf, nbyte);
}
