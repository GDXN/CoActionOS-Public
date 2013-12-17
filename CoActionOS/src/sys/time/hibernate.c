/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup TIME
 * @{
 */

/*! \file */


#include "config.h"
#include <errno.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "hwpl.h"
#include "hwpl/wdt.h"
#include "dev/rtc.h"

#include "../sched/sched_flags.h"

extern const int microcomputer_osc_freq;
extern const int cpu_init_freq;

static int set_alarm(int seconds);
static void priv_powerdown(void * args);
static void priv_hibernate(void * args);

void priv_powerdown(void * args){
	hwpl_core_sleep(0, (void*)CORE_DEEPSLEEP_STANDBY);
}

void priv_hibernate(void * args){

	//The WDT only runs in hibernate on certain clock sources
	hwpl_wdt_priv_reset(NULL);

	hwpl_core_sleep(0, (void*)CORE_DEEPSLEEP_STOP);

	//reinitialize the Clocks
	_hwpl_core_setclock(cpu_init_freq, microcomputer_osc_freq); //Set the main clock
	_hwpl_core_setusbclock(microcomputer_osc_freq); //set the USB clock

	//Set WDT to previous value (it only runs in deep sleep with certain clock sources)
	hwpl_wdt_priv_reset(NULL);
}

int set_alarm(int seconds){
	int fd;
	rtc_alarm_t alarm;
	time_t alarm_time;
	int ret;

	fd = open("/dev/rtc", O_RDWR);
	if ( fd >= 0 ){
		alarm_time = time(NULL);
		alarm_time += seconds;
		gmtime_r(&alarm_time, (struct tm*)&alarm.time.time);
		alarm.type = RTC_ALARM_ONCE;

		//set the alarm for "seconds" from now
		ret = ioctl(fd, I_RTC_SETALARM, &alarm);
		close(fd);
		return ret;
	}
	return -1;
}

int hibernate(int seconds){
	bool deepsleep;
	int ret = -1;
	deepsleep = false;
	if ( seconds > 0 ){
		if (set_alarm(seconds) == 0 ){
			deepsleep = true;
		}
	} else if ( seconds == 0 ){
		deepsleep = true;
	}


	if( deepsleep ){
		hwpl_core_privcall(priv_hibernate, NULL);
	}
	return ret;
}


void powerdown(int seconds){
	bool deepsleep;
	deepsleep = false;
	if ( seconds > 0 ){
		if (set_alarm(seconds) == 0 ){
			deepsleep = true;
		}
	} else if ( seconds == 0 ){
		deepsleep = true;
	}

	if ( deepsleep ){
		hwpl_core_privcall(priv_powerdown, NULL);
	}
}

/*! @} */
