#include <unistd.h>
#include <fcntl.h>
#include "usb.h"
#include "util.h"

#define NUM_PORTS HWPL_USB_PORTS
#define PERIPH_NAME "usb"

static int fd[NUM_PORTS];
static bool isinitialized = false;

int usb_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int usb_close(port_t port){
	return phylib_util_close(port, fd);
}

int usb_getattr(port_t port, usb_attr_t * attr){
	return ioctl(fd[port], I_USB_GETATTR, attr);
}

int usb_setattr(port_t port, usb_attr_t * attr){
	return ioctl(fd[port], I_USB_SETATTR, attr);
}

int usb_reset(port_t port){
	return ioctl(fd[port], I_USB_RESET, NULL);
}

int usb_attach(port_t port){
	return ioctl(fd[port], I_USB_ATTACH, NULL);
}

int usb_configure(port_t port){
	return ioctl(fd[port], I_USB_CONFIGURE, NULL);
}

int usb_detach(port_t port){
	return ioctl(fd[port], I_USB_DETACH, NULL);
}

int usb_disableep(port_t port, int ep){
	return ioctl(fd[port], I_USB_DISABLEEP, (void*)ep);
}

int usb_enableep(port_t port, int ep){
	return ioctl(fd[port], I_USB_ENABLEEP, (void*)ep);
}

bool usb_isconnected(port_t port){
	return ioctl(fd[port], I_USB_ISCONNECTED, NULL);
}

int usb_resetep(port_t port, int ep){
	return ioctl(fd[port], I_USB_RESETEP, (void*)ep);
}

int usb_setaddr(port_t port, int addr){
	return ioctl(fd[port], I_USB_SETADDR, (void*)addr);
}

int usb_stallep(port_t port, int ep){
	return ioctl(fd[port], I_USB_STALLEP, (void*)ep);
}

int usb_unstallep(port_t port, int ep){
	return ioctl(fd[port], I_USB_UNSTALLEP, (void*)ep);
}

int usb_read(port_t port, int loc, void * buf, int nbyte){
	return read(fd[port], buf, nbyte);
}

int usb_write(port_t port, int loc, const void * buf, int nbyte){
	return write(fd[port], buf, nbyte);
}

