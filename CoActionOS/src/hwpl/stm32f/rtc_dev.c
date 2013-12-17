/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */
#include <errno.h>
#include "hwpl/rtc.h"
#include "hwpl/debug.h"
#include "hwpl/core.h"

typedef struct {
	hwpl_callback_t callback;
	void * context;
	uint8_t ref_count;
} rtc_local_t;
static rtc_local_t rtc_local HWPL_SYS_MEM;

static void _hwpl_rtc_enable_write(void);
static void _hwpl_rtc_disable_write(void);


void _hwpl_rtc_dev_power_on(int port){
	if ( rtc_local.ref_count == 0 ){
		rtc_local.callback = NULL;
		//LPC_RTC->CCR = (CLKEN);
		RCC->APB1ENR |= (RCC_APB1ENR_BKPEN|RCC_APB1ENR_PWREN);
	}
	rtc_local.ref_count++;
}

void _hwpl_rtc_dev_power_off(int port){
	if ( rtc_local.ref_count > 0 ){
		if ( rtc_local.ref_count == 1 ){
			//core_priv_disable_irq((void*)RTC_IRQn);
			//LPC_RTC->CCR = 0;
			RCC->APB1ENR &= ~(RCC_APB1ENR_BKPEN|RCC_APB1ENR_PWREN);

		}
		rtc_local.ref_count--;
	}
}

int _hwpl_rtc_dev_powered_on(int port){
	return (RCC->BDCR & RCC_BDCR_RTCEN) != 0;
	/*
	if ( LPC_RTC->CCR & (CLKEN) ){
		return 1;
	} else {
		return 0;
	}
	*/
}


int hwpl_rtc_getattr(int port, void * ctl){
	rtc_attr_t * ctlp;
	ctlp = (rtc_attr_t *)ctl;
	ctlp->pin_assign = 0;
	return 0;
}

int hwpl_rtc_setattr(int port, void * ctl){
	rtc_attr_t * attr;
	attr = (rtc_attr_t *)ctl;
	uint32_t fdiv;

	_hwpl_rtc_enable_write();
	RCC->BDCR &= ~(RCC_BDCR_RTCSEL|RCC_BDCR_RTCEN); //clear clock selection bits and disable RTC


	if( attr->clock == RTC_CLKSRC_INTERNAL_40000 ){
		RCC->CSR |= RCC_CSR_LSION; //turn the LSI on
		while( (RCC->CSR & RCC_CSR_LSIRDY) == 0 ){
			//wait for LSI to be ready to use
		}
		RCC->BDCR |= RCC_BDCR_RTCSEL_LSI; //set RTC to use LSI

		//set the RTC divisor
		fdiv = attr->freq - 1;
		RTC->PRLH = (fdiv >> 16) & 0xFFFF;
		RTC->PRLL = (fdiv & 0xFFFF);
	}

	RCC->BDCR |= RCC_BDCR_RTCEN; //enable the RTC
	_hwpl_rtc_disable_write();

	return 0;
}

int hwpl_rtc_setaction(int port, void * ctl){
	hwpl_action_t * action = (hwpl_action_t*)ctl;
	rtc_local.callback = action->callback;
	rtc_local.context = action->context;
	//Set the event
	return hwpl_rtc_setcountevent(port, (void*)action->event);
}

int _hwpl_rtc_dev_read(const device_cfg_t * cfg, device_transfer_t * rop){
	rtc_local.callback = rop->callback;
	rtc_local.context = rop->context;
	return 0;
}

int hwpl_rtc_setalarm(int port, void * ctl){
	rtc_alarm_t * t = ctl;
	uint32_t tmp = t->time.time_t;
	_hwpl_rtc_enable_write();
	RTC->ALRL = tmp & 0xFFFF;
	RTC->ALRH = (tmp >> 16) & 0xFFFF;
	RTC->CRH |= RTC_CRH_ALRIE;
	_hwpl_rtc_disable_write();

	_hwpl_rtc_enable_write();
	_hwpl_rtc_disable_write();

	NVIC_EnableIRQ(RTC_IRQn);


	EXTI->IMR |= EXTI_IMR_MR17;
	EXTI->RTSR |= EXTI_RTSR_TR17;
	//EXTI->FTSR |= EXTI_FTSR_TR17;
	NVIC_EnableIRQ(RTCAlarm_IRQn);

	return 0;
}

int hwpl_rtc_getalarm(int port, void * ctl){
	rtc_alarm_t * t = ctl;
	memset(t, 0, sizeof(rtc_time_t));
	t->time.time_t = RTC->ALRL + (RTC->ALRH << 16);
	t->time.use_time_t = 1;
	return 0;
}

int hwpl_rtc_disablealarm(int port, void * ctl){
	//LPC_RTC->AMR = 0xFF;
	return 0;
}

int hwpl_rtc_set(int port, void * ctl){
	rtc_time_t * t = ctl;
	uint32_t tmp = t->time_t;
	_hwpl_rtc_enable_write();
	RTC->CNTL = tmp & 0xFFFF;
	RTC->CNTH = (tmp >> 16) & 0xFFFF;
	_hwpl_rtc_disable_write();

	//set CNT requires another cycle to finalize
	_hwpl_rtc_enable_write();
	_hwpl_rtc_disable_write();

	return 0;
}

int hwpl_rtc_get(int port, void * ctl){
	rtc_time_t * t = ctl;
	memset(t, 0, sizeof(rtc_time_t));
	t->time_t = RTC->CNTL + (RTC->CNTH << 16);
	t->use_time_t = 1;
	return 0;
}


int hwpl_rtc_setcountevent(int port, void * ctl){
	int ret;
	rtc_event_count_t event = (rtc_event_count_t)ctl;
	ret = 0;
	NVIC_EnableIRQ(RTC_IRQn);
	_hwpl_rtc_enable_write();
	switch(event){
	case RTC_EVENT_COUNT_NONE:
		RTC->CRH &= ~RTC_CRH_SECIE;
		break;
	case RTC_EVENT_COUNT_SECOND:
		RTC->CRH |= RTC_CRH_SECIE;
		break;
	default:
		errno = EINVAL;
		ret = -1;
		break;
	}
	_hwpl_rtc_disable_write();
	return ret;
}

void _hwpl_core_rtc_isr(void){
	rtc_event_t event;
	int flags;


	flags = RTC->CRL;
	event = 0;
	if ( flags & RTC_CRL_ALRF ){
		event |= (1<<RTC_EVENT_ALARM);
		RTC->CRL &= ~(RTC_CRL_ALRF);
	}
	if ( flags & RTC_CRL_SECF ){
		event |= (1<<RTC_EVENT_COUNT);
		RTC->CRL &= ~(RTC_CRL_SECF);
	}


	if ( rtc_local.callback != NULL ){
		if( rtc_local.callback(rtc_local.context, (const void*)event) == 0 ){
			rtc_local.callback = NULL;
		}
	}
}

void _hwpl_core_rtcalarm_isr(void){
	EXTI->PR |= EXTI_PR_PR17;
}


void _hwpl_rtc_enable_write(void){
	PWR->CR |= (PWR_CR_DBP); //enable write access to backup domain
	while( (RTC->CRL & RTC_CRL_RTOFF) == 0){
		;
	}
	RTC->CRL |= RTC_CRL_CNF;
}

void _hwpl_rtc_disable_write(void){
	RTC->CRL &= ~RTC_CRL_CNF;
	while( (RTC->CRL & RTC_CRL_RTOFF) == 0){
		break;
	}
	PWR->CR &= ~(PWR_CR_DBP); //enable write protection
}




