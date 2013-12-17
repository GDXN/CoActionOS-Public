/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



#include <string.h>
#include <errno.h>

#include "hwpl.h"
#include "hwpl/debug.h"
#include "hwpl/core.h"
#include "hwpl/task.h"
#include "task_flags.h"

#define SYSTICK_MIN_CYCLES 10000


int task_rr_reload HWPL_SYS_MEM;
int task_total HWPL_SYS_MEM;
task_t * task_table HWPL_SYS_MEM;
volatile int task_current HWPL_SYS_MEM;

static void task_context_switcher(void);

static void system_reset(void); //This is used if the OS process returns
void system_reset(void){
	hwpl_core_privcall(_hwpl_core_priv_reset, NULL);
}

int task_init(int interval,
		void (*scheduler_function)(void),
		void * system_memory,
		int system_memory_size){
	void * system_stack;
	hw_stack_frame_t * frame;
	int i;
	//Initialize the core system tick timer

	if ( task_table == NULL ){
		//The table must be allocated before the task manager is initialized
		return -1;
	}

	//Initialize the main process
	if ( system_memory == NULL ){
		system_memory = &_data;
	}

	system_stack = system_memory + system_memory_size;

	task_table[0].sp = system_stack - sizeof(hw_stack_frame_t);
	task_table[0].flags = TASK_FLAGS_EXEC | TASK_FLAGS_USED;
	task_table[0].pid = 0;
	task_table[0].reent = _impure_ptr;
	task_table[0].global_reent = _global_impure_ptr;

	frame = (hw_stack_frame_t *)task_table[0].sp;
	frame->r0 = 0;
	frame->r1 = 0;
	frame->r2 = 0;
	frame->r3 = 0;
	frame->r12 = 0;
	frame->pc = ((uint32_t)scheduler_function);
	frame->lr = (uint32_t)system_reset;
	frame->psr = 0x21000000; //default PSR value

	//enable use of PSP
	_hwpl_core_priv_set_thread_stack_ptr( (void*)task_table[0].sp );
	task_current = 0;

	//Set the interrupt priorities
	for(i=0; i <= DEV_LAST_IRQ; i++){
		NVIC_SetPriority( i, DEV_MIDDLE_PRIORITY ); //higher priority than the others
	}

	NVIC_SetPriority(SysTick_IRQn, DEV_MIDDLE_PRIORITY);
	NVIC_SetPriority(PendSV_IRQn, DEV_MIDDLE_PRIORITY);
	NVIC_SetPriority(SVCall_IRQn, DEV_MIDDLE_PRIORITY);

	//Turn on the task timer (MCU implementation dependent)
	task_set_interval(interval);
	task_table[0].rr_time = task_rr_reload;

	_hwpl_core_priv_set_stack_ptr( (void*)&_top_of_stack ); //reset the handler stack pointer
	core_tick_enable_irq();  //Enable context switching
	task_priv_switch_context(NULL);
	return 0;
}

int task_set_interval(int interval){
	uint32_t reload;
	int core_tick_freq;
	reload = (hwpl_core_cpu_freq * interval + 500) / 1000;
	if ( reload > (0x00FFFFFF) ){
		reload = (0x00FFFFFF);
	} else if ( reload < SYSTICK_MIN_CYCLES ){
		reload = SYSTICK_MIN_CYCLES;
	}
	core_tick_freq = hwpl_core_cpu_freq / reload;
	SysTick->LOAD = reload;
	task_rr_reload = reload;
	SysTick->CTRL = SYSTICK_CTRL_ENABLE| //enable the timer
			SYSTICK_CTRL_CLKSROUCE; //Internal Clock CPU
	SCB->CCR = 0;
	return core_tick_freq;
}

int task_new_thread(void *(*p)(void*),
		void (*cleanup)(void*),
		void * arg,
		void * mem_addr,
		int mem_size,
		int pid){
	int tid;
	int thread_zero;
	void * stackaddr;
	new_task_t task;

	thread_zero = task_get_thread_zero(pid);
	if ( thread_zero < 0 ){
		//The PID is not valid
		return 0;
	}

	//Variable initialization
	stackaddr = mem_addr + mem_size;
	//Check the stack alignment
	if ( (unsigned int)stackaddr & 0x03 ){
		return -1;
	}

	//Initialize the task
	task.stackaddr = stackaddr;
	task.start = (uint32_t)p;
	task.stop = (uint32_t)cleanup;
	task.r0 = (uint32_t)arg;
	task.r1 = 0;
	task.pid = pid;
	//task.mem_size = mem_size;
	task.flags = TASK_FLAGS_USED |
			TASK_FLAGS_PARENT( task_get_parent(thread_zero) ) |
			TASK_FLAGS_IS_THREAD;
	task.reent = (struct _reent *)mem_addr;
	task.global_reent = task_table[ thread_zero ].global_reent;
	task.mem = &(task_table[ thread_zero ].mem);

	//Do a priv call while accessing the task table so there are no interruptions
	hwpl_core_privcall( (core_privcall_t)task_priv_new_task, &task);
	tid = task.tid;

	return tid;
}

void task_priv_new_task(new_task_t * task){
	int i;
	hw_stack_frame_t * frame;

	for(i=1; i < task_get_total(); i++){
		if ( !task_used_asserted(i) ){
			//initialize the process stack pointer
			task_table[i].pid = task->pid;
			task_table[i].flags = task->flags;
			task_table[i].sp = task->stackaddr - sizeof(hw_stack_frame_t) - sizeof(sw_stack_frame_t);
			task_table[i].reent = task->reent;
			task_table[i].global_reent = task->global_reent;
			task_table[i].timer.t = 0;
			task_table[i].rr_time = task_rr_reload;
			memcpy(&(task_table[i].mem), task->mem, sizeof(task_memories_t));
			break;
		}
	}
	if ( i == task_get_total() ){
		task->tid = 0;
	} else {
		//Initialize the stack frame
		frame = (hw_stack_frame_t *)(task->stackaddr - sizeof(hw_stack_frame_t));
		frame->r0 = task->r0;
		frame->r1 = task->r1;
		frame->r2 = 0;
		frame->r3 = 0;
		frame->r12 = 0;
		frame->pc = task->start;
		frame->lr = task->stop;
		frame->psr = 0x21000000; //default PSR value
		task->tid = i;
	}
}


void task_priv_del(int id){
	if ( (id < task_get_total() ) && (id >= 1)){
		task_deassert_used(id);
		task_deassert_exec(id);
	}
}


void task_priv_resetstack(int id){
	//set the stack pointer to the original value
	task_table[id].sp = task_table[id].mem.data.addr + task_table[id].mem.data.size;
}

void * task_get_sbrk_stack_ptr(struct _reent * reent_ptr){
	int i;
	void * stackaddr;
	if ( task_table != NULL ){
		for(i=0; i < task_get_total(); i++){

			//If the reent and global reent are the same then this is the main thread
			if ( (task_table[i].reent == reent_ptr) && (task_table[i].global_reent == reent_ptr) ){

				//If the main thread is not in use, the stack is not valid
				if ( task_used_asserted(i) ){
					if ( (i == task_get_current()) ){
						//If the main thread is the current thread return the current stack
						hwpl_core_privcall(_hwpl_core_priv_get_thread_stack_ptr, &stackaddr);
						return stackaddr;
					} else {
						//Return the stack value from thread 0 if another thread is running
						return (void*)task_table[i].sp;
					}
				} else {
					//The main thread is not in use, so there is no valid stack value
					return NULL;
				}
			}
		}
	}

	//No task table (or not matching reent structure) so no valid stack value
	return NULL;
}

int task_get_thread_zero(int pid){
	int i;
	for(i=0; i < task_get_total(); i++){
		if ( task_used_asserted(i) ){
			if( pid == task_get_pid(i) && !task_isthread_asserted(i) ){
				return i;
			}
		}
	}
	return -1;
}

static void priv_task_tmr_rd(uint32_t * val){
	*val = task_rr_reload - SysTick->VAL;
}


uint64_t task_priv_gettime(int tid){
	uint32_t val;
	if ( tid != task_get_current() ){
		return task_table[tid].timer.t + (task_rr_reload - task_table[tid].rr_time);
	} else {
		priv_task_tmr_rd(&val);
		return task_table[tid].timer.t + val;
	}
}


uint64_t task_gettime(int tid){
	uint32_t val;
	if ( tid != task_get_current() ){
		return task_table[tid].timer.t + (task_rr_reload - task_table[tid].rr_time);
	} else {
		hwpl_core_privcall((core_privcall_t)priv_task_tmr_rd, &val);
		return task_table[tid].timer.t + val;
	}
}

/*! \details This function changes to another process.  It is executed
 * during the core systick timer and pend SV interrupts.
 */
void task_context_switcher(void){
	int i;
	asm volatile ("MRS %0, psp\n\t" : "=r" (task_table[task_current].sp) );

	//disable task specific MPU regions
	MPU->RBAR = 0x16;
	MPU->RASR = 0;
	MPU->RBAR = 0x17;
	MPU->RASR = 0;
	MPU->RBAR = 0x12;
	MPU->RASR = 0;

	_hwpl_core_priv_disable_interrupts(NULL);

	do {
		task_current++;
		if ( task_current == task_get_total() ){
			//The scheduler only uses OS mem -- disable the process MPU regions
			task_current = 0;
			if( task_table[0].rr_time < SYSTICK_MIN_CYCLES ){
				task_table[0].rr_time = task_rr_reload;
				task_table[0].timer.t += (task_rr_reload);
			}

			//see if all tasks have used up their RR time
			for(i=1; i < task_get_total(); i++){
				if ( task_exec_asserted(i) && (task_table[i].rr_time >= SYSTICK_MIN_CYCLES) ){
					break;
				}
			}

			//if all executing tasks have used up their RR time -- reload the RR time for executing tasks
			if ( i == task_get_total() ){
				for(i=1; i < task_get_total(); i++){
					if ( task_exec_asserted(i) ){
						task_table[i].timer.t += (task_rr_reload - task_table[i].rr_time);
						task_table[i].rr_time = task_rr_reload;
					}
				}
			}

			break;
		} else if ( task_exec_asserted(task_current) ){
			if ( (task_table[task_current].rr_time >= SYSTICK_MIN_CYCLES) || //is there time remaining on the RR
					task_isfifo_asserted(task_current) ){ //is this a FIFO task
				//Enable the MPU for the process code section
				MPU->RBAR = (uint32_t)(task_table[task_current].mem.code.addr);
				MPU->RASR = (uint32_t)(task_table[task_current].mem.code.size);

				//Enable the MPU for the process data section
				MPU->RBAR = (uint32_t)(task_table[task_current].mem.data.addr);
				MPU->RASR = (uint32_t)(task_table[task_current].mem.data.size);
				break;
			}
		}
	} while(1);

	//Enable the MPU for the task stack guard
	MPU->RBAR = (uint32_t)(task_table[task_current].mem.stackguard.addr);
	MPU->RASR = (uint32_t)(task_table[task_current].mem.stackguard.size);

	_hwpl_core_priv_enable_interrupts(NULL);

	_impure_ptr = task_table[task_current].reent;
	_global_impure_ptr = task_table[task_current].global_reent;

	if ( task_isfifo_asserted(task_current) ){
		//disable the systick interrupt (because this is a fifo task)
		core_tick_disable_irq();
	} else {
		//init sys tick to the amount of time remaining
		SysTick->LOAD = task_table[task_current].rr_time;
		SysTick->VAL = 0; //force a reload
		//enable the systick interrupt
		core_tick_enable_irq();
	}

	asm volatile ("MSR psp, %0\n\t" : : "r" (task_table[task_current].sp) );
}

void task_priv_switch_context(void * args){
	task_table[task_get_current()].rr_time = SysTick->VAL; //save the RR time from the SYSTICK
	SCB->ICSR |= (1<<28);
}

void _task_check_countflag(void){
	if ( SysTick->CTRL & (1<<16) ){ //check the countflag
		task_table[task_current].rr_time = 0;
		task_context_switcher();
	}
}

void _hwpl_core_systick_handler(void) HWPL_NAKED;
void _hwpl_core_systick_handler(void){
	//The value 0xFFFFFFFD is pushed
	asm("mvn.w r0, #2\n\t");
	asm volatile ("push {r0}\n\t");
	task_save_context();
	_task_check_countflag();
	task_load_context();
	asm volatile ("pop {pc}\n\t"); //The value 0xFFFFFFFD is popped to do a thread stack return
}

void _hwpl_core_pendsv_handler(void) HWPL_NAKED;
void _hwpl_core_pendsv_handler(void){
	//The value 0xFFFFFFFD is pushed
	asm("mvn.w r0, #2\n\t");
	asm volatile ("push {r0}\n\t");
	task_save_context();
	task_context_switcher();
	task_load_context();
	asm volatile ("pop {pc}\n\t"); //The value 0xFFFFFFFD is popped to do a thread stack return
}

static void priv_task_restore(void * args) HWPL_NAKED;
static void priv_task_restore(void * args){
	asm volatile ("push {lr}\n\t");
	uint32_t pstack;

	//discard the current HW stack by adjusting the PSP up by sizeof(hw_stack_frame_t) --sw_stack_frame_t is same size
	pstack = __get_PSP();

	//the extra 4 bytes are added by the handler if using the old ABI (arm-cm3-elf)
	__set_PSP(pstack + 0 + sizeof(hw_stack_frame_t));

	//Load the software context that is on the stack from the pre-interrupted task
	task_load_context();
	//This function will now return to the original execution stack
	asm volatile ("pop {pc}\n\t");
}

void task_restore(void){
	//handlers inserted with task_interrupt() must call this function when the task completes in order to restore the stack
	hwpl_core_privcall(priv_task_restore, NULL);
}

int task_priv_interrupt_call(void * args){
	uint32_t pstack;
	task_interrupt_t * intr = (task_interrupt_t*)args;
	hw_stack_frame_t * hw_frame;

	if ( task_enabled(intr->tid) ){
		if ( intr->tid == task_get_current() ){
			pstack = __get_PSP();
			__set_PSP(pstack - sizeof(hw_stack_frame_t));
			hw_frame = (hw_stack_frame_t *)__get_PSP(); //frame now points to the new HW stack
		} else {
			//Since this is another task, the current PSP is not touched
			hw_frame = (hw_stack_frame_t *)(task_table[intr->tid].sp - sizeof(hw_stack_frame_t));
			task_table[intr->tid].sp = task_table[intr->tid].sp - (sizeof(hw_stack_frame_t) + sizeof(sw_stack_frame_t));
		}

		hw_frame->r0 = intr->arg[0];
		hw_frame->r1 = intr->arg[1];
		hw_frame->r2 = intr->arg[2];
		hw_frame->r3 = intr->arg[3];
		hw_frame->r12 = 0;
		hw_frame->pc = (uint32_t)intr->handler;
		hw_frame->lr = (uint32_t)task_restore;
		hw_frame->psr = 0x21000000; //default PSR value
	}

	if ( intr->sync_callback != NULL ){
		intr->sync_callback(intr->sync_callback_arg);
	}

	if ( intr->tid != task_get_current() ){
		//this task is not the currently executing task
		return 1;
	}
	return 0;
}

uint32_t task_interrupt_stacksize(){
	return sizeof(hw_stack_frame_t) + sizeof(sw_stack_frame_t);
}


void task_priv_interrupt(void * args){
	task_save_context(); //save the current software context
	//This function inserts the handler on the specified task's stack

	if( task_priv_interrupt_call(args) ){
		task_load_context(); //the context needs to be loaded for the current task -- otherwise it is loaded by the switcher
	}
}

int task_interrupt(task_interrupt_t * intr){
	if ( intr->tid < task_get_total() ){
		hwpl_core_privcall(task_priv_interrupt, intr);
		return 0;
	}
	return -1;
}











