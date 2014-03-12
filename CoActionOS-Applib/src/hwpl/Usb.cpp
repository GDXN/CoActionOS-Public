
#include <stdlib.h>
#include <hwpl/usb.h>
#include "Usb.hpp"
#include "util.h"

Usb::Usb(port_t port) : Pblock(port, HWPL_USB_PORTS) {}

int Usb::open(int flags){
	this->flags = flags;
	return hwpl_usb_open((const device_cfg_t*)&(port_));
}

int Usb::close(){
	return hwpl_usb_close((const device_cfg_t*)&(port_));
}

int Usb::getattr(usb_attr_t * attr){
	return hwpl_usb_getattr(port_, attr);
}

int Usb::setattr(usb_attr_t * attr){
	return hwpl_usb_setattr(port_, attr);
}
int Usb::reset(void){
	return hwpl_usb_reset(port_, NULL);
}
int Usb::attach(void){
	return hwpl_usb_attach(port_, NULL);
}
int Usb::configure(void){
	return hwpl_usb_configure(port_, NULL);
}

int Usb::detach(void){
	return hwpl_usb_detach(port_, NULL);
}

int Usb::disableep(int ep){
	return hwpl_usb_disableep(port_, (void*)ep);
}

int Usb::enableep(int ep){
	return hwpl_usb_enableep(port_, (void*)ep);
}

bool Usb::isconnected(void){
	return hwpl_usb_isconnected(port_, NULL);
}

int Usb::resetep(int ep){
	return hwpl_usb_resetep(port_, (void*)ep);
}

int Usb::setaddr(int addr){
	return hwpl_usb_setaddr(port_, (void*)addr);
}

int Usb::stallep(int ep){
	return hwpl_usb_stallep(port_, (void*)ep);
}

int Usb::unstallep(int ep){
	return hwpl_usb_unstallep(port_, (void*)ep);
}

int Usb::read(void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_usb_read, this->loc, buf, nbyte, flags | APPLIB_UTIL_READ_FLAG);
}
int Usb::write(const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port_, hwpl_usb_write, this->loc, buf, nbyte, flags | APPLIB_UTIL_WRITE_FLAG);
}

int Usb::read(Aio & aio){
	return applib_util_aio_transfer((const device_cfg_t*)&port_, hwpl_usb_read, &aio.aio_var);
}

int Usb::write(Aio & aio){
	return applib_util_aio_transfer((const device_cfg_t*)&port_, hwpl_usb_write, &aio.aio_var);
}
