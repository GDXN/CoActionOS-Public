/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup USB_DEV_RUNTIME_DFU USB Runtime Device Firmware Upgrade
 *
 *
 * @{
 * \ingroup USB_DEV
 *
 */

#ifndef USB_DEV_RUNTIME_DFU_H_
#define USB_DEV_RUNTIME_DFU_H_

#include "config.h"

#define USB_DEV_RUNTIME_DFU_SW_REQ_LOC 0x10002000
#define USB_DEV_RUNTIME_DFU_SW_REQ_VALUE 0x55AA55AA

void usb_dev_runtime_dfu_init(void);
void usb_dev_runtime_dfu_reset_event(void);
extern volatile usb_dev_dfu_status_t usb_dev_runtime_dfu_status;
static inline uint8_t usb_dev_runtime_dfu_get_state(void) HWPL_ALWAYS_INLINE;
uint8_t usb_dev_runtime_dfu_get_state(void){ return usb_dev_runtime_dfu_status.bState; }


static inline void usb_dev_runtime_dfu_set_appDETACH(void) HWPL_ALWAYS_INLINE;
void usb_dev_runtime_dfu_set_appDETACH(void){ usb_dev_runtime_dfu_status.bState = appDETACH; }

#endif /* USB_DEV_RUNTIME_DFU_H_ */

/*! @} */
