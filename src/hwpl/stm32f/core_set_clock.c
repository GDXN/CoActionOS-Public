#include "hwpl.h"
#include "core_flags.h"

int hwpl_core_cpu_freq;

void _hwpl_core_setclock(int fclk,
		int fosc){

	int fmult;
	int fdiv;

	if ( fosc ){
		RCC->CR |= RCC_CR_HSEON; //Enable the HSE (Main oscillator)
		// Wait till HSE is ready and if Time out is reached exit
		while ( !(RCC->CR & RCC_CR_HSERDY) );
	} else {
		//use the HSI (internal oscillator)
	}


	/* Enable Prefetch Buffer */
	FLASH->ACR |= FLASH_ACR_PRFTBE;

	/* Flash 2 wait state */
	FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
	FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

	//Default dividers are 0
	/*
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1; //HCLK = SYSCLK
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1; //PCLK1 = HCLK
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; //PCLK2 = HCLK
	*/


#ifdef STM32F10X_CL
	/* Configure PLLs ------------------------------------------------------*/
	/* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
	/* PREDIV1 configuration: PREDIV1CLK = PLL2 / 5 = 8 MHz */

	RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
			RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
	RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
			RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV5);

	/* Enable PLL2 */
	RCC->CR |= RCC_CR_PLL2ON;
	/* Wait till PLL2 is ready */
	while((RCC->CR & RCC_CR_PLL2RDY) == 0)
	{
	}


	/* PLL configuration: PLLCLK = PREDIV1 * 9 = 72 MHz */
	RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
	RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 |
			RCC_CFGR_PLLMULL9);
#else
	/*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
			RCC_CFGR_PLLMULL));
	RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);
#endif /* STM32F10X_CL */

	/* Enable PLL */
	RCC->CR |= RCC_CR_PLLON;

	/* Wait till PLL is ready */
	while((RCC->CR & RCC_CR_PLLRDY) == 0)
	{
	}

	/* Select PLL as system clock source */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

	/* Wait till PLL is used as system clock source */
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
	{
	}


}


/*! @} */
