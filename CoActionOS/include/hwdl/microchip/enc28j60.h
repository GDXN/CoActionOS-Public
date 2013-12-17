/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup ENC28J60 ENC28J60 SPI to Ethernet Device
 * @{
 *
 */

#ifndef HWDL_ENC28J60_H_
#define HWDL_ENC28J60_H_

#include "hwpl.h"
#include "dev/eth.h"
#include "dev/microchip/enc28j60.h"

/*! \details This defines the device specific volatile configuration.
 *
 */
typedef struct {
	const char * buf;
	int nbyte;
	uint8_t cmd[8];
	device_transfer_t op;
	hwpl_callback_t callback;
	void * context;
	uint16_t next_pkt;
} enc28j60_state_t;

typedef struct {
	uint32_t rx_buffer_size;
	uint32_t tx_buffer_size;
} enc28j60_cfg_t;

int enc28j60_open(const device_cfg_t * cfg);
int enc28j60_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int enc28j60_read(const device_cfg_t * cfg, device_transfer_t * rop);
int enc28j60_write(const device_cfg_t * cfg, device_transfer_t * wop);
int enc28j60_close(const device_cfg_t * cfg);

int enc28j60_getattr(const device_cfg_t * cfg, void * ctl);
int enc28j60_setattr(const device_cfg_t * cfg, void * ctl);
int enc28j60_setaction(const device_cfg_t * cfg, void * ctl);
int enc28j60_init_tx_pkt(const device_cfg_t * cfg, void * ctl);
int enc28j60_send_tx_pkt(const device_cfg_t * cfg, void * ctl);
int enc28j60_tx_pkt_busy(const device_cfg_t * cfg, void * ctl);
int enc28j60_rx_pkt_rdy(const device_cfg_t * cfg, void * ctl);
int enc28j60_rx_pkt_complete(const device_cfg_t * cfg, void * ctl);



#endif /* HWDL_ENC28J60_H_ */

/*! @} */
