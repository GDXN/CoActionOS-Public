/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <stddef.h>
#include "hwpl/pio.h"
#include "hwdl/sys.h"

#include "hwpl/debug.h"


int led_open(const device_cfg_t * cfg){
	return 0;
}

int led_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	led_req_t * req = ctl;
	pio_t port_pin;
	pio_attr_t req_attr;
	if( request == I_LED_SET ){
		if ( req->channel > 3 ){
			errno = EINVAL;
			return -1;
		}

		port_pin = cfg->pcfg.pio[req->channel];

		if ( req->on != 0 ){
			req_attr.mask = (1<<port_pin.pin);
			req_attr.mode = PIO_MODE_OUTPUT;
			hwpl_pio_setattr(port_pin.port, &req_attr);
			if ( cfg->pin_assign == LED_ACTIVE_LOW ){
				hwpl_pio_clrmask(port_pin.port, (void*)(1<<port_pin.pin));
			} else {
				hwpl_pio_setmask(port_pin.port, (void*)(1<<port_pin.pin));
			}
		} else {
			req_attr.mask = (1<<port_pin.pin);
			req_attr.mode = PIO_MODE_INPUT;
			hwpl_pio_setattr(port_pin.port, &req_attr);
			if ( cfg->pin_assign == LED_ACTIVE_LOW ){
				hwpl_pio_setmask(port_pin.port, (void*)(1<<port_pin.pin));
			} else {
				hwpl_pio_clrmask(port_pin.port, (void*)(1<<port_pin.pin));
			}
		}

	}
	return 0;
}

int led_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}

int led_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int led_close(const device_cfg_t * cfg){
	return 0;
}

