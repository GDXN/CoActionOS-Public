/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl/arch.h"
#include "hwpl/wdt.h"
#include "hwpl/core.h"

#define WDEN (1<<0)
#define WDRESET (1<<1)
#define WDTOF (1<<2)
#define WDINT (1<<3)

#define WDLOCK (1<<31)

int hwpl_wdt_init(int mode, int interval){

	if ( mode & WDT_MODE_INTERRUPT ){
		//! \todo enable the interrupt
		//LPC_WDT->MOD |= (WDINT);
		return -1; //Don't support interrupt -- if WDT resets, it is because interrupts are disabled for too long
	}

	//Set the clock source
#ifdef __lpc17xx
	int clk_src;
	clk_src = mode & WDT_MODE_CLK_SRC_MASK;
	LPC_WDT->CLKSEL = 0;
	switch(clk_src){
	case WDT_MODE_CLK_SRC_INTERNAL_RC:
		LPC_WDT->CLKSEL = 0;
		break;
	case WDT_MODE_CLK_SRC_MAIN:
		LPC_WDT->CLKSEL = 1;
		break;
	case WDT_MODE_CLK_SRC_RTC:
		LPC_WDT->CLKSEL = 2;
		break;
	default:
		return -1;
	}

	//Lock the clock source
	LPC_WDT->CLKSEL |= WDLOCK;
#endif

	LPC_WDT->MOD = 0;

	//Set Reset mode
	if ( mode & WDT_MODE_RESET ){
		LPC_WDT->MOD |= (WDRESET);
	}

	//Set the interval
	hwpl_wdt_setinterval(interval);

	//Start the watchdog timer
	LPC_WDT->MOD |= WDEN;

	//feed sequence is required to start the timer
	LPC_WDT->FEED = 0xAA;
	LPC_WDT->FEED = 0x55;

	return 0;
}

int hwpl_wdt_setinterval(int interval){
	uint32_t counts;

#ifdef __lpc177x_8x
	//WDT oscillator is 500KHz
	counts = interval*500;
#else
	int clk_src;
	clk_src = LPC_WDT->CLKSEL & 0x03;
	//Set the clock source
	counts = 0xFFFFFFFF;
	switch(clk_src){
	case 0:
		counts = (1000 * interval);
		break;
	case 1:
		counts = ( (_hwpl_core_getclock() / 1000) * interval + 2 ) / 4;
		break;
	case 2:
		counts = ( 8 * interval );
		break;
	}
#endif

	//Set the interval
	LPC_WDT->TC = (uint32_t)counts;
	hwpl_wdt_reset();

	return 0;
}

void hwpl_wdt_priv_reset(void * args){
	LPC_WDT->FEED = 0xAA;
	LPC_WDT->FEED = 0x55;
}

void hwpl_wdt_reset(void){
	LPC_WDT->FEED = 0xAA;
	LPC_WDT->FEED = 0x55;
}
