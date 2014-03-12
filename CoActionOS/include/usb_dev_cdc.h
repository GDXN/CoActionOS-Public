/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup USB_DEV_CDC Communications Device Class
 *
 *
 * @{
 * \ingroup USB_DEV
 *
 * \details This is not yet implemented.
 *
 */


/*! \file */

#ifndef USB_DEV_CDC_H_
#define USB_DEV_CDC_H_

#include <stdint.h>
#include "hwpl/usb.h"

//bDescriptorType field
#define CDC_CS_INTERFACE 0x24
#define CDC_CS_ENDPOINT 0x25

//bDescriptorSubType
#define CDC_DESCRIPTOR_SUBTYPE_HEADER 0x00
#define CDC_DESCRIPTOR_SUBTYPE_CALL_MGT 0x01
#define CDC_DESCRIPTOR_SUBTYPE_ABSTRACT_CONTROL_MGT 0x02
#define CDC_DESCRIPTOR_SUBTYPE_DIRECT_LINE_MGT 0x03
#define CDC_DESCRIPTOR_SUBTYPE_TELEPHONE_RINGER 0x04
#define CDC_DESCRIPTOR_SUBTYPE_TELEPHONE_CALL_AND_LINE_STATE_REPORTING_CAPABILITIES 0x05
#define CDC_DESCRIPTOR_SUBTYPE_UNION 0x06
#define CDC_DESCRIPTOR_SUBTYPE_COUNTRY_SEL 0x07
#define CDC_DESCRIPTOR_SUBTYPE_TELEPHONE_OPERATION 0x08
#define CDC_DESCRIPTOR_SUBTYPE_USB_TERMINAL 0x09
#define CDC_DESCRIPTOR_SUBTYPE_NETWORK_CHANNEL 0x0A
#define CDC_DESCRIPTOR_SUBTYPE_PROTOCOL_UNIT 0x0B
#define CDC_DESCRIPTOR_SUBTYPE_EXTENSION_UNIT 0x0C
#define CDC_DESCRIPTOR_SUBTYPE_MULTI_CHANNEL 0x0D
#define CDC_DESCRIPTOR_SUBTYPE_CAPI_CONTROL 0x0E
#define CDC_DESCRIPTOR_SUBTYPE_ETHERNET_NETWORKING 0x0F
#define CDC_DESCRIPTOR_SUBTYPE_ATM_NETWORKING 0x10

//Interface Class specific requests
#define SEND_ENCAPSULATED_COMMAND 0x00
#define GET_ENCAPSULATED_RESPONSE 0x01
#define SET_COMM_FEATURE 0x02
#define GET_COMM_FEATURE 0x03
#define CLEAR_COMM_FEATURE 0x04
#define SET_AUX_LINE_STATE 0x10
#define SET_HOOK_STATE 0x11
#define PULSE_SETUP 0x12
#define SEND_PULSE 0x13
#define SET_PULSE_TIME 0x14
#define RING_AUX_JACK 0x15
#define SET_LINE_CODING 0x20
#define GET_LINE_CODING 0x21
#define SET_CONTROL_LINE_STATE 0x22
#define SEND_BREAK 0x23
#define SET_RINGER_PARMS 0x30
#define GET_RINGER_PARMS 0x31
#define SET_OPERATION_PARMS 0x32
#define GET_OPERATION_PARMS 0x33
#define SET_LINE_PARMS 0x34
#define GET_LINE_PARMS 0x35
#define DIAL_DIGITS 0x36
#define SET_UNIT_PARAMETER 0x37
#define GET_UNIT_PARAMETER 0x38
#define CLEAR_UNIT_PARAMETER 0x39
#define GET_PROFILE 0x3A
#define SET_ETHERNET_MULTICAST_FILTERS 0x40
#define SET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER 0x41
#define GET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER 0x42
#define SET_ETHERNET_PACKET_FILTER 0x43
#define GET_ETHERNET_STATISTIC 0x44
#define SET_ATM_DATA_FORMAT 0x50
#define GET_ATM_DEVICE_STATISTICS 0x51
#define SET_ATM_DEFAULT_VC 0x52
#define GET_ATM_VC_STATISTICS 0x53


typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t functionSpecificData[];
} cdc_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint16_t bcdCDC;
} cdc_header_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bmCapabilities;
	uint8_t bDataInterface;
} cdc_call_mgt_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bmCapabilities;
} cdc_abstract_contorl_mgt_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bmCapabilities;
} cdc_direct_line_mgt_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bRingerVolSteps;
	uint8_t bNumRingerPatterns;
} cdc_telephone_ringer_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bmCapabilities;
} cdc_telephone_operational_modes_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bmCapabilities;
} cdc_telephone_call_and_line_reporting_capabilities_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bMasterInterface;
	uint8_t bSlaveInterface[];
} cdc_union_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t iCountryCodeRelDate;
	uint16_t wCountryCode[];
} cdc_country_selection_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bEntityId;
	uint8_t bInInterfaceNo;
	uint8_t bOutInterfaceNo;
	uint8_t bmOptions;
	uint8_t bChildId[];
} cdc_usb_terminal_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bEntityId;
	uint8_t iName;
	uint8_t bChannelIndex;
	uint8_t bPhysicalInterface;
} cdc_network_channel_terminal_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bEntityId;
	uint8_t bProtocol;
	uint8_t bChildId[];
} cdc_protocol_unit_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bEntityId;
	uint8_t bExtensionCode;
	uint8_t iName;
	uint8_t bChildId[];
} cdc_extension_unit_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bmCapabilities;
} cdc_multi_channel_mgt_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bmCapabilities;
} cdc_capi_control_mgt_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t iMACAddress;
	uint32_t bmEthernetStatistics;
	uint16_t wMaxSegmentSize;
	uint16_t wNumberMCFilters;
	uint8_t bNumberPowerFilters;
} cdc_ethernet_netorking_func_desc_t;

typedef struct {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t iEndSystemIdentifier;
	uint8_t bmDataCapabilities;
	uint8_t bmATMDeviceStatistics;
	uint16_t wType2MaxSegmentSize;
	uint16_t wType3MaxSegmentSize;
	uint16_t wMaxVC;
} cdc_atm_netorking_func_desc_t;


#include "usb_dev_std.h"
int usb_dev_cdc_if_req(int event);

#endif /* USB_DEV_CDC_H_ */

/*! @} */
