/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup CORE
 *
 * \ingroup LPC1XXX
 *
 * @{
 */


#include "hwpl.h"
#include "core_flags.h"


/*! \brief sets the USB clock based on the system oscillator input.
 * \details This function sets the USB clock based on the input oscillator.
 * The input oscillator should be an integral divisor of 48Mhz such as
 * 12Mhz, 16MHz, 24Mhz, etc.
 *
 */
int _hwpl_core_setusbclock(int fosc){
	uint8_t psel;
	LPC_SYSCON->PDRUNCFG &= ~(1<<10); // Power-up USB PHY
	LPC_SYSCON->PDRUNCFG &= ~(1<<8); // Power-up USB PLL
	LPC_SYSCON->USBPLLCLKSEL = 0x01; // Select System clock as PLL Input
	LPC_SYSCON->USBPLLCLKUEN = 0x01; // Update Clock Source
	LPC_SYSCON->USBPLLCLKUEN = 0x00; // Toggle Update Register
	LPC_SYSCON->USBPLLCLKUEN = 0x01;
	while (!(LPC_SYSCON->USBPLLCLKUEN & 0x01)); // Wait Until Updated

	psel = (260000000UL + 48000000UL) / (48000000UL<<1);
	LPC_SYSCON->USBPLLCTRL = PLLCFG_MSEL(48000000UL/fosc) | PLLCFG_PSEL(psel/2);
	while (!(LPC_SYSCON->USBPLLSTAT   & 0x01)); // Wait Until PLL Locked
	LPC_SYSCON->USBCLKSEL = 0x00; // Select USB PLL
	return 0;
}


/*! @} */
