#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "util.h"
#include "Adc.hpp"

#define NUM_PORTS HWPL_ADC_PORTS
#define PERIPH_NAME "adc"

int Adc::fd[NUM_PORTS];
bool Adc::isinitialized = false;

Adc::Adc(port_t port) : Pblock(port, NUM_PORTS, (int*)&fd, &isinitialized){}

int Adc::getports(){
	return NUM_PORTS;
}

int Adc::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Adc::getattr(adc_attr_t * attr){
	return ioctl(I_ADC_GETATTR, attr);
}

int Adc::setattr(const adc_attr_t * attr){
	return ioctl(I_ADC_SETATTR, (void*)attr);
}

