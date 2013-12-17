/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwpl.h"
#include "hwpl/core.h"
#include "task_flags.h"

int task_new_process(void (*p)(int,char*const*),
		void (*cleanup)(void*),
		int argc,
		char *const argv[],
		task_memories_t * mem,
		void * reent_ptr){

	int tid;
	int err;
	void * stackaddr;
	new_task_t task;
	task_memories_t task_memories;
	static int task_process_counter = 1;

	//Variable initialization
	stackaddr = mem->data.addr + mem->data.size;

	//Check the stack alignment
	if ( (unsigned int)stackaddr & 0x03 ){
		errno = EIO;
		return -1;
	}

	//Initialize the task
	task.stackaddr = stackaddr;
	task.start = (uint32_t)p;
	task.stop = (uint32_t)cleanup;
	task.r0 = (uint32_t)argc;
	task.r1 = (uint32_t)argv;

	task.pid = task_process_counter++; //Assign a new pid
	//task.mem_size = mem_size;
	task.reent = (struct _reent*)reent_ptr;
	task.global_reent = task.reent;

	task.flags = TASK_FLAGS_USED |
			TASK_FLAGS_PARENT(task_current);

	memcpy(&task_memories, mem, sizeof(task_memories_t));

	if ( (err = task_mpu_calc_protection(&task_memories)) < 0 ){
		return err;
	}
	task.mem = &task_memories;

	//Do a priv call while accessing the task table so there are no interruptions
	hwpl_core_privcall( (core_privcall_t)task_priv_new_task, &task);
	tid = task.tid;
	return tid;
}
