/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup I2C_DEV
 * @{
 *
 * \ingroup DEV
 */

#ifndef _HWPL_I2C_H_
#define _HWPL_I2C_H_


#include "../device.h"
#include "../dev/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_i2c_open(const device_cfg_t * cfg);
/*! \details
 *
 * \sa periph_read()
 *
 */
int hwpl_i2c_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_i2c_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_i2c_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_i2c_close(const device_cfg_t * cfg);

int hwpl_i2c_getattr(int port, void * ctl);
int hwpl_i2c_setattr(int port, void * ctl);
int hwpl_i2c_setaction(int port, void * ctl);

int hwpl_i2c_setup(int port, void * ctl);
int hwpl_i2c_geterr(int port, void * ctl);


#ifdef __cplusplus
}
#endif

#endif // _HWPL_I2C_H_

/*! @} */

