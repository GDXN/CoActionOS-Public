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

#include <stdint.h>
#include "device.h"
#include "link_protocol.h"


int link_init(void);
void * link_update(void * arg);


#endif /* LINK_H_ */


/*! @} */
