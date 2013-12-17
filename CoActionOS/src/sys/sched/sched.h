/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SCHED Scheduler
 * @{
 *
 * \ingroup MULTITASK
 *
 * \details This software module is the CAOS scheduler.  It manages processes in multi-threaded
 * applications.  The basic approach is to execute the highest priority tasks in a round robin
 * fashion.  A task with a lower priority will only be executed if all higher priority tasks
 * are in the idle state.  Hardware interrupts have priority over all tasks (as well as any
 * callbacks which have been attached to those interrupts).
 *
 * The following is a checklist for things that must be done before calling the scheduler() function:
 *  - Enable the RTC -- if RTC is not enabled sleep() won't work properly
 *  - Call sched_hwcfg_init(): enables core processes and microsecond timer
 *  - Enable interrupts
 *  - Call sched_init()
 *  - Call scheduler()
 *
 *
 */


/*! \file */
#ifndef SCHED_H_
#define SCHED_H_

//#include "config.h"

#include <pthread.h>
#include "hwpl.h"

#ifdef __cplusplus
extern "C" {
#endif

int sched_start(void * (*init)(void*), int priority);
int sched_init(void);

//Schedule Debugging
void sched_debug_print_tasks(void);
void sched_debug_print_active(void);
void sched_debug_print_executing(void);
void sched_debug_print_timers(void);

void sched_set_fault(int pid, int tid, fault_t * fault);

#ifdef __cplusplus
}
#endif

#endif /* SCHED_H_ */

/*! @} */
