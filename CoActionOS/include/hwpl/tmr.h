/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup TMR_DEV TMR
 * @{
 *
 * \ingroup DEV
 */

#ifndef _HWPL_TMR_H_
#define _HWPL_TMR_H_

#include "../device.h"
#include "../dev/tmr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_tmr_open(const device_cfg_t * cfg);
/*! \details The timer value is read using the ioctl request: \ref I_TMR_GET.  This
 * function always returns an error.
 * \sa periph_read()
 *
 */
int hwpl_tmr_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details The timer value is written using the ioctl request: \ref I_TMR_SET.  This
 * function always returns an error.
 * \sa periph_write()
 */
int hwpl_tmr_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_tmr_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_tmr_close(const device_cfg_t * cfg);


int hwpl_tmr_getattr(int port, void * ctl);
int hwpl_tmr_setattr(int port, void * ctl);
int hwpl_tmr_setaction(int port, void * ctl);

int hwpl_tmrsimple_getattr(int port, void * ctl);
int hwpl_tmrsimple_setattr(int port, void * ctl);
int hwpl_tmrsimple_setaction(int port, void * ctl);

int hwpl_tmr_on(int port, void * ctl);
int hwpl_tmr_off(int port, void * ctl);
int hwpl_tmr_setoc(int port, void * ctl);
int hwpl_tmr_getoc(int port, void * ctl);
int hwpl_tmr_setic(int port, void * ctl);
int hwpl_tmr_getic(int port, void * ctl);
int hwpl_tmr_set(int port, void * ctl);
int hwpl_tmr_get(int port, void * ctl);



#ifdef __cplusplus
}
#endif

#endif // _HWPL_TMR_H_

/*! @} */

