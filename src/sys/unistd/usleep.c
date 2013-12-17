/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_SLEEP
 * @{
 */

/*! \file */


#include <unistd.h>
#include <time.h>
#include <errno.h>

#include "hwpl/debug.h"
#include "../sched/sched_flags.h"

static void priv_usleep(void * args);

/*! \details This function causes the calling thread to sleep for \a useconds microseconds.
 *
 * \return 0 or -1 for an error with errno (see \ref ERRNO) set to:
 * - EINVAL: \a useconds is greater than 1 million.
 */
int usleep(useconds_t useconds){
	uint32_t clocks;
	uint32_t tmp;
	int i;
	if ( useconds == 0 ){
		return 0;
	}
	if ( useconds <= 1000000UL ){
		clocks =  sched_useconds_to_clocks(useconds);
		tmp = sched_useconds_to_clocks(1);
		if( (task_get_current() == 0) || (clocks < 8000) ){

			for(i = 0; i < clocks; i+=14){
				asm volatile("nop");
				asm volatile("nop");
				asm volatile("nop");
				asm volatile("nop");
				asm volatile("nop");
				asm volatile("nop");
				asm volatile("nop");
				asm volatile("nop");
				asm volatile("nop");
				asm volatile("nop");
			}

		} else {
			//clocks is greater than 4800 -- there is time to change to another task
			useconds -= (600 / tmp);
			hwpl_core_privcall(priv_usleep, &useconds);
		}
	} else {
		errno = EINVAL;
		return -1;
	}
	return 0;
}

void priv_usleep(void * args){
	useconds_t * p;
	struct sched_timeval abs_time;
	p = (useconds_t*)args;
	sched_priv_get_realtime(&abs_time);
	abs_time.tv_usec = abs_time.tv_usec + *p;

	if ( abs_time.tv_usec > SCHED_TIMEVAL_SECONDS*1000000UL ){
		abs_time.tv_sec++;
		abs_time.tv_usec -= SCHED_TIMEVAL_SECONDS*1000000UL;
	}

	sched_priv_timedblock(NULL, &abs_time);
}

/*! @} */
