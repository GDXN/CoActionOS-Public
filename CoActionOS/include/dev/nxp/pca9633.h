/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PCA9633 PCA9633 Quad LED Driver
 *

 *
 * @{
 *
 * \ingroup NXP
 *
 * \details This driver controls NXP's PCA9633 LED driver chip.  The chip features four PWM signals to drive
 * and LED.
 *
 * Required Peripheral Hardware:
 * - 1 I2C Bus
 *
 */

#ifndef DEV_PCA9633_H_
#define DEV_PCA9633_H_


#include <stdint.h>
#include "hwpl/types.h"


#ifdef __cplusplus
extern "C" {
#endif

/*! \details This defines the ptr value for
 * auto-incrementing through all registers.
 *
 */
#define PCA9633_PTR_AUTO_INCREMENT_ALL 0x80

/*! \details Auto-Increment for individual brightness registers only.
 * D3, D2, D1, D0 roll over to Ô0010Õ after the last register (0101) is accessed.
 *
 */
#define PCA9633_PTR_AUTO_INCREMENT_BRIGHTNESS 0xA0

/*! \details Auto-Increment for global control registers only.
 * D3, D2, D1, D0 roll over to Ô0110Õ after the last register (0111) is accessed.
 *
 */
#define PCA9633_PTR_AUTO_INCREMENT_GLOBAL 0xC0

/*! \details Auto-Increment for individual and global control registers only.
 * D3, D2, D1, D0 roll over to Ô0010Õ after the last register (0111) is accessed.
 *
 */
#define PCA9633_PTR_AUTO_INCREMENT_BRIGHTNESS_GLOBAL 0xE0

/*! \details This defines the data structure
 * for the device's registers.  The pointer value
 * can be assigned a PCA9633_PTR_AUTO_INCREMENT_ value OR'd
 * with the offset the of the starting variable.
 *
 * For example:
 * \code
 * i2c_reqattr_t transfer;
 * uint8_t pwm0;
 *
 * transfer.ptr = PCA9633_PTR_AUTO_INCREMENT_ALL | offsetof(pca9633_t, pwm0);
 * ioctl(pca9633_fd, I_I2C_SETUP, &transfer);
 * read(pca9633_fd, &mode1, 1);
 *
 * \endcode
 */
typedef struct HWPL_PACK {
	uint8_t mode1 /*! \brief See below
	\details Bits:
	- 7: Auto increment 2 (read-only)
	- 6: Auto increment 1 (read-only)
	- 5: Auto increment 0 (read-only)
	- 4: Sleep if set
	- 3: Respond to subadr1 if set
	- 2: Respond to subadr2 if set
	- 1: Respond to subadr3 if set
	- 0: Respond to allcall if set
	*/;
	uint8_t mode2 /*! \brief See below
	\details Bits:
	- 7: reserved (read-only)
	- 6: reserved (read-only)
	- 5: Group control dimming for 0 and group control blinking for 1
	- 4: If set, invert logic state
	- 3: If set, outputs change on ACK; if clear, outputs change on STOP
	- 2: If set, outputs configured with totem pole; if clear, outputs configured with open drain
	- 1 to 0: See datasheet (output enable configuration)
	*/;
	uint8_t pwm0 /*! \brief Duty cycle for channel 0 */;
	uint8_t pwm1 /*! \brief Duty cycle for channel 1 */;
	uint8_t pwm2 /*! \brief Duty cycle for channel 2 */;
	uint8_t pwm3 /*! \brief Duty cycle for channel 3 */;
	uint8_t grppwm /*! \brief See datasheet for details */;
	uint8_t grpfreq /*! \brief See datasheet for details */;
	uint8_t ledout /*! \brief See PCA9633_LEDOUT_ values for details */;
	uint8_t subadr1 /*! \brief Left-justified 7-bit sub address 1 (responds if mode1 bit 3 is set) */;
	uint8_t subadr2 /*! \brief Left-justified 7-bit sub address 2 (responds if mode1 bit 2 is set) */;
	uint8_t subadr3 /*! \brief Left-justified 7-bit sub address 3 (responds if mode1 bit 1 is set) */;
	uint8_t allcall /*! \brief All Call address (responds if mode1 bit 0 is set) */;
} pca9633_t;

/*! \details When written to pca9633_t:ledout, turns the led off.
 *
 */
#define PCA9633_LEDOUT_OFF(x) (0)

/*! \details When written to pca9633_t:ledout, turns the led on.
 *
 */
#define PCA9633_LEDOUT_ON(x) (1 << (x*2))

/*! \details When written to pca9633_t:ledout, led is controlled by PWM.
 *
 */
#define PCA9633_LEDOUT_PWM(x) (2 << (x*2))

/*! \details When written to pca9633_t:ledout, led is controlled by GRPPWM
 * (see datasheet).
 *
 */
#define PCA9633_LEDOUT_GRPPWM(x) (3 << (x*2))



#ifdef __cplusplus
}
#endif



#endif /* DEV_PCA9633_H_ */

/*! @} */
