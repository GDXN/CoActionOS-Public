/*
 * Gpio.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: tgil
 */

#include <unistd.h>
#include <fcntl.h>
#include "Gpio.hpp"
#include "util.h"

#define NUM_PORTS HWPL_GPIO_PORTS
#define PERIPH_NAME "gpio"


int Gpio::fd[NUM_PORTS];
bool Gpio::isinitialized = false;

Gpio::Gpio(port_t port) : Periph(port, NUM_PORTS, (int*)&fd, &isinitialized){}

int Gpio::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Gpio::getattr(gpio_attr_t * attr){
	return ioctl(I_GPIO_GETATTR, attr);
}

int Gpio::setattr(gpio_attr_t * attr){
	return ioctl(I_GPIO_SETATTR, attr);
}

int Gpio::setmask(unsigned int mask){
	return ioctl(I_GPIO_SETMASK, mask);

}

int Gpio::clrmask(unsigned int mask){
	return ioctl(I_GPIO_CLRMASK, mask);
}

unsigned int Gpio::get(void){
	return ioctl(I_GPIO_GET);

}

int Gpio::set(unsigned int value){
	return ioctl(I_GPIO_SET, value);
}
