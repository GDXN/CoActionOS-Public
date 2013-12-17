/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl/wdt.h"
#include "hwpl/core.h"
#include "core_wdt_flags.h"

int hwpl_wdt_init(int mode, int interval){
	int clk_src;

	if ( mode & WDT_MODE_INTERRUPT ){
		//! \todo Enable the interrupt -- make it the highest priority
		return -1; //Don't support interrupt -- if WDT resets, it is because interrupts are disabled for too long
	}

	//Set the clock source
	clk_src = mode & WDT_MODE_CLK_SRC_MASK;
	LPC_SYSCON->WDTCLKDIV = 1; //divide the clock by 1
	LPC_SYSCON->WDTCLKSEL = 0;
	switch(clk_src){
	case WDT_MODE_CLK_SRC_INTERNAL_RC:
		LPC_SYSCON->WDTCLKSEL = 0;
		break;
	case WDT_MODE_CLK_SRC_MAIN:
		LPC_SYSCON->WDTCLKSEL = 1;
		break;
	case WDT_MODE_CLK_SRC_WDT_OSC:
		LPC_SYSCON->WDTCLKSEL = 2;
		break;
	case WDT_MODE_CLK_SRC_RTC:
		return -1;
	default:
		return -1;
	}

	//Update the clock source
	LPC_SYSCON->WDTCLKUEN |= (1<<0);

	LPC_WDT->MOD = 0;

	//Set Reset mode
	if ( mode & WDT_MODE_RESET ){
		LPC_WDT->MOD |= (WDRESET);
	}

	//Set the interval
	hwpl_wdt_setinterval(interval);

	//Start the watchdog timer
	LPC_WDT->MOD |= WDEN;

	return 0;
}

int hwpl_wdt_setinterval(int interval){
	uint32_t counts;
	int clk_src;

	clk_src = LPC_SYSCON->WDTCLKSEL & 0x03;
	//Set the clock source
	counts = 0xFFFFFFFF;
	switch(clk_src){
	case 0:
		//IRC oscillator
		counts = (12000 * interval);
		break;
	case 1:
		//main oscillator
		counts = ( (_hwpl_core_getclock() / 1000) * interval );
		break;
	case 2:
		//WDT oscillator -- this clock resets to 1.6MHz divided by 2 = 0.8MHz
		counts = 800 * interval;
		break;
	}

	//Set the interval
	LPC_WDT->TC = (uint32_t)counts;
	hwpl_wdt_reset();

	return 0;
}

void hwpl_wdt_reset(void){
	LPC_WDT->FEED = 0xAA;
	LPC_WDT->FEED = 0x55;
}
