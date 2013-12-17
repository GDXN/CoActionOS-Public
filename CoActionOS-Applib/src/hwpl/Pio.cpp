/*
 * Pio.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: tgil
 */


#include <hwpl/pio.h>
#include "Pio.hpp"


Pio::Pio(port_t port) : Periph(port, HWPL_PIO_PORTS, NULL, NULL){}

int Pio::open(int flags){
	return hwpl_pio_open((const device_cfg_t*)&(this->port_));
}

int Pio::close(){
	return hwpl_pio_close((const device_cfg_t*)&this->port_);
}

int Pio::getattr(pio_attr_t * attr){
	return hwpl_pio_getattr(this->port_, attr);
}

int Pio::setattr(const pio_attr_t * attr){
	return hwpl_pio_setattr(this->port_, (void*)attr);
}

int Pio::setaction(const pio_action_t * action){
	return hwpl_pio_setaction(this->port_, (void*)action);
}

int Pio::setmask(unsigned int mask){
	return hwpl_pio_setmask(this->port_, (void*)mask);
}

int Pio::clrmask(unsigned int mask){
	return hwpl_pio_clrmask(this->port_, (void*)mask);
}

unsigned int Pio::get(void){
	return hwpl_pio_get(this->port_, NULL);
}

int Pio::set(unsigned int value){
	return hwpl_pio_set(this->port_, (void*)value);
}
