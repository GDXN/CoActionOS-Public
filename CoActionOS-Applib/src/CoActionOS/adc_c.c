
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "adc.h"
#include "util.h"

#define NUM_PORTS HWPL_ADC_PORTS
#define PERIPH_NAME "adc"

static int fd[NUM_PORTS];
static bool isinitialized = false;

int adc_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int adc_close(port_t port){
	return phylib_util_close(port, fd);
}

int adc_getattr(port_t port, adc_attr_t * attr){
	return ioctl(fd[port], I_ADC_GETATTR, attr);
}

int adc_setattr(port_t port, const adc_attr_t * attr){
	return ioctl(fd[port], I_ADC_SETATTR, attr);
}

int adc_read(port_t port, int chan, void * buf, int nbyte){
	lseek(fd[port], chan, SEEK_SET);
	return read(fd[port], buf, nbyte);
}

