/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup GPIO General Purpose Input/Output (GPIO)
 * @{
 *
 *
 * \details The GPIO module gives access the the GPIO pins including reading, writing, and configuring
 * the direction register.  The GPIO module does not allow the pin to be configured for another
 * function (such as UART TX).  The core module provides this functionality with core_set_pinsel_func().  The core
 * module is also used to configure internal resistors and modes using core_wr_pinmode().
 *
 *
 * The following is an example of how to use the GPIO in an OS environment:
 *
 * \code
 *
 * #include <unistd.h>
 * #include <fcntl.h>
 * #include <errno.h>
 * #include <stdio.h>
 * #include "hwpl.h"
 *
 *
 * int gpio_example(void){
 * 	int fd;
 * 	gpio_attr_t attr;
 * 	gpio_action_t action;
 *
 * 	fd = open("/dev/gpio0", O_RDWR);
 * 	if ( fd < 0 ){
 * 		printf("Error opening peripheral (%d)\n", errno);
 * 	} else {
 *
 *		attr.output = (1<<1)|(1<<0);
 *		attr.input = 0;
 *		//this will configure pins 0 and 1 of GPIO Port 0 as outputs
 *		if ( ioctl(fd, I_GPIO_SETATTR, &attr) < 0 ){
 *			printf("Failed to configure peripheral pins (%d)\n", errno);
 *		}
 *
 *		//this will set pin 0 of GPIO Port 0
 *		if ( ioctl(fd, I_GPIO_SETMASK, (void*)((1<<0))) ){
 *			printf("Failed to set pin (%d)\n", errno);
 *		}
 *
 *		//this will clear pin 1 of GPIO 0
 *		if ( ioctl(fd, I_GPIO_CLRMASK, (void*)((1<<1))) ){
 *			printf("Failed to clear pin (%d)\n", errno);
 *		}
 *
 *
 * 	}
 * 	close(fd);
 * 	return 0;
 * }
 *
 * \endcode
 * \ingroup PERIPHIO
 *
 */

/*! \file
 * \brief General Purpose Input/Output Header File
 *
 */

#ifndef DEV_GPIO_H_
#define DEV_GPIO_H_

#include <stdbool.h>
#include <stdint.h>
#include "ioctl.h"
#include "hwpl/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_IOC_IDENT_CHAR 'g'

#ifndef GPIO_SAMPLE_T
#define GPIO_SAMPLE_T
/*! \details This is an MCU specific typedef of the GPIO sample size.  For
 * 32-bit MCU's this is typically a uint32_t.
 *
 */
typedef uint32_t gpio_sample_t;
#endif


/*! \details This defines a GPIO action.
 *
 * See \ref gpio_action_event_t for GPIO specific events.  There
 * are no GPIO specific channel definitions.
 *
 */
typedef hwpl_action_t gpio_action_t;


/*! \brief GPIO Attribute Data Structure
 * \details This structure defines how the control structure
 * for configuring the GPIO port.
 */
typedef struct HWPL_PACK {
	gpio_sample_t output /*! \brief Bits set here will configure the corresponding pins as GPIO outputs */;
	gpio_sample_t input /*! \brief Bits set here will configure the corresponding pins as GPIO inputs */;
} gpio_attr_t;

/*! \details This lists the PIN mode options.  Not all
 * options are supported on all architectures.
 */
typedef enum {
	GPIO_PINMODE_PULLUP /*! Use the built-in pullup */,
	GPIO_PINMODE_REPEATER,
	GPIO_PINMODE_FLOAT /*! Leave the pin floating */,
	GPIO_PINMODE_PULLDOWN /*! Use the built-in pull-down */,
	GPIO_PINMODE_OUTPUT_2MHZ /*! Max speed 2Mhz (stm32f only) */,
	GPIO_PINMODE_OUTPUT_10MHZ /*! Max speed 10Mhz (stm32f only) */,
	GPIO_PINMODE_OUTPUT_50MHZ /*! Max speed 50Mhz (stm32f only) */,
	GPIO_PINMODE_TOTAL
} gpio_pinmode_t;

/*! \details This data structure is used with the I_GPIO_SETPINMODE request.
 *
 */
typedef struct HWPL_PACK {
	gpio_pinmode_t mode /*! \brief The mode to use */;
	gpio_sample_t mask /*! \brief Each bit set in the mask will be set to \a mode using I_GPIO_SET_PINMODE */;
	bool opendrain /*! \brief Sets whether or not pin should be configured as an open drain */;
	bool hysteresis /*! \brief Sets whether or not to enable hysteresis on this pin (if supported) */;
} gpio_pinmode_req_t;

/*! \details This lists defines the values to use for the
 * event when setting the action for a pin.
 */
typedef enum {
	GPIO_ACTION_EVENT_NONE /*! Specifies No GPIO event */,
	GPIO_ACTION_EVENT_RISING /*! Specifies a rising edge on the GPIO */,
	GPIO_ACTION_EVENT_FALLING /*! Specifies a falling edge on the GPIO */,
	GPIO_ACTION_EVENT_BOTH /*! Specifies both edges on the GPIO */
} gpio_action_event_t;

/*! \brief IOCTL request to get the attributes.
 * \hideinitializer
 */
#define I_GPIO_GETATTR _IOCTLR(GPIO_IOC_IDENT_CHAR, I_GLOBAL_GETATTR, gpio_attr_t)

/*! \brief IOCTL request to set the attributes.
 * \hideinitializer
 */
#define I_GPIO_SETATTR _IOCTLW(GPIO_IOC_IDENT_CHAR, I_GLOBAL_SETATTR, gpio_attr_t)

/*! \brief IOCTL request to set the action.
 * \hideinitializer
 */
#define I_GPIO_SETACTION _IOCTLW(GPIO_IOC_IDENT_CHAR, I_GLOBAL_SETACTION, gpio_action_t)

/*! \brief See below for details.
 * \details This request sets the pins that are
 * sets in the mask. The ctl argument is the mask.
 * The following code sets bits 0 to 4 on port 4:
 * \code
 * gpio_fd = open("/dev/gpio4", O_RDWR");
 * ioctl(gpio_fd, I_GPIO_SETMASK, 0x0F);
 * \endcode
 * \hideinitializer
 */
#define I_GPIO_SETMASK _IOCTL(GPIO_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 0)
#define I_GPIO_SET_MASK I_GPIO_SETMASK

/*! \brief See below for details.
 *  \details This request clears the pins that are
 * set in the mask.  The ctl argument is the mask.  This
 * request works like \ref I_GPIO_SETMASK but instead
 * of setting pins, they are cleared.
 *
 * Example:
 * \code
 * ioctl(gpio_fd, I_GPIO_CLRMASK, 0x0F);
 * \endcode
 * \hideinitializer
 */
#define I_GPIO_CLRMASK _IOCTL(GPIO_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 1)
#define I_GPIO_CLR_MASK I_GPIO_CLRMASK

/*! \brief See below for details.
 *  \details This request sets the action to take when an event
 * occurs on a GPIO port.
 * The ctl argument should be of the form void (*callback)(int) where
 * the "int" argument will have a bit set for each pin that has a
 * pending event.
 *
 * Example:
 *  * \code
 * uint32_t value;
 * value = ioctl(gpio_fd, I_GPIO_GET);
 * \endcode
 * \hideinitializer
 *
 */
#define I_GPIO_GET _IOCTL(GPIO_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 2)

/*! \brief See below for details.
 *  \details This request sets the action to take when an event
 * occurs on a GPIO port.
 * The ctl argument should be of the form void (*callback)(int) where
 * the "int" argument will have a bit set for each pin that has a
 * pending event.
 *
 * Example:
 *  * \code
 * uint32_t value;
 * ioctl(gpio_fd, I_GPIO_SET, value);
 * \endcode
 * \hideinitializer
 *
 */
#define I_GPIO_SET _IOCTL(GPIO_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 3)

/*! \brief See below for details.
 *  \details This request sets the pin mode.
 * The following code snippet demonstrates how:
 * \code
 * #include <stdbool.h>
 * #include <dev/gpio.h>
 * gpio_pinmode_req_t req;
 * req.mode = GPIO_PINMODE_PULLUP;
 * req.mask = 0x0F; //the bottom 4 bits
 * req.open_drain = false;
 * req.hysteresis = true;
 * ioctl(gpio_fd, I_GPIO_SETPINMODE, &req);
 * \endcode
 *
 *
 * \hideinitializer
 */
#define I_GPIO_SETPINMODE _IOCTLW(GPIO_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 4, gpio_pinmode_req_t)

#define I_GPIO_TOTAL 5

#ifdef __cplusplus
}
#endif

#endif /* DEV_GPIO_H_ */

/*! @} */
