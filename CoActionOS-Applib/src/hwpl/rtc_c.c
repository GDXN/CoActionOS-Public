
#include <stdlib.h>
#include <hwpl/rtc.h>
#include "rtc.h"
#include "util.h"





int rtc_open(port_t port){
	return hwpl_rtc_open((const device_cfg_t*)&(port));
}

int rtc_close(port_t port){
	return hwpl_rtc_close((const device_cfg_t*)&(port));
}

int rtc_getattr(port_t port, rtc_attr_t * attr){
	return hwpl_rtc_getattr(port, attr);
}

int rtc_setattr(port_t port, rtc_attr_t * attr){
	return hwpl_rtc_setattr(port, attr);
}

int rtc_setaction(port_t port, rtc_action_t * action){
	return hwpl_rtc_setaction(port, action);
}

int rtc_setalarm(port_t port, rtc_alarm_t * alarm){
	return hwpl_rtc_setalarm(port, alarm);
}

int rtc_getalarm(port_t port, rtc_alarm_t * alarm){
	return hwpl_rtc_getalarm(port, alarm);
}

int rtc_disablealarm(port_t port){
	return hwpl_rtc_disablealarm(port, NULL);
}

int rtc_set(port_t port, rtc_time_t * time){
	return hwpl_rtc_set(port, time);
}

int rtc_get(port_t port, rtc_time_t * time){
	return hwpl_rtc_get(port, time);
}


