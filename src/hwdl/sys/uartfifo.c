/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include "dev/uartfifo.h"
#include "hwdl/uartfifo.h"
#include "hwpl/debug.h"


static int set_read_action(const device_cfg_t * cfg, hwpl_callback_t callback){
	hwpl_action_t action;
	const uartfifo_cfg_t * cfgp = cfg->dcfg;
	action.callback = callback;
	action.context = (void*)cfg;
	action.event = UART_EVENT_DATA_READY;
	if( hwpl_uart_setaction(cfgp->port, &action) < 0 ){
		return -1;
	}

	return 0;
}

static void inc_head(uartfifo_state_t * state, int size){
	state->head++;
	if ( state->head == size ){
		state->head = 0;
	}

	if ( state->head == state->tail ){
		state->tail++;
		if ( state->tail == size ){
			state->tail = 0;
		}
		state->overflow = true;
	}
}

static int read_buffer(const uartfifo_cfg_t * cfgp, uartfifo_state_t * state, device_transfer_t * rop){
	int i;
	for(i=0; i < state->len; i++){
		if ( state->head == state->tail ){ //check for data in the fifo buffer
			break;
		} else {
			rop->chbuf[i] = cfgp->buffer[state->tail];
			state->tail++;
			if ( state->tail == cfgp->size ){
				state->tail = 0;
			}
		}
	}
	return i; //number of bytes read
}

static int data_received(void * context, const void * data){
	char c;
	int bytes_read;
	const device_cfg_t * cfg;
	const uartfifo_cfg_t * cfgp;
	uartfifo_state_t * state;
	cfg = context;
	cfgp = cfg->dcfg;
	state = cfg->state;

	while( hwpl_uart_getbyte(cfgp->port, &c) == 0 ){
		cfgp->buffer[ state->head ] = c;
		inc_head(state, cfgp->size);
	}

	if( state->rop != NULL ){
		if( (bytes_read = read_buffer(cfgp, state, state->rop)) > 0 ){
			state->rop->nbyte = bytes_read;
			if ( state->rop->callback(state->rop->context, NULL) == 0 ){
				state->rop = NULL;
			}
		}
	}
	return 1; //leave the callback in place
}

int uartfifo_open(const device_cfg_t * cfg){
	const uartfifo_cfg_t * cfgp = cfg->dcfg;
	uartfifo_state_t * state = cfg->state;
	state->head = 0;
	state->tail = 0;
	state->rop = NULL;
	state->overflow = false;
	//setup the device to write to the fifo when data arrives
	if( hwpl_uart_open((const device_cfg_t*)&(cfgp->port)) < 0 ){
		return -1;
	}

	return 0;
}

int uartfifo_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	fifo_attr_t * attr = ctl;
	hwpl_action_t * action = ctl;
	const uartfifo_cfg_t * cfgp = cfg->dcfg;
	uartfifo_state_t * state = cfg->state;
	switch(request){
	case I_FIFO_GETATTR:
		attr->size = cfgp->size;
		if( state->head >= state->tail ){
			attr->used = state->head - state->tail;
		} else {
			attr->used = cfgp->size - state->tail + state->head;
		}
		attr->overflow = state->overflow;
		state->overflow = false; //clear the overflow flag now that it has been read
		break;
	case I_GLOBAL_SETACTION:
	case I_UART_SETACTION:
		if( action->callback == 0 ){
			//This needs to cancel an ongoing operation
			if( state->rop != NULL ){
				state->rop->nbyte = -1;
				if ( state->rop->callback(state->rop->context, DEVICE_OP_CANCELLED) == 0 ){
					state->rop = NULL;
				}
			}
			return 0;
		}
		errno = ENOTSUP;
		return -1;
	case I_UART_FLUSH:
	case I_FIFO_FLUSH:
		state->head = 0;
		state->tail = 0;
		state->overflow = 0;
		return hwpl_uart_flush(cfgp->port, NULL);
		break;
	case I_UART_SETATTR:
		if(  hwpl_uart_setattr(cfgp->port, ctl) < 0 ){
			return -1;
		}
	case I_FIFO_INIT:
		state->head = 0;
		state->tail = 0;
		state->overflow = 0;
		if ( set_read_action(cfg, data_received) < 0 ){
			return -1;
		}
		break;
	default:
		return hwpl_uart_ioctl((const device_cfg_t*)&(cfgp->port), request, ctl);
}
return 0;
}


int uartfifo_read(const device_cfg_t * cfg, device_transfer_t * rop){
	const uartfifo_cfg_t * cfgp = cfg->dcfg;
	uartfifo_state_t * state = cfg->state;
	int bytes_read;

	if ( state->rop != NULL ){
		errno = EAGAIN; //the device is temporarily unavailable
		return -1;
	}

	state->len = rop->nbyte;
	bytes_read = read_buffer(cfgp, state, rop); //see if there are bytes in the buffer
	if ( bytes_read == 0 ){
		if ( !(rop->flags & O_NONBLOCK) ){ //check for a blocking operation
			state->rop = rop;
		} else {
			errno = EAGAIN;
			return -1;
		}
		state->len = rop->nbyte;
		rop->nbyte = 0;
	}

	return bytes_read;
}

int uartfifo_write(const device_cfg_t * cfg, device_transfer_t * wop){
	const uartfifo_cfg_t * cfgp = cfg->dcfg;
	return hwpl_uart_write((const device_cfg_t*)&(cfgp->port), wop);
}

int uartfifo_close(const device_cfg_t * cfg){
	const uartfifo_cfg_t * cfgp = cfg->dcfg;

	//clear the callback for the device
	if( set_read_action(cfg, NULL) < 0 ){
		return -1;
	}

	return hwpl_uart_close((const device_cfg_t*)&(cfgp->port));
}

