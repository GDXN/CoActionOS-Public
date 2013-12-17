/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include "dev/usbfifo.h"
#include "hwdl/usbfifo.h"
#include "hwpl/usb.h"
#include "hwpl/debug.h"


static int set_read_action(const device_cfg_t * cfg, hwpl_callback_t callback){
	hwpl_action_t action;
	const usbfifo_cfg_t * cfgp = cfg->dcfg;
	action.callback = callback;
	action.context = (void*)cfg;
	action.event = USB_EVENT_DATA_READY;
	action.channel = cfgp->endpoint;
	if( hwpl_usb_setaction(cfgp->port, &action) < 0 ){
		return -1;
	}

	return 0;
}

static void inc_head(usbfifo_state_t * state, int size){
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

static int read_buffer(const usbfifo_cfg_t * cfgp, usbfifo_state_t * state, device_transfer_t * rop){
	int i;
	for(i=0; i < rop->nbyte; i++){
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
	int i;
	int bytes_read;
	const device_cfg_t * cfg;
	const usbfifo_cfg_t * cfgp;
	usbfifo_state_t * state;
	cfg = context;
	cfgp = cfg->dcfg;
	state = cfg->state;
	char buffer[cfgp->endpoint_size];

	//check to see if USB was disconnected
	if( hwpl_usb_isconnected(cfgp->port, NULL) == 0 ){
		//leave the callback in place
		return 1;
	}

	//read the endpoint directly
	bytes_read = hwpl_usb_rd_ep(cfgp->port, cfgp->endpoint, buffer);

	for(i=0; i < bytes_read; i++){
		cfgp->buffer[ state->head ] = buffer[i];
		inc_head(state, cfgp->size);
	}

	if( state->rop != NULL ){
		state->rop->nbyte = state->len;
		if( (bytes_read = read_buffer(cfgp, state, state->rop)) > 0 ){
			state->rop->nbyte = bytes_read;
			if ( state->rop->callback(state->rop->context, NULL) == 0 ){
				state->rop = NULL;
			}
		}
	}

	return 1; //leave the callback in place
}

int usbfifo_open(const device_cfg_t * cfg){
	const usbfifo_cfg_t * cfgp = cfg->dcfg;
	return hwpl_usb_open((const device_cfg_t*)&(cfgp->port));
}

int usbfifo_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	fifo_attr_t * attr = ctl;
	hwpl_action_t * action = ctl;
	const usbfifo_cfg_t * cfgp = cfg->dcfg;
	usbfifo_state_t * state = cfg->state;
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
	case I_USB_SETACTION:
	case I_GLOBAL_SETACTION:
		if( action->callback == 0 ){
			if ( state->rop != NULL ){
				state->rop->nbyte = -1;
				if ( state->rop->callback(state->rop->context, DEVICE_OP_CANCELLED) == 0 ){
					state->rop = NULL;
				}
			}
		} else {
			return hwpl_usb_setaction(cfgp->port, ctl);
		}
		return 0;
	case I_FIFO_FLUSH:
		state->head = 0;
		state->tail = 0;
		state->rop = NULL;
		state->overflow = false;
		break;
	case I_USB_SETATTR:
		state->head = 0;
		state->tail = 0;
		state->rop = NULL;
		state->overflow = false;
		//setup the device to write to the fifo when data arrives

		if(  hwpl_usb_setattr(cfgp->port, ctl) < 0 ){
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
	case I_FIFO_EXIT:
		//clear the callback for the device
		if( set_read_action(cfg, NULL) < 0 ){
			return -1;
		}
		return hwpl_usb_close((const device_cfg_t*)&(cfgp->port));
	default:
		return hwpl_usb_ioctl((const device_cfg_t*)&(cfgp->port), request, ctl);
	}
	return 0;
}


int usbfifo_read(const device_cfg_t * cfg, device_transfer_t * rop){
	const usbfifo_cfg_t * cfgp = cfg->dcfg;
	usbfifo_state_t * state = cfg->state;
	int bytes_read;

	if ( state->rop != NULL ){
		errno = EAGAIN; //the device is temporarily unavailable
		return -1;
	}

	bytes_read = read_buffer(cfgp, state, rop); //see if there are bytes in the buffer
	if ( bytes_read == 0 ){
		if ( !(rop->flags & O_NONBLOCK) ){ //check for a blocking operation
			state->rop = rop;
			state->len = rop->nbyte;
			rop->nbyte = 0;
		} else {
			errno = EAGAIN;
			return -1;
		}
	}

	return bytes_read;
}

int usbfifo_write(const device_cfg_t * cfg, device_transfer_t * wop){
	const usbfifo_cfg_t * cfgp = cfg->dcfg;
	wop->loc = 0x80 | cfgp->endpoint;
	//core_priv_enable_interrupts(NULL);
	//hwpl_priv_debug("wr %d\n", task_get_current());
	return hwpl_usb_write((const device_cfg_t*)&(cfgp->port), wop);
}

int usbfifo_close(const device_cfg_t * cfg){
	//use I_FIFO_EXIT to close the USB
	return 0;
}

