/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "core_flags.h"


int _hwpl_core_setusbclock(int fosc /*! The oscillator frequency (between 10 and 25MHz) */){
	uint16_t m_mult;
	uint32_t fcco;

	//see if PLL0 can be used instead of PLL1
	if ( LPC_SC->PLL0CON & (1<<PLLC0) ){
		m_mult = LPC_SC->PLL0STAT + 1; //Assume N = 1
		fcco = fosc * (uint32_t)m_mult * 2;
		switch(fcco){
		case 288000000UL:
			LPC_SC->USBCLKCFG = 5;
			return 0;
		case 384000000UL:
			LPC_SC->USBCLKCFG = 7;
			return 0;
		case 480000000UL:
			LPC_SC->USBCLKCFG = 9;
			return 0;
		}

	}

	//Configure the USB PLL system to produce 48MHz
	switch(fosc){
	case 24000000UL:
		m_mult = 2;
		break;
	case 16000000UL:
		m_mult = 3;
		break;
	case 12000000UL:
		m_mult = 4;
		break;
	default:
		//USB is not supported
		return -1;
	}

	//If connected to PLL1, disconnect with a feed sequence
	if ( HWPL_TEST_BIT(LPC_SC->PLL1STAT, PLLC1_STAT)){
		HWPL_CLR_BIT(LPC_SC->PLL1CON, PLLC1);
		//PLL1 is connected
		LPC_SC->PLL1FEED = 0xAA;
		LPC_SC->PLL1FEED = 0x55;
	}

	//disable PLL1
	HWPL_CLR_BIT(LPC_SC->PLL1CON, PLLE1);
	LPC_SC->PLL1FEED = 0xAA;
	LPC_SC->PLL1FEED = 0x55;

	//If PLL1 is enabled, it is automatically used for the USB clock source
	//Update PLL1CFG and execute feed
	LPC_SC->PLL1CFG = (m_mult-1)|(1<<5);
	LPC_SC->PLL1FEED = 0xAA;
	LPC_SC->PLL1FEED = 0x55;

	//Enable PLL1
	HWPL_SET_BIT(LPC_SC->PLL1CON, PLLE1);
	LPC_SC->PLL1FEED = 0xAA;
	LPC_SC->PLL1FEED = 0x55;

	//Wait for the PLL to lock
	while( !HWPL_TEST_BIT(LPC_SC->PLL1STAT, PLOCK1) );

	//Connect the PLL
	HWPL_SET_BIT(LPC_SC->PLL1CON, PLLC1);
	LPC_SC->PLL1FEED = 0xAA;
	LPC_SC->PLL1FEED = 0x55;

	return 0;

}


/*! @} */
