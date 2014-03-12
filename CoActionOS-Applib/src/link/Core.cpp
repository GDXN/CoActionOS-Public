
#include <unistd.h>
#include <fcntl.h>
#include "Core.hpp"

#define NUM_PORTS HWPL_CORE_PORTS
#define PERIPH_NAME "core"


int Core::fd[NUM_PORTS];
bool Core::isinitialized = false;

const int port = 0;

Core::Core(port_t port) : Periph(port, NUM_PORTS, (int*)&fd, &isinitialized){}

int Core::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Core::getattr(core_attr_t * attr){
	return ioctl(I_CORE_GETATTR, attr);
}

int Core::setattr(const core_attr_t * attr){
	return ioctl(I_CORE_SETATTR, attr);
}

int Core::setpinfunc(const core_pinfunc_t * req){
	return ioctl(I_CORE_SETPINFUNC, req);
}

int Core::sleep(core_sleep_t level){
	return ioctl(I_CORE_SLEEP, level);
}

void Core::reset(void){
	ioctl(I_CORE_RESET);
}

void Core::invokebootloader(void){
	ioctl(I_CORE_INVOKEBOOTLOADER);
}

int Core::setirqprio(const core_irqprio_t * req){
	return ioctl(I_CORE_SETIRQPRIO, req);
}

int Core::setclkout(const core_clkout_t * clkout){
	return ioctl(I_CORE_SETCLKOUT, clkout);
}
