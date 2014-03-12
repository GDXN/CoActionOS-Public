/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup EEPROM EEPROM (EE)
 *
 * @{
 *
 * \ingroup PERIPHIO
 *
 * \details The EEPROM module has functions to access the on chip EEPROM.
 *
 *
 */

/*! \file
 * \brief EEPROM Header file
 */

#ifndef DEV_EEPROM_H_
#define DEV_EEPROM_H_

#include "device.h"
#include "dev/eeprom.h"

#ifdef __cplusplus
extern "C" {
#endif

int hwpl_eeprom_open(const device_cfg_t * cfg);
int hwpl_eeprom_read(const device_cfg_t * cfg, device_transfer_t * rop);
int hwpl_eeprom_write(const device_cfg_t * cfg, device_transfer_t * wop);
int hwpl_eeprom_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int hwpl_eeprom_close(const device_cfg_t * cfg);

int hwpl_eeprom_getattr(int port, void * ctl);
int hwpl_eeprom_setattr(int port, void * ctl);
int hwpl_eeprom_setaction(int port, void * ctl);


#ifdef __cplusplus
}
#endif

#endif // EEPROM_HEADER

/*! @} */
