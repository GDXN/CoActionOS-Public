/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PWM_DEV PWM
 * @{
 * \ingroup DEV
 *
 */

#ifndef _HWPL_PWM_H_
#define _HWPL_PWM_H_

#include "../device.h"
#include "../dev/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_pwm_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_pwm_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_pwm_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_pwm_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_pwm_close(const device_cfg_t * cfg);

int hwpl_pwm_getattr(int port, void * ctl);
int hwpl_pwm_setattr(int port, void * ctl);
int hwpl_pwm_setaction(int port, void * ctl);
int hwpl_pwm_set(int port, void * ctl);

#ifdef __cplusplus
}
#endif

#endif /* _HWPL_PWM_H_ */

/*! @} */
