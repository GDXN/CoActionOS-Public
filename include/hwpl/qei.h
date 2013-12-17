/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup QEI_DEV QEI
 * @{
 *
 * \ingroup DEV
 */

#ifndef _HWPL_QEI_H_
#define _HWPL_QEI_H_

#include "../device.h"
#include "../dev/qei.h"

#ifdef __cplusplus
extern "C" {
#endif


/*! \details
 * \sa periph_open()
 *
 */
int hwpl_qei_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_qei_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_qei_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_qei_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_qei_close(const device_cfg_t * cfg);


int hwpl_qei_getattr(int port, void * ctl);
int hwpl_qei_setattr(int port, void * ctl);
int hwpl_qei_setaction(int port, void * ctl);

int hwpl_qei_get(int port, void * ctl);
int hwpl_qei_getvelocity(int port, void * ctl);
int hwpl_qei_getindex(int port, void * ctl);
int hwpl_qei_reset(int port, void * ctl);

#ifdef __cplusplus
}
#endif


#endif /* _HWPL_QEI_H_ */

/*! @} */
