/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl/usb.h"
#include "dev/tty.h"


int tty_usbbulk_open(const device_cfg_t * cfg){
	return 0;
}

int tty_usbbulk_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_usb_ioctl(cfg, request, ctl);
}

int tty_usbbulk_write(const device_cfg_t * cfg, device_transfer_t * wop){
	const tty_cfg_t * dcfg;
	dcfg = (const tty_cfg_t*)cfg->dcfg;
	wop->loc = dcfg->write_ep;
	return hwpl_usb_write(cfg, wop);
}

int tty_usbbulk_read(const device_cfg_t * cfg, device_transfer_t * rop){ //The stdio is read by the USB connection
	const tty_cfg_t * dcfg;
	dcfg = (const tty_cfg_t*)cfg->dcfg;
	rop->loc = dcfg->read_ep;
	return hwpl_usb_read(cfg, rop);
}

int tty_usbbulk_close(const device_cfg_t * cfg){
	return 0;
}
