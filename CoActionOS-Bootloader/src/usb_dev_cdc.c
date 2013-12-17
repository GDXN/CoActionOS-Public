/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

//#include "config.h"


#include <string.h>
#include <stdio.h>
#include <hwpl.h>
#include <hwpl/usb.h>

#include "usb_dev.h"
#include "usb_dev_std.h"
#include "usb_dev_cdc.h"


/*
typedef struct {
	device_transfer_t wr;
	device_transfer_t rd;
} usb_dev_cdc_local_t;

static usb_dev_cdc_local_t cdc_local HWPL_SYS_MEM;

int usb_dev_cdc_open(const device_cfg_t * cfg){
	return 0;
}

int usb_dev_cdc_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_usb_ioctl(USBDEV_PORT, request, ctl);
}

int usb_dev_cdc_write(const device_cfg_t * cfg, device_transfer_t * wop){ //Writing to the CDC sends data to the Host
	//write the data class bulk endpoint
	return 0;
}

int usb_dev_cdc_read(const device_cfg_t * cfg, device_transfer_t * rop){ //Reading from the CDC gets data from the host
	//read the data class bulk endpoint

	return 0;
}

int usb_dev_cdc_close(const device_cfg_t * cfg){
	return 0;
}
*/

int usb_dev_cdc_if_req(int event){
	uint32_t rate = 12000000;

	if ( (usb_setup_pkt.wIndex.u8[0] == 0) || (usb_setup_pkt.wIndex.u8[0] == 1) ) { //! \todo The wIndex should equal the CDC interface number

		if ( (event == USB_SETUP_EVENT) ){
			switch(usb_setup_pkt.bRequest){
			case SET_LINE_CODING:
			case SET_COMM_FEATURE:
			case SEND_BREAK:
			case SEND_ENCAPSULATED_COMMAND:
				//need to receive information from the host
				usb_dev_std_ep0_data.dptr = usb_dev_std_ep0_buf;
				return 1;
			case SET_CONTROL_LINE_STATE:
				usb_dev_std_statusin_stage();
				return 1;
			case GET_LINE_CODING:
				usb_dev_std_ep0_data.dptr = usb_dev_std_ep0_buf;

				//copy line coding to usb_dev_std_ep0_buf
				usb_dev_std_ep0_buf[0] = (rate >>  0) & 0xFF;
				usb_dev_std_ep0_buf[1] = (rate >>  8) & 0xFF;
				usb_dev_std_ep0_buf[2] = (rate >> 16) & 0xFF;
				usb_dev_std_ep0_buf[3] = (rate >> 24) & 0xFF;  //rate
				usb_dev_std_ep0_buf[4] =  0; //stop bits 1
				usb_dev_std_ep0_buf[5] =  0; //no parity
				usb_dev_std_ep0_buf[6] =  8; //8 data bits

				usb_dev_std_statusin_stage();
				return 1;
			case CLEAR_COMM_FEATURE:
				usb_dev_std_statusin_stage();
				return 1;
			case GET_COMM_FEATURE:
				usb_dev_std_ep0_data.dptr = usb_dev_std_ep0_buf;
				//copy data to usb_dev_std_ep0_buf
				usb_dev_std_statusin_stage();
				return 1;
			case GET_ENCAPSULATED_RESPONSE:
				usb_dev_std_ep0_data.dptr = usb_dev_std_ep0_buf;
				//copy data to usb_dev_std_ep0_buf
				usb_dev_std_statusin_stage();
				return 1;
			default:
				return 0;
			}
		} else if ( event == USB_OUT_EVENT ){
			switch(usb_setup_pkt.bRequest){
			case SET_LINE_CODING:
			case SET_CONTROL_LINE_STATE:
			case SET_COMM_FEATURE:
			case SEND_ENCAPSULATED_COMMAND:
				//use data in usb_dev_std_ep0_buf to take action
				usb_dev_std_statusin_stage();
				return 1;
			default:
				return 0;
			}
		}
	}

	//The request was not handled
	return 0;
}
