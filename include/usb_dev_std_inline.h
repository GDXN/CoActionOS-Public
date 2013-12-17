/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup USB_DEV_STD
 *
 *
 * @{
 * \ingroup USB_DEV
 *
 */

#ifndef USB_DEV_STD_INLINE_H_
#define USB_DEV_STD_INLINE_H_

#include <stdbool.h>
#include "hwpl.h"
#include "hwpl/usb.h"

extern uint8_t usb_alt_setting[USB_DEV_ALT_SETTING_SIZE];
extern const void * usb_dev_descriptor;
extern const void * usb_cfg_descriptor;
extern const void * usb_string_descriptor;

#define USB_DEV_EP_MASK (USB_ENDPOINT_IN|(USB_LOGIC_EP_NUM-1))

static inline int usb_dev_decode_ep(int ep) HWPL_ALWAYS_INLINE;
int usb_dev_decode_ep(int ep){
	if ( ep & USB_ENDPOINT_IN ){
		return ((ep << USB_LOGIC_EP_NUM) << (ep & (USB_LOGIC_EP_NUM-1)));
	} else {
		return (1<<ep);
	}
}

static inline void * usb_dev_std_add_ptr(void * ptr, uint32_t n) HWPL_ALWAYS_INLINE;
void * usb_dev_std_add_ptr(void * ptr, uint32_t value){
	return (char*)ptr + value;
}


static void usb_dev_std_init_ep(void) HWPL_NEVER_INLINE;
void usb_dev_std_init_ep(void){
	usb_dev_std_ep_halt  = 0;
	usb_dev_std_ep_mask  = USB_DEFAULT_EP_MASK;
	usb_dev_std_ep_stall = 0;
}



static inline uint32_t usb_dev_std_req_get_status (void) HWPL_ALWAYS_INLINE;
uint32_t usb_dev_std_req_get_status (void) {
	uint32_t i;
	uint32_t j;

	uint16_t * ep0_bufp = (uint16_t*)usb_dev_std_ep0_buf;
	switch (usb_setup_pkt.bmRequestType.bitmap_t.recipient){
	case USB_DEV_REQUEST_TO_DEVICE:
		usb_dev_std_ep0_data.dptr = (uint8_t *)&usb_dev_std_status;
		break;

	case USB_DEV_REQUEST_TO_INTERFACE:
		if ((usb_dev_std_cfg != 0) && (usb_setup_pkt.wIndex.u8[0] < usb_dev_std_num_interfaces)) {
			*ep0_bufp = 0;
			usb_dev_std_ep0_data.dptr = usb_dev_std_ep0_buf;
		} else {
			return false;
		}
		break;

	case USB_DEV_REQUEST_TO_ENDPOINT:
		i = usb_setup_pkt.wIndex.u8[0] & USB_DEV_EP_MASK;
		j = usb_dev_decode_ep(i);
		if (((usb_dev_std_cfg != 0) || ((i & (USB_LOGIC_EP_NUM-1)) == 0)) && (usb_dev_std_ep_mask & j)) {
			*ep0_bufp = (usb_dev_std_ep_halt & j) ? 1 : 0;
			usb_dev_std_ep0_data.dptr = usb_dev_std_ep0_buf;
		} else {
			return false;
		}
		break;

	default:
		return false;

	}
	return (true);
}


static inline uint32_t usb_dev_std_reg_set_clrfeature (uint32_t sc) HWPL_ALWAYS_INLINE;
uint32_t usb_dev_std_reg_set_clrfeature (uint32_t sc) {
	uint32_t i;
	uint32_t j;

	switch (usb_setup_pkt.bmRequestType.bitmap_t.recipient) {

	case USB_DEV_REQUEST_TO_DEVICE:
		if (usb_setup_pkt.wValue.u16 == USB_FEATURE_REMOTE_WAKEUP) {
			if (sc) {
				usb_dev_std_status |=  USB_GETSTATUS_REMOTE_WAKEUP;
			} else {
				usb_dev_std_status &= ~USB_GETSTATUS_REMOTE_WAKEUP;
			}
		} else {
			return false;
		}
		break;

	case USB_DEV_REQUEST_TO_INTERFACE:
		return false;

	case USB_DEV_REQUEST_TO_ENDPOINT:
		i = usb_setup_pkt.wIndex.u8[0] & USB_DEV_EP_MASK;
		j = usb_dev_decode_ep(i);
		if ((usb_dev_std_cfg != 0) && ((i & (USB_LOGIC_EP_NUM-1)) != 0) && (usb_dev_std_ep_mask & j)) {
			if (usb_setup_pkt.wValue.u16 == USB_FEATURE_ENDPOINT_STALL) {
				if (sc) {
					hwpl_usb_stallep(USB_DEV_PORT, (void*)i);
					usb_dev_std_ep_halt |=  j;
				} else {
					if ((usb_dev_std_ep_stall & j) != 0) {
						return (true);
					}
					hwpl_usb_unstallep(USB_DEV_PORT, (void*)i);
					usb_dev_std_ep_halt &= ~j;
				}
			} else {
				return false;
			}
		} else {
			return false;
		}
		break;

	default:
		return false;

	}
	return (true);
}

extern char htoc(int nibble);

static inline void usb_dev_std_get_serialno(void * dest){
	uint32_t tmp[4];
	union {
		uint8_t * u8;
		uint16_t * u16;
	} ptr;
	int i, j;
	ptr.u16 = (uint16_t *)dest;
	_hwpl_core_getserialno(tmp);
	for(j=3; j >= 0; j--){
		for(i=0; i < 8; i++){
			ptr.u8[0] = htoc((tmp[j] >> 28) & 0x0F);
			ptr.u8[1] = 0;
			tmp[j] <<= 4;
			ptr.u16++;
		}
	}

}

static inline uint32_t usb_dev_std_req_getdesc(void) HWPL_ALWAYS_INLINE;
uint32_t usb_dev_std_req_getdesc(void) {
	union {
		uint8_t  *u8;
		const usb_string_desc_t * cstr;
		usb_string_desc_t * str;
		const usb_cfg_desc_t * cfg;
	} ptr;
	uint32_t len;
	uint32_t i;

	switch (usb_setup_pkt.bmRequestType.bitmap_t.recipient) {
	case USB_DEV_REQUEST_TO_DEVICE:
		if (1){  //This makes it so that Eclipse auto-indent works
			switch (usb_setup_pkt.wValue.u8[1]) {

			case USB_DEVICE_DESCRIPTOR_TYPE:
				usb_dev_std_ep0_data.dptr = (uint8_t * const)usb_dev_descriptor;
				len = sizeof(usb_dev_desc_t);
				break;

			case USB_CONFIGURATION_DESCRIPTOR_TYPE:
				ptr.cfg = usb_cfg_descriptor;
				for (i = 0; i != usb_setup_pkt.wValue.u8[0]; i++) {
					if (ptr.cfg->bLength != 0) {
						ptr.u8 += ptr.cfg->wTotalLength;
					}
				}
				if (ptr.cfg->bLength == 0) {
					return false;
				}
				usb_dev_std_ep0_data.dptr = ptr.u8;
				len = ptr.cfg->wTotalLength;
				break;

			case USB_STRING_DESCRIPTOR_TYPE:
				ptr.cstr = usb_string_descriptor;
				for (i = 0; i != usb_setup_pkt.wValue.u8[0]; i++) {
					if (ptr.cstr->bLength != 0) {
						ptr.u8 += ptr.cstr->bLength;
					}
				}

				len = ptr.cstr->bLength;
				if ( len == 0) { return false; }

				if ( ptr.cstr->bString == NULL ){
					//generate the string from the device serial number
					ptr.u8 = usb_dev_std_ep0_buf;
					ptr.str->bLength = 32*2;
					ptr.str->bDescriptorType = 3;
					usb_dev_std_get_serialno( &(ptr.str->bString) );
					len = ptr.str->bLength;
					usb_dev_std_ep0_data.dptr = usb_dev_std_ep0_buf;
				} else {
					usb_dev_std_ep0_data.dptr = ptr.u8;
				}

				break;

			default:
				return false;
			}
		}
		break;

	case USB_DEV_REQUEST_TO_INTERFACE:
		switch (usb_setup_pkt.wValue.u8[1]) {

		default:
			return false;
		}
		break;

		default:
			return false;
	}

	if (usb_dev_std_ep0_data.cnt > len) {
		usb_dev_std_ep0_data.cnt = len;
	}

	return (true);
}

static inline uint32_t usb_dev_std_req_setaddr (void) HWPL_ALWAYS_INLINE;
uint32_t usb_dev_std_req_setaddr (void) {

	switch (usb_setup_pkt.bmRequestType.bitmap_t.recipient) {

	case USB_DEV_REQUEST_TO_DEVICE:
		usb_dev_std_addr = USB_ENDPOINT_IN | usb_setup_pkt.wValue.u8[0];
		break;

	default:
		return false;
	}
	return (true);
}

static inline uint32_t usb_dev_std_req_getcfg (void) HWPL_ALWAYS_INLINE;
uint32_t usb_dev_std_req_getcfg (void) {

	switch (usb_setup_pkt.bmRequestType.bitmap_t.recipient) {

	case USB_DEV_REQUEST_TO_DEVICE:
		usb_dev_std_ep0_data.dptr = &usb_dev_std_cfg;
		break;

	default:
		return false;
	}
	return (true);
}

static inline uint32_t usb_dev_std_req_setcfg (void) HWPL_ALWAYS_INLINE;
uint32_t usb_dev_std_req_setcfg (void) {
	uint32_t alternate_setting = 0;
	uint32_t i;
	uint32_t j;
	usb_common_desc_t *dptr;

	switch (usb_setup_pkt.bmRequestType.bitmap_t.recipient) {

	case USB_DEV_REQUEST_TO_DEVICE:
		if ( usb_setup_pkt.wValue.u8[0] ) {
			dptr = (usb_common_desc_t*)usb_cfg_descriptor;
			while (dptr->bLength) {
				switch (dptr->bDescriptorType) {

				case USB_CONFIGURATION_DESCRIPTOR_TYPE:
					if (((usb_cfg_desc_t *)dptr)->bConfigurationValue == usb_setup_pkt.wValue.u8[0]) {
						usb_dev_std_cfg = usb_setup_pkt.wValue.u8[0];
						usb_dev_std_num_interfaces = ((usb_cfg_desc_t *)dptr)->bNumInterfaces;
						for (i = 0; i < USB_DEV_ALT_SETTING_SIZE; i++) {
							usb_alt_setting[i] = 0;
						}
						for (i = 1; i < USB_LOGIC_EP_NUM; i++) {
							if (usb_dev_std_ep_mask & (1 << i)) {
								hwpl_usb_disableep(USB_DEV_PORT, (void*)i);
							}
							if (usb_dev_std_ep_mask & ((1 << USB_LOGIC_EP_NUM) << i)) {
								hwpl_usb_disableep(USB_DEV_PORT, (void*)(i|USB_ENDPOINT_IN));
							}
						}
						usb_dev_std_init_ep();
						hwpl_usb_configure(USB_DEV_PORT, (void*)true);

						if (((usb_cfg_desc_t *)dptr)->bmAttributes & USB_CONFIG_POWERED_MASK) {
							usb_dev_std_status |=  USB_GETSTATUS_SELF_POWERED;
						} else {
							usb_dev_std_status &= ~USB_GETSTATUS_SELF_POWERED;
						}
					} else {
						dptr = usb_dev_std_add_ptr(dptr,((usb_cfg_desc_t *)dptr)->wTotalLength);
						continue;
					}
					break;

				case USB_INTERFACE_DESCRIPTOR_TYPE:
					alternate_setting = ((usb_interface_desc_t *)dptr)->bAlternateSetting;
					break;

				case USB_ENDPOINT_DESCRIPTOR_TYPE:
					if (alternate_setting == 0) {
						i = ((usb_ep_desc_t *)dptr)->bEndpointAddress & USB_DEV_EP_MASK;
						j = usb_dev_decode_ep(i);
						usb_dev_std_ep_mask |= j;
						hwpl_usb_cfgep(USB_DEV_PORT, dptr);
						hwpl_usb_enableep(USB_DEV_PORT, (void*)i);
						hwpl_usb_resetep(USB_DEV_PORT, (void*)i);
					}
					break;

				}

				dptr = usb_dev_std_add_ptr(dptr, dptr->bLength);
			}
		} else {
			usb_dev_std_cfg = 0;
			for (i = 1; i < USB_LOGIC_EP_NUM; i++) {
				if (usb_dev_std_ep_mask & (1 << i)) {
					hwpl_usb_disableep(USB_DEV_PORT, (void*)i);
				}
				if (usb_dev_std_ep_mask & ((1 << USB_LOGIC_EP_NUM) << i)) {
					hwpl_usb_disableep(USB_DEV_PORT, (void*)(i|USB_ENDPOINT_IN));
				}
			}
			usb_dev_std_init_ep();
			hwpl_usb_configure(USB_DEV_PORT, (void*)false);
		}

		if (usb_dev_std_cfg != usb_setup_pkt.wValue.u8[0]) {
			return false;
		}
		break;

	default:
		return false;
	}
	return (true);
}

static inline uint32_t usb_dev_std_req_getinterface(void);
uint32_t usb_dev_std_req_getinterface(void) {

	switch (usb_setup_pkt.bmRequestType.bitmap_t.recipient) {

	case USB_DEV_REQUEST_TO_INTERFACE:
		if ((usb_dev_std_cfg != 0) && (usb_setup_pkt.wIndex.u8[0] < usb_dev_std_num_interfaces)) {
			usb_dev_std_ep0_data.dptr = usb_alt_setting + usb_setup_pkt.wIndex.u8[0];
		} else {
			return false;
		}
		break;

	default:
		return false;

	}
	return (true);
}

static inline uint32_t usb_dev_std_req_setinterface(void) HWPL_ALWAYS_INLINE;
uint32_t usb_dev_std_req_setinterface(void){
	uint32_t interface_number = 0;
	uint32_t alternate_setting = 0;
	uint32_t prev_interface_number = 0;
	uint32_t mask = 0;
	uint32_t i;
	uint32_t j;
	uint32_t ret;
	usb_common_desc_t *dptr;

	switch (usb_setup_pkt.bmRequestType.bitmap_t.recipient) {
	case USB_DEV_REQUEST_TO_INTERFACE:
		if (usb_dev_std_cfg == 0){
			return false;
		}

		ret = false;
		dptr  = (usb_common_desc_t *)usb_cfg_descriptor;

		while (dptr->bLength) {
			switch (dptr->bDescriptorType) {

			case USB_CONFIGURATION_DESCRIPTOR_TYPE:
				if (((usb_cfg_desc_t *)dptr)->bConfigurationValue != usb_dev_std_cfg) {
					dptr = usb_dev_std_add_ptr(dptr, ((usb_cfg_desc_t *)dptr)->wTotalLength);
					continue;
				}
				break;

			case USB_INTERFACE_DESCRIPTOR_TYPE:
				interface_number = ((usb_interface_desc_t *)dptr)->bInterfaceNumber;
				alternate_setting = ((usb_interface_desc_t *)dptr)->bAlternateSetting;
				mask = 0;
				if ((interface_number == usb_setup_pkt.wIndex.u8[0]) && (alternate_setting == usb_setup_pkt.wValue.u8[0])) {
					ret = true;
					prev_interface_number = usb_alt_setting[interface_number];
					usb_alt_setting[interface_number] = (uint8_t)alternate_setting;
				}
				break;

			case USB_ENDPOINT_DESCRIPTOR_TYPE:
				if (interface_number == usb_setup_pkt.wIndex.u8[0]) {
					i = ((usb_ep_desc_t *)dptr)->bEndpointAddress & USB_DEV_EP_MASK;
					j = usb_dev_decode_ep(i);
					if (alternate_setting == usb_setup_pkt.wValue.u8[0]) {
						usb_dev_std_ep_mask |=  j;
						usb_dev_std_ep_halt &= ~j;
						hwpl_usb_cfgep(USB_DEV_PORT, dptr);
						hwpl_usb_enableep(USB_DEV_PORT, (void*)i);
						hwpl_usb_resetep(USB_DEV_PORT, (void*)i);
						mask |= j;
					}
					else if ((alternate_setting == prev_interface_number) && ((mask & j) == 0)) {
						usb_dev_std_ep_mask &= ~j;
						usb_dev_std_ep_halt &= ~j;
						hwpl_usb_disableep(USB_DEV_PORT, (void*)i);
					}
				}
				break;

			}
			dptr = usb_dev_std_add_ptr(dptr, dptr->bLength);
		}
		break;

	default:
		return false;
	}

	return ret;
}


#endif  /* USB_DEV_STD_INLINE_H_ */

/*! @} */
