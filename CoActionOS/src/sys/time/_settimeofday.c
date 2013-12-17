/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup TIME
 * @{
 */

/*! \file */


#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "hwpl.h"
#include "dev/rtc.h"

#include "../sched/sched_flags.h"

struct timeval time_of_day_offset HWPL_SYS_MEM;

static void priv_set_time(void * args){
	div_t d;
	struct sched_timeval tv;
	struct timeval tmp;
	struct timeval * t = (struct timeval *)args;
	sched_priv_get_realtime(&tv);

	//time = value + offset
	//offset = time - value
	d = div(tv.tv_usec, 1000000);
	tmp.tv_sec = tv.tv_sec * SCHED_TIMEVAL_SECONDS + d.quot;
	tmp.tv_usec = d.rem;
	time_of_day_offset.tv_usec = t->tv_usec - tmp.tv_usec;
	time_of_day_offset.tv_sec = t->tv_sec - tmp.tv_sec;
	if( time_of_day_offset.tv_usec < 0 ){
		time_of_day_offset.tv_usec += 1000000;
		time_of_day_offset.tv_sec--;
	}
}

/*! \details This function sets the current time of day to the
 * time stored in \a tp.  The timezone (\a tzp) is ignored.
 *
 * \return Zero or -1 with errno (see \ref ERRNO) set to:
 * - EIO: IO error when setting the real time clock
 *
 */
int settimeofday(const struct timeval * tp, const struct timezone * tzp);

static int settimeofday_rtc(const struct timeval * tp);

int _settimeofday(const struct timeval * tp, const struct timezone * tzp) {
	if ( settimeofday_rtc(tp) < 0 ){
		//otherwise, use the simulated version
		hwpl_core_privcall(priv_set_time, (void*)tp);
	}

	return 0;
}

int settimeofday_rtc(const struct timeval * tp){
	int fd;
	rtc_time_t cal_time;

	fd = open("/dev/rtc", O_RDWR);
	if ( fd < 0 ){
		return -1;
	}

	gmtime_r( &tp->tv_sec, (struct tm*)&cal_time.time);
	cal_time.useconds = tp->tv_usec;

	if (ioctl(fd, I_RTC_SET, &cal_time) < 0 ){
		close(fd);
		return -1;
	}

	return 0;
}


/*! @} */
