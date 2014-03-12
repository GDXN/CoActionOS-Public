/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"

#include "core_flags.h"

int hwpl_core_cpu_freq;

static void _hwpl_update_clock_source(int source){
	LPC_SYSCON->MAINCLKSEL = source;
	LPC_SYSCON->MAINCLKUEN = 0x01;  // Update MCLK Clock Source
	LPC_SYSCON->MAINCLKUEN = 0x00;  // Toggle Update Register
	LPC_SYSCON->MAINCLKUEN = 0x01;
	while (!(LPC_SYSCON->MAINCLKUEN & 0x01));       // Wait Until Updated
}

static void _hwpl_update_pllclock_source(int source){
	LPC_SYSCON->SYSPLLCLKSEL  = source;
	LPC_SYSCON->SYSPLLCLKUEN  = 0x01;
	LPC_SYSCON->SYSPLLCLKUEN  = 0x00;
	LPC_SYSCON->SYSPLLCLKUEN  = 0x01; //this sequence updates the PLL clock source
	while (!(LPC_SYSCON->SYSPLLCLKUEN & 0x01)); //wait for the signal to go low
}

static void cfg_pll(uint8_t msel, uint8_t psel);
static void cfg_pll_irc(uint8_t msel, uint8_t psel);

void _hwpl_core_setclock(int fclk /*! The new clock frequency */,
		int fosc /*! The oscillator frequency (between 10 and 25MHz) */){
	uint8_t psel;
	uint8_t msel;
	int div;
	//use approx 260MHz for Fcco


	LPC_SYSCON->PDRUNCFG &= ~(1 << 5);          // Power-up System Osc

	if( fosc > 18000000 ){
		LPC_SYSCON->SYSOSCCTRL = 1<<1; //use a high frequency oscillator
	}

	if( fclk == 0 ){
		fclk = fosc;
	}

	if( fclk <= fosc ){
		//don't use the PLL
		div = fosc / fclk;
		div &= 0xFF; //255 is the max value
		LPC_SYSCON->SYSAHBCLKDIV  = div;
		hwpl_core_cpu_freq = fosc / div;
		_hwpl_update_pllclock_source(1); //use system clock for PLL input
		_hwpl_update_clock_source(1); //use the PLL input as the main clock
		return;
	}


	msel = (fclk + (fosc>>1)) / fosc;
	if ( msel > 32 ) {
		msel = 32;
	}
	psel = (260000000UL + fclk) / (fclk<<1);
	if ( psel > 8 ){
		psel = 8;
	}
	if ( psel < 2 ){
		psel = 2;
	}
	psel >>= 1;
	cfg_pll(msel, psel);
	hwpl_core_cpu_freq = (uint32_t)fclk;

	//power down the IRC
	//LPC_SYSCON->PDRUNCFG |= (1 << 1);  //Power down the IRC

}

void cfg_pll(uint8_t msel, uint8_t psel){
	// set PLL multiplier & divisor.
	// values computed from config.h
	_hwpl_update_pllclock_source(1); //use main for PLL input

	LPC_SYSCON->SYSPLLCTRL = PLLCFG_MSEL(msel) | PLLCFG_PSEL(psel/2);
	LPC_SYSCON->PDRUNCFG &= ~(1 << 7);          // Power-up SYSPLL
	while (!(LPC_SYSCON->SYSPLLSTAT & 0x01));	// Wait Until PLL Locked

	_hwpl_update_clock_source(3); //use the PLL output
}


void _hwpl_core_setclockinternal(uint32_t fclk /*! The new clock frequency */){
	uint8_t psel;
	uint8_t msel;
	uint32_t div;
	//use approx 260MHz for Fcco

	if( fclk == 0 ){
		hwpl_core_cpu_freq = 12000000;
		return;
	}

	if( fclk <= 12000000 ){
		div = 12000000 / fclk;
		div &= 0xFF; //255 is the max value
		LPC_SYSCON->SYSAHBCLKDIV  = div;
		hwpl_core_cpu_freq = 12000000 / div;
		return;
	}

	//calculate PLL values
	msel = (fclk + (12000000UL>>1)) / 12000000UL;
	if ( msel > 32 ) {
		msel = 32;
	}
	psel = (260000000UL + fclk) / (fclk<<1);
	if ( psel > 8 ){
		psel = 8;
	}
	if ( psel < 2 ){
		psel = 2;
	}
	psel >>= 1;
	cfg_pll_irc(msel, psel);
	hwpl_core_cpu_freq = fclk;
}

void cfg_pll_irc(uint8_t msel, uint8_t psel){
	// set PLL multiplier & divisor.
	// values computed from config.h

	_hwpl_update_pllclock_source(0); //use IRC for PLL input

	LPC_SYSCON->SYSPLLCTRL = PLLCFG_MSEL(msel) | PLLCFG_PSEL(psel/2);
	LPC_SYSCON->PDRUNCFG &= ~(1 << 7);          // Power-up SYSPLL
	while (!(LPC_SYSCON->SYSPLLSTAT & 0x01));	      // Wait Until PLL Locked

	_hwpl_update_clock_source(3); //use the PLL output
}


/*! @} */
