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

#ifndef DEV_UARTFIFO_H_
#define DEV_UARTFIFO_H_

#include <stdint.h>
#include "dev/ioctl.h"
#include "dev/uart.h"
#include "device.h"
#include "fifo.h"
#include "hwpl/types.h"

/*! \details This defines the configuration values for a FIFO that
 * is connected to an underlying device.
 *
 * \param device_name The name of the device (e.g "uart-fifo")
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
 *	DEVICE_PERIPH("uart0", uart, 0, 0666, USER_ROOT, GROUP_ROOT, S_IFCHR),
 *	DEVFIFO_DEVICE("uart0-fifo", &devices[0], 0666, USER_ROOT, GROUP_ROOT),
 * 	...
 * 	DEVICE_TERMINATOR
 * }
 * \endcode
 *
 */
#define UARTFIFO_DEVICE(device_name, cfg_ptr, state_ptr, mode_value, uid_value, gid_value) { \
		.name = device_name, \
		DEVICE_MODE(mode_value, uid_value, gid_value, S_IFCHR), \
		DEVICE_DRIVER(uartfifo), \
		.cfg.state = state_ptr, \
		.cfg.dcfg = cfg_ptr \
}


#define UARTFIFO_DEVICE_CFG(target_port, target_buffer, buffer_size) { \
	.port = target_port, \
	.buffer = target_buffer, \
	.size = buffer_size, \
}


/*! \details This is used for the configuration of the device.
 *
 */
typedef struct {
	int port /*! The UART port associated with the device */;
	char * buffer /*! \brief The buffer for the fifo */;
	int size /*! \brief The size of the fifo */;
} uartfifo_cfg_t;


#endif /* DEV_UARTFIFO_H_ */


/*! @} */
