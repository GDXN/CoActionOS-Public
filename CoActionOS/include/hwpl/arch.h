/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup ARCHITECTURES Architectures
 * @{
 *
 * \details Some aspects of HWPL are MCU specific.  For
 * example, an MCU may use a different type to define the size of an ADC sample.  Despite some aspects
 * of the API being MCU specific, programs that use HWPL will be portable to other platforms without
 * modification if the MCU specific definitions are used correctly.  The following
 * example illustrates this with the ADC:
 *
 * Right (portable):
 * \code
 * adc_sample_t samples[100];
 * adc_read(0, 0, samples, sizeof(adc_sample_t)*100);
 * \endcode
 *
 * The above code will work with any HWPL supported architecture.
 *
 * Wrong (non-portable):
 * \code
 * uint16_t samples[100];
 * adc_read(0, 0, samples, sizeof(uint16_t)*100);
 * \endcode
 *
 * The above code will only work on HWPL devices that use 16-bits for an ADC sample.
 *
 *
 */


/*! \file
 * \brief Architecture Header File includes correct architecture.
 */

#ifndef ARCH_H_
#define ARCH_H_

#include <stdint.h>
#include <stdlib.h> //defines NULL
#include "types.h"

#ifdef __lpc13xx
#define ARM_MATH_CM3
#include "arch/lpc13xx.h"
#endif

#ifdef __lpc17xx
#define ARM_MATH_CM3
#include "arch/lpc17xx.h"
#endif

#ifdef __lpc177x_8x
#define ARM_MATH_CM3
#include "arch/lpc177x_8x.h"
#endif

#ifdef __link
#ifdef __cplusplus
extern "C" {
#endif
typedef uint8_t tmr_action_channel_t;
typedef uint32_t hwpl_target_ptr_t;
#define PWM_DUTY_T
typedef uint32_t pwm_duty_t;

#undef NAME_MAX
#define NAME_MAX 24
#ifdef __cplusplus
}
#endif

#define HWPL_CORE_PORTS 32
#define HWPL_SPI_PORTS 32
#define HWPL_GPIO_PORTS 32
#define HWPL_PIO_PORTS 32
#define HWPL_I2C_PORTS 32
#define HWPL_UART_PORTS 32
#define HWPL_TMR_PORTS 32
#define HWPL_EINT_PORTS 32
#define HWPL_FLASH_PORTS 32
#define HWPL_MEM_PORTS 32
#define HWPL_ADC_PORTS 32
#define HWPL_DAC_PORTS 32
#define HWPL_QEI_PORTS 32
#define HWPL_RTC_PORTS 32
#define HWPL_USB_PORTS 32
#define HWPL_PWM_PORTS 32

#else
#include <sys/syslimits.h>
#endif

#ifdef __stm32f
#include "arch/stm32f.h"
#endif

struct link_tm {
	int32_t	tm_sec;
	int32_t	tm_min;
	int32_t	tm_hour;
	int32_t	tm_mday;
	int32_t	tm_mon;
	int32_t	tm_year;
	int32_t	tm_wday;
	int32_t	tm_yday;
	int32_t	tm_isdst;
};

#endif /* ARCH_H_ */

/*! @} */

