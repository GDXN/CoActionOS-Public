/*
 * Core.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: tgil
 */


#include <hwpl/core.h>
#include "Core.hpp"


Core::Core(port_t port) : Periph(port, 1, NULL, NULL){}

int Core::open(int flags){
	return hwpl_core_open((const device_cfg_t*)&(this->port_));
}

int Core::close(){
	return hwpl_core_close((const device_cfg_t*)&this->port_);
}

int Core::getattr(core_attr_t * attr){
	return hwpl_core_getattr(this->port_, attr);
}

int Core::setattr(const core_attr_t * attr){
	return hwpl_core_setattr(this->port_, (void*)attr);
}

int Core::setpinfunc(const core_pinfunc_t * req){
	return hwpl_core_setpinfunc(port_, (void*)req);
}

int Core::sleep(core_sleep_t level){
	return hwpl_core_sleep(port_, (void*)level);
}

void Core::reset(void){
	hwpl_core_reset(port_, NULL);
}

void Core::invokebootloader(void){
	hwpl_core_invokebootloader(port_, NULL);
}

int Core::setirqprio(const core_irqprio_t * req){
	return hwpl_core_setirqprio(port_, (void*)req);
}

int Core::setclkout(const core_clkout_t * clkout){
	return hwpl_core_setclkout(port_, (void*)clkout);
}
