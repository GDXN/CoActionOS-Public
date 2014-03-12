/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl/debug.h"
#include "dev/uart.h"
#include "hwpl/uart.h"

char hwpl_debug_buffer[HWPL_DEBUG_BUFFER_SIZE];

#ifndef NO_DEBUG
static volatile int busy;

int hwpl_debug_init(void){
	int port;
	uart_attr_t uart_cfg;
	hwpl_action_t action;

	//Open the debugging UART
	port = 0;
	if( hwpl_uart_open((device_cfg_t*)&port) < 0 ){
		return -1;
	}

	uart_cfg.pin_assign = 0;
	uart_cfg.baudrate = 76800;
	uart_cfg.parity = UART_PARITY_NONE;
	uart_cfg.stop = UART_ATTR_STOP_BITS_1;
	uart_cfg.start = UART_ATTR_START_BITS_1; //not used on LPC17XX
	uart_cfg.width = 8;

	return hwpl_uart_ioctl((device_cfg_t*)&port, I_UART_SETATTR, &uart_cfg);
}

static int write_done(void * context, const void * data){
	busy = 0;
	return 0;
}


void hwpl_priv_write_debug_uart(void * args){
	int nbyte;
	int err;
	device_transfer_t wop;
	device_periph_t periph;
	periph.port = 0;
	int port = 0;
	wop.loc = 0;
	wop.buf = hwpl_debug_buffer;
	wop.nbyte = 0;
	wop.callback = write_done;
	nbyte = strlen(hwpl_debug_buffer);
	wop.nbyte = nbyte;
	busy = 1;
	if ( (err = hwpl_uart_write((device_cfg_t*)&port, &wop)) == 0 ){
		while(busy == 1);
	} else {
		if( err < 0 ){
			gled_on();
			while(1);
		}
	}
}

#endif






