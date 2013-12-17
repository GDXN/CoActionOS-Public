/*
 * hwpl_types.h
 *
 *  Created on: Dec 13, 2011
 *      Author: tgil
 */

#ifndef _HWPL_TYPES_H_
#define _HWPL_TYPES_H_

#include <stdint.h>
#include <stdbool.h>


#define HWPL_TEST_BIT(x,y) (x & (1<<y))
#define HWPL_SET_BIT(x,y) (x |= (1<<y))
#define HWPL_SET_MASK(x,y) (x |= y)
#define HWPL_CLR_BIT(x,y) (x &= ~(1<<y))
#define HWPL_CLR_MASK(x,y) (x &= ~y)

#ifdef __cplusplus
extern "C" {
#endif

/*! \details This macro defines how to declare a weakly-bound
 * alias.
 */
#define HWPL_ALIAS(f) __attribute__((weak, alias (#f)))

#define HWPL_WEAK __attribute__((weak))

/*! \details This macro is used to specify that a structure
 * should be packed rather than word aligned.
 */
#define HWPL_PACK __attribute__((packed))
#define HWPL_NAKED __attribute__((naked))

/*! \details This macro is used to specify functions
 * which are always inline
 */
#define HWPL_ALWAYS_INLINE __attribute__((always_inline))


/*! \details This macro specifies to never inline a function.
 *
 */
#define HWPL_NEVER_INLINE __attribute__((noinline))


/*! \details This is a HWPL callback pointer.  It is
 * a member of the \ref hwpl_action_t structure.
 *
 * See \ref I_SETACTION for an example to use a HWPL callback.
 *
 *
 */
typedef int (*hwpl_callback_t)(void*,const void*);


/*! \details This attribute can be applied to RAM so
 * that it is stored in system memory that is universally
 * readable but can only be written in privileged mode.
 *
 * Example:
 * \code
 * static char buffer[512] HWPL_SYS_MEM;
 * \endcode
 */
#define HWPL_SYS_MEM __attribute__((section(".sysmem")));

/*! \details This structure defines an action
 * to take when an asynchronous event occurs (such as
 * a pin change interrupt).
 *
 * \sa I_SETACTION
 */
typedef struct {
	uint8_t channel /*! \brief The channel (a GPIO pin or timer channel) */;
	int event /*! \brief The peripheral specific event */;
	hwpl_callback_t callback /*! \brief The function to call.  First argument is the context, second argument is peripheral specific */;
	void * context /*! \brief The context to execute the callback in */;
} hwpl_action_t;

/*! \details This lists the bits used in the
 * sigcode.
 */
typedef enum {
	HWPL_SIGCODE_OP_COMPLETE /*! Operation complete bit */ = (1<<0),
	HWPL_SIGCODE_NEW_DATA /*! New data bit */ = (1<<1),
	HWPL_SIGCODE_EVENT /*! Event bit */ = (1<<2)
} hwpl_sigcode_bitmask_t;

/*! \details This defines a global ioctl() request.  It
 * specifies to read the peripheral's attribute data structure.  The data
 * is written to the data pointed to by the third argument to ioctl().
 * Example:
 * \code
 * adc_attr_t attr;
 * ioctl(adc_fd, I_GETATTR, &attr);
 * \endcode
 *
 * \hideinitializer
 */
#define I_GLOBAL_GETATTR 0

/*! \details This defines a global ioctl() request.  It
 * specifies to write the peripheral's attribute data structure.
 * The attribute structure is passed as a pointer in the third argument
 * to ioctl().
 *
 * Example:
 * \code
 * adc_attr_t attr;
 * attr.pin_assign = 0;
 * attr.freq = 200000;
 * attr.enabled_channels = (1<<0);
 * ioctl(adc_fd, I_SETATTR, &attr);
 * \endcode
 *
 * \hideinitializer
 */
#define I_GLOBAL_SETATTR 1

/*! \details This defines a global ioctl() request.  It
 * sets an action to take when the operation completes.
 *
 * The third argument to ioctl() is always a pointer to an \ref hwpl_action_t
 * data structure.
 *
 *
 * \hideinitializer
 */
#define I_GLOBAL_SETACTION 2
#define I_GLOBAL_CANCEL I_GLOBAL_SETACTION

#define I_GLOBAL_TOTAL 3

/*! \details This specifies the value for pin_assign
 * when opening a port on a peripheral.  Most MCU's
 * allow a port to be mapped to different GPIO
 * locations.  The standard locations are supported
 * by using values 0, 1, 2, etc (see the device specific
 * documentation for details).  If none
 * of the standard locations works for your
 * application, you can configure the GPIO
 * manually, then send this value to the
 * driver to indicate the GPIO was already configured.
 */
#define HWPL_PINASSIGN_USER 50
#define HWPL_GPIO_CFG_USER HWPL_PINASSIGN_USER


#ifdef __cplusplus
}
#endif


#endif /* _HWPL_TYPES_H_ */
