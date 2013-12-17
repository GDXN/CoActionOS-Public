/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup LINK
 *
 * @{
 *
 * \details This module provides the interface to CoActionOS Link as a USB device.  The interface can be
 * customized by adding new initialization values for \ref link_dev_desc and \ref link_string_desc in the
 * \a devices.c file.  Only the following values can be changed by the user:
 * - USB VID
 * - USB PID
 * - Manufacturer String
 * - Product String
 * - Serial Number String
 *
 * Changing any of the above values will require custom USB drivers to make software applications work with
 * CoActionOS.
 *
 */

/*! \file */


#ifndef LINK_H_
#define LINK_H_

#include "device.h"
#include <link_protocol.h>
#include "usb_dev_typedefs.h"
#include "debug.h"

typedef struct HWPL_PACK {
	usb_dev_cdc_header_t header;
	usb_dev_cdc_acm_t acm;
	usb_dev_cdc_uniondescriptor_t union_descriptor;
	usb_dev_cdc_callmanagement_t call_management;
} link_cdc_acm_interface_t;

typedef struct HWPL_PACK {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bFirstInterface;
	uint8_t bInterfaceCount;
	uint8_t bFunctionClass;
	uint8_t bFunctionSubClass;
	uint8_t bFunctionProtocol;
	uint8_t iFunction;
} usb_dev_interface_assocation_t;


/* \details This structure defines the USB descriptors.  This
 * value is read over the control channel by the host to configure
 * the device.
 */
typedef struct HWPL_PACK {
	usb_cfg_desc_t cfg /* The configuration descriptor */;
	//usb_dev_interface_assocation_t ifasso;
	usb_interface_desc_t ifcontrol /* The interface descriptor */;
	link_cdc_acm_interface_t acm /*! The CDC ACM Class descriptor */;
	usb_ep_desc_t control /* Endpoint:  Interrupt out for control packets */;
	usb_interface_desc_t ifdata /* The interface descriptor */;
	usb_ep_desc_t data_out /* Endpoint:  Bulk out */;
	usb_ep_desc_t data_in /* Endpoint:  Bulk in */;
	uint8_t terminator  /* A null terminator used by the driver (required) */;
} link_cfg_desc_t;


int link_init(void);
void * link_update(void * arg);
void link_set_cmd_handler(int (*handler)(link_op_t*));

#endif /* LINK_H_ */


/*! @} */
