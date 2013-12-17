
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include "Pio.hpp"
#include "util.h"

#define NUM_PORTS HWPL_PIO_PORTS
#define PERIPH_NAME "pio"

int Pio::fd[NUM_PORTS];
bool Pio::isinitialized = false;

Pio::Pio(port_t port) : Periph(port, NUM_PORTS, (int*)&fd, &isinitialized){}

int Pio::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Pio::getattr(pio_attr_t * attr){
	return ioctl(I_PIO_GETATTR, attr);
}

int Pio::setattr(const pio_attr_t * attr){
	return ioctl(I_PIO_SETATTR, attr);
}

int Pio::setaction(const pio_action_t * action){
	return ioctl(I_PIO_SETACTION, action);
}

int Pio::setmask(unsigned int mask){
	return ioctl(I_PIO_SETMASK, mask);
}

int Pio::clrmask(unsigned int mask){
	return ioctl(I_PIO_CLRMASK, mask);
}

unsigned int Pio::get(void){
	return ioctl(I_PIO_GET);
}

int Pio::set(unsigned int value){
	return ioctl(I_PIO_SET, value);
}

