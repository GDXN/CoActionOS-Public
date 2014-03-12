/*
 * lpc177x_8x.h
 *
 *  Created on: Sep 27, 2013
 *      Author: tgil
 */

#ifndef LPC177X_8X_H_
#define LPC177X_8X_H_

#include <stdint.h>

#include <string.h>
#include "hwpl/types.h"

#include "LPC177x_8x_cm3.h"
#include "lpc17xx_dma.h"
#include "delay_cm3.h"
#include "lpc17xx_core.h"
#include "common_cm3.h"

/*! \details This defines the timer channels.
 * It is assigned to the channel member of \ref tmr_action_t
 * and \ref tmr_reqattr_t.
 */
typedef enum {
	TMR_ACTION_CHANNEL_OC0 /*! Output compare channel 0 */,
	TMR_ACTION_CHANNEL_OC1 /*! Output compare channel 1 */,
	TMR_ACTION_CHANNEL_OC2 /*! Output compare channel 2 */,
	TMR_ACTION_CHANNEL_OC3 /*! Output compare channel 3 */,
	TMR_ACTION_CHANNEL_IC0 /*! Input capture channel 0 */,
	TMR_ACTION_CHANNEL_IC1 /*! Input capture channel 1 */,
} lpc177x_8x_tmr_action_channel_t;

typedef lpc177x_8x_tmr_action_channel_t tmr_action_channel_t;


typedef uint32_t pwm_duty_t;

#define PIO_SAMPLE_T
/*! \brief This defines the type of a PIO sample for
 * reading/writing the PIO.
 */
typedef uint32_t pio_sample_t;

/*! \brief This defines a pointer on the lpc17xx.
 */
typedef void * hwpl_target_ptr_t;


/*! \brief The maximum ADC value on the LPC17xx.
 */
#define ADC_MAX 0x0000FFF0

/*! \brief Minimum ADC set value
 */
#define ADC_MIN 0x01

/*! \brief The maximum ADC sampling frequency.
 */
#define ADC_MAX_FREQ 200000
#define ADC_SAMPLE_T

/*! \brief The maximum DAC output frequency.
 */
#define DAC_MAX_FREQ 1000000

/*! \brief This defines the type of an ADC sample (see adc_read() and adc_write()).
 */
typedef uint16_t adc_sample_t;

#define HWPL_CORE_PORTS 1
#define HWPL_EEPROM_PORTS 1
#define HWPL_SPI_PORTS 0
#define HWPL_SSP_PORTS 3
#define HWPL_SSP_REGS { LPC_SSP0, LPC_SSP1, LPC_SSP2 }
#define HWPL_GPIO_PORTS 5
#define HWPL_PIO_PORTS 6
#define HWPL_PIO_REGS { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4, LPC_GPIO5 }
#define HWPL_I2C_PORTS 3
#define HWPL_UART_PORTS 5
#define HWPL_UART_REGS { (LPC_UART_TypeDef *)LPC_UART0, (LPC_UART_TypeDef *)LPC_UART1, LPC_UART2, LPC_UART3, (LPC_UART_TypeDef *)LPC_UART4 }


#define HWPL_TMR_PORTS 4
#define HWPL_EINT_PORTS 4
#define HWPL_FLASH_PORTS 1
#define HWPL_MEM_PORTS 1
#define HWPL_ADC_PORTS 1
#define HWPL_DAC_PORTS 1
#define HWPL_QEI_PORTS 1
#define HWPL_RTC_PORTS 1
#define HWPL_USB_PORTS 1
#define HWPL_PWM_PORTS 2
#define HWPL_PWM_REGS { LPC_PWM0, LPC_PWM1 }

typedef struct HWPL_PACK {
	  __IO uint32_t StaticConfig;
	  __IO uint32_t StaticWaitWen;
	  __IO uint32_t StaticWaitOen;
	  __IO uint32_t StaticWaitRd;
	  __IO uint32_t StaticWaitPage;
	  __IO uint32_t StaticWaitWr;
	  __IO uint32_t StaticWaitTurn;
} LPC_EMC_Static_TypeDef;

typedef struct HWPL_PACK {
	  __IO uint32_t DynamicConfig;
	  __IO uint32_t DynamicRasCas;
} LPC_EMC_Dynamic_TypeDef;

#define HWPL_EMC_PORTS 1
#define HWPL_EMC_CHANS 4
#define HWPL_EMC_STATIC_REGS { (LPC_EMC_Static_TypeDef*)&LPC_EMC->StaticConfig0, (LPC_EMC_Static_TypeDef*)&LPC_EMC->StaticConfig1, (LPC_EMC_Static_TypeDef*)&LPC_EMC->StaticConfig2, (LPC_EMC_Static_TypeDef*)&LPC_EMC->StaticConfig3 }
#define HWPL_EMC_DYNAMIC_REGS { (LPC_EMC_Dynamic_TypeDef*)&LPC_EMC->DynamicConfig0, (LPC_EMC_Dynamic_TypeDef*)&LPC_EMC->DynamicConfig1, (LPC_EMC_Dynamic_TypeDef*)&LPC_EMC->DynamicConfig2, (LPC_EMC_Dynamic_TypeDef*)&LPC_EMC->DynamicConfig3 }




#define HWPL_START_OF_SRAM 0x10000000
#define HWPL_START_OF_AHB_SRAM 0x20000000

/*! \brief This is the minimum flash write size. */
#define FLASH_MIN_WRITE_SIZE 256

/*! \brief This is the maximum flash write size. */
#define FLASH_MAX_WRITE_SIZE 1024

/*! \brief This is the minimum flash write size.
 */
#define MEM_MIN_FLASH_WRITE_SIZE 256

/*! \brief This is the maximum flash write size.
 */
#define MEM_MAX_FLASH_WRITE_SIZE 1024


#define DEV_LAST_IRQ EEPROM_IRQn
#define DEV_MIDDLE_PRIORITY 16
#define IAP_ADDRESS 0x1FFF1FF1
#define HWPL_CORE_BOOTLOADER_LOC 0x10002000
#define HWPL_CORE_BOOTLOADER_VALUE 0x55AA55AA

#define HWPL_LAST_BOOTLOADER_PAGE 4

//Pin mapping for lpc17xx vs lpc177x_8x
#define HWPL_ADC_CHANNEL6_PIN 12
#define HWPL_ADC_CHANNEL7_PIN 13

int _hwpl_lpc_flash_write_page(int page, void * addr, const void * src, int size);
int _hwpl_lpc_flash_erase_page(int page);
int _hwpl_lpc_flash_get_serialno(uint32_t * dest);

#endif /* LPC177X_8X_H_ */
