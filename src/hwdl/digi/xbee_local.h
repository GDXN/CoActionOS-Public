/*
 * xbee_local.h
 *
 *  Created on: Dec 27, 2011
 *      Author: tgil
 */

#ifndef XBEE_LOCAL_H_
#define XBEE_LOCAL_H_

typedef enum {
	XBEE_FRAMETYPE_ATCOMMAND /*! An AT Command */ = 0x08,
	XBEE_FRAMETYPE_ATCOMMAND_QUEUE /*! An AT Command */ = 0x09,
	XBEE_FRAMETYPE_ATCOMMAND_RESPONSE /*! AT Command response */ = 0x88,
	XBEE_FRAMETYPE_REMOTE_ATCOMMAND /*! An AT Command to a remote device */ = 0x17,
	XBEE_FRAMETYPE_REMOTE_ATCOMMAND_RESPONSE /*! An AT Command */ = 0x97,

	XBEE_FRAMETYPE_ZBEE_TX_REQ /*! Transmit Zigbee Packet */ = 0x10,
	XBEE_FRAMETYPE_ZBEE_EXPLICIT_TX_REQ /*! Explicit Zigbee packet transmission */ = 0x11,
	XBEE_FRAMETYPE_ZBEE_TX_STATUS /*! Zigbee packet transmit status */ = 0x8B,
	XBEE_FRAMETYPE_ZBEE_RX_PACKET /*! Received Zigbee data */ = 0x90,
	XBEE_FRAMETYPE_ZBEE_RX_PACKET_EXPLICIT /*! Received an explicit packet */ = 0x91,
	XBEE_FRAMETYPE_ZBEE_IO_DATA_SAMPLE_RX_INDICATOR /*! An AT Command */ = 0x92,

	XBEE_FRAMETYPE_CREATE_SOURCE_ROUTE /*! An AT Command */ = 0x21,
	XBEE_FRAMETYPE_MODEM_STATUS /*! An AT Command */ = 0x8A,
	XBEE_FRAMETYPE_XBEE_SENSOR_READ_INDICATOR /*! An AT Command */ = 0x94,
	XBEE_FRAMETYPE_NODE_ID_INDICATOR /*! An AT Command */ = 0x95,
	XBEE_FRAMETYPE_OTA_FW_UPDATE_STATUS /*! An AT Command */ = 0xA0,
	XBEE_FRAMETYPE_ROUTE_RECORD_INDICATOR /*! An AT Command */ = 0xA1,
	XBEE_FRAMETYPE_MANY_TO_ONE_ROUTE_REQ_INDICATOR /*! An AT Command */ = 0xA3,
	XBEE_FRAMETYPE_TOTAL
} xbee_frametype_t;

typedef struct HWPL_PACK {
	uint8_t start_delimiter;
	uint8_t len_msb;
	uint8_t len_lsb;
	uint8_t frame_type;
	uint8_t frame_id;
} xbee_hdr_t;

typedef struct HWPL_PACK{
	xbee_hdr_t hdr;
	char at_command[2];
	uint8_t parameter;
} xbee_frame_atcommand_t;

typedef struct HWPL_PACK {
	uint8_t dev_addr[8] /*! The device address (big endian) */;
	uint8_t net_addr[2] /*! The network address (big endian) */;
} xbee_zbee_addrs_t;

typedef struct HWPL_PACK{
	xbee_hdr_t hdr /*! The XBee API header */;
	xbee_zbee_addrs_t addrs /*! The Zigbee addresses */;
	uint8_t bcast_rad /*! The broadcast radius */;
	uint8_t options /*! Options bit mask */;
	uint8_t data[] /*! The data to transmit */;
} xbee_frame_zbee_tx_req_t;



#endif /* XBEE_LOCAL_H_ */
