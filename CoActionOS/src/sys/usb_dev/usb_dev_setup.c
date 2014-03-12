/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <stdio.h>

#include "usb_dev.h"
#include "usb_dev_std.h"
#include "usb_dev_std_inline.h"


uint16_t usb_dev_std_status;
uint8_t usb_dev_std_addr;
uint8_t usb_dev_std_cfg;
uint32_t usb_dev_std_ep_mask;
uint32_t usb_dev_std_ep_halt;
uint32_t usb_dev_std_ep_stall;                         // EP must stay stalled
uint8_t usb_dev_std_num_interfaces;
uint8_t usb_dev_std_ep0_buf[USB_MAX_PACKET0];
usb_ep_data_t usb_dev_std_ep0_data;
usb_setup_pkt_t usb_setup_pkt;

uint8_t usb_default_power;

//! \todo These need to be callbacks stored in RAM and configurable on the fly
void usb_feature_event(void){}
void usb_configure_event(void){}
void usb_interface_event(void){}


/*! \details
 */
void usb_dev_std_reset_state(void) {
	usb_dev_std_status  = usb_default_power;
	usb_dev_std_addr = 0;
	usb_dev_std_cfg = 0;
	usb_dev_std_init_ep();
}

/*! \details This function reads the setup packet as part of the setup stage.
 */
void usb_dev_std_setup_stage(void){
	hwpl_usb_rd_ep(USB_DEV_PORT, 0x00, (uint8_t *)&usb_setup_pkt);
}


/*! \details
 */

void usb_dev_std_datain_stage (void) {
	uint32_t cnt;
	if (usb_dev_std_ep0_data.cnt > USB_MAX_PACKET0) {
		cnt = USB_MAX_PACKET0;
	} else {
		cnt = usb_dev_std_ep0_data.cnt;
	}
	cnt = hwpl_usb_wr_ep(USB_DEV_PORT, 0x80, usb_dev_std_ep0_data.dptr, cnt);
	usb_dev_std_ep0_data.dptr += cnt;
	usb_dev_std_ep0_data.cnt -= cnt;
}


/*! \details
 */
void usb_dev_std_dataout_stage (void){
	uint32_t cnt;
	cnt = hwpl_usb_rd_ep(USB_DEV_PORT, 0x00, usb_dev_std_ep0_data.dptr);
	usb_dev_std_ep0_data.dptr += cnt;
	usb_dev_std_ep0_data.cnt -= cnt;
}


/*! \details
 */
void usb_dev_std_statusin_stage (void){
	hwpl_usb_wr_ep(USB_DEV_PORT, 0x80, NULL, 0);
}

/*! \details
 */
void usb_dev_std_statusout_stage (void){
	hwpl_usb_rd_ep(USB_DEV_PORT, 0x00, usb_dev_std_ep0_buf);
}




