/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PCA9551 PCA9551 Eight Channel LED Driver
 *

 *
 * @{
 *
 * \ingroup NXP
 *
 * \details This driver controls NXP's PCA9551 LED driver chip.  The chip features four PWM signals to drive
 * and LED.
 *
 * Required Peripheral Hardware:
 * - 1 I2C Bus
 *
 *
 */

#ifndef DEV_PCA9551_H_
#define DEV_PCA9551_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*! \details This defines the ptr value for
 * auto-incrementing through all registers.
 *
 */
#define PCA9551_PTR_AUTO_INCREMENT 0x10

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
 * lseek(pca9551_fd, offsetof(pca9551_t, pwm0), SEEK_SET);
 * read(pca9551_fd, &pwm0, 1);
 * \endcode
 */
typedef struct HWPL_PACK {
	uint8_t input /* \brief This reflects the state of the output pins (read-only) */;
	uint8_t psc0 /*! \brief Used to program the period of the PWM output (BLINK0 = (psc0 + 1)/38) */;
	uint8_t pwm0 /*! \brief Determines the duty cycle of BLINK0 */;
	uint8_t psc1 /*! \brief Used to program the period of the PWM output (BLINK1 = (psc1 + 1)/38) */;
	uint8_t pwm1 /*! \brief Determines the duty cycle of BLINK1 */;
	uint16_t ls /*! \brief See below
	\details LED settings for LEDs 0 to 3 using 2-bits per LED:
	- 00: output is low (LED On)
	- 01: output is high (LED off -- Default)
	- 10: output uses PWM0 rate
	- 11: output uses PWM1 rate
	 */;
} pca9551_t;

/*! \details This gets the psc setting based on the
 * period x (in milliseconds).
 */
#define PCA9551_PERIOD(x) ((uint8_t)(((uint16_t)x * 38)/1000 - 1))


/*! details This specifies the values for
 * setting the led state using pca9551_led_set().
 *
 */
typedef enum {
	PCA9551_ON /*! \brief On */,
	PCA9551_OFF /*! \brief Off */,
	PCA9551_PWM0 /*! \brief Use PWM0 */,
	PCA9551_PWM1 /*! \brief Use PWM1 */
} pca9551_ledstate_t;


static inline void pca9551_led_set(pca9551_t * dest, int led, pca9551_ledstate_t state);
/*! \details This sets the state of the led in the data structure.  The structure
 * must late be written tot the device.
 */
void pca9551_led_set(pca9551_t * dest /*! destination data */,
		int led /*! The led to set */,
		pca9551_ledstate_t state /*! The desired state */){
	int mask;
	led = led << 1;
	mask = (0x03 << led);
	dest->ls &= ~(mask);
	dest->ls |= (state<<led);
}


#ifdef __cplusplus
}
#endif


#endif /* DEV_PCA9551_H_ */

/*! @} */
