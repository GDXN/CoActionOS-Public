/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "../lpc17xx/core_flags.h"


int _hwpl_core_setusbclock(int fosc /*! The oscillator frequency (between 10 and 25MHz) */){

	uint8_t p;
	uint8_t m;

	//see if PPL0 is on
	/*

	uint32_t fcco;
	uint32_t stat;
	//this doesn't work quite right
	stat = LPC_SC->PLL0STAT;


	if( stat & (1<<8) ){
		m = (stat & 0x1F) + 1;
		p = (stat >> 5) & 0x03;
		p = (1<<p);
		fcco = fosc * 2 * m * p;

		if( fcco == 288000000 ){
			//set USBCLKSEL div to 6
			LPC_SC->USBCLKSEL = (1<<8) | 6;
			return 0;
		} else if( fcco == 192000000 ){
			//setUSBCLKSEL div to 4
			LPC_SC->USBCLKSEL = (1<<8) | 4;
			return 0;
		}

	}
	*/

	//Configure the USB PLL system to produce 48MHz
	switch(fosc){
	case 24000000UL:
		m = 1; //msel is m - 1 (24 * (1+1) = 48)
		p = 2;
		break;
	case 16000000UL:
		m = 2;
		p = 3;
		break;
	case 12000000UL:
		m = 3;
		p = 3;
		break;
	default:
		//USB is not supported
		return -1;
	}

	//If PLL1 is on, turn it off
	if ( HWPL_TEST_BIT(LPC_SC->PLL1STAT, 8)){
		LPC_SC->PLL1CON = 0;
		//PLL0 is connected
		LPC_SC->PLL1FEED = 0xAA;
		LPC_SC->PLL1FEED = 0x55;
	}


	//If PLL1 is enabled, it is automatically used for the USB clock source
	//Update PLL1CFG and execute feed
	LPC_SC->PLL1CFG = m | (p<<5);

	//Enable PLL1
	LPC_SC->PLL1CON = 1;
	LPC_SC->PLL1FEED = 0xAA;
	LPC_SC->PLL1FEED = 0x55;

	//Wait for the PLL to lock
	while( !HWPL_TEST_BIT(LPC_SC->PLL1STAT, 10) );

	LPC_SC->USBCLKSEL = (2<<8) | 1; //Use PLL1 with no clock divider

	return 0;

}


/*! @} */
