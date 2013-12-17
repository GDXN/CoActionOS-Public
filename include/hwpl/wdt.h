/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup WDT Watch Dog Timer (WDT)
 * @{
 *
 * \ingroup CORE
 *
 * \details The Watch Dog Timer (WDT) is used to reset the device if the timer expires.  It is
 * designed as a fail safe to prevent software from freezing the processor.  It should be
 * integrated at the OS level to ensure no application processes or threads disable
 * context switching.
 *
 * The following code example initializes the WDT using the internal RC oscillator.
 * \code
 * wdt_init(WDT_MODE_RESET|WDT_MODE_CLK_SRC_INTERNAL_RC, 3000);
 * \endcode
 *
 * If three seconds (3000ms) passes without a call to wdt_reset(), the MCU will reset.
 *
 */

/*! \file
 * \brief Watch Dog Timer Header File
 *
 */

#ifndef WDT_H_
#define WDT_H_

#include "arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details This enum lists the valid values of
 * the WDT clock sources.  Not all sources
 * are supported on all devices.  Consult the device
 * documentation to see which sources are supported.
 *
 * The WDT can be an OR'd value of one clock source and one of
 * WDT_MODE_RESET or WDT_MODE_INTERRUPT.  For example:
 * \code
 * wdt_init(WDT_MODE_RESET|WDT_MODE_CLK_SRC_INTERNAL_RC, 3000);
 * \endcode
 *
 */
typedef enum {
	WDT_MODE_RESET /*! Reset the device when the WDT times out */ = (1<<0),
	WDT_MODE_INTERRUPT /*! Interrupt the processor on a WDT time out */ = (1<<1),
	WDT_MODE_CLK_SRC_INTERNAL_RC /*! WDT clock source is an internal RC oscillator */ = (0<<2),
	WDT_MODE_CLK_SRC_MAIN /*! WDT clock source is the same as the core CPU */ = (1<<2),
	WDT_MODE_CLK_SRC_RTC /*! WDT is clocked the same as the RTC (usually 32KHz) */ = (2<<2),
	WDT_MODE_CLK_SRC_WDT_OSC /*! The WDT is clock using the dedicated WDT oscillator */ = (3<<2)
} wdt_mode_t;

#define WDT_MODE_CLK_SRC_MASK (0x07 << 2 )

/*! \details This function opens the watch dog timer.  The timer
 * will run and either reset or interrupt the device at
 * the specified interval.
 *
 * Once the WDT is intialized, it cannot be disabled and must
 * be reset (wdt_reset()) on a regular basis to prevent a device
 * reset or interrupt.
 *
 * \return An HWPL Error code (Zero on success)
 */
int hwpl_wdt_init( int mode /*! The WDT mode to use (see \ref wdt_mode_t) */,
		int interval /*! The timeout value in ms */);

/*! \details This function resets the watchdog timer
 * so that it won't trigger until the interval has expired.
 * This should be called more frequently than 1/interval to prevent
 * the WDT from triggering a reset or interrupt.
 */
void hwpl_wdt_reset(void);

void hwpl_wdt_priv_reset(void * args);

int hwpl_wdt_setaction(int (*action)(const void *, void *));


/*! \details This function sets the WDT timeout interval in ms.
 * \return Zero on success.
 */
int hwpl_wdt_setinterval(int interval);

#ifdef __cplusplus
}
#endif


#endif /* WDT_H_ */

/*! @} */
