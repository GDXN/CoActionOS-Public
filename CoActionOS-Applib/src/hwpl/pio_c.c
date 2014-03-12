
#include <stdlib.h>
#include <hwpl/pio.h>

#include "pio.h"
#include "util.h"


int pio_open(port_t port){
	return hwpl_pio_open((const device_cfg_t*)&(port));
}

int pio_close(port_t port){
	return hwpl_pio_close((const device_cfg_t*)&(port));
}

int pio_getattr(port_t port, pio_attr_t * attr){
	return hwpl_pio_getattr(port, attr);
}

int pio_setattr(port_t port, const pio_attr_t * attr){
	return hwpl_pio_setattr(port, (void*)attr);
}

int pio_setaction(port_t port, const pio_action_t * action){
	return hwpl_pio_setaction(port, (void*)action);
}

int pio_setmask(port_t port, pio_sample_t mask){
	return hwpl_pio_setmask(port, (void*)(uint32_t)mask);
}

int pio_clrmask(port_t port, pio_sample_t mask){
	return hwpl_pio_clrmask(port, (void*)(uint32_t)mask);
}

pio_sample_t pio_get(port_t port){
	return hwpl_pio_get(port, NULL);
}

int pio_set(port_t port, pio_sample_t value){
	return hwpl_pio_set(port, (void*)(uint32_t)value);
}
