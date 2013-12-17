/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl/debug.h"
#include "dev/uart.h"
#include "hwpl/uart.h"

#ifndef NO_DEBUG

char hwpl_debug_buffer[HWPL_DEBUG_BUFFER_SIZE];

int hwpl_debug_init(void){
	int port;
	int err;
	uart_attr_t uart_cfg;

	//Open the debugging UART
	port = 0;
	err = hwpl_uart_open((device_cfg_t*)&port);
	if ( err < 0 ){
		return err;
	}


	uart_cfg.pin_assign = 0;
	uart_cfg.baudrate = 115200;
	uart_cfg.parity = UART_PARITY_NONE;
	uart_cfg.stop = UART_ATTR_STOP_BITS_1;
	uart_cfg.start = 0;
	uart_cfg.width = 8;

	err = hwpl_uart_ioctl((device_cfg_t*)&port, I_UART_SETATTR, &uart_cfg);

	if ( err < 0 ){
		return err;
	}

	return err;
}


void hwpl_priv_write_debug_uart(void * args){
	device_transfer_t wop;
	int port = 0;
	wop.loc = 0;
	wop.buf = NULL;
	wop.nbyte = 0;
	wop.callback = NULL;
	while( hwpl_uart_write((device_cfg_t*)&port, &wop) == 0 );
	wop.buf = hwpl_debug_buffer;
	wop.nbyte = strlen(hwpl_debug_buffer);
	if ( hwpl_uart_write((device_cfg_t*)&port, &wop) == 0 ){
		wop.nbyte = 0;
		wop.buf = NULL;
		while( hwpl_uart_write((device_cfg_t*)&port, &wop) == 0 );
	}
}

#endif







