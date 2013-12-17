#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "Pwm.hpp"

#define NUM_PORTS HWPL_PWM_PORTS
#define PERIPH_NAME "pwm"

int Pwm::fd[NUM_PORTS];
bool Pwm::isinitialized = false;

Pwm::Pwm(port_t port) : Pblock(port, NUM_PORTS, (int*)&fd, &isinitialized){}

int Pwm::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Pwm::getattr(pwm_attr_t * attr){
	return ioctl(I_PWM_GETATTR, attr);
}

int Pwm::setattr(const pwm_attr_t * attr){
	return ioctl(I_PWM_SETATTR, attr);
}

int Pwm::set(const pwm_reqattr_t * req){
	return ioctl(I_PWM_SET, req);
}


