
#include <stdlib.h>
#include <hwpl/qei.h>

#include "qei.h"
#include "util.h"


int qei_open(port_t port){
	return hwpl_qei_open((const device_cfg_t*)&(port));
}

int qei_close(port_t port){
	return hwpl_qei_close((const device_cfg_t*)&(port));
}

int qei_getattr(port_t port, qei_attr_t * attr){
	return hwpl_qei_getattr(port, attr);
}

int qei_setattr(port_t port, const qei_attr_t * attr){
	return hwpl_qei_setattr(port, (void*)attr);
}

int qei_setaction(port_t port, const qei_action_t * action){
	return hwpl_qei_setaction(port, (void*)action);
}

int qei_get(port_t port){
	return hwpl_qei_get(port, NULL);
}

int qei_getvelocity(port_t port){
	return hwpl_qei_getvelocity(port, NULL);
}

int qei_getindex(port_t port){
	return hwpl_qei_getindex(port, NULL);
}

int qei_reset(port_t port, int flags){
	return hwpl_qei_reset(port, (void*)flags);
}


