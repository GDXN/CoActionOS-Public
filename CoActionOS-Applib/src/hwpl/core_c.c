/*
 * Core.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: tgil
 */


#include <hwpl/core.h>
#include "core.h"


int core_open(port_t port){
	return hwpl_core_open((const device_cfg_t*)&(port));
}

int core_close(port_t port){
	return hwpl_core_close((const device_cfg_t*)&port);
}

int core_getattr(port_t port, core_attr_t * attr){
	return hwpl_core_getattr(port, attr);
}

int core_setattr(port_t port, const core_attr_t * attr){
	return hwpl_core_setattr(port, (void*)attr);
}

int core_setpinfunc(port_t port, const core_pinfunc_t * req){
	return hwpl_core_setpinfunc(port, (void*)req);
}

int core_sleep(port_t port, core_sleep_t level){
	return hwpl_core_sleep(port, (void*)level);
}

void core_reset(port_t port){
	hwpl_core_reset(port, NULL);
}

void core_invokebootloader(port_t port){
	hwpl_core_invokebootloader(port, NULL);
}

int core_setirqprio(port_t port, const core_irqprio_t * req){
	return hwpl_core_setirqprio(port, (void*)req);
}

int core_setclkout(port_t port, const core_clkout_t * clkout){
	return hwpl_core_setclkout(port, (void*)clkout);
}

