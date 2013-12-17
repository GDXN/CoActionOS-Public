
#include <stdlib.h>
#include <hwpl/tmr.h>
#include "util.h"
#include "Tmr.hpp"


Tmr::Tmr(port_t port) : Pchar(port, HWPL_TMR_PORTS, NULL, NULL){}

int Tmr::open(int flags){
	return hwpl_tmr_open((const device_cfg_t*)&(port_));
}

int Tmr::close(){
	return hwpl_tmr_close((const device_cfg_t*)&(port_));
}

int Tmr::getattr(tmr_attr_t * attr){
	return hwpl_tmr_getattr(port_, attr);
}

int Tmr::setattr(const tmr_attr_t * attr){
	return hwpl_tmr_setattr(port_, (void*)attr);
}

int Tmr::setaction(const tmr_action_t * action){
	return hwpl_tmr_setaction(port_, (void*)action);
}

int Tmr::on(void){
	return hwpl_tmr_on(port_, NULL);
}

int Tmr::off(void){
	return hwpl_tmr_off(port_, NULL);
}

int Tmr::setoc(const tmr_reqattr_t * req){
	return hwpl_tmr_setoc(port_, (void*)req);
}

int Tmr::getoc(tmr_reqattr_t * req){
	return hwpl_tmr_getoc(port_, req);
}

int Tmr::setic(const tmr_reqattr_t * req){
	return hwpl_tmr_setic(port_, (void*)req);
}

int Tmr::getic(tmr_reqattr_t * req){
	return hwpl_tmr_getic(port_, req);
}

tmr_sample_t Tmr::get(void){
	return (tmr_sample_t)hwpl_tmr_get(port_, NULL);
}

int Tmr::set(tmr_sample_t value){
	return hwpl_tmr_set(port_, (void*)value);
}

int Tmr::read(void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_tmr_read, 0, buf, nbyte, O_RDWR | APPLIB_UTIL_READ_FLAG);
}

int Tmr::write(const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_tmr_write, 0, buf, nbyte, O_RDWR | APPLIB_UTIL_WRITE_FLAG);
}



