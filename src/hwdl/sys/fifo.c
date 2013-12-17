/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include "hwpl/debug.h"
#include "dev/fifo.h"
#include "hwdl/fifo.h"



static int read_buffer(const fifo_cfg_t * cfgp, fifo_state_t * state, device_transfer_t * rop){
	int i;
	for(i=0; i < state->rop_len; i++){
		if ( state->head == state->tail ){ //check for data in the fifo buffer
			//there is no more data in the buffer
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

static int write_buffer(const fifo_cfg_t * cfgp, fifo_state_t * state, device_transfer_t * wop){
	int i;
	for(i=0; i < state->wop_len; i++){

		if( state->tail == 0 ){
			if( state->head == cfgp->size -1 ){
				if( state->write_block ){
					break; //no more room
				} else {
					state->tail++;
				}
			}
		} else if( (state->head) + 1 == state->tail ){
			if( state->write_block ){
				break; //no more room
			} else {
				state->tail++;
			}
		}

		if( state->tail == cfgp->size ){
			state->tail = 0;
		}


		cfgp->buffer[state->head] = wop->cchbuf[i];

		state->head++;
		if ( state->head == cfgp->size ){
			state->head = 0;
		}

	}
	return i; //number of bytes written
}

static void data_received(const device_cfg_t * cfg){
	int bytes_read;
	const fifo_cfg_t * cfgp = cfg->dcfg;
	fifo_state_t * state = cfg->state;

	if( state->rop != NULL ){
		if( (bytes_read = read_buffer(cfgp, state, state->rop)) > 0 ){
			state->rop->nbyte = bytes_read;
			if ( state->rop->callback(state->rop->context, NULL) == 0 ){
				state->rop = NULL;
			}
		}
	}
}

static int data_transmitted(const device_cfg_t * cfg){
	int bytes_written;
	const fifo_cfg_t * cfgp = cfg->dcfg;
	fifo_state_t * state = cfg->state;

	if( state->wop != NULL ){
		if( (bytes_written = write_buffer(cfgp, state, state->wop)) > 0 ){
			state->wop->nbyte = bytes_written;
			if ( state->wop->callback(state->wop->context, NULL) == 0 ){
				state->wop = NULL;
			}
		}
	}

	return 1; //leave the callback in place
}




int fifo_open(const device_cfg_t * cfg){
	//const fifo_cfg_t * cfgp = cfg->dcfg;
	//fifo_state_t * state = cfg->state;
	return 0;
}

int fifo_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	fifo_attr_t * attr = ctl;
	const fifo_cfg_t * cfgp = cfg->dcfg;
	fifo_state_t * state = cfg->state;
	switch(request){
	case I_FIFO_GETATTR:
		attr->size = cfgp->size - 1;
		if( state->head >= state->tail ){
			attr->used = state->head - state->tail;
		} else {
			attr->used = cfgp->size - state->tail + state->head;
		}
		return 0;
	case I_FIFO_INIT:
		state->rop = NULL;
		state->wop = NULL;
	case I_FIFO_FLUSH:
		state->head = 0;
		state->tail = 0;
		data_transmitted(cfg); //something might be waiting to write the fifo
		return 0;
	case I_FIFO_SETWRITEBLOCK:
		state->write_block = (bool)ctl;
		if( state->write_block == 0 ){
			//make sure the FIFO is not currently blocked
			data_transmitted(cfg);
		}
		return 0;
	}
	errno = EINVAL;
	return -1;
}


int fifo_read(const device_cfg_t * cfg, device_transfer_t * rop){
	const fifo_cfg_t * cfgp = cfg->dcfg;
	fifo_state_t * state = cfg->state;
	int bytes_read;

	if ( state->rop != NULL ){
		rop->nbyte = 0;
		return 0; //caller will block until data is read
	}

	state->rop_len = rop->nbyte;
	bytes_read = read_buffer(cfgp, state, rop); //see if there are bytes in the buffer
	if ( bytes_read == 0 ){
		if( (rop->flags & O_NONBLOCK) ){
			errno = EAGAIN;
			bytes_read = -1;
		} else {
			state->rop = rop;
			state->rop_len = rop->nbyte;
			rop->nbyte = 0;
		}
	} else {
		//see if anything needs to write the FIFO
		data_transmitted(cfg);
	}

	return bytes_read;
}

int fifo_write(const device_cfg_t * cfg, device_transfer_t * wop){
	const fifo_cfg_t * cfgp = cfg->dcfg;
	fifo_state_t * state = cfg->state;
	int bytes_written;

	if ( state->wop != NULL ){
		wop->nbyte = 0;
		return 0; //caller will block until data is read
	}

	state->wop_len = wop->nbyte;
	bytes_written = write_buffer(cfgp, state, wop); //see if there are bytes in the buffer
	if ( bytes_written == 0 ){
		if( wop->flags & O_NONBLOCK ){
			errno = EAGAIN;
			bytes_written = -1;
		} else {
			state->wop = wop;
			state->wop_len = wop->nbyte;
			wop->nbyte = 0;
		}
	} else {
		data_received(cfg);
	}

	return bytes_written;
}

int fifo_close(const device_cfg_t * cfg){
	return 0;
}

