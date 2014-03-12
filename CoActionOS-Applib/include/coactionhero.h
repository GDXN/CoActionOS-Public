/*! \file */

/*! \addtogroup COACTION_HERO CoAction Hero
 * @{
 */


#ifndef COACTIONHERO_H_
#define COACTIONHERO_H_

#include <hwpl.h>

/*! \brief Expansion PIO0 Port Number */
#define COACTION_EXP_PIO0_PORT 0
/*! \brief Expansion PIO0 Pin Mask */
#define COACTION_EXP_PIO0_PINMASK (1<<22)
/*! \brief Expansion PIO1 Port Number */
#define COACTION_EXP_PIO1_PORT 1
/*! \brief Expansion PIO1 Pin Mask */
#define COACTION_EXP_PIO1_PINMASK (1<<10)
/*! \brief Expansion PIO2 Port Number */
#define COACTION_EXP_PIO2_PORT 1
/*! \brief Expansion PIO2 Pin Mask */
#define COACTION_EXP_PIO2_PINMASK (1<<14)
/*! \brief Expansion PIO3 Port Number */
#define COACTION_EXP_PIO3_PORT 1
/*! \brief Expansion PIO3 Pin Mask */
#define COACTION_EXP_PIO3_PINMASK (1<<15)

/*! \brief Expansion DAC Port Number */
#define COACTION_EXP_DAC_PORT 0
/*! \brief Expansion DAC Channel Number */
#define COACTION_EXP_DAC_CHANNEL (0)
/*! \brief Expansion DAC Pin Assignment */
#define COACTION_EXP_DAC_PINASSIGN (0)

/*! \brief Expansion ADC0 Port Number */
#define COACTION_EXP_ADC0_PORT 0
/*! \brief Expansion ADC0 Channel Number */
#define COACTION_EXP_ADC0_CHANNEL (5)
/*! \brief Expansion ADC0 Pin Assignment */
#define COACTION_EXP_ADC0_PINASSIGN (0)
/*! \brief Expansion ADC1 Port Number */
#define COACTION_EXP_ADC1_PORT 0
/*! \brief Expansion ADC1 Channel Number */
#define COACTION_EXP_ADC1_CHANNEL (2)
/*! \brief Expansion ADC1 Pin Assignment */
#define COACTION_EXP_ADC1_PINASSIGN (0)

/*! \brief Expansion UART Port Number */
#define COACTION_EXP_UART_PORT 1
/*! \brief Expansion UART Pin Assignment */
#define COACTION_EXP_UART_PINASSIGN (1)

/*! \brief Expansion PWM0 Port Number */
#define COACTION_EXP_PWM0_PORT 1
/*! \brief Expansion PWM0 Channel Number */
#define COACTION_EXP_PWM0_CHANNEL PWM_CHANNEL5
/*! \brief Expansion PWM0 Pin Assignment */
#define COACTION_EXP_PWM0_PINASSIGN (1)
/*! \brief Expansion PWM1 Port Number */
#define COACTION_EXP_PWM1_PORT 1
/*! \brief Expansion PWM1 Channel Number */
#define COACTION_EXP_PWM1_CHANNEL PWM_CHANNEL6
/*! \brief Expansion PWM1 Pin Assignment */
#define COACTION_EXP_PWM1_PINASSIGN (1)

/*! \brief Expansion I2C Port Number */
#define COACTION_EXP_I2C_PORT 2
/*! \brief Expansion I2C Pin Assignment */
#define COACTION_EXP_I2C_PINASSIGN (0)

/*! \brief Expansion SPI Port Number */
#define COACTION_EXP_SPI_PORT 1
/*! \brief Expansion SPI Pin Assignment */
#define COACTION_EXP_SPI_PINASSIGN (0)
/*! \brief Expansion SPI Port Number */
#define COACTION_EXP_SPI_CS_PORT 0
/*! \brief Expansion SPI Pin Assignment */
#define COACTION_EXP_SPI_CS_PINMASK (1<<6)

/*! \brief Expansion EINT Port Number */
#define COACTION_EXP_EINT_PORT (0)
/*! \brief Expansion EINT Channel Number */
#define COACTION_EXP_EINT_CHANNEL (0)
/*! \brief Expansion EINT Pin Assignment */
#define COACTION_EXP_EINT_PINASSIGN (0)

/*! \brief Expansion IC0 Port Number */
#define COACTION_EXP_IC0_PORT (1)
/*! \brief Expansion IC0 Channel Number */
#define COACTION_EXP_IC0_CHANNEL (TMR_ACTION_CHANNEL_IC0)
/*! \brief Expansion IC0 Pin Assignment */
#define COACTION_EXP_IC0_PINASSIGN (0)

/*! \brief Expansion IC1 Port Number */
#define COACTION_EXP_IC1_PORT (1)
/*! \brief Expansion IC1 Channel Number */
#define COACTION_EXP_IC1_CHANNEL (TMR_ACTION_CHANNEL_IC1)
/*! \brief Expansion IC1 Pin Assignment */
#define COACTION_EXP_IC1_PINASSIGN (0)

/*! \brief Expansion OC0 Port Number */
#define COACTION_EXP_OC0_PORT (2)
/*! \brief Expansion OC0 Channel Number */
#define COACTION_EXP_OC0_CHANNEL (TMR_ACTION_CHANNEL_OC0)
/*! \brief Expansion OC0 Pin Assignment */
#define COACTION_EXP_OC0_PINASSIGN (1)

/*! \brief Expansion OC1 Port Number */
#define COACTION_EXP_OC1_PORT (2)
/*! \brief Expansion OC1 Channel Number */
#define COACTION_EXP_OC1_CHANNEL (TMR_ACTION_CHANNEL_OC1)
/*! \brief Expansion OC1 Pin Assignment */
#define COACTION_EXP_OC1_PINASSIGN (1)

#endif /* COACTIONHERO_H_ */

/*! @} */
