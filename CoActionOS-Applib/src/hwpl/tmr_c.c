
#include <hwpl/tmr.h>
#include "tmr.h"
#include "util.h"

int tmr_open(port_t port){
	return hwpl_tmr_open((const device_cfg_t*)&(port));
}

int tmr_close(port_t port){
	return hwpl_tmr_close((const device_cfg_t*)&(port));
}

int tmr_getattr(port_t port, tmr_attr_t * attr){
	return hwpl_tmr_getattr(port, attr);
}

int tmr_setattr(port_t port, const tmr_attr_t * attr){
	return hwpl_tmr_setattr(port, (void*)attr);
}

int tmr_setaction(port_t port, const tmr_action_t * action){
	return hwpl_tmr_setaction(port, (void*)action);
}

int tmr_on(port_t port){
	return hwpl_tmr_on(port, NULL);
}

int tmr_off(port_t port){
	return hwpl_tmr_off(port, NULL);
}

int tmr_setoc(port_t port, tmr_reqattr_t * attr){
	return hwpl_tmr_setoc(port, attr);
}

int tmr_setic(port_t port, tmr_reqattr_t * attr){
	return hwpl_tmr_setic(port, attr);
}

int tmr_getoc(port_t port, tmr_reqattr_t * attr){
	return hwpl_tmr_getoc(port, attr);
}

int tmr_getic(port_t port, tmr_reqattr_t * attr){
	return hwpl_tmr_getic(port, attr);
}

int tmr_get(port_t port){
	return hwpl_tmr_get(port, NULL);
}

int tmr_set(port_t port, int value){
	return hwpl_tmr_set(port, (void*)value);
}

