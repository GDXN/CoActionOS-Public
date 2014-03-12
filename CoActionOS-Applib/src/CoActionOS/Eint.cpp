/*
 * Eint.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: tgil
 */


#include <unistd.h>
#include <fcntl.h>
#include "Eint.hpp"
#include "util.h"

#define NUM_PORTS HWPL_EINT_PORTS
#define PERIPH_NAME "eint"

int Eint::fd[HWPL_EINT_PORTS];
bool Eint::isinitialized = false;

Eint::Eint(port_t port) : Periph(port, NUM_PORTS, (int*)&fd, &isinitialized){}

int Eint::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Eint::getattr(eint_attr_t * attr){
	return ioctl(I_EINT_GETATTR, attr);
}

int Eint::setattr(const eint_attr_t * attr){
	return ioctl(I_EINT_SETATTR, attr);
}

int Eint::setaction(const eint_action_t * action){
	return ioctl(I_EINT_SETACTION, action);
}

