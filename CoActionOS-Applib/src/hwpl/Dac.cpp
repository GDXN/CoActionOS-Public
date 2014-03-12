
#include <hwpl/dac.h>
#include "Dac.hpp"
#include "util.h"

Dac::Dac(port_t port) : Pblock(port, HWPL_DAC_PORTS){}

int Dac::open(int flags){
	return hwpl_dac_open((const device_cfg_t*)&(this->port_));
}

int Dac::close(){
	return hwpl_dac_close((const device_cfg_t*)&this->port_);
}

int Dac::getattr(dac_attr_t * attr){
	return hwpl_dac_getattr(this->port_, attr);
}

int Dac::setattr(const dac_attr_t * attr){
	return hwpl_dac_setattr(this->port_, (void*)attr);
}

dac_sample_t Dac::get(void){
	return hwpl_dac_get(this->port_, NULL);
}

int Dac::write(void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_dac_write, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_WRITE_FLAG);
}
