/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup XBEE XBee RF Transceiver
 * @{
 *
 *
 * \ingroup DIGI
 *
 * \details This device driver reads and writes wireless data using the XBee RF transceiver.  This driver
 * is not implemented in this version.
 *
 *
 */

/*! \file  */

#ifndef XBEE_H_
#define XBEE_H_

#include <stdint.h>
#include "dev/ioctl.h"
#ifdef __cplusplus
extern "C" {
#endif


#define XBEE_IOC_CHAR 'x'


typedef struct {
	char cmd[2];
	uint8_t parameter;
} xbee_at_command_t;


#define I_XBEE_SENDATCOMMAND _IOCTLW(XBEE_IOC_CHAR, 0, xbee_at_command_t)
#define I_XBEE_SEND_AT_COMMAND I_XBEE_SENDATCOMMAND


#ifdef __cplusplus
}
#endif


#endif /* XBEE_H_ */


/*! @} */
