/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "link_flags.h"


int link_settime(link_phy_t handle, struct tm * t){
	int fd;
	int ret;
	struct link_tm ltm;

	ltm.tm_hour = t->tm_hour;
	ltm.tm_isdst = t->tm_isdst;
	ltm.tm_mday = t->tm_mday;
	ltm.tm_min = t->tm_min;
	ltm.tm_mon = t->tm_mon;
	ltm.tm_sec = t->tm_sec;
	ltm.tm_wday = t->tm_wday;
	ltm.tm_yday = t->tm_yday;
	ltm.tm_year = t->tm_year;

	link_debug(6, "open rtc device\n");
	fd = link_open(handle, "/dev/rtc", LINK_O_RDWR);
	if( fd < 0 ){
		return -1;
	}

	link_debug(6, "write time\n");
	ret = link_ioctl(handle,
			fd,
			I_RTC_SET,
			&ltm);

	link_debug(6, "close\n");
	if( link_close(handle, fd) < 0 ){
		return -1;
	}

	return ret;
}


int link_gettime(link_phy_t handle, struct tm * t){
	int fd;
	int ret;
	struct link_tm ltm;

	link_debug(6, "Open RTC fildes\n");
	fd = link_open(handle, "/dev/rtc", LINK_O_RDWR);
	if( fd < 0 ){
		return -1;
	}

	ret = link_ioctl(handle,
			fd,
			I_RTC_GET,
			&ltm);
	if( ret < 0 ){
		link_error("Failed to I_RTC_GET\n");
		return -1;
	}

	link_debug(6, "Close RTC fildes\n");
	if( link_close(handle, fd) < 0 ){
		link_error("failed to close\n");
		return -1;
	}

	link_debug(6, "Translate time\n");
	if( ret == 0 ){
		t->tm_hour = ltm.tm_hour;
		t->tm_isdst = ltm.tm_isdst;
		t->tm_mday = ltm.tm_mday;
		t->tm_min = ltm.tm_min;
		t->tm_mon = ltm.tm_mon;
		t->tm_sec = ltm.tm_sec;
		t->tm_wday = ltm.tm_wday;
		t->tm_yday = ltm.tm_yday;
		t->tm_year = ltm.tm_year;
	}

	link_debug(5, "Done with translate\n");

	return ret;
}






/*! @} */
