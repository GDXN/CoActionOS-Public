/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef TASK_FLAGS_H_
#define TASK_FLAGS_H_

#include "hwpl/arch.h"
#include "hwpl/types.h"
#include "hwpl/task.h"
#include "hwpl/mpu.h"

#define SYSTICK_CTRL_ENABLE (1<<0)
#define SYSTICK_CTRL_CLKSROUCE (1<<2)
#define SYSTICK_CTRL_COUNTFLAG (1<<16)


typedef struct {
	//uint32_t exc_return;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
} hw_stack_frame_t;

typedef struct {
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
} sw_stack_frame_t;


#define TASK_DEBUG 0

#define task_debug(...) do { if ( TASK_DEBUG == 1 ){ hwpl_debug("%s:", __func__); hwpl_debug(__VA_ARGS__); } } while(0)

extern int task_total HWPL_SYS_MEM;
extern task_t * task_table HWPL_SYS_MEM;
extern volatile int task_current HWPL_SYS_MEM;

typedef struct {
	int tid;
	int pid;
	int flags;
	struct _reent * reent;
	struct _reent * global_reent;
	task_memories_t * mem;
	void * stackaddr;
	uint32_t start;
	uint32_t stop;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
} new_task_t;

void task_priv_new_task(new_task_t * task);



#endif /* TASK_FLAGS_H_ */
