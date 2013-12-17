/*
 * c_spi.c
 *
 *  Created on: Sep 9, 2012
 *      Author: tgil
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>


#include "gpio.h"
#include "util.h"

#define NUM_PORTS HWPL_GPIO_PORTS
#define PERIPH_NAME "gpio"

static int fd[HWPL_GPIO_PORTS];
static bool isinitialized = false;

int gpio_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int gpio_close(port_t port){
	return phylib_util_close(port, fd);
}

int gpio_getattr(port_t port, gpio_attr_t * attr){
	return ioctl(fd[port], I_GPIO_GETATTR, attr);
}

int gpio_setattr(port_t port, gpio_attr_t * attr){
	return ioctl(fd[port], I_GPIO_SETATTR, attr);
}

int gpio_setmask(port_t port, gpio_sample_t mask){
	return ioctl(fd[port], I_GPIO_SETMASK, mask);
}

int gpio_clrmask(port_t port, gpio_sample_t mask){
	return ioctl(fd[port], I_GPIO_CLRMASK, mask);
}

gpio_sample_t gpio_get(port_t port){
	return ioctl(fd[port], I_GPIO_GET);
}

int gpio_set(port_t port, gpio_sample_t value){
	return ioctl(fd[port], I_GPIO_SET, value);
}
