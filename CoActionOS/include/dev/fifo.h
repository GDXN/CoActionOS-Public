/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYS_FIFO FIFO Buffer
 * @{
 *
 *
 * \ingroup SYS
 *
 * \details This device is a FIFO RAM buffer.  A write to the buffer adds
 * data to the FIFO.  A read from the buffer reads from the FIFO.  If the FIFO
 * fills up, additional writes will block until data arrives.  Similarly, if the FIFO
 * is empty, a read will block until data arrives.
 *
 */

/*! \file  */

#ifndef DEV_FIFO_H_
#define DEV_FIFO_H_

#include <stdint.h>
#include "dev/ioctl.h"
#include "hwpl/types.h"

#define FIFO_IOC_CHAR 'f'

/*! \brief See below.
 * \details This defines the configuration values for a FIFO device.
 *
 * \param device_name The name of the device (e.g "fifo0")
 * \param cfg_ptr A pointer to the const \ref fifo_cfg_t data structure
 * \param state_ptr A pointer to the state structure (see \ref fifo_state_t)
 * \param mode_value The access mode (usually 0666)
 * \param uid_value The User ID
 * \param gid_value The Group ID
 *
 * \hideinitializer
 *
 * Example:
 * \code
 * #include <hwdl/fifo.h>
 * const fifo_cfg_t fifo_cfg0 = { .buffer = fifo_buf0, .size = FIFO_BUF_SIZE };
 * fifo_state_t fifo_state0;
 * const device_t devices[DEVICES_TOTAL+1] = {
 * 	...
 * 	FIFO_DEVICE("fifo0", &fifo_cfg0, &fifo_state0, 0666, USER_ROOT, GROUP_ROOT),
 * 	...
 * 	DEVICE_TERMINATOR
 * }
 * \endcode
 *
 */

#define FIFO_DEVICE(device_name, cfg_ptr, state_ptr, mode_value, uid_value, gid_value) { \
		.name = device_name, \
		DEVICE_MODE(mode_value, uid_value, gid_value, S_IFCHR), \
		DEVICE_DRIVER(fifo), \
		.cfg.state = state_ptr, \
		.cfg.dcfg = cfg_ptr \
}


#define FIFO_DEVICE_CFG(buf, buf_size) { .buffer = buf, .size = buf_size }

/*! \brief FIFO Attributes
 * \details This structure defines the attributes of a FIFO.
 *  The attributes are read-only using \ref I_FIFO_GETATTR.
 */
typedef struct HWPL_PACK {
	uint32_t size /*! \brief The total number of bytes in the FIFO */;
	uint32_t used /*! \brief The number of bytes ready to be read from the FIFO */;
	uint32_t overflow /*! \brief If non-zero, indicates data received in the buffer was discarded */;
} fifo_attr_t;

/*! \brief See below.
 * \details This request gets the size of the FIFO in bytes.
 * Example:
 * \code
 * fifo_attr_t attr;
 * ioctl(fifo_fd, I_FIFO_GETATTR, &attr);
 * \endcode
 * \hideinitializer
 */
#define I_FIFO_GETATTR _IOCTLR(FIFO_IOC_CHAR, 0, fifo_attr_t)


/*! \brief This request flushes all data out of the fifo (third ioctl() arg is omitted).
 */
#define I_FIFO_FLUSH _IOCTL(FIFO_IOC_CHAR, 1)

/*! \brief This request initializes the fifo (third ioctl() arg is omitted).
 */
#define I_FIFO_INIT _IOCTL(FIFO_IOC_CHAR, 2)

/*! \brief This request shuts down the operation of the FIFO.
 */
#define I_FIFO_EXIT _IOCTL(FIFO_IOC_CHAR, 3)

/*! \brief This request tells the FIFO whether or not
 * to block when data is written and the buffer is full.
 *
 */
#define I_FIFO_SETWRITEBLOCK _IOCTL(FIFO_IOC_CHAR, 4)

#define I_FIFO_TOTAL 5

/*! \brief FIFO Configuration
 * \details This structure defines the static FIFO configuration.
 *
 */
typedef struct HWPL_PACK {
	uint32_t size /*! \brief The size of the buffer (only size-1 is usable) */;
	char * buffer /*! \brief A pointer to the buffer */;
} fifo_cfg_t;


#endif /* DEV_FIFO_H_ */


/*! @} */
