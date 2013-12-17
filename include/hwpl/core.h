/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup CORE Core Access
 * @{
 *
 *
 *
 * \details The Core module allows access to the core CPU functionality such as clock speed, power, and
 * interrupts.  In general, the OS should make extensive use of the Core access API while the device
 * drivers use the \ref PERIPHIO API.  To maximize portability, applications should rely on the
 * OS abstraction of all hardware.
 *
 *
 */

/*! \file
 * \brief Core Functionality Header File
 *
 */

#ifndef _HWPL_CORE_H_
#define _HWPL_CORE_H_

#include <stdint.h>
#include "hwpl/types.h"
#include "../device.h"
#include "../dev/core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*core_privcall_t)(void*);

/*! \details A privileged call.
 */
#ifndef __link
/*! \details This performs a privileged call.
 *
 */
void hwpl_core_privcall(core_privcall_t call /*! the function to execute */,
		void * args /*! the pointer to pass to call */) __attribute__((optimize("1")));
#endif

/*! \details This function reads the OS signature value.
 *
 */
int _hwpl_core_getsignature(int port, void * arg);

/*! \details This function sets the main clock speed based on
 * the desired speed (fclk) and the given oscillator frequency (fosc).  This function rounds the target
 * frequency to the closest value achievable given any hardware limitations.
 *
 * \note This function is the de facto initialization routine for HWPL.  It should
 * be called before any other HWPL functions are called.
 *
 */
void _hwpl_core_setclock(int fclk /*! The target clock frequency */,
		int fosc /*! The oscillator frequency (zero to use the internal oscillator) */ );

void _hwpl_core_setclockinternal(int fclk);

//int hwpl_core_setclock(int port, void * arg);

/*! \details This sets the CPU block to 72Mhz assuming
 * a 12MHz clock is connected.
 */
void _hwpl_core_setclock_main_12mhz_72mhz(void);

/*! \details This function sets the USB clock speed to the required
 * value for USB operation on the device.  The target frequency is defined
 * by the MCU (usually 48MHz).  Most MCUs have limitations on what
 * oscillator frequencies are allowed to achieve the target frequency.
 *
 * \return Zero on success
 *
 */
int _hwpl_core_setusbclock(int fosc /*! The oscillator frequency */);

/*! \details This function puts the MCU in sleep mode.
 */
int _hwpl_core_sleep(core_sleep_t level /*! The sleep type */);


/*! \details This function writes the pin function select value.  When a pin can
 * have multiple functions (such as GPIO or UART RX), this function selects
 * what function the pin is to have.  This function is portable; however
 * the arguments of the function will invariably be different between
 * MCU architectures.
 *
 *
 * \note  The respective periph_open()
 * function will configure the correct function for the pin without the need to call
 * core_set_pinsel_func().
 * It is only necessary to use this function if the pin_assign
 * value is set to \ref HWPL_GPIO_CFG_USER.
 *
 */
int _hwpl_core_set_pinsel_func(int gpio_port /*! The GPIO port number */,
		int pin /*! The GPIO pin number */,
		core_periph_t function /*! The peripheral to use */,
		int periph_port /*! The peripheral port to use (e.g. 1 for UART1) */);

int hwpl_core_setpinfunc(int port, void * arg);

void _hwpl_core_getserialno(uint32_t * serialno);


extern int hwpl_core_cpu_freq;

#ifndef __link
static inline int _hwpl_core_getclock(void) HWPL_ALWAYS_INLINE;
int _hwpl_core_getclock(void){
	return hwpl_core_cpu_freq;
}
#endif


/*! \details
 * \sa periph_open()
 *
 */
int hwpl_core_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_core_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_core_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_core_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_core_close(const device_cfg_t * cfg);

int hwpl_core_getattr(int port, void * arg);
int hwpl_core_setattr(int port, void * arg);
int hwpl_core_setaction(int port, void * arg);
int hwpl_core_setpinfunc(int port, void * arg);
int hwpl_core_sleep(int port, void * arg);
int hwpl_core_reset(int port, void * arg);
int hwpl_core_invokebootloader(int port, void * arg);
int hwpl_core_setirqprio(int port, void * arg);
int hwpl_core_setclkout(int port, void * arg);



#ifdef __cplusplus
}
#endif

#endif // _HWPL_CORE_H_

/*! @} */
