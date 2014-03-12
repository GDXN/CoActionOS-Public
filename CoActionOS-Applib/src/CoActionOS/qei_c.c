
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "qei.h"
#include "util.h"

#define NUM_PORTS HWPL_QEI_PORTS
#define PERIPH_NAME "qei"

static int fd[NUM_PORTS];
static bool isinitialized = false;

int qei_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int qei_close(port_t port){
	return phylib_util_close(port, fd);
}

int qei_getattr(port_t port, qei_attr_t * attr){
	return ioctl(fd[port], I_QEI_GETATTR, attr);
}

int qei_setattr(port_t port, const qei_attr_t * attr){
	return ioctl(fd[port], I_QEI_SETATTR, attr);
}

int qei_setaction(port_t port, const qei_action_t * action){
	return ioctl(fd[port], I_QEI_SETACTION);
}

int qei_get(port_t port){
	return ioctl(fd[port], I_QEI_GET);
}

int qei_getvelocity(port_t port){
	return ioctl(fd[port], I_QEI_GETVELOCITY);
}

int qei_getindex(port_t port){
	return ioctl(fd[port], I_QEI_GETINDEX);
}

int qei_reset(port_t port, int flags){
	return ioctl(fd[port], I_QEI_RESET, flags);
}



