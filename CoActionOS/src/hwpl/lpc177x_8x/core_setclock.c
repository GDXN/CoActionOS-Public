/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "../lpc17xx/core_flags.h"

int hwpl_core_cpu_freq HWPL_SYS_MEM;


typedef struct {
	uint32_t clock;
	uint32_t fcco;
	uint8_t msel;
	uint8_t psel;
} hwpl_core_clock_cfg_t;


void _hwpl_core_setclock(int fclk /*! The target clock frequency */,
		int fosc /*! The oscillator frequency (between 10 and 25MHz) */){
	uint16_t clk_div;
	uint8_t msel;
	uint8_t psel;

	LPC_SC->CLKSRCSEL = 0;
	LPC_SC->CCLKSEL = 1; //Use sysclk with no divider
	LPC_SC->PCLKSEL = 1;

	//Select the main clock source
	if ( fosc ){
		if ( fosc > 18000000 ){  //If fosc is > 18M set to high speed oscillator mode
			HWPL_SET_BIT(LPC_SC->SCS, OSCRANGE);
		}
		HWPL_SET_BIT(LPC_SC->SCS, OSCEN); //enable the main oscillator
		while ( !HWPL_TEST_BIT(LPC_SC->SCS, OSCSTAT ));
		LPC_SC->CLKSRCSEL = MAIN_OSC;

	} else {  //use the IRC instead
		fosc = 4000000UL;
	}

	//If PLL0 is on, turn it off
	if ( HWPL_TEST_BIT(LPC_SC->PLL0STAT, 8)){
		LPC_SC->PLL0CON = 0;
		//PLL0 is connected
		LPC_SC->PLL0FEED = 0xAA;
		LPC_SC->PLL0FEED = 0x55;
	}

	if( fclk <= fosc ){
		clk_div = (fosc + fclk/2) / (fclk);
		LPC_SC->CCLKSEL = (clk_div);
		hwpl_core_cpu_freq = fosc / clk_div;
		return;
	}

	//fclk = fosc * M -- calculate M
	msel = (fclk + (fosc>>1)) / fosc;
	if ( msel > 32 ) {
		msel = 32;
	}

	hwpl_core_cpu_freq = msel * fosc;

	msel--; //msel is m minus 1

	//fcco = fclk * 2 * p (fcc must be 156MHz to 320MHz)

	//P is 1, 2, 4, 8 and psel is 0, 1, 2, 3
	psel = 0;
	if( fclk < 80000000 ){ //fcco must be at least 156MHz -- 80MHz * 2 = 160MHz below 80 fcco will be at least 80
		psel = 1;
	}

	if( fclk < 40000000 ){
		psel = 2;
	}

	if( fclk < 20000000 ){
		psel = 3;
	}

	LPC_SC->PLL0CFG = (msel) | (psel<<5);



	//Enable PLL0
	HWPL_SET_BIT(LPC_SC->PLL0CON, PLLE0);
	LPC_SC->PLL0FEED = 0xAA;
	LPC_SC->PLL0FEED = 0x55;

	//Wait for PLL to lock
	while( !HWPL_TEST_BIT(LPC_SC->PLL0STAT, 10) );

	//Update clock divider
	LPC_SC->CCLKSEL = (1) | (1<<8);
}


/*! @} */
