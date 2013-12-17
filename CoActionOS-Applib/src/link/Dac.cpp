#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "Dac.hpp"


#define NUM_PORTS HWPL_DAC_PORTS
#define PERIPH_NAME "dac"

int Dac::fd[NUM_PORTS];
bool Dac::isinitialized = false;

Dac::Dac(port_t port) : Pblock(port, NUM_PORTS, (int*)&fd, &isinitialized){}

int Dac::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Dac::getattr(dac_attr_t * attr){
	return ioctl(I_DAC_GETATTR, (void*)attr);
}

int Dac::setattr(const dac_attr_t * attr){
	return ioctl(I_DAC_SETATTR, (void*)attr);
}

dac_sample_t Dac::get(void){
	return ioctl(I_DAC_GET);
}

