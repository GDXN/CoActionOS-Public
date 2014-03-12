/*
 * Eint.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: tgil
 */


#include <hwpl/eint.h>
#include "Eint.hpp"


Eint::Eint(port_t port) : Periph(port, HWPL_EINT_PORTS, NULL, NULL){}

int Eint::open(int flags){
	return hwpl_eint_open((const device_cfg_t*)&(this->port_));
}

int Eint::close(){
	return hwpl_eint_close((const device_cfg_t*)&this->port_);
}

int Eint::getattr(eint_attr_t * attr){
	return hwpl_eint_getattr(this->port_, attr);
}

int Eint::setattr(const eint_attr_t * attr){
	return hwpl_eint_setattr(this->port_, (void*)attr);
}

int Eint::setaction(const eint_action_t * action){
	return hwpl_eint_setaction(this->port_, (void*)action);
}

