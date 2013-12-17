/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/usb.h"

//These functions are device specific
extern void _hwpl_usb_dev_power_on(int port);
extern void _hwpl_usb_dev_power_off(int port);
extern int _hwpl_usb_dev_powered_on(int port);
extern int _hwpl_usb_dev_read(const device_cfg_t * cfg, device_transfer_t * rop);
extern int _hwpl_usb_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const usb_ioctl_func_table[I_GLOBAL_TOTAL + I_USB_TOTAL])(int, void*) = {
		hwpl_usb_getattr,
		hwpl_usb_setattr,
		hwpl_usb_setaction,
		hwpl_usb_reset,
		hwpl_usb_attach,
		hwpl_usb_detach,
		hwpl_usb_configure,
		hwpl_usb_setaddr,
		hwpl_usb_resetep,
		hwpl_usb_enableep,
		hwpl_usb_disableep,
		hwpl_usb_stallep,
		hwpl_usb_unstallep,
		hwpl_usb_cfgep,
		hwpl_usb_seteventhandler,
		hwpl_usb_isconnected
};

int hwpl_usb_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_USB_PORTS,
			_hwpl_usb_dev_powered_on,
			_hwpl_usb_dev_power_on);
}

int hwpl_usb_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_USB_PORTS,
			_hwpl_usb_dev_powered_on,
			usb_ioctl_func_table,
			I_GLOBAL_TOTAL + I_USB_TOTAL);
}

int hwpl_usb_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_read(cfg, rop,
			HWPL_USB_PORTS,
			_hwpl_usb_dev_powered_on,
			_hwpl_usb_dev_read);
}

int hwpl_usb_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg, wop,
			HWPL_USB_PORTS,
			_hwpl_usb_dev_powered_on,
			_hwpl_usb_dev_write);

}

int hwpl_usb_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_USB_PORTS, _hwpl_usb_dev_powered_on, _hwpl_usb_dev_power_off);
}


