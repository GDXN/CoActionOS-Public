/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup RTC_DEV RTC
 * @{
 *
 * \ingroup DEV
 */

#ifndef _HWPL_RTC_H_
#define _HWPL_RTC_H_

#include "../device.h"
#include "../dev/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_rtc_open(const device_cfg_t * cfg);
/*! \details The RTC time is read using the ioctl request: \ref I_RTC_GET.  This
 * function always returns an error.
 * \sa periph_read()
 *
 */
int hwpl_rtc_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details The RTC time is written using the ioctl request: \ref I_RTC_SET.  This
 * function always returns an error.
 * \sa periph_write()
 */
int hwpl_rtc_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_rtc_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_rtc_close(const device_cfg_t * cfg);

int hwpl_rtc_getattr(int port, void * ctl);
int hwpl_rtc_setattr(int port, void * ctl);
int hwpl_rtc_setaction(int port, void * ctl);

int hwpl_rtc_setalarm(int port, void * ctl);
int hwpl_rtc_getalarm(int port, void * ctl);
int hwpl_rtc_disablealarm(int port, void * ctl);
int hwpl_rtc_set(int port, void * ctl);
int hwpl_rtc_get(int port, void * ctl);
int hwpl_rtc_settime(int port, void * ctl);
int hwpl_rtc_gettime(int port, void * ctl);
int hwpl_rtc_setcountevent(int port, void * ctl);

#ifdef __cplusplus
}
#endif

#endif /* _HWPL_RTC_H_ */

/*! @} */
