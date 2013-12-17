/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <fcntl.h>

#include "hwpl/types.h"
#include "hwpl/adc.h"
#include "dev/gpio.h"
#include "hwpl/debug.h"
#include "hwpl/core.h"

#include "gpio_flags.h"

struct pin_cfg {
	uint8_t port;
	uint8_t pin;
};


struct pin_cfg const pin_cfg_table00[HWPL_ADC_CHANNELS] = {
		{0, 0}, //PA0 = IN 0
		{0, 1}, //PA1 = IN 1
		{0, 2}, //PA2 = IN 2
		{0, 3}, //PA3 = IN 3
		{0, 4}, //PA4 = IN 4
		{0, 5}, //PA5 = IN 5
		{0, 6}, //PA6 = IN 6
		{0, 7}, //PA7 = IN 7
		{1, 0}, //PB0 = IN 8
		{1, 1} //PB1 = IN 9
#if HWPL_ADC_CHANNELS > 10
		,
		{2, 0}, //PC0 = IN 10
		{2, 1}, //PC0 = IN 11
		{2, 2}, //PC0 = IN 12
		{2, 3}, //PC0 = IN 13
		{2, 4}, //PC0 = IN 14
		{2, 5} //PC0 = IN 15
#endif
};


typedef struct {
	hwpl_callback_t callback;
	void * context;
	adc_sample_t * bufp;
	int len;
	uint8_t ref_count;
	uint8_t enabled_channels;
} adc_local_t;

static adc_local_t adc_local[HWPL_ADC_PORTS] HWPL_SYS_MEM;
static void adc_isr(int port);

ADC_TypeDef * const adc_regs_table[HWPL_ADC_PORTS] = HWPL_ADC_REGS;

//Register access functions
int _hwpl_adc_dev_port_is_invalid(int port){
	if ( port >= HWPL_ADC_PORTS ){
		return -1;
	}
	return 0;
}

void _hwpl_adc_dev_power_on(int port){
	ADC_TypeDef * regs = adc_regs_table[port];
	if ( adc_local[port].ref_count == 0 ){
		switch(port){
		case 0:
			RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
			regs->CR2 |= ADC_CR2_ADON;
			break;
		}

		NVIC_EnableIRQ(18);
		memset(&adc_local[port], 0, sizeof(adc_local_t));
	}
	adc_local[port].ref_count++;
}

void _hwpl_adc_dev_power_off(int port){
	ADC_TypeDef * regs = adc_regs_table[port];
	if ( adc_local[port].ref_count > 0 ){
		if ( adc_local[port].ref_count == 1 ){
			switch(port){
			case 0:
				RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;
				regs->CR2 &= ~ADC_CR2_ADON;
				break;
			}
			NVIC_DisableIRQ(18);
		}
		adc_local[port].ref_count--;
	}
}

int _hwpl_adc_dev_powered_on(int port){
	return (adc_local[port].ref_count != 0);
}

int _hwpl_adc_dev_read(const device_cfg_t * cfg, device_transfer_t * rop){
	int port;
	port = cfg->periph.port;
	ADC_TypeDef * regs = adc_regs_table[port];

	if ( (uint8_t)rop->loc >= HWPL_ADC_CHANNELS ){
		errno = EINVAL;
		return -1;
	}

	if ( regs->CR1 & ADC_CR1_EOCIE ){
		errno = EAGAIN;
		return -1;
	}

	adc_local[port].bufp = rop->buf;
	adc_local[port].len = rop->nbyte & ~(sizeof(adc_sample_t)-1);
	rop->nbyte = adc_local[port].len;
	adc_local[port].callback = rop->callback;
	adc_local[port].context = rop->context;

	//select the channel
	regs->SQR3 = rop->loc;

	//start the conversion
	regs->CR1 |= ADC_CR1_EOCIE;
	regs->CR2 |= ADC_CR2_ADON;
	return 0;
}

int hwpl_adc_getattr(int port, void * ctl){
	adc_attr_t * ctlp;
	uint8_t chan;
	uint16_t clk_div;
	ctlp = (adc_attr_t*)ctl;

	ctlp->pin_assign = 0; //GPIO config is always 0
	ctlp->enabled_channels = adc_local[port].enabled_channels;
	//clk_div = (LPC_ADC->ADCR >> 8) & 0xFF;
	ctlp->freq = _hwpl_core_getclock() / ((clk_div + 1) * 65); //calculate the truncated frequency value
	return 0;
}

int hwpl_adc_setattr(int port, void * ctl){
	adc_attr_t * ctlp;
	ctlp = (adc_attr_t*)ctl;
	uint16_t clk_div;
	int i;
	const struct pin_cfg * pin_cfg_table;

	if ( ctlp->freq == 0 ){
		errno = EINVAL;
		return -1;
	}

	if ( ctlp->freq > ADC_MAX_FREQ ){
		ctlp->freq = ADC_MAX_FREQ;
	}

	if ( ctlp->pin_assign != 0 ){
		errno = EINVAL;
		return -1;
	}

	if ( ctlp->enabled_channels & ~(0x3FF) ){
		errno = EINVAL;
		return -1;
	}

	//Calculate the clock setting
	clk_div = _hwpl_core_getclock()/(ctlp->freq * 65);
	if ( clk_div > 0 ){
		clk_div = clk_div - 1;
	}
	clk_div = clk_div > 255 ? 255 : clk_div;

	if( (port == 0) && (ctlp->pin_assign == 0) ){
		pin_cfg_table = pin_cfg_table00;
	} else {
		errno = EINVAL;
		return -1;
	}

	//Update the channels
	adc_local[port].enabled_channels = ctlp->enabled_channels;
	for(i=0; i < HWPL_ADC_CHANNELS; i++){
		if( ctlp->enabled_channels & (1<<i) ){
			if( pin_cfg_table[i].pin < 8 ){
				configure_gpio_low(hwpl_gpio_regs[port], pin_cfg_table[i].pin, MODE_CNF_INPUT_ANALOG);
			} else {
				configure_gpio_high(hwpl_gpio_regs[port], pin_cfg_table[i].pin, MODE_CNF_INPUT_ANALOG);
			}
		}
	}

	//Enable the pins to use the ADC and Disable pull-up/down resistors

	//LPC_ADC->ADCR = (1<<ADC_PDN)|(clk_div<<8); //Set the clock bits
	//LPC_ADC->ADINTEN = 0;
	return 0;
}

int hwpl_adc_setaction(int port, void * ctl){
	hwpl_action_t * action = (hwpl_action_t*)ctl;
	adc_local[port].callback = action->callback;
	adc_local[port].context = action->context;
	return 0;
}


//! \todo Should this use DMA instead of this interrupt?
void adc_isr(int port){
	uint16_t status;
	ADC_TypeDef * regs = adc_regs_table[port];

	if ( adc_local[port].len > 0 ){
		*adc_local[port].bufp++ = regs->DR;
		adc_local[port].len -= sizeof(adc_sample_t);
	}

	if ( adc_local[port].len > 0 ){
		regs->CR2 |= ADC_CR2_ADON;
	} else {
		adc_local[port].bufp = NULL;
		//Disable the interrupt
		regs->CR1 &= ~ADC_CR1_EOCIE;
		if ( adc_local[port].callback != NULL ){
			if ( adc_local[port].callback(adc_local[port].context, 0) == 0 ){
				adc_local[port].callback = NULL;
			}
		}
	}
}

void _hwpl_core_adc12_isr(void){
	//is this ADC 1 or 2?
#if HWPL_ADC_PORTS == 1
	adc_isr(0);
#else


#endif

}

