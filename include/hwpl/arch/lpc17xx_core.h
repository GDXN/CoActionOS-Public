/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup CORE
 * @{
 *
 * \ingroup LPC17XXDEV
 *
 *
 */

/*! \file */

#ifndef _HWPL_ARCH_LPC17XX_CORE_H_
#define _HWPL_ARCH_LPC17XX_CORE_H_

//Peripheral Clocks
enum {
	PCLK_WDT,
	PCLK_TIMER0,
	PCLK_TIMER1,
	PCLK_UART0,
	PCLK_UART1,
	PCLK_RESERVED0,
	PCLK_PWM1,
	PCLK_I2C0,
	PCLK_SPI,
	PCLK_RESERVED1,
	PCLK_SSP1,
	PCLK_DAC,
	PCLK_ADC,
	PCLK_CAN1,
	PCLK_CAN2,
	PCLK_ACF,
	PCLK_QEI,
	PCLK_GPIOINT,
	PCLK_PCB,
	PCLK_I2C1,
	PCLK_RESERVED3,
	PCLK_SSP0,
	PCLK_TIMER2,
	PCLK_TIMER3,
	PCLK_UART2,
	PCLK_UART3,
	PCLK_I2C2,
	PCLK_I2S,
	PCLK_RESERVED4,
	PCLK_RIT,
	PCLK_SYSCON,
	PCLK_MC
};

#define PCLK_MASK(x,v) (v<<(x*2))


#define PCLK_4 0
#define PCLK_1 1
#define PCLK_2 2
#define PCLK_8 3


//#define
enum {
	PCLCD,
	PCTIM0,
	PCTIM1,
	PCUART0,
	PCUART1,
	PCPWM0,
	PCPWM1,
	PCI2C0,
	PCSPI,
	PCRTC,
	PCSSP1,
	PCEMC,
	PCADC,
	PCCAN1,
	PCCAN2,
	PCGPIO,
	PCRIT,
	PCMCPWM,
	PCQEI,
	PCI2C1,
	PCSSP2,
	PCSSP0,
	PCTIM2,
	PCTIM3,
	PCUART2,
	PCUART3,
	PCI2C2,
	PCI2S,
	PCSDC,
	PCGPDMA,
	PCENET,
	PCUSB
};

#define PCUART4 PCSPI



static inline void _hwpl_lpc_core_enable_pwr(int periph) HWPL_ALWAYS_INLINE;
static inline void _hwpl_lpc_core_disable_pwr(int periph) HWPL_ALWAYS_INLINE;
static inline int _hwpl_lpc_core_pwr_enabled(int periph) HWPL_ALWAYS_INLINE;


void _hwpl_lpc_core_enable_pwr(int periph){ HWPL_SET_BIT(LPC_SC->PCONP, periph); }
void _hwpl_lpc_core_disable_pwr(int periph){ HWPL_CLR_BIT(LPC_SC->PCONP, periph); }
int _hwpl_lpc_core_pwr_enabled(int periph){ return (HWPL_TEST_BIT(LPC_SC->PCONP, periph)); }



#endif /* _HWPL_ARCH_LPC17XX_CORE_H_ */

/*! @} */
