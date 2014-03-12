#include <stdlib.h>
#include <fcntl.h>
#include <hwpl/usb.h>
#include <hwdl/usbfifo.h>
#include "usb.h"
#include "util.h"

#define USB0_DEVFIFO_BUFFER_SIZE 64
char usb0_fifo_buffer[USB0_DEVFIFO_BUFFER_SIZE];
const usbfifo_cfg_t usb0_fifo_cfg = USBFIFO_DEVICE_CFG(0,
		2,
		64,
		usb0_fifo_buffer,
		USB0_DEVFIFO_BUFFER_SIZE);
usbfifo_state_t usb0_fifo_state HWPL_SYS_MEM;


const device_t device_usbfifo[] = {
		USBFIFO_DEVICE("link-phy-usb", &usb0_fifo_cfg, &usb0_fifo_state, 0666, 0, 0)
};

int usb_setattr_fifo(port_t port, usb_attr_t * attr){
	return usbfifo_ioctl((const device_cfg_t*)&port, I_USB_SETATTR, attr);
}


int usb_read_fifo(port_t port, int loc, void * buf, int nbyte){
	return applib_util_transfer(&device_usbfifo[port].cfg, usbfifo_read, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_READ_FLAG);
}

int usb_write_fifo(port_t port, int loc, const void * buf, int nbyte){
	return applib_util_transfer(&device_usbfifo[port].cfg, usbfifo_write, loc, buf, nbyte, O_RDWR | APPLIB_UTIL_WRITE_FLAG);
}

