/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYS_DEV System Devices
 * @{
 *
 *
 * \ingroup PERIPHIO
 *
 * \details This driver provides details about the currently installed operating system.
 *
 *
 */

/*! \file  */

#ifndef DEV_SYS_H_
#define DEV_SYS_H_

#include <stdint.h>
#include "ioctl.h"

#include "hwpl/types.h"


#ifdef __cplusplus
extern "C" {
#endif

/*! \brief This defines the configuration values for
 * the system device (/dev/sys) device in the device table.
 * \hideinitializer
 */
#define SYS_DEVICE { \
		.name = "sys", \
		DEVICE_MODE(0666, 0, 0, S_IFCHR), \
		DEVICE_DRIVER(sys), \
		.cfg.periph.port = 0 \
}

/*! \brief This defines the configuration values for
 * the null device (/dev/null) device in the device table.
 *
 *
 * \hideinitializer
 *
 */
#define NULL_DEVICE { \
		.name = "null", \
		DEVICE_MODE(0666, 0, 0, S_IFCHR), \
		DEVICE_DRIVER(null), \
		.cfg.periph.port = 0 \
}

/*! \brief This defines the configuration values for
 * the null device (/dev/null) device in the device table.
 *
 *
 * \hideinitializer
 *
 */
#define ZERO_DEVICE { \
		.name = "zero", \
		DEVICE_MODE(0666, 0, 0, S_IFCHR), \
		DEVICE_DRIVER(zero), \
		.cfg.periph.port = 0 \
}

/*! \brief This defines the configuration values for
 * the full device (/dev/full) device in the device table.
 *
 *
 * \hideinitializer
 *
 */
#define FULL_DEVICE { \
		.name = "full", \
		DEVICE_MODE(0666, 0, 0, S_IFCHR), \
		DEVICE_DRIVER(full), \
		.cfg.periph.port = 0 \
}


/*! \brief See below.
 *
 * \details This defines the configuration values for
 * an LED device that can drive up to 4 LEDs.
 *
 * \param dev_name The name of the led device
 * \param active_level LED_ACTIVE_LOW or LED_ACTIVE_HIGH
 * \param pio_port0 The port for channel 0
 * \param pio_pin0 The pin for channel 0
 * \param pio_port1 The port for channel 1 (use LED_DEVICE_UNUSED if not used)
 * \param pio_pin1 The pin for channel 1 (use LED_DEVICE_UNUSED if not used)
 * \param pio_port2 The port for channel 2 (use LED_DEVICE_UNUSED if not used)
 * \param pio_pin2 The pin for channel 2 (use LED_DEVICE_UNUSED if not used)
 * \param pio_port3 The port for channel 3 (use LED_DEVICE_UNUSED if not used)
 * \param pio_pin3 The pin for channel 3 (use LED_DEVICE_UNUSED if not used)
 *
 * \hideinitializer
 *
 */
#define LED_DEVICE(dev_name, active_level, pio_port0, pio_pin0, pio_port1, pio_pin1, pio_port2, pio_pin2, pio_port3, pio_pin3) { \
		.name = dev_name, \
		DEVICE_MODE(0666, 0, 0, S_IFCHR), \
		DEVICE_DRIVER(led), \
		.cfg.periph.port = 0, \
		.cfg.pin_assign = active_level, \
		.cfg.pcfg.pio[0].port = pio_port0, \
		.cfg.pcfg.pio[0].pin = pio_pin0, \
		.cfg.pcfg.pio[1].port = pio_port1, \
		.cfg.pcfg.pio[1].pin = pio_pin1, \
		.cfg.pcfg.pio[2].port = pio_port2, \
		.cfg.pcfg.pio[2].pin = pio_pin2, \
		.cfg.pcfg.pio[3].port = pio_port3, \
		.cfg.pcfg.pio[3].pin = pio_pin3, \
}

#define LED_DEVICE_UNUSED (-1)

#define RANDOM_DEVICE { \
		.name = "random", \
		DEVICE_MODE(0666, 0, 0, S_IFCHR), \
		DEVICE_DRIVER(random), \
		.cfg.periph.port = 0 \
}

#define URANDOM_DEVICE { \
		.name = "urandom", \
		DEVICE_MODE(0666, 0, 0, S_IFCHR), \
		DEVICE_DRIVER(urandom), \
		.cfg.periph.port = 0 \
}



#define SYS_IOC_CHAR 's'

/*! \brief Flags in \a security (member of \ref sys_attr_t).
 */
typedef enum {
	SYS_SECURITY_RESET /*! Allows desktop interface to reset the device */ = (1<<0),
	SYS_SECURITY_INVOKEBOOTLOADER /*! Allows desktop interface to invoke the bootloader (can always be invoked manually) */ = (1<<1),
	SYS_SECURITY_READFLASH /*! Allows desktop interface to read the flash memory (where OS and application binaries are stored) */ = (1<<2),
	SYS_SECURITY_APPINSTALL /*! \brief Allows desktop interface to install custom applications (warning: custom applications can get around security) */ =  (1<<3)
} sys_security_flags_t;


/*! \details This structure defines the system attributes.
 *
 */
typedef struct HWPL_PACK {
	char version[8] /*! \brief The OS Version */;
	char arch[8] /*! \brief The target architecture (lpc17xx, lpc13xx, etc) */;
	uint32_t signature /*! \brief Ths OS library signature used to ensure proper build system is used for applications */;
	uint32_t security /*! \brief Security flags (see \ref sys_security_flags_t)*/;
	uint32_t cpu_freq /*! \brief The CPU clock frequency */;
	uint32_t sys_mem_size /*! \brief The number of bytes in RAM shared across OS and other processes */;
	char stdout_name[NAME_MAX] /*! \brief Default value for the standard output */;
	char stdin_name[NAME_MAX] /*! \brief Default value for the standard output */;
} sys_attr_t;

/*! \brief Task Attributes
 * \details This structure contains the task attributes
 * used with I_SYS_GETTASK.
 */
typedef struct HWPL_PACK {
	uint32_t pid /*! \brief PID for the task */;
	uint32_t tid /*! \brief Task ID */;
	uint64_t timer /*! \brief Timer used for tracking task CPU usage */;
	uint32_t mem_loc /*! \brief location of data memory */;
	uint32_t mem_size /*! \brief Size of data memory */;
	uint32_t malloc_loc /*! \brief malloc base + size */;
	uint32_t stack_ptr /*! \brief location of stack pointer */;
	uint8_t prio /*! \brief Task Priority */;
	uint8_t prio_ceiling /*! \brief Task priority ceiling */;
	uint8_t is_active /*! \brief Non-zero for an active task */;
	uint8_t is_thread /*! \brief Non-zero if not main process thread */;
	uint8_t is_enabled /*! \brief Non-zero if associated with running process */;
	char name[NAME_MAX] /*! \brief Name of the process associated with the task */;
} sys_taskattr_t;

/*! \brief Structure for I_SYS_KILL and I_SYS_PTHREADKILL
 * \details This structure is used with the I_SYS_KILL
 * and I_SYS_PHTHREADKILL requests.
 */
typedef struct HWPL_PACK {
	uint32_t id /*! \brief The id (PID or pthread id) to send the signal to */;
	uint32_t si_signo /*! \brief The signal to send (SIGKILL or LINK_SIGKILL for example) */;
	int32_t si_sigcode /*! \brief The signal code (SI_USER or LINK_SI_USER for example) */;
	int32_t si_sigvalue /*! \brief The signal value (not used by default handlers) */;
} sys_killattr_t;

/*! \brief Structure for checking the status of
 * a named process.
 * \details This structure is used for checking the status of a named
 * process.  It is used with the I_SYS_GETPROCESS request.
 */
typedef struct HWPL_PACK {
	char name[24] /*! \brief Written by caller to specify name */;
	int32_t pid /*! \brief Process ID (written by driver; -1 is process is not running) */;
	int32_t pthread_id /*! \brief Thread ID of primary thread in process (written by driver; -1 if process is not running) */;
} sys_process_t;


/*! \brief Data structure to unlock the security word features.
 * \details This data structure is used with I_SYS_UNLOCK.  A successful
 * request will unlock the security features (see \a security in \ref sys_attr_t) of the device giving access
 * to certain parts of the device.
 */
typedef struct HWPL_PACK {
	char key[32] /*! \brief The password used to unlock the device */;
	uint32_t status /*! \brief The status of the lock after using \a key */;
} sys_lock_t;

/*! \brief See below.
 * \details This request applies the software write protect
 * to the entire device.
 *
 * Example:
 * \code
 * #include <dev/sys.h>
 * #include <uinstd.h>
 * #include <stdio.h>
 * ...
 * sys_attr_t attr;
 * ioctl(fildes, I_SYS_GETATTR, &attr);
 * printf("Version is %s\n", attr.version);
 * \endcode
 *
 * \hideinitializer
 *
 */
#define I_SYS_GETATTR _IOCTLR(SYS_IOC_CHAR, 0, sys_attr_t)

/*! \brief See below for details.
 * \details This request gets the information about the specified task.
 * \code
 * sys_task_t task;
 * task.tid = 1;
 * ioctl(fd, I_SYS_GETTASK, &task);
 * \endcode
 */
#define I_SYS_GETTASK _IOCTLRW(SYS_IOC_CHAR, 1, sys_taskattr_t)

/*! \brief See below for details
 * \details This request sends a signal to all the tasks in the
 * specified process group.
 */
#define I_SYS_KILL _IOCTLW(SYS_IOC_CHAR, 2, sys_killattr_t)

/*! \brief See below for details
 * \details This request sends a signal to a single task (thread)
 */
#define I_SYS_PTHREADKILL _IOCTLW(SYS_IOC_CHAR, 3, sys_killattr_t)

/*! \brief See below for details
 * \details This requests checks to see if the named process is currently
 * running.
 *
 * \code
 * sys_process_t p;
 * strcpy(p.name, "hello");
 * ioctl(fd, I_SYS_ISRUNNING, &p);
 * if( p.pid < 0 ){
 * 	//process is not running
 * }
 * \endcode
 */
#define I_SYS_GETPROCESS _IOCTLRW(SYS_IOC_CHAR, 4, sys_process_t)

/*! \brief See below for details
 * \details This request unlocks the device.  When unlocked, all
 * operations are available (the security word has no effect).
 *
 * \code
 * sys_lock_t lock;
 * strcpy(lock.key, "the passord");
 * ioctl(fd, I_SYS_UNLOCK, &lock);
 * if( lock.status == 0 ){
 * 	//device is still locked
 * }
 * \endcode
 */
#define I_SYS_UNLOCK _IOCTLRW(SYS_IOC_CHAR, 5, sys_lock_t)


/*! \brief See below for details
 * \details This request locks the device.  When locked, certain operations
 * are disabled based on the value of \a security in \ref sys_attr_t;
 *
 * \code
 * ioctl(fd, I_SYS_LOCK);
 * \endcode
 */
#define I_SYS_LOCK _IOCTL(SYS_IOC_CHAR, 6)

#define I_SYS_TOTAL 7

#define LED_IOC_CHAR 'l'
#define LED_ACTIVE_LOW 0
#define LED_ACTIVE_HIGH 1


/*! \details This is used to turn the
 * specified LED on and off.
 */
typedef struct HWPL_PACK {
	uint8_t channel /*! \brief the channel (from 0 to 4) */;
	uint8_t on /*! \brief Non-zero to turn the LED on (zero to turn it off) */;
} led_req_t;


/*! \brief See below.
 * \details This sets the state of the specified LED.
 *
 * Example:
 * \code
 * #include <unistd.h>
 * #include <dev/sys.h>
 * ...
 * led_req_t req;
 * req.channel = 0;
 * req.on = 1;
 * ioctl(led_fd, I_LED_SET, &req); //turns LED on channel 1 on
 * \endcode
 */
#define I_LED_SET _IOCTLW(LED_IOC_CHAR, 0, led_req_t)


#ifdef __cplusplus
}
#endif

#endif /* DEV_SYS_H_ */

/*! @} */
