
#include <stdlib.h>
#include <hwpl/adc.h>
#include <fcntl.h>
#include "adc.h"
#include "util.h"


int adc_open(port_t port){
	return hwpl_adc_open((const device_cfg_t*)&(port));
}

int adc_close(port_t port){
	return hwpl_adc_close((const device_cfg_t*)&(port));
}

int adc_getattr(port_t port, adc_attr_t * attr){
	return hwpl_adc_getattr(port, attr);
}

int adc_setattr(port_t port, const adc_attr_t * attr){
	return hwpl_adc_setattr(port, (void*)attr);
}

int adc_read(port_t port, int chan, void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_adc_read, chan, buf, nbyte, O_RDWR | APPLIB_UTIL_READ_FLAG);
}


