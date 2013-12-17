/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */
#include <errno.h>
#include "hwpl/rtc.h"
#include "hwpl/debug.h"
#include "hwpl/core.h"

#define CLKEN (1<<0)
#define CTCRST (1<<1)
#define CCALEN (1<<4)

#define RTCCIF (1<<0)
#define RTCALF (1<<1)

#define AMRSEC (1<<0)
#define AMRMIN (1<<1)
#define AMRHOUR (1<<2)
#define AMRDOM (1<<3)
#define AMRDOW (1<<4)
#define AMRDOY (1<<5)
#define AMRMON (1<<6)
#define AMRYEAR (1<<7)

#define RTC_ISDST_REG (LPC_RTC->GPREG0)

typedef struct {
	hwpl_callback_t callback;
	void * context;
	uint8_t ref_count;
} rtc_local_t;
static rtc_local_t rtc_local HWPL_SYS_MEM;


void _hwpl_rtc_dev_power_on(int port){
	if ( rtc_local.ref_count == 0 ){
		rtc_local.callback = NULL;
		LPC_RTC->CCR = (CLKEN);
	}
	rtc_local.ref_count++;
}

void _hwpl_rtc_dev_power_off(int port){
	if ( rtc_local.ref_count > 0 ){
		if ( rtc_local.ref_count == 1 ){
			hwpl_priv_debug("RTC OFF\n");
			//core_priv_disable_irq((void*)RTC_IRQn);
			//LPC_RTC->CCR = 0;
		}
		rtc_local.ref_count--;
	}
}

int _hwpl_rtc_dev_powered_on(int port){
	if ( LPC_RTC->CCR & (CLKEN) ){
		return 1;
	} else {
		return 0;
	}
}


int hwpl_rtc_getattr(int port, void * ctl){
	rtc_attr_t * ctlp;
	ctlp = (rtc_attr_t *)ctl;
	ctlp->pin_assign = 0;
	return 0;
}

int hwpl_rtc_setattr(int port, void * ctl){
	rtc_attr_t * ctlp;
	ctlp = (rtc_attr_t *)ctl;
	if ( ctlp->pin_assign != 0 ){
		errno = EINVAL;
		return -1 - offsetof(rtc_attr_t, pin_assign);
	}
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
	rtc_alarm_t * alarmp;

	alarmp = (rtc_alarm_t *)ctl;
	_hwpl_core_priv_enable_irq((void*)RTC_IRQn);

	LPC_RTC->ALSEC = alarmp->time.time.tm_sec;
	LPC_RTC->ALMIN = alarmp->time.time.tm_min;
	LPC_RTC->ALHOUR = alarmp->time.time.tm_hour;
	LPC_RTC->ALDOM = alarmp->time.time.tm_mday;
	LPC_RTC->ALDOW = alarmp->time.time.tm_wday;
	LPC_RTC->ALDOY = alarmp->time.time.tm_yday + 1;
	LPC_RTC->ALMON = alarmp->time.time.tm_mon + 1;
	LPC_RTC->ALYEAR = alarmp->time.time.tm_year;

	switch(alarmp->type){
	case RTC_ALARM_ONCE:
		LPC_RTC->AMR = AMRDOY|AMRDOW; //don't compare the day of the year or day of week
		break;
	case RTC_ALARM_MINUTE:
		LPC_RTC->AMR = (~(AMRSEC)) & 0xFF; //only compare seconds
		break;
	case RTC_ALARM_HOURLY:
		LPC_RTC->AMR = (~(AMRSEC|AMRMIN))  & 0xFF;
		break;
	case RTC_ALARM_DAILY:
		LPC_RTC->AMR =  (~(AMRSEC|AMRMIN|AMRHOUR))  & 0xFF;
		break;
	case RTC_ALARM_WEEKLY:
		LPC_RTC->AMR = (~(AMRSEC|AMRMIN|AMRHOUR|AMRDOW)) & 0xFF;
		break;
	case RTC_ALARM_MONTHLY:
		LPC_RTC->AMR = (~(AMRSEC|AMRMIN|AMRHOUR|AMRDOM)) & 0xFF;
		break;
	case RTC_ALARM_YEARLY:
		LPC_RTC->AMR = (~(AMRSEC|AMRMIN|AMRHOUR|AMRDOM|AMRMON)) & 0xFF;
		break;
	}
	return 0;
}

int hwpl_rtc_getalarm(int port, void * ctl){
	rtc_alarm_t * alarmp;
	alarmp = (rtc_alarm_t *)ctl;
	alarmp->time.time.tm_sec = LPC_RTC->ALSEC;
	alarmp->time.time.tm_min = LPC_RTC->ALMIN;
	alarmp->time.time.tm_hour = LPC_RTC->ALHOUR;
	alarmp->time.time.tm_mday = LPC_RTC->ALDOM;
	alarmp->time.time.tm_wday = LPC_RTC->ALDOW;
	alarmp->time.time.tm_yday = LPC_RTC->ALDOY - 1;
	alarmp->time.time.tm_mon = LPC_RTC->ALMON - 1;
	alarmp->time.time.tm_year = LPC_RTC->ALYEAR;
	return 0;
}

int hwpl_rtc_disablealarm(int port, void * ctl){
	LPC_RTC->AMR = 0xFF;
	return 0;
}

int hwpl_rtc_set(int port, void * ctl){
	rtc_time_t * timep;
	timep = (rtc_time_t*)ctl;
	LPC_RTC->SEC = timep->time.tm_sec;
	LPC_RTC->MIN = timep->time.tm_min;
	LPC_RTC->HOUR = timep->time.tm_hour;
	LPC_RTC->DOW = timep->time.tm_wday;
	LPC_RTC->DOM = timep->time.tm_mday;
	LPC_RTC->DOY = timep->time.tm_yday + 1;
	LPC_RTC->MONTH = timep->time.tm_mon + 1;
	LPC_RTC->YEAR = timep->time.tm_year;
	RTC_ISDST_REG = timep->time.tm_isdst;
	return 0;
}

int hwpl_rtc_get(int port, void * ctl){
	rtc_time_t * timep;
	timep = (rtc_time_t*)ctl;
	timep->time.tm_sec = LPC_RTC->SEC;
	timep->time.tm_min = LPC_RTC->MIN;
	timep->time.tm_hour = LPC_RTC->HOUR;
	timep->time.tm_wday = LPC_RTC->DOW;
	timep->time.tm_mday = LPC_RTC->DOM;
	timep->time.tm_yday = LPC_RTC->DOY - 1;
	timep->time.tm_mon = LPC_RTC->MONTH - 1;
	timep->time.tm_year = LPC_RTC->YEAR;
	timep->time.tm_isdst = RTC_ISDST_REG;
	timep->useconds = 0;
	return 0;
}

int hwpl_rtc_setcountevent(int port, void * ctl){
	rtc_event_count_t event = (rtc_event_count_t)ctl;
	_hwpl_core_priv_enable_irq((void*)RTC_IRQn);
	switch(event){
	case RTC_EVENT_COUNT_NONE:
		LPC_RTC->CIIR = 0;
		break;
	case RTC_EVENT_COUNT_SECOND:
		LPC_RTC->CIIR = 1;
		break;
	case RTC_EVENT_COUNT_MINUTE:
		LPC_RTC->CIIR = 2;
		break;
	case RTC_EVENT_COUNT_HOUR:
		LPC_RTC->CIIR = 4;
		break;
	case RTC_EVENT_COUNT_DAY:
		LPC_RTC->CIIR = 8;
		break;
	case RTC_EVENT_COUNT_WEEK:
		LPC_RTC->CIIR = 16;
		break;
	case RTC_EVENT_COUNT_MONTH:
		LPC_RTC->CIIR = 64;
		break;
	case RTC_EVENT_COUNT_YEAR:
		LPC_RTC->CIIR = 128;
		break;
	default:
		errno = EINVAL;
		return -1;
	}
	return 0;
}

void _hwpl_core_rtc_isr(void){
	rtc_event_t event;
	int flags;
	flags = LPC_RTC->ILR & 0x03;
	LPC_RTC->ILR = flags; //clear the flags
	event = 0;
	if ( flags & RTCALF ){
		event |= (1<<RTC_EVENT_ALARM);
	}
	if ( flags & RTCCIF ){
		event |= (1<<RTC_EVENT_COUNT);
	}

	if ( rtc_local.callback != NULL ){
		if( rtc_local.callback(rtc_local.context, (const void*)event) == 0 ){
			rtc_local.callback = NULL;
		}
	}
}





