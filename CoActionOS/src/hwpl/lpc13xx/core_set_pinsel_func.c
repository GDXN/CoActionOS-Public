/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "hwpl/core.h"

__IO uint32_t *  _hwpl_get_iocon_regs(int port, int pin);

typedef struct {
	uint8_t entry[4];
} pinsel_table_t;

#define ENTRY(function, port) (port<<5|function)
#define ENTRY_GET_FUNCTION(entry) (entry&~0xE0)
#define ENTRY_GET_PORT(entry) ((entry>>5)&0x07)

#define TOTAL_PINS (12*3+6)

const pinsel_table_t pinsel_func_table[TOTAL_PINS] = {
		{{ ENTRY(CORE_PERIPH_RESET, 0), ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.0
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_CLKOUT, 0), ENTRY(CORE_PERIPH_TMR, 2), ENTRY(CORE_PERIPH_USB, 0) }}, //0.1
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_SPI, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.2
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_USB, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.3
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_I2C, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.4
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_I2C, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.5
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_USB, 0), ENTRY(CORE_PERIPH_SPI, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.6
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.7
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_SPI, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.8
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_SPI, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_JTAG, 0) }}, //0.9
		{{ ENTRY(CORE_PERIPH_JTAG, 0), ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_SPI, 0), ENTRY(CORE_PERIPH_TMR, 0) }}, //0.10
		{{ ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 0) }}, //0.11

		{{ ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 3) }}, //1.0
		{{ ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 3) }}, //1.1
		{{ ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 3) }}, //1.2
		{{ ENTRY(CORE_PERIPH_JTAG, 0), ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 3) }}, //1.3
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 3), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.4
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.5
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.6
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.7
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 1), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.8
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 1), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.9
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 1), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.10
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.11

		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_SPI, 1), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.0
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_SPI, 1), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.1
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_SPI, 1), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.2
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_SPI, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.3
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.4
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.5
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.6
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.7
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.8
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.9
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.10
		{{ ENTRY(CORE_PERIPH_PIO, 2), ENTRY(CORE_PERIPH_SPI, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //2.11

		{{ ENTRY(CORE_PERIPH_PIO, 3), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //3.0
		{{ ENTRY(CORE_PERIPH_PIO, 3), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //3.1
		{{ ENTRY(CORE_PERIPH_PIO, 3), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //3.2
		{{ ENTRY(CORE_PERIPH_PIO, 3), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //3.3
		{{ ENTRY(CORE_PERIPH_PIO, 3), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //3.4
		{{ ENTRY(CORE_PERIPH_PIO, 3), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //3.5
};



int _hwpl_core_set_pinsel_func(int gpio_port, int pin, core_periph_t function, int periph_port){
	int i;
	pinsel_table_t entry;
	int value;
	__IO uint32_t * regs_iocon;

	if( (gpio_port == 3) && (pin > 5) ){
		return -1;
	}

	value = gpio_port * 12 + pin;

	entry = pinsel_func_table[value];
	value = -1;
	for(i = 0; i < 4; i++){
		if ( (function == ENTRY_GET_FUNCTION(entry.entry[i])) &&
				(periph_port == ENTRY_GET_PORT(entry.entry[i])) ){
					//this is a valid pin
					value = i;
					break;
				}
	}

	if ( value < 0 ){
		return -1;
	}

	regs_iocon = _hwpl_get_iocon_regs(gpio_port, pin);
	*regs_iocon &= ~0x07;
	*regs_iocon |= value;
	return 0;
}