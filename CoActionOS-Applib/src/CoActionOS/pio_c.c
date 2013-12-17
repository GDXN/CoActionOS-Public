
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "pio.h"
#include "util.h"

#define NUM_PORTS HWPL_PIO_PORTS
#define PERIPH_NAME "pio"

static int fd[NUM_PORTS];
static bool isinitialized = false;

int pio_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int pio_close(port_t port){
	return phylib_util_close(port, fd);
}

int pio_getattr(port_t port, pio_attr_t * attr){
	return ioctl(fd[port], I_PIO_GETATTR, attr);
}

int pio_setattr(port_t port, const pio_attr_t * attr){
	return ioctl(fd[port], I_PIO_SETATTR, attr);
}

int pio_setaction(port_t port, const pio_action_t * action){
	return ioctl(fd[port], I_PIO_SETACTION, action);
}

int pio_setmask(port_t port, pio_sample_t mask){
	return ioctl(fd[port], I_PIO_SETMASK, (void*)mask);
}

int pio_clrmask(port_t port, pio_sample_t mask){
	return ioctl(fd[port], I_PIO_CLRMASK, (void*)mask);
}

pio_sample_t pio_get(port_t port){
	return ioctl(fd[port], I_PIO_GET);
}

int pio_set(port_t port, pio_sample_t value){
	return ioctl(fd[port], I_PIO_SET, (void*)value);
}
