
#include <unistd.h>
#include <stdlib.h>
#include <hwpl/tmr.h>
#include "util.h"
#include "Tmr.hpp"

#define NUM_PORTS HWPL_TMR_PORTS
#define PERIPH_NAME "tmr"

int Tmr::fd[NUM_PORTS];
bool Tmr::isinitialized = false;
//bool isinitialized;

Tmr::Tmr(port_t port) : Pchar(port, NUM_PORTS, (int*)&fd, &isinitialized) {}

int Tmr::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Tmr::getattr(tmr_attr_t * attr){
	return ioctl(I_TMR_GETATTR, attr);
}

int Tmr::setattr(const tmr_attr_t * attr){
	return ioctl(I_TMR_SETATTR, attr);
}

int Tmr::setaction(const tmr_action_t * action){
	return ioctl(I_TMR_SETACTION, action);
}

int Tmr::on(void){
	return ioctl(I_TMR_ON);
}

int Tmr::off(void){
	return ioctl(I_TMR_OFF);
}

int Tmr::setoc(const tmr_reqattr_t * req){
	return ioctl(I_TMR_SETOC, req);
}

int Tmr::getoc(tmr_reqattr_t * req){
	return ioctl(I_TMR_GETOC, req);
}

int Tmr::setic(const tmr_reqattr_t * req){
	return ioctl(I_TMR_SETIC, req);
}

int Tmr::getic(tmr_reqattr_t * req){
	return ioctl(I_TMR_GETIC, req);}

tmr_sample_t Tmr::get(void){
	return ioctl(I_TMR_GET);
}

int Tmr::set(tmr_sample_t value){
	return ioctl(I_TMR_SET, value);
}


