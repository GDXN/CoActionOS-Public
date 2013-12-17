/*
 * Core.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: tgil
 */


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "core.h"
#include "util.h"


#ifndef HWPL_CORE_PORTS
#define HWPL_CORE_PORTS 1
#endif

#define NUM_PORTS HWPL_CORE_PORTS
#define PERIPH_NAME "core"

static int fd[HWPL_CORE_PORTS];
static bool isinitialized = false;

int core_open(port_t port){
	printf("core open\n");
	return phylib_util_open(PERIPH_NAME, NO_PORT_NUMBER, fd, NUM_PORTS, &isinitialized);
}

int core_close(port_t port){
	return phylib_util_close(port, fd);
}

int core_getattr(port_t port, core_attr_t * attr){
	return ioctl(fd[port], I_CORE_GETATTR, attr);
}

int core_setattr(port_t port, const core_attr_t * attr){
	return ioctl(fd[port], I_CORE_SETATTR, attr);
}

int core_setpinfunc(port_t port, const core_pinfunc_t * req){
	return ioctl(fd[port], I_CORE_SETPINFUNC, req);
}

int core_sleep(port_t port, core_sleep_t level){
	return ioctl(fd[port], I_CORE_SLEEP, (void*)level);
}

void core_reset(port_t port){
	ioctl(fd[port], I_CORE_RESET);
}

void core_invokebootloader(port_t port){
	ioctl(fd[port], I_CORE_INVOKEBOOTLOADER);
}

int core_setirqprio(port_t port, const core_irqprio_t * req){
	return ioctl(fd[port], I_CORE_SETIRQPRIO, req);
}

int core_setclkout(port_t port, const core_clkout_t * clkout){
	return ioctl(fd[port], I_CORE_SETCLKOUT, clkout);
}
