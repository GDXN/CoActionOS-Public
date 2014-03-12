
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "pwm.h"
#include "util.h"

#define NUM_PORTS HWPL_PWM_PORTS
#define PERIPH_NAME "pwm"

static int fd[NUM_PORTS];
static bool isinitialized = false;

int pwm_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int pwm_close(port_t port){
	return phylib_util_close(port, fd);
}

int pwm_getattr(port_t port, pwm_attr_t * attr){
	return ioctl(fd[port], I_PWM_GETATTR, attr);
}

int pwm_setattr(port_t port, const pwm_attr_t * attr){
	return ioctl(fd[port], I_PWM_SETATTR, attr);
}

int pwm_setaction(port_t port, const pwm_action_t * action){
	return ioctl(fd[port], I_PWM_SETACTION, action);
}

int pwm_set(port_t port, const pwm_reqattr_t * req){
	return ioctl(fd[port], I_PWM_SET, req);
}

int pwm_write(port_t port, int loc, const void * buf, int nbyte){
	lseek(fd[port], loc, SEEK_SET);
	return write(fd[port], buf, nbyte);
}

