
#include <stdlib.h>
#include <hwpl/rtc.h>
#include "Rtc.hpp"
#include "util.h"


Rtc::Rtc(port_t port) : Periph(port, HWPL_RTC_PORTS, NULL, NULL){}

int Rtc::open(int flags){
	return hwpl_rtc_open((const device_cfg_t*)&(port_));
}

int Rtc::close(){
	return hwpl_rtc_close((const device_cfg_t*)&(port_));
}

int Rtc::getattr(rtc_attr_t * attr){
	return hwpl_rtc_getattr(port_, attr);
}

int Rtc::setattr(rtc_attr_t * attr){
	return hwpl_rtc_setattr(port_, attr);
}

int Rtc::setaction(rtc_action_t * action){
	return hwpl_rtc_setaction(port_, action);
}

int Rtc::setalarm(rtc_alarm_t * alarm){
	return hwpl_rtc_setalarm(port_, alarm);
}

int Rtc::getalarm(rtc_alarm_t * alarm){
	return hwpl_rtc_getalarm(port_, alarm);
}

int Rtc::disablealarm(void){
	return hwpl_rtc_disablealarm(port_, NULL);
}

int Rtc::set(rtc_time_t * time){
	return hwpl_rtc_set(port_, time);
}

int Rtc::get(rtc_time_t * time){
	return hwpl_rtc_get(port_, time);
}


