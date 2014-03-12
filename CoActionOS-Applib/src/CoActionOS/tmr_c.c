/*
 * tmr_c.c
 *
 *  Created on: Oct 13, 2012
 *      Author: tgil
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "tmr.h"
#include "util.h"

#define NUM_PORTS HWPL_TMR_PORTS
#define PERIPH_NAME "tmr"

static int fd[HWPL_TMR_PORTS];
static bool isinitialized = false;


int tmr_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int tmr_close(port_t port){
	return phylib_util_close(port, fd);
}

int tmr_getattr(port_t port, tmr_attr_t * attr){
	return ioctl(fd[port], I_TMR_GETATTR, attr);
}

int tmr_setattr(port_t port, const tmr_attr_t * attr){
	return ioctl(fd[port], I_TMR_SETATTR, attr);
}

int tmr_setaction(port_t port, const tmr_action_t * action){
	return ioctl(fd[port], I_TMR_SETACTION, action);
}

int tmr_on(port_t port){
	return ioctl(fd[port], I_TMR_ON);
}

int tmr_off(port_t port){
	return ioctl(fd[port], I_TMR_OFF);
}

int tmr_setoc(port_t port, tmr_reqattr_t * attr){
	return ioctl(fd[port], I_TMR_SETOC, attr);
}

int tmr_setic(port_t port, tmr_reqattr_t * attr){
	return ioctl(fd[port], I_TMR_SETIC, attr);
}

int tmr_getoc(port_t port, tmr_reqattr_t * attr){
	return ioctl(fd[port], I_TMR_GETOC, attr);
}

int tmr_getic(port_t port, tmr_reqattr_t * attr){
	return ioctl(fd[port], I_TMR_GETIC, attr);
}

int tmr_get(port_t port){
	return ioctl(fd[port], I_TMR_GET);
}

int tmr_set(port_t port, int value){
	return ioctl(fd[port], I_TMR_SET, value);
}

