#include <stdlib.h>
#include <hwpl/usb.h>
#include <fcntl.h>
#include "usb.h"
#include "util.h"

int usb_open(port_t port){
	return hwpl_usb_open((const device_cfg_t*)&(port));
}

int usb_close(port_t port){
	return hwpl_usb_close((const device_cfg_t*)&(port));
}

int usb_getattr(port_t port, usb_attr_t * attr){
	return hwpl_usb_getattr(port, attr);
}

int usb_setattr(port_t port, usb_attr_t * attr){
	return hwpl_usb_setattr(port, attr);
}

int usb_reset(port_t port){
	return hwpl_usb_reset(port, NULL);
}

int usb_attach(port_t port){
	return hwpl_usb_attach(port, NULL);
}

int usb_configure(port_t port){
	return hwpl_usb_configure(port, NULL);
}

int usb_detach(port_t port){
	return hwpl_usb_detach(port, NULL);
}

int usb_disableep(port_t port, int ep){
	return hwpl_usb_disableep(port, (void*)ep);
}

int usb_enableep(port_t port, int ep){
	return hwpl_usb_enableep(port, (void*)ep);
}

bool usb_isconnected(port_t port){
	return hwpl_usb_isconnected(port, NULL);
}

int usb_resetep(port_t port, int ep){
	return hwpl_usb_resetep(port, (void*)ep);
}

int usb_setaddr(port_t port, int addr){
	return hwpl_usb_setaddr(port, (void*)addr);
}

int usb_stallep(port_t port, int ep){
	return hwpl_usb_stallep(port, (void*)ep);
}

int usb_unstallep(port_t port, int ep){
	return hwpl_usb_unstallep(port, (void*)ep);
}

int usb_read(port_t port, int loc, void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_usb_read, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_READ_FLAG);
}

int usb_write(port_t port, int loc, const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_usb_write, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_WRITE_FLAG);
}

