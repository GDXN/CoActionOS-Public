/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup DAC_DEV DAC
 * @{
 * \ingroup DEV
 */

#ifndef _HWPL_DAC_H_
#define _HWPL_DAC_H_

#include "../device.h"
#include "../dev/dac.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_dac_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_dac_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_dac_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_dac_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_dac_close(const device_cfg_t * cfg);


int hwpl_dac_getattr(int port, void * ctl);
int hwpl_dac_setattr(int port, void * ctl);
int hwpl_dac_setaction(int port, void * ctl);
int hwpl_dac_get(int port, void * ctl);


#ifdef __cplusplus
}
#endif


#endif // _HWPL_DAC_H_

/*! @} */


