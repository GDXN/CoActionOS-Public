/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SI9986 Si9986 Motor Driver
 * @{
 *
 *
 * \ingroup VISHAY
 *
 * \details This device driver controls the Si9986 H-Bridge Motor driver.
 *
 * Required Peripheral Hardware:
 * - PWM (2 channels)
 *
 * The driver can be in one of four modes:
 * - Forward (braking between pulses)
 * - Forward (high impedance or Hi Z between pulses)
 * - Reverse (braking between pulses)
 * - Reverse (high impedance or Hi Z between pulses)
 *
 * The following example writes a ramp to the motor then puts the brake on.
 * \code
 *
 * int fildes;
 * pwm_duty_t ramp[100];
 * int i;
 * si9986_convert_t d;
 *
 * fildes = open("/dev/si9986-0", O_RDWR, 0);
 * d.loc = SI9986_LOC_HIZ|SI9986_LOC_FORWARD;
 * for(i=0; i < 100; i++){
 * 	d.float_duty = i * 1.0 / 100.0;
 * 	ioctl(fildes, I_SI9986_CONVERT, &d);
 * 	ramp[i] = d.duty;
 * }
 * //This will write the ramp (execution of the thread is blocked until the write is complete)
 * lseek(fildes, SI9986_LOC_FORWARD|SI9986_LOC_HIZ, SEEK_SET);
 * write(fildes, ramp, sizeof(pwm_duty_t)*100);
 * ioctl(fildes, I_SI9986_BRAKE, NULL); //Now put the brake on
 *
 * close(fildes)
 * \endcode
 *
 */

/*! \file  */

#ifndef DEV_SI9986_H_
#define DEV_SI9986_H_

#include "dev/ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif


/*! \details This defines the configuration values for
 * a SI9986 device in the device table.
 *
 *
 * \param device_name The name of the device (e.g "si9986-0")
 * \param port_number The SPI peripheral port number
 * \param pin_assign_value The GPIO configuration to use with the PWM
 * \param pwm_top The top value for the PWM
 * \param pwm_freq The PWM frequency
 * \param pwm_chan_0 The primary PWM channel
 * \param pwm_chan_1 The secondary PWM channel
 * \param mode_value The access mode (usually 0666)
 * \param uid_value The User ID
 * \param gid_value The Group ID
 *
 * \hideinitializer
 *
 */
#define SI9986_DEVICE(device_name, port_number, pin_assign_value, pwm_top, pwm_freq, pwm_chan_0, pwm_chan_1, mode_value, uid_value, gid_value) { \
		.name = device_name, \
		DEVICE_MODE(mode_value, uid_value, gid_value, S_IFCHR), \
		DEVICE_DRIVER(si9986), \
		.cfg.periph.port = port_number, \
		.cfg.pin_assign = pin_assign_value, \
		.cfg.pcfg.pwm.top = pwm_top, \
		.cfg.pcfg.pwm.freq = pwm_freq, \
		.cfg.pcfg.pwm.channels[0] = pwm_chan_0, \
		.cfg.pcfg.pwm.channels[1] = pwm_chan_1, \
		.cfg.pcfg.pwm.channels[2] = -1, \
		.cfg.pcfg.pwm.channels[3] = -1, \
		.cfg.pcfg.pwm.channels[4] = -1, \
		.cfg.pcfg.pwm.channels[5] = -1, \
		.cfg.pcfg.pwm.channels[6] = -1, \
		.cfg.pcfg.pwm.channels[7] = -1 \
}

/*! \brief Data structure for converting duty cycle types
 * \details This defines the
 * data structure used with \ref I_SI9986_CONVERT
 */
typedef struct HWPL_PACK {
	pwm_duty_t duty /*! \brief The duty cycle used with write() */;
	float float_duty /*! \brief A floating point value from 0.0 to 1.0 */;
	int loc /*! \brief Specify the braking/hi-z scheme (either SI9986_LOC_BRAKE or SI9986_LOC_HIZ)  */;
} si9986_convert_t;

/*! \details When writing the device, this bit
 * set in the loc value will go from on to brake when
 * applying the PWM.
 *
 */
#define SI9986_LOC_BRAKE (1<<0)

/*! \details When writing the device, this bit
 * set in the loc value will go from on to hi-z when
 * applying the PWM.
 *
 */
#define SI9986_LOC_HIZ (0<<0)

/*! \details When writing the device, this bit
 * set in the loc value will cause the device
 * to go "forward" (which is defined as the opposite
 * direction as "reverse").
 *
 */
#define SI9986_LOC_FORWARD (1<<1)

/*! \details When writing the device, this bit
 * set in the loc value will cause the device
 * to go in "reverse" (which is defined as the opposite
 * direction as "forward").
 *
 */
#define SI9986_LOC_REVERSE (0<<1)


#define SI9986_IOC_CHAR 's'


/*! \details This request puts the brakes on the
 * driver.  The ctl argument is NULL.
 *
 * Example:
 * \code
 * ioctl(fildes, I_SI9986_BRAKE, NULL);
 * \endcode
 *
 */
#define I_SI9986_BRAKE _IOCTL(SI9986_IOC_CHAR, 0)

/*! \details This request puts the device in high impedance mode.
 * The ctl argument is NULL.
 *
 * Example:
 * \code
 * ioctl(fildes, I_SI9986_HIZ, NULL);
 * \endcode
 *
 */
#define I_SI9986_HIZ _IOCTL(SI9986_IOC_CHAR, 1)

/*! \details This request sets the duty cycle.  The ctl
 * argument is a pointer to the floating point duty cycle from -1.0 to 1.0 (negative for reverse;
 * positive for forward; the duty will be bounded to this range).  The PWM is braking when off.
 *
 * Example:
 * \code
 * float duty = 0.25;
 * si9986_ioctl(&cfg, I_SI9986_SET_BRAKE, &duty);
 * \endcode
 *
 *
 */
#define I_SI9986_SETBRAKE _IOCTL(SI9986_IOC_CHAR, 2)
#define I_SI9986_SET_BRAKE I_SI9986_SETBRAKE

/*! \details This request sets the duty cycle.  The ctl
 * argument is a pointer to the floating point duty cycle from -1.0 to 1.0 (negative for reverse;
 * positive for forward; the duty will be bounded to this range).  The PWM is Hi-Z when off.
 *
 * Example:
 * \code
 * float duty = 0.5;
 * si9986_ioctl(&cfg, I_SI9986_SET_HIZ, &duty);
 * \endcode
 *
 *
 */
#define I_SI9986_SETHIZ _IOCTL(SI9986_IOC_CHAR, 3)
#define I_SI9986_SET_HIZ I_SI9986_SETHIZ

/*! \details This request converts a floating point duty cycle from
 * (0.0 to 1.0; the duty will be bounded to this range) to a pwm_duty_t that can be used to write
 * a ramp to the motor.
 *
 * \todo This should be changed to a static inline function
 */
#define I_SI9986_CONVERT _IOCTLRW(SI9986_IOC_CHAR, 4, si9986_convert_t)

#define I_SI9986_TOTAL 5

#ifdef __cplusplus
}
#endif


#endif /* DEV_SI9986_H_ */


/*! @} */
