#include <unistd.h>
#include <fcntl.h>
#include "Usb.hpp"
#include "util.h"

#define NUM_PORTS HWPL_USB_PORTS
#define PERIPH_NAME "usb"

int Usb::fd[NUM_PORTS];
bool Usb::isinitialized = false;

Usb::Usb(port_t port) : Pblock(port, NUM_PORTS, (int*)&fd, &isinitialized){}


int Usb::open(int flags){
	return _open(PERIPH_NAME, flags);
}

int Usb::getattr(usb_attr_t * attr){
	return ioctl(I_USB_GETATTR, attr);
}

int Usb::setattr(usb_attr_t * attr){
	return ioctl(I_USB_GETATTR, attr);
}
int Usb::reset(void){
	return ioctl(I_USB_RESET);
}
int Usb::attach(void){
	return ioctl(I_USB_ATTACH);
}
int Usb::configure(void){
	return ioctl(I_USB_CONFIGURE);
}

int Usb::detach(void){
	return ioctl(I_USB_DETACH);
}

int Usb::disableep(int ep){
	return ioctl(I_USB_DISABLEEP, ep);
}

int Usb::enableep(int ep){
	return ioctl(I_USB_ENABLEEP, ep);
}

bool Usb::isconnected(void){
	return ioctl(I_USB_ISCONNECTED);
}

int Usb::resetep(int ep){
	return ioctl(I_USB_RESETEP, ep);
}

int Usb::setaddr(int addr){
	return ioctl(I_USB_SETATTR, addr);
}

int Usb::stallep(int ep){
	return ioctl(I_USB_STALLEP, ep);
}

int Usb::unstallep(int ep){
	return ioctl(I_USB_UNSTALLEP, ep);
}
