/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include "hwpl/adc.h"
#include "hwpl/core.h"
#include "hwpl/pio.h"
#include "hwpl/debug.h"

#define ADC_PDN 21
#define ADC_START 24
#define ADC_DONE 31

typedef struct {
	hwpl_callback_t callback;
	void * context;
	adc_sample_t * bufp;
	int len;
	uint8_t ref_count;
	uint8_t enabled_channels;
} adc_local_t;

static adc_local_t adc_local HWPL_SYS_MEM;

static void exec_callback(void * data);

static void enable_pin(int pio_port, int pio_pin, int adc_port){
	pio_attr_t pattr;
	pattr.mask = (1<<pio_pin);
	pattr.mode = PIO_MODE_INPUT | PIO_MODE_FLOAT;
	hwpl_pio_setattr(pio_port, &pattr);
	_hwpl_core_set_pinsel_func(pio_port,pio_pin,CORE_PERIPH_ADC,adc_port);
}


void _hwpl_adc_dev_power_on(int port){

	if ( adc_local.ref_count == 0 ){
		_hwpl_lpc_core_enable_pwr(PCADC);
		_hwpl_core_priv_enable_irq((void*)ADC_IRQn);
		LPC_ADC->INTEN = 0;
		memset(&adc_local, 0, sizeof(adc_local_t));
	}

	adc_local.ref_count++;

}

void _hwpl_adc_dev_power_off(int port){
	if ( adc_local.ref_count > 0 ){
		if ( adc_local.ref_count == 1 ){
			LPC_ADC->CR = 0; //reset the control
			_hwpl_core_priv_disable_irq((void*)ADC_IRQn);
			_hwpl_lpc_core_disable_pwr(PCADC);
		}
		adc_local.ref_count--;
	}
}

int _hwpl_adc_dev_powered_on(int port){
	return (adc_local.ref_count != 0);
}

int _hwpl_adc_dev_read(const device_cfg_t * cfg, device_transfer_t * rop){
	if ( (uint8_t)rop->loc > 7 ){
		errno = EINVAL;
		return -1;
	}

	if ( LPC_ADC->INTEN & 0xFF ){
		//The interrupt is on -- port is busy
		errno = EAGAIN;
		return -1;
	}

	adc_local.callback = rop->callback;
	adc_local.context = rop->context;
	adc_local.bufp = rop->buf;
	adc_local.len = rop->nbyte & ~(sizeof(adc_sample_t)-1);
	rop->nbyte = adc_local.len;
	LPC_ADC->INTEN = (1<<8);
	LPC_ADC->CR |= ((1<<ADC_START)|(1<<rop->loc)); //start the first conversion



	return 0;
}

void exec_callback(void * data){
	adc_local.bufp = NULL;
	//Disable the interrupt
	LPC_ADC->INTEN = 0;
	LPC_ADC->CR &= ((1<<ADC_PDN)|(0xFF00)); //leave the clock div bits in place and PDN
	if ( adc_local.callback != NULL ){
		if ( adc_local.callback(adc_local.context, data) == 0 ){
			adc_local.callback = NULL;
		}
	}
}

//! \todo Should this use DMA instead of this interrupt?
void _hwpl_core_adc_isr(void){
	if ( adc_local.len > 0 ){
		*adc_local.bufp++ = (adc_sample_t)LPC_ADC->GDR;
		adc_local.len = adc_local.len - sizeof(adc_sample_t);
	}

	if ( adc_local.len > 0 ){
		LPC_ADC->CR |= (1<<ADC_START); //set the start bit
	} else {
		exec_callback(0);
	}
}


int hwpl_adc_getattr(int port, void * ctl){
	adc_attr_t * ctlp;
	uint16_t clk_div;
	ctlp = (adc_attr_t*)ctl;

	ctlp->pin_assign = 0; //Pin assign is always 0
	ctlp->enabled_channels = adc_local.enabled_channels;
	clk_div = (LPC_ADC->CR >> 8) & 0xFF;
	ctlp->freq = _hwpl_core_getclock() / ((clk_div + 1) * 65); //calculate the truncated frequency value
	return 0;
}

int hwpl_adc_setattr(int port, void * ctl){
	adc_attr_t * ctlp;
	ctlp = (adc_attr_t*)ctl;
	uint16_t clk_div;

	if ( ctlp->freq == 0 ){
		errno = EINVAL;
		return -1 - offsetof(adc_attr_t, freq);
	}

	if ( ctlp->freq > ADC_MAX_FREQ ){
		ctlp->freq = ADC_MAX_FREQ;
	}

	if ( ctlp->pin_assign != 0 ){
		errno = EINVAL;
		return -1 - offsetof(adc_attr_t, pin_assign);
	}

	if ( ctlp->enabled_channels & ~(0xFF) ){
		errno = EINVAL;
		return -1 - offsetof(adc_attr_t, enabled_channels);
	}

	//Calculate the clock setting
	clk_div = _hwpl_core_getclock()/(ctlp->freq * 65);
	if ( clk_div > 0 ){
		clk_div = clk_div - 1;
	}
	clk_div = clk_div > 255 ? 255 : clk_div;

	//Update the channels
	adc_local.enabled_channels = ctlp->enabled_channels;

	//Enable the pins to use the ADC and Disable pull-up/down resistors
	if ( adc_local.enabled_channels & (1<<0) ){
		enable_pin(0, 23, 0);
	}

	if ( adc_local.enabled_channels & (1<<1) ){
		enable_pin(0, 24, 0);
	}

	if ( adc_local.enabled_channels & (1<<2) ){
		enable_pin(0, 25, 0);
	}

	if ( adc_local.enabled_channels & (1<<3) ){
		enable_pin(0, 26, 0);
	}

	if ( adc_local.enabled_channels & (1<<4) ){
		enable_pin(1, 30, 0);
	}

	if ( adc_local.enabled_channels & (1<<5) ){
		enable_pin(1, 31, 0);
	}

	if ( adc_local.enabled_channels & (1<<6) ){
		enable_pin(0, HWPL_ADC_CHANNEL6_PIN, 0);
	}

	if ( adc_local.enabled_channels & (1<<7) ){
		enable_pin(0, HWPL_ADC_CHANNEL7_PIN, 0);
	}

	LPC_ADC->CR = (1<<ADC_PDN)|(clk_div<<8); //Set the clock bits
	LPC_ADC->INTEN = 0;
	return 0;
}

int hwpl_adc_setaction(int port, void * ctl){
	hwpl_action_t * action = (hwpl_action_t*)ctl;
	if( action->callback == 0 ){
		if ( LPC_ADC->INTEN & 0xFF ){
			exec_callback(DEVICE_OP_CANCELLED);
		}
	}
	adc_local.callback = action->callback;
	adc_local.context = action->context;
	return 0;
}

