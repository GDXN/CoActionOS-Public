
#include <stdlib.h>
#include <hwpl/pwm.h>
#include "Pwm.hpp"
#include "util.h"


Pwm::Pwm(port_t port) : Pblock(port, HWPL_PWM_PORTS){}

int Pwm::open(int flags){
	return hwpl_pwm_open((const device_cfg_t*)&(this->port_));
}

int Pwm::close(void){
	return hwpl_pwm_close((const device_cfg_t*)&(this->port_));
}

int Pwm::getattr(pwm_attr_t * attr){
	return hwpl_pwm_getattr(this->port_, (void*)attr);
}

int Pwm::setattr(const pwm_attr_t * attr){
	return hwpl_pwm_setattr(this->port_, (void*)attr);
}

int Pwm::set(const pwm_reqattr_t * req){
	return hwpl_pwm_set(this->port_, (void*)req);
}

int Pwm::write(const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_pwm_write, loc, (void*)buf, nbyte, O_RDWR);
}

