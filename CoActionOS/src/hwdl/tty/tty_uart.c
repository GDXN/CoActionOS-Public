/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwdl.h"
#include "hwpl/uart.h"

#include "dev/tty.h"

static const int tty_speeds[B921600 + 1] = {
		0,
		50,
		75,
		110,
		134,
		150,
		200,
		300,
		600,
		1200,
		2400,
		4800,
		9600,
		19200,
		38400,
		57600,
		115200,
		230400,
		4600800,
		921600
};


int tty_uart_open(const device_cfg_t * cfg){
	//make sure the port is open and configured the way we want
	return hwdl_check_uart_port(cfg);
}

int tty_uart_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	uart_attr_t attr;
	int tmp;
	struct termios * p = (struct termios*)ctl;

	switch(request){
	case I_TTY_SETATTR:

		if ( p->c_ispeed != p->c_ospeed ){
			return -1;
		}

		tmp = p->c_ospeed;
		if ( tmp < (B921600 + 1) ){
			attr.baudrate = tty_speeds[tmp];
		} else {
			return -1;
		}

		tmp = p->c_cflag & CSIZE;
		if ( 1 ){
			switch(tmp){
			case CS5:
				attr.width = 5;
				break;
			case CS6:
				attr.width = 6;
				break;
			case CS7:
				attr.width = 7;
				break;
			case CS8:
				attr.width = 8;
				break;

			}
		}

		if ( p->c_cflag & CSTOPB ){
			attr.stop = 2;
		} else {
			attr.stop = 1;
		}

		if ( p->c_cflag & PARENB ){
			if ( p->c_cflag & PARODD){
				attr.parity = UART_PARITY_ODD;
			} else {
				attr.parity = UART_PARITY_EVEN;
			}
		} else {
			attr.parity = UART_PARITY_NONE;
		}

		return hwpl_uart_setattr(cfg->periph.port, &attr);

		break;
	}

	return 0;
}

int tty_uart_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_uart_write(cfg, wop);
}

int tty_uart_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_uart_read(cfg, rop);
}

int tty_uart_close(const device_cfg_t * cfg){
	return 0;
}
