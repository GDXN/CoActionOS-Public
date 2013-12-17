/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "core_flags.h"

int hwpl_core_cpu_freq HWPL_SYS_MEM;


#define PCLKSEL0_ALL_1 0x55515155
#define PCLKSEL1_ALL_1 0x54555455


void _hwpl_core_setclock(int fclk /*! The target clock frequency */,
		int fosc /*! The oscillator frequency (between 10 and 25MHz) */){
	uint16_t m_mult;
	uint16_t clk_div;
	uint8_t clk_src = 0;
	uint32_t fcco;


	//-- Errata won't let PCLK be changed after PLL is running
	LPC_SC->PCLKSEL0 = PCLKSEL0_ALL_1;
	LPC_SC->PCLKSEL1 = PCLKSEL1_ALL_1;
	//If connected to PLL0, disconnect with a feed sequence
	if ( HWPL_TEST_BIT(LPC_SC->PLL0STAT, PLLC0_STAT)){
		HWPL_CLR_BIT(LPC_SC->PLL0CON, PLLC0);
		//PLL0 is connected
		LPC_SC->PLL0FEED = 0xAA;
		LPC_SC->PLL0FEED = 0x55;
	}

	//disable PLL0
	HWPL_CLR_BIT(LPC_SC->PLL0CON, PLLE0);
	LPC_SC->PLL0FEED = 0xAA;
	LPC_SC->PLL0FEED = 0x55;

	//Select the main clock source
	if ( fosc ){
		HWPL_SET_BIT(LPC_SC->SCS, OSCEN);
		while ( !HWPL_TEST_BIT(LPC_SC->SCS, OSCSTAT ));
		if ( fosc && HWPL_TEST_BIT(LPC_SC->SCS, OSCSTAT) ){
			//Enable the main oscillator
			HWPL_SET_BIT(LPC_SC->SCS, OSCEN);
			if ( fosc > 18000000 ){  //If fosc is > 18M set to high speed oscillator mode
				HWPL_SET_BIT(LPC_SC->SCS, OSCRANGE);
			}
			clk_src = MAIN_OSC;
		}
	} else {  //use the IRC instead
		fosc = 4000000UL;
		clk_src = IRC_OSC;
	}

	if ( fclk > 96000000UL ){
		fcco = 480000000UL;
	} else {
		fcco = 288000000UL;
	}


	LPC_SC->CLKSRCSEL = clk_src;

	if ( fclk > fosc ){
		//FCCO must be between 275 MHZ and 550MHz
		//Calcute the divisors and multiplier for the PLL
		m_mult = (fcco + fosc)/(fosc*2);
		m_mult = ( m_mult > 512 ) ? 512 : m_mult;
		m_mult =  ( m_mult < 6 ) ? 6 : m_mult;

		clk_div = (fcco + fclk/2) / (fclk);
		clk_div = ( clk_div < 3 ) ? 3 : clk_div;
		clk_div = ( clk_div > 256 ) ? 256 : clk_div;
		hwpl_core_cpu_freq = fosc * 2 * m_mult / clk_div;
	} else if ( fclk <= fosc ){
		clk_div = (fosc + fclk/2) / (fclk);
		LPC_SC->CCLKCFG = (clk_div - 1);
		hwpl_core_cpu_freq = fosc / clk_div;
		return;
	}


	//Update PLL0CFG and execute feed
	LPC_SC->PLL0CFG = (m_mult-1);  //Assume N = 1
	LPC_SC->PLL0FEED = 0xAA;
	LPC_SC->PLL0FEED = 0x55;

	//Enable PLL0
	HWPL_SET_BIT(LPC_SC->PLL0CON, PLLE0);
	LPC_SC->PLL0FEED = 0xAA;
	LPC_SC->PLL0FEED = 0x55;

	//Update clock divider
	LPC_SC->CCLKCFG = (clk_div - 1);

	//Wait for the PLL to lock
	while( !HWPL_TEST_BIT(LPC_SC->PLL0STAT, PLOCK0) );

	//Connect the PLL
	HWPL_SET_BIT(LPC_SC->PLL0CON, PLLC0);
	LPC_SC->PLL0FEED = 0xAA;
	LPC_SC->PLL0FEED = 0x55;


}


/*! @} */
