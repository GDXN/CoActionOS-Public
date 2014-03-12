/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PCA9674 PCA9674 Eight Channel LED Driver
 *

 *
 * @{
 *
 * \ingroup NXP
 *
 * \details This driver controls NXP's PCA9674 LED driver chip.  The chip features four PWM signals to drive
 * and LED.
 *
 * Required Peripheral Hardware:
 * - 1 I2C Bus
 *
 */

#ifndef DEV_PCA9674_H_
#define DEV_PCA9674_H_

#include <stdint.h>
#include "hwpl/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*! \details This defines the ptr value for
 * auto-incrementing through all registers.
 *
 */
#define PCA9674_PTR_AUTO_INCREMENT 0x10

/*! \details This defines the data structure
 * for the device's registers.  For example to read the pwm0 value:
 *
 * \code
 * #include <unistd.h>
 * #include <fcntl.h>
 * #include <stddef.h>
 *
 * uint8_t pwm0;
 *
 * lseek(pca9674_fd, offsetof(pca9674_t, pwm0), SEEK_SET);
 * read(pca9674_fd, &pwm0, 1);
 * \endcode
 */
typedef struct HWPL_PACK {
	uint8_t port /*! \brief 8-bit quasi-bidirectional IO value */;
} pca9674_t;


#ifdef __cplusplus
}
#endif

#endif /* DEV_PCA9674_H_ */

/*! @} */
