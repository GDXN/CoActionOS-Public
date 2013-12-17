/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup EINT_DEV EINT
 * @{
 *
 * \ingroup DEV
 */

#ifndef _HWPL_EINT_H_
#define _HWPL_EINT_H_


#include "../device.h"
#include "../dev/eint.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_eint_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_eint_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_eint_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_eint_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_eint_close(const device_cfg_t * cfg);

int hwpl_eint_getattr(int port, void * ctl);
int hwpl_eint_setattr(int port, void * ctl);
int hwpl_eint_setaction(int port, void * ctl);


#ifdef __cplusplus
}
#endif

#endif /* EINT_H_ */

/*! @} */

