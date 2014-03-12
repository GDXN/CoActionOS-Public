/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl/debug.h"
#include "dev/uart.h"
#include "hwpl/uart.h"
#include "hwpl/core.h"

char hwpl_debug_buffer[HWPL_DEBUG_BUFFER_SIZE];

#ifndef NO_DEBUG

int hwpl_debug_init(void){
	int port;
	uart_attr_t attr;



	//Open the debugging UART
	port = HWPL_DEBUG_PORT;
	if( hwpl_uart_open((device_cfg_t*)&port) < 0 ){
		return -1;
	}

	attr.pin_assign = 0;
	attr.baudrate = 115200;
	attr.parity = UART_PARITY_NONE;
	attr.stop = UART_ATTR_STOP_BITS_1;
	attr.start = UART_ATTR_START_BITS_1; //not used on LPC17XX
	attr.width = 8;

	return hwpl_uart_setattr(port, &attr);
}

static int write_done(void * context, const void * data){
	int * busy = (int*)context;
	*busy = 0;
	return 0;
}


void hwpl_priv_write_debug_uart(void * args){
	int nbyte;
	int err;
	volatile int busy;
	device_transfer_t wop;
	int port = HWPL_DEBUG_PORT;
	wop.loc = 0;
	wop.buf = hwpl_debug_buffer;
	wop.nbyte = 0;
	wop.callback = write_done;
	wop.context = (void*)&busy;
	nbyte = strlen(hwpl_debug_buffer);
	wop.nbyte = nbyte;
	busy = 1;

	if ( (err = hwpl_uart_write((device_cfg_t*)&port, &wop)) == 0 ){
		while(busy == 1){

		}
	} else {
		if( err < 0 ){
			while(1);
		}
	}
}

#endif






