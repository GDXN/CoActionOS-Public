/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SCHED
 * @{
 *
 */

/*! \file */

#include "sched_flags.h"

void sched_priv_assert_active(int id, int unblock_type){
	sched_table[id].flags |= (1<< SCHED_TASK_FLAGS_ACTIVE);
	sched_priv_set_unblock_type(id, unblock_type);
	sched_priv_deassert_aiosuspend(id);
	//Remove all blocks (mutex, timing, etc)
	sched_table[id].block_object = NULL;
	sched_table[id].wake.tv_sec = SCHED_TIMEVAL_SEC_INVALID;
	sched_table[id].wake.tv_usec = 0;
}

void sched_priv_deassert_active(int id){
	sched_table[id].flags &= ~(1<< SCHED_TASK_FLAGS_ACTIVE);
	task_deassert_exec(id);  //stop executing the task
	sched_priv_assert_status_change(); //notify the scheduler a task has changed status
}

void sched_priv_assert_status_change(void){
	sched_status_changed = 1;
}

inline void sched_priv_deassert_status_change(void){
	sched_status_changed = 0;
}

void sched_priv_stop_task(int id){
	sched_priv_deassert_active(id);
}

void sched_priv_start_task(int id){
	sched_priv_assert_active(id, SCHED_UNBLOCK_SIGNAL);
}

void sched_priv_assert(int id, int flag){
	sched_table[id].flags |= (1<<flag);

}
void sched_priv_deassert(int id, int flag){
	sched_table[id].flags &= ~(1<<flag);
}




/*! @} */
