
#ifndef APPLIB_USB_H_
#define APPLIB_USB_H_

#include <dev/usb.h>
#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

int usb_open(port_t port);
int usb_close(port_t port);
int usb_getattr(port_t port, usb_attr_t * attr);
int usb_setattr(port_t port, usb_attr_t * attr);
int usb_reset(port_t port);
int usb_attach(port_t port);
int usb_configure(port_t port);
int usb_detach(port_t port);
int usb_disableep(port_t port, int ep);
int usb_enableep(port_t port, int ep);
bool usb_isconnected(port_t port);
int usb_resetep(port_t port, int ep);
int usb_setaddr(port_t port, int addr);
int usb_stallep(port_t port, int ep);
int usb_unstallep(port_t port, int ep);
int usb_read(port_t port, int loc, void * buf, int nbyte);
int usb_write(port_t port, int loc, const void * buf, int nbyte);

int usb_setattr_fifo(port_t port, usb_attr_t * attr);
int usb_read_fifo(port_t port, int loc, void * buf, int nbyte);
int usb_write_fifo(port_t port, int loc, const void * buf, int nbyte);

#ifdef __cplusplus
}
#endif

#endif /* APPLIB_USB_H_ */
