/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PIO Pin Input/Output (PIO)
 * @{
 *
 *
 * \details The PIO module gives access to the pins including reading, writing, and configuring
 * the direction register.  The PIO module does not allow the pin to be configured for another
 * function (such as UART TX).
 *
 *
 * The following is an example of how to use the PIO in an OS environment:
 *
 * \code
 *
 * #include <unistd.h>
 * #include <fcntl.h>
 * #include <errno.h>
 * #include <stdio.h>
 * #include <dev/pio.h>
 *
 *
 * int pio_example(void){
 * 	int fd;
 * 	pio_attr_t attr;
 * 	pio_action_t action;
 *
 * 	fd = open("/dev/pio0", O_RDWR);
 * 	if ( fd < 0 ){
 * 		printf("Error opening peripheral (%d)\n", errno);
 * 	} else {
 *
 *		attr.output = (1<<1)|(1<<0);
 *		attr.input = 0;
 *		//this will configure pins 0 and 1 of PIO Port 0 as outputs
 *		if ( ioctl(fd, I_PIO_SETATTR, &attr) < 0 ){
 *			printf("Failed to configure peripheral pins (%d)\n", errno);
 *		}
 *
 *		//this will set pin 0 of PIO Port 0
 *		if ( ioctl(fd, I_PIO_SETMASK, (void*)((1<<0))) ){
 *			printf("Failed to set pin (%d)\n", errno);
 *		}
 *
 *		//this will clear pin 1 of PIO 0
 *		if ( ioctl(fd, I_PIO_CLRMASK, (void*)((1<<1))) ){
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

#ifndef DEV_PIO_H_
#define DEV_PIO_H_

#include <stdbool.h>
#include <stdint.h>
#include "ioctl.h"
#include "hwpl/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PIO_IOC_IDENT_CHAR 'p'

#ifndef PIO_SAMPLE_T
#define PIO_SAMPLE_T
/*! \details This is an MCU specific typedef of the PIO sample size.  For
 * 32-bit MCU's this is typically a uint32_t.
 *
 */
typedef uint32_t pio_sample_t;
#endif


/*! \details This defines a PIO action.
 *
 * See \ref pio_action_event_t for PIO specific events.  There
 * are no PIO specific channel definitions.
 *
 */
typedef hwpl_action_t pio_action_t;

/*! \brief IO port and pin
 * \details This data structures holds the details of a single port/pin combination.
 */
typedef struct HWPL_PACK {
	uint8_t port /*! \brief The port number */;
	uint8_t pin /*! \brief The pin number */;
} pio_t;

/*! \details This lists the PIN mode options.  Not all
 * options are supported on all architectures.
 */
typedef enum {
	PIO_MODE_INPUT /*! Input flag*/ = (1<<0),
	PIO_MODE_OUTPUT /*! Output flag */ = (1<<1),
	PIO_MODE_PULLUP /*! Use the built-in pullup */ = (1<<2),
	PIO_MODE_PULLDOWN /*! Use the built-in pull-down */ = (1<<3),
	PIO_MODE_REPEATER /*! Use the built-in repeater function */ = (1<<4),
	PIO_MODE_FLOAT /*! Leave the pin floating */ = (1<<5),
	PIO_MODE_OUTPUT_2MHZ /*! Max speed 2Mhz (stm32f only) */ = (1<<6),
	PIO_MODE_OUTPUT_10MHZ /*! Max speed 10Mhz (stm32f only) */ = (1<<7),
	PIO_MODE_OUTPUT_50MHZ /*! Max speed 50Mhz (stm32f only) */ = (1<<8),
	PIO_MODE_OPENDRAIN /*! Configure as open drain */ = (1<<9),
	PIO_MODE_HYSTERESIS /*! Enable hysteresis on pin */ = (1<<10),
	PIO_MODE_DIRONLY /*! Only set input/output (ignore other settings) */ = (1<<11),
	PIO_MODE_ANALOG /*! Use an analog rather than digital input */ = (1<<12),
	PIO_MODE_INVERT /*! Invert the logic on the pin */ = (1<<13),
	PIO_MODE_FILTER /*! Filter noise on pin */ = (1<<14)
} pio_mode_t;

/*! \brief PIO Attribute Data Structure
 * \details This structure defines how the control structure
 * for configuring the PIO port.
 */
typedef struct HWPL_PACK {
	pio_sample_t mask /*! \brief Each bit set in the mask will be set to \a mode using I_PIO_SET_PINMODE */;
	uint16_t mode /*! \brief OR the pio_mode_t */;
} pio_attr_t;


/*! \details This lists defines the values to use for the
 * event when setting the action for a pin.
 */
typedef enum {
	PIO_ACTION_EVENT_NONE /*! Specifies No PIO event */,
	PIO_ACTION_EVENT_RISING /*! Specifies a rising edge on the PIO */,
	PIO_ACTION_EVENT_FALLING /*! Specifies a falling edge on the PIO */,
	PIO_ACTION_EVENT_BOTH /*! Specifies both edges on the PIO */
} pio_action_event_t;


/*! \brief Data struct for passing data to callbacks. */
/*! \details This data structure is passed to interrupt callbacks to give
 * information about what pins caused the interrupt.
 */
typedef struct {
	uint32_t mask /*! \brief Mask of pin(s) that caused interrupt */;
} pio_action_data_t;

/*! \brief IOCTL request to get the attributes.
 * \hideinitializer
 */
#define I_PIO_GETATTR _IOCTLR(PIO_IOC_IDENT_CHAR, I_GLOBAL_GETATTR, pio_attr_t)

/*! \brief IOCTL request to set the attributes.
 * \hideinitializer
 */
#define I_PIO_SETATTR _IOCTLW(PIO_IOC_IDENT_CHAR, I_GLOBAL_SETATTR, pio_attr_t)

/*! \brief IOCTL request to set the action.
 * \hideinitializer
 */
#define I_PIO_SETACTION _IOCTLW(PIO_IOC_IDENT_CHAR, I_GLOBAL_SETACTION, pio_action_t)

/*! \brief See below for details.
 * \details This request sets the pins that are
 * sets in the mask. The ctl argument is the mask.
 * The following code sets bits 0 to 4 on port 4:
 * \code
 * pio_fd = open("/dev/pio4", O_RDWR");
 * ioctl(pio_fd, I_PIO_SETMASK, 0x0F);
 * \endcode
 * \hideinitializer
 */
#define I_PIO_SETMASK _IOCTL(PIO_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 0)
#define I_PIO_SET_MASK I_PIO_SETMASK

/*! \brief See below for details.
 *  \details This request clears the pins that are
 * set in the mask.  The ctl argument is the mask.  This
 * request works like \ref I_PIO_SETMASK but instead
 * of setting pins, they are cleared.
 *
 * Example:
 * \code
 * ioctl(pio_fd, I_PIO_CLRMASK, 0x0F);
 * \endcode
 * \hideinitializer
 */
#define I_PIO_CLRMASK _IOCTL(PIO_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 1)
#define I_PIO_CLR_MASK I_PIO_CLRMASK

/*! \brief See below for details.
 *  \details This request sets the action to take when an event
 * occurs on a PIO port.
 * The ctl argument should be of the form void (*callback)(int) where
 * the "int" argument will have a bit set for each pin that has a
 * pending event.
 *
 * Example:
 *  * \code
 * uint32_t value;
 * value = ioctl(pio_fd, I_PIO_GET);
 * \endcode
 * \hideinitializer
 *
 */
#define I_PIO_GET _IOCTL(PIO_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 2)

/*! \brief See below for details.
 *  \details This request sets the action to take when an event
 * occurs on a PIO port.
 * The ctl argument should be of the form void (*callback)(int) where
 * the "int" argument will have a bit set for each pin that has a
 * pending event.
 *
 * Example:
 *  * \code
 * uint32_t value;
 * ioctl(pio_fd, I_PIO_SET, value);
 * \endcode
 * \hideinitializer
 *
 */
#define I_PIO_SET _IOCTL(PIO_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 3)


#define I_PIO_TOTAL 4

#ifdef __cplusplus
}
#endif

#endif /* DEV_PIO_H_ */

/*! @} */
