/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup USB_DEV_STD USB Standard Device
 *
 *
 * @{
 * \ingroup USB_DEV
 *
 */

#ifndef USB_DEV_STD_H_
#define USB_DEV_STD_H_

#include <stdint.h>

#include "hwpl/types.h"
#include "usb_dev_typedefs.h"

typedef struct {
  uint8_t  *dptr;
  uint16_t cnt;
} usb_ep_data_t;


void usb_dev_std_reset_state(void);
int usb_dev_std_setup(void * context, const void * data);
void usb_dev_dfu_std_setup(uint32_t event);
void usb_dev_std_datain_stage(void) HWPL_NEVER_INLINE;
void usb_dev_std_dataout_stage (void);
void usb_dev_std_statusin_stage(void);
void usb_dev_std_statusout_stage (void);


//Is provided by usb_dev_std
extern usb_setup_pkt_t usb_setup_pkt;
extern usb_ep_data_t usb_dev_std_ep0_data;

extern uint16_t usb_dev_std_status;
extern uint8_t usb_dev_std_addr;
extern uint8_t usb_dev_std_cfg;
extern uint32_t usb_dev_std_ep_mask;
extern uint32_t usb_dev_std_ep_halt;
extern uint32_t usb_dev_std_ep_stall;
extern uint8_t usb_dev_std_num_interfaces;
extern uint8_t usb_dev_std_ep0_buf[USB_MAX_PACKET0];
extern usb_ep_data_t usb_dev_std_ep0_data;
extern usb_setup_pkt_t usb_setup_pkt;


#endif  /* USB_DEV_STD_H_ */

/*! @} */
