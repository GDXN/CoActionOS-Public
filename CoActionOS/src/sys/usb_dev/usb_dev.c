/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <stdio.h>
#include <unistd.h>
#include "sys/ioctl.h"
#include "hwpl/usb.h"
#include "usb_dev.h"
#include "usb_dev_std.h"
#include "usb_dev_cdc.h"
#include "hwpl/debug.h"
#include "hwdl/sys.h"

int default_req(int event);

int usb_dev_adc_if_req(int event) HWPL_ALIAS(default_req);
int usb_dev_msc_if_req(int event) HWPL_ALIAS(default_req);
int usb_dev_hid_if_req(int event) HWPL_ALIAS(default_req);
//Class Requests
//TBD

int default_req(int event){ return 0; }

uint8_t usb_alt_setting[USB_DEV_ALT_SETTING_SIZE];
const void * usb_dev_descriptor = NULL;
const void * usb_cfg_descriptor = NULL;
const void * usb_string_descriptor = NULL;

//This must be below the above variable definitions
#include "usb_dev_std_inline.h"

static void stall(void){
	hwpl_usb_stallep(USB_DEV_PORT, (void*)(USB_ENDPOINT_IN|0x00));
	usb_dev_std_ep0_data.cnt = 0;
}


#ifdef __SECURE
static void super_user(void * args){
	if( args != 0 ){
		sys_setroot();
	} else {
		sys_setuser();
	}
}
#endif

/*! \details This function initializes the USB device configuration.
 */
int usb_dev_init(int usb_fd,
		const void * dev_desc /*! A pointer to the device descriptor */,
		const void * cfg_desc /*! A pointer to the configuration descriptor */,
		const void * string_desc /*! A pointer to the string descriptor */){
	usb_action_t action;
	int ret;

	//set descriptors
	usb_dev_descriptor = dev_desc;
	usb_cfg_descriptor = cfg_desc;
	usb_string_descriptor = string_desc;

	//Set up the action to take when there is data on the control endpoint

#ifdef __lpc13xx
	action.channel = 0;
	action.callback = usb_dev_std_setup;
	action.event = USB_EVENT_DATA_READY;
	hwpl_usb_setaction(usb_fd, &action);

	action.event = USB_EVENT_WRITE_COMPLETE;
	hwpl_usb_setaction(usb_fd, &action);
	hwpl_usb_attach(usb_fd, NULL);
#else
	action.channel = 0;
	action.context = NULL;
	action.callback = usb_dev_std_setup;
	action.event = USB_EVENT_DATA_READY;

#ifdef __SECURE
	hwpl_core_privcall(super_user, (void*)1);
#endif

	ret = ioctl(usb_fd, I_USB_SETACTION, &action);
	if( ret < 0 ){
#ifdef __SECURE
		hwpl_core_privcall(super_user, (void*)0);
#endif
		return -1;
	}

	action.event = USB_EVENT_WRITE_COMPLETE;
	if ( ioctl(usb_fd, I_USB_SETACTION, &action) < 0 ){
#ifdef __SECURE
		hwpl_core_privcall(super_user, (void*)0);
#endif
		return -1;
	}

#ifdef __SECURE
		hwpl_core_privcall(super_user, (void*)0);
#endif

	//Attach the USB
	if ( ioctl(usb_fd, I_USB_ATTACH, NULL) < 0 ){
		return -1;
	}
#endif

	return 0;
}


/*! \details This function handles the standard requests on endpoint 0.
 * This should be the default action for endpoint 0.  The default
 * action is set using usb_dev_init() with the following code
 * \code
 * usb_set_ep_action(USB_DEV_PORT, 0, usb_dev_std_setup);
 * \endcode
 *
 *
 */
int usb_dev_std_setup(void * context, const void * data /*! Callback data */){
	uint32_t event = (uint32_t)data;
	if ( event == USB_SETUP_EVENT ){
		usb_dev_std_setup_stage();
		usb_dev_std_ep0_data.cnt = usb_setup_pkt.wLength;
		if ( usb_setup_pkt.bmRequestType.bitmap_t.type == USB_DEV_REQUEST_STANDARD){
			switch (usb_setup_pkt.bRequest) {

			case USB_DEV_REQUEST_GET_STATUS:
				if (!usb_dev_std_req_get_status()) {
					stall(); return 1;
				}
				usb_dev_std_datain_stage();
				break;

			case USB_DEV_REQUEST_CLEAR_FEATURE:
				if (!usb_dev_std_reg_set_clrfeature(0)) {
					stall(); return 1;
				}
				usb_dev_std_statusin_stage();
				usb_feature_event();
				break;

			case USB_DEV_REQUEST_SET_FEATURE:
				if (!usb_dev_std_reg_set_clrfeature(1)) {
					stall(); return 1;
				}
				usb_dev_std_statusin_stage();
				usb_feature_event();
				break;

			case USB_DEV_REQUEST_SET_ADDRESS:
				if (!usb_dev_std_req_setaddr()) {
					stall(); return 1;
				}
				usb_dev_std_statusin_stage();
				break;

			case USB_DEV_REQUEST_GET_DESCRIPTOR:
				if (!usb_dev_std_req_getdesc()) {
					stall(); return 1;
				}
				usb_dev_std_datain_stage();
				break;

			case USB_DEV_REQUEST_SET_DESCRIPTOR:
				hwpl_usb_stallep(USB_DEV_PORT, (void*)0x00);
				usb_dev_std_ep0_data.cnt = 0;
				break;

			case USB_DEV_REQUEST_GET_CONFIGURATION:
				if (!usb_dev_std_req_getcfg()) {
					stall(); return 1;
				}
				usb_dev_std_datain_stage();
				break;

			case USB_DEV_REQUEST_SET_CONFIGURATION:
				if (!usb_dev_std_req_setcfg()) {
					stall(); return 1;
				}
				usb_dev_std_statusin_stage();
				usb_configure_event();
				break;

			case USB_DEV_REQUEST_GET_INTERFACE:
				if (!usb_dev_std_req_getinterface()) {
					stall(); return 1;
				}
				usb_dev_std_datain_stage();
				break;

			case USB_DEV_REQUEST_SET_INTERFACE:
				if (!usb_dev_std_req_setinterface()) {
					stall(); return 1;
				}
				usb_interface_event();
				usb_dev_std_statusin_stage();
				break;
			default:
				stall(); return 1;
			}

		} else if ( usb_setup_pkt.bmRequestType.bitmap_t.type == USB_DEV_REQUEST_CLASS ){

			switch (usb_setup_pkt.bmRequestType.bitmap_t.recipient) {

			case USB_DEV_REQUEST_TO_DEVICE:
				stall();
				return 1;

			case USB_DEV_REQUEST_TO_INTERFACE:

				if ( usb_dev_adc_if_req(event) ) return 1;
				if ( usb_dev_msc_if_req(event) ) return 1;
				if ( usb_dev_cdc_if_req(event) ) return 1;
				if ( usb_dev_hid_if_req(event) ) return 1;
				break;
			}

			stall();
			return 1;
		}

	} else if ( event == USB_OUT_EVENT ){
		if (usb_setup_pkt.bmRequestType.bitmap_t.dir == USB_DEV_REQUEST_HOST_TO_DEVICE) {

			if (usb_dev_std_ep0_data.cnt) {

				usb_dev_std_dataout_stage();
				if (usb_dev_std_ep0_data.cnt == 0){

					switch (usb_setup_pkt.bmRequestType.bitmap_t.type) {

					case USB_DEV_REQUEST_STANDARD:
						stall(); return 1;

					case USB_DEV_REQUEST_CLASS:
						if (1){

							switch (usb_setup_pkt.bmRequestType.bitmap_t.recipient) {
							case USB_DEV_REQUEST_TO_DEVICE:
								stall(); return 1;

							case USB_DEV_REQUEST_TO_INTERFACE:
								if ( usb_dev_adc_if_req(event) ) return 1;
								if ( usb_dev_msc_if_req(event) ) return 1;
								if ( usb_dev_cdc_if_req(event) ) return 1;
								if ( usb_dev_hid_if_req(event) ) return 1;

							case USB_DEV_REQUEST_TO_ENDPOINT:

							default:
								stall();
								return 1;
							}
							break;

							default:
								stall();
								return 1;
						}
						break;
					}

				}
			}
		} else {
			usb_dev_std_statusout_stage();
		}

	} else if ( event == USB_IN_EVENT){
		if (usb_setup_pkt.bmRequestType.bitmap_t.dir == USB_DEV_REQUEST_DEVICE_TO_HOST) {
			usb_dev_std_datain_stage();
		} else {
			if (usb_dev_std_addr & USB_ENDPOINT_IN) {
				usb_dev_std_addr &= 0x7F;
				hwpl_usb_setaddr(USB_DEV_PORT, (void*)((int)usb_dev_std_addr));
			}
		}
	} else if ( event == USB_OUT_EVENT_STALL){
		hwpl_usb_unstallep(USB_DEV_PORT, (void*)0x00);
	} else if ( event == USB_IN_EVENT_STALL ){
		hwpl_usb_unstallep(USB_DEV_PORT, (void*)(USB_ENDPOINT_IN|0x00));
	}

	return 1;
}

