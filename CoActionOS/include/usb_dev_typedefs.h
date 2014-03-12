/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup USBDEFS USB Definitions
 *
 *
 * @{
 *
 * \ingroup USB_DEV
 *
 */

#ifndef USB_TYPEDEFS_H_
#define USB_TYPEDEFS_H_

typedef union HWPL_PACK {
	uint16_t u16;
	uint8_t u8[2];
} uint16_union_t;

typedef union HWPL_PACK {
	struct HWPL_PACK {
		uint8_t recipient:5;
		uint8_t type:2;
		uint8_t dir:1;
	} bitmap_t;
	uint8_t u8;
} usb_req_t;

typedef struct HWPL_PACK {
	usb_req_t bmRequestType;
	uint8_t bRequest;
	uint16_union_t wValue;
	uint16_union_t wIndex;
	uint16_t wLength;
} usb_setup_pkt_t;

typedef struct HWPL_PACK {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
} usb_ep_desc_t;

typedef struct HWPL_PACK {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
} usb_interface_desc_t;

typedef struct HWPL_PACK {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
} usb_dev_desc_t;

typedef struct HWPL_PACK {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize;
	uint8_t bNumConfigurations;
	uint8_t bReserved;
} usb_dev_qual_desc_t;

typedef struct HWPL_PACK {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t bMaxPower;
} usb_cfg_desc_t;

typedef struct HWPL_PACK {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t * bString;
	void * next;
} usb_string_desc_t;

typedef struct HWPL_PACK {
	uint8_t bLength;
	uint8_t bDescriptorType;
}usb_common_desc_t;

typedef struct HWPL_PACK {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint16_t bcdCDC;
} usb_dev_cdc_header_t;

typedef struct HWPL_PACK {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bmCapabilities;
} usb_dev_cdc_acm_t;

typedef struct HWPL_PACK {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bMasterInterface;
	uint8_t bSlaveInterface;
} usb_dev_cdc_uniondescriptor_t;

typedef struct HWPL_PACK {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bmCapabilities;
	uint8_t bDataInterface;
} usb_dev_cdc_callmanagement_t;


typedef struct HWPL_PACK {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bEntityId;
	uint8_t bExtensionCode;
	uint8_t iName;
	uint8_t bChildId[];
} usb_dev_cdc_extensionunit_t;

typedef struct HWPL_PACK {
	uint16_t bcdDevice;
	uint16_t idProduct;
	uint16_t idVendor;
	uint16_t bcdDFU;
	char ucDfuSignature[3];
	uint8_t bLength;
	uint32_t dwCRC;
} usb_dev_dfu_file_suffix_t;

typedef struct HWPL_PACK {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bmAttributes;
	uint16_t wDetachTimeOut;
	uint16_t wTransferSize;
	uint16_t bcdDFUVersion;
} usb_dev_dfu_func_desc_t;

typedef struct HWPL_PACK {
	uint8_t bStatus;
	uint8_t bwPollTimeout[3];
	uint8_t bState;
	uint8_t iString;
} usb_dev_dfu_status_t;

/*! \details This macro function allows the user to declare a USB string
 * data structure.
 */
#define usb_declare_string(len) struct \
		HWPL_PACK { \
	uint8_t bLength; \
	uint8_t bDescriptorType; \
	uint16_t string[len]; \
}

/*! \details This macro function allows the user to assign values to a USB
 * string using comma separated characters.
 *
 * \param len The number of characters in the string.
 * \param ... Comma separated characters (e.g. 'E','x','a','m','p','l','e')
 */
#define usb_assign_string(len, ...) { \
		.bLength = len*2+2, \
		.bDescriptorType = USB_STRING_DESCRIPTOR_TYPE, \
		.string = { __VA_ARGS__ } }


#endif /* USB_TYPEDEFS_H_ */

/*! @} */
