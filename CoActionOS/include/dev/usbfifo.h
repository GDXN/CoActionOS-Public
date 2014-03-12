/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYS_UARTFIFO UART FIFO
 * @{
 *
 *
 * \ingroup SYS
 *
 * \details This device driver is used to add a FIFO read buffer to a UART port.
 *
 *
 *
 */

/*! \file  */

#ifndef DEV_USBFIFO_H_
#define DEV_USBFIFO_H_

#include <stdint.h>
#include "dev/ioctl.h"
#include "dev/usb.h"
#include "device.h"
#include "fifo.h"
#include "hwpl/types.h"

/*! \details This defines the configuration values for a FIFO that
 * is connected to an underlying device.
 *
 * \param device_name The name of the device (e.g "usb0-fifo")
 * \param cfg_ptr A pointer to the const \ref devfifo_cfg_t data structure
 * \param state_ptr A pointer to the state structure (see \ref devfifo_state_t)
 * \param mode_value The access mode (usually 0666)
 * \param uid_value The User ID
 * \param gid_value The Group ID
 *
 * \hideinitializer
 *
 * Example:
 * \code
 * const device_t devices[DEVICES_TOTAL+1] = {
 *	USBFIFO_DEVICE("usb0-fifo", &usbfifo_cfg, 0666, USER_ROOT, GROUP_ROOT),
 * 	...
 * 	DEVICE_TERMINATOR
 * }
 * \endcode
 *
 */
#define USBFIFO_DEVICE(device_name, cfg_ptr, state_ptr, mode_value, uid_value, gid_value) { \
		.name = device_name, \
		DEVICE_MODE(mode_value, uid_value, gid_value, S_IFCHR), \
		DEVICE_DRIVER(usbfifo), \
		.cfg.state = state_ptr, \
		.cfg.dcfg = cfg_ptr \
}


#define USBFIFO_DEVICE_CFG(target_port, target_endpoint, target_endpoint_size, target_buffer, buffer_size) { \
	.port = target_port, \
	.endpoint = target_endpoint, \
	.endpoint_size = target_endpoint_size, \
	.buffer = target_buffer, \
	.size = buffer_size, \
}


/*! \details This is used for the configuration of the device.
 *
 */
typedef struct {
	int port /*! The USB port associated with the device */;
	int endpoint /*! The USB endpoint number to read */;
	int endpoint_size /*! The USB endpoint number to read */;
	char * buffer /*! \brief The buffer for the fifo */;
	int size /*! \brief The size of the fifo */;
} usbfifo_cfg_t;


#endif /* DEV_USBFIFO_H_ */


/*! @} */
