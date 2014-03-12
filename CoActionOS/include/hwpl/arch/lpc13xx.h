/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


/*! \addtogroup LPC13XXDEV LPC13xx Devices
 * @{
 *
 * \ingroup ARCHITECTURES
 *
 * \details This contains the device specific definitions for the LPC13xx microcontroller series.
 *
 * The following devices are supported:
 * - lpc1342
 * - lpc1343
 *
 *
 * GPIO Configurations used with pin_assign member of the *_attr_t data structures:
 * - TBD
 */

/*! \file
 * \brief LPC13xx Header File
 *
 */

#ifndef LPC13XX_H_
#define LPC13XX_H_

#include "hwpl/types.h"
#include <string.h>
#include <stdint.h>

/*! \brief A pointer on the lpc13xx */
typedef void * hwpl_target_ptr_t;

#define HWPL_SYS_MEM __attribute__((section(".sysmem")));

/*! \brief The maximum ADC value on the LPC13xx */
#define ADC_MAX 0x0000FFF0

/*! \brief The minimum ADC step value on the LPC13xx */
#define ADC_MIN 0x01

/*! \brief The maximum ADC sampling frequency */
#define ADC_MAX_FREQ 400000
#define ADC_SAMPLE_T

/*! \brief ADC Sample type (when reading ADC) */
typedef uint16_t adc_sample_t;

#define GPIO_SAMPLE_T
/*! \brief This defines the type of a GPIO sample for
 * reading/writing the GPIO (see gpio_read() and gpio_write()).
 *
 */
typedef uint32_t gpio_sample_t;


#define PWM_DUTY_T
/*! \brief This defines the type for the PWM duty cycle for reading/writing the PWM */
typedef uint32_t pwm_duty_t;


/*! \brief The minimum flash write size */
#define FLASH_MIN_WRITE_SIZE 256

/*! \brief The maximum flash write size */
#define FLASH_MAX_WRITE_SIZE 1024


/*! \details This lists the timer action channels.
 *
 */
typedef enum {
	TMR_ACTION_CHANNEL_OC0 /*! Output compare channel 0 */,
	TMR_ACTION_CHANNEL_OC1 /*! Output compare channel 1 */,
	TMR_ACTION_CHANNEL_OC2 /*! Output compare channel 2 */,
	TMR_ACTION_CHANNEL_OC3 /*! Output compare channel 3 */,
	TMR_ACTION_CHANNEL_IC0 /*! Input capture channel 0 */,
} lpc13xx_tmr_action_channel_t;

typedef lpc13xx_tmr_action_channel_t tmr_action_channel_t;


#define HWPL_PERIPH_MEMORY_BASE
#define HWPL_PERIPH_MEMORY_SIZE

int _hwpl_lpc_flash_write_page(int page, void * addr, const void * src, int size);
int _hwpl_lpc_flash_erase_page(int page);
int _hwpl_lpc_flash_get_serialno(uint32_t * dest);

#define IAP_ADDRESS 0x1FFF1FF1

#include "hwpl/arch/LPC13xx_cm3.h"
#include "hwpl/arch/delay_cm3.h"

#define MPU_ACCESS_T
#define MPU_SIZE_T

#include "hwpl/arch/lpc13xx_flags.h"
#include "hwpl/arch/common_cm3.h"

#define DEV_LAST_IRQ EINT0_IRQn
#define DEV_MIDDLE_PRIORITY 4

#define SYSAHBCLKCTRL_SYS (1<<0)
#define SYSAHBCLKCTRL_ROM (1<<1)
#define SYSAHBCLKCTRL_RAM (1<<2)
#define SYSAHBCLKCTRL_FLASH1 (1<<3)
#define SYSAHBCLKCTRL_FLASH2 (1<<4)
#define SYSAHBCLKCTRL_I2C (1<<5)
#define SYSAHBCLKCTRL_GPIO (1<<6)
#define SYSAHBCLKCTRL_CT16B0 (1<<7)
#define SYSAHBCLKCTRL_CT16B1 (1<<8)
#define SYSAHBCLKCTRL_CT32B0 (1<<9)
#define SYSAHBCLKCTRL_CT32B1 (1<<10)
#define SYSAHBCLKCTRL_SSP (1<<11)
#define SYSAHBCLKCTRL_UART (1<<12)
#define SYSAHBCLKCTRL_ADC (1<<13)
#define SYSAHBCLKCTRL_USB_REG (1<<14)
#define SYSAHBCLKCTRL_WDT (1<<15)
#define SYSAHBCLKCTRL_IOCON (1<<16)


#include <string.h>

#define WDMOD_WDEN 0x01
#define WDMOD_WDRESET (1<<1)
#define WDMOD_WDTOF (1<<2)
#define WDMOD_WDINT (1<<3)

enum {
	CLKOUT_IRC_OSC,
	CLKOUT_SYS_OSC,
	CLKOUT_WDT_OSC,
	CLKOUT_MAIN
};

#include "lpc13xx_flags.h"

#define HWPL_CORE_PORTS 1
#define HWPL_SPI_PORTS 1
#define HWPL_SSP_PORTS 1
#define HWPL_SSP_REGS { LPC_SSP0 }
#define HWPL_GPIO_PORTS 4
#define HWPL_GPIO_REGS { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3 }
#define HWPL_PIO_PORTS HWPL_GPIO_PORTS
#define HWPL_PIO_REGS { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3 }
#define HWPL_I2C_PORTS 1
#define HWPL_I2C_REGS { LPC_I2C }
#define HWPL_UART_PORTS 1
#define HWPL_UART_REGS { LPC_UART }
#define HWPL_TMR_PORTS 4
#define HWPL_TMR_REGS { LPC_TMR16B0, LPC_TMR16B1, LPC_TMR32B0, LPC_TMR32B1 }
#define HWPL_EINT_PORTS 40

#define HWPL_FLASH_PORTS 1
#define HWPL_MEM_PORTS 1
#define HWPL_ADC_PORTS 1
#define HWPL_ADC_REGS { LPC_ADC }
#define HWPL_DAC_PORTS 0
#define HWPL_QEI_PORTS 0
#define HWPL_RTC_PORTS 1
#define HWPL_USB_PORTS 1
#define HWPL_USB_REGS { LPC_USB }
#define HWPL_PWM_PORTS 0

#define HWPL_EEPROM_PORTS 0
#define HWPL_EMC_PORTS 0

#endif /* LPC13XX_H_ */

/*! @} */
