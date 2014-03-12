
#include <unistd.h>
#include <fcntl.h>

#include "eint.h"
#include "util.h"

#define NUM_PORTS HWPL_EINT_PORTS
#define PERIPH_NAME "eint"

static int fd[NUM_PORTS];
static bool isinitialized = false;

int eint_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int eint_close(port_t port){
	return phylib_util_close(port, fd);
}

int eint_getattr(port_t port, eint_attr_t * attr){
	return ioctl(fd[port], I_EINT_GETATTR, attr);
}

int eint_setattr(port_t port, const eint_attr_t * attr){
	return ioctl(fd[port], I_EINT_SETATTR, attr);

}

int eint_setaction(port_t port, const eint_action_t * action){
	return ioctl(fd[port], I_EINT_SETACTION, action);
}
