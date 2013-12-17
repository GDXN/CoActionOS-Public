/*
 * c_rtc.c
 *
 *  Created on: Sep 9, 2012
 *      Author: tgil
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "rtc.h"
#include "util.h"

#define NUM_PORTS HWPL_SPI_PORTS
#define PERIPH_NAME "rtc"

static int fd[HWPL_RTC_PORTS];
static bool isinitialized = false;

int rtc_open(port_t port){
	return phylib_util_open(PERIPH_NAME, port, fd, NUM_PORTS, &isinitialized);
}

int rtc_close(port_t port){
	return phylib_util_close(port, fd);
}

int rtc_getattr(port_t port, rtc_attr_t * attr){
	return ioctl(fd[port], I_RTC_GETATTR, attr);
}

int rtc_setattr(port_t port, rtc_attr_t * attr){
	return ioctl(fd[port], I_RTC_SETATTR, attr);
}

int rtc_setaction(port_t port, rtc_action_t * action){
	return ioctl(fd[port], I_RTC_SETACTION, action);
}

int rtc_setalarm(port_t port, rtc_alarm_t * alarm){
	return ioctl(fd[port], I_RTC_SETALARM, alarm);
}

int rtc_getalarm(port_t port, rtc_alarm_t * alarm){
	return ioctl(fd[port], I_RTC_GETALARM, alarm);
}

int rtc_disablealarm(port_t port){
	return ioctl(fd[port], I_RTC_DISABLEALARM);
}

int rtc_set(port_t port, rtc_time_t * time){
	return ioctl(fd[port], I_RTC_SET, time);
}

int rtc_get(port_t port, rtc_time_t * time){
	return ioctl(fd[port], I_RTC_GET, time);
}

