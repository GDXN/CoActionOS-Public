
#include <stdlib.h>
#include <hwpl/pwm.h>
#include <fcntl.h>

#include "pwm.h"
#include "util.h"


int pwm_open(port_t port){
	return hwpl_pwm_open((const device_cfg_t*)&(port));
}


int pwm_close(port_t port){
	return hwpl_pwm_close((const device_cfg_t*)&(port));
}

int pwm_getattr(port_t port, pwm_attr_t * attr){
	return hwpl_pwm_getattr(port, attr);
}

int pwm_setattr(port_t port, const pwm_attr_t * attr){
	return hwpl_pwm_setattr(port, (void*)attr);
}

int pwm_set(port_t port, const pwm_reqattr_t * req){
	return hwpl_pwm_set(port, (void*)req);
}

int pwm_write(port_t port, int loc, const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_pwm_write, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_WRITE_FLAG);

}
