/*
 * Adc.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: tgil
 */


#include <hwpl/adc.h>
#include "Adc.hpp"
#include "util.h"

Adc::Adc(port_t port) : Pblock(port, HWPL_ADC_PORTS){}

int Adc::open(int flags){
	return hwpl_adc_open((const device_cfg_t*)&(this->port_));
}

int Adc::close(){
	return hwpl_adc_close((const device_cfg_t*)&this->port_);
}

int Adc::getattr(adc_attr_t * attr){
	return hwpl_adc_getattr(this->port_, attr);
}

int Adc::setattr(const adc_attr_t * attr){
	return hwpl_adc_setattr(this->port_, (void*)attr);
}

int Adc::read(void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_adc_read, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_READ_FLAG);
}
