/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup CAOS
 * @{
 */

/*! \file */

#include "config.h"
#include <errno.h>
#include "dev/sys.h"
#include "hwpl/sys.h"
#include "hwpl/debug.h"
#include "sched/sched_flags.h"
#include "signal/sig_local.h"
#include "hwdl/sys.h"
#include "symbols.h"

extern const char _stdin_dev[];
extern const char _stdout_dev[];

extern const int caoslib_system_memory_size;

static int read_task(sys_taskattr_t * task);

uint8_t sys_euid HWPL_SYS_MEM;

int sys_open(const device_cfg_t * cfg){
	return 0;
}

int sys_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	sys_attr_t * sys = ctl;
	sys_killattr_t * killattr = ctl;


	int i;
	switch(request){
	case  I_SYS_GETATTR:
		strncpy(sys->version, VERSION, 7);
		sys->version[7] = 0;
		strncpy(sys->arch, ARCH, 7);
		sys->arch[7] = 0;
		sys->security = _hwpl_sys_getsecurity();
		sys->signature = symbols_table[0];
		sys->cpu_freq = _hwpl_core_getclock();
		sys->sys_mem_size = caoslib_system_memory_size;
		strncpy(sys->stdin_name, _stdin_dev, NAME_MAX-1);
		strncpy(sys->stdout_name, _stdout_dev, NAME_MAX-1);
		return 0;
	case I_SYS_GETTASK:
		return read_task(ctl);
	case I_SYS_KILL:
		for(i = 1; i < task_get_total(); i++){
			if( (task_get_pid(i) == killattr->id) &&
					!task_isthread_asserted(i)
			){
				signal_priv_send(task_get_current(),
						i,
						killattr->si_signo,
						killattr->si_sigcode,
						killattr->si_sigvalue, 1);
				break;
			}
		}
		return 0;
	case I_SYS_PTHREADKILL:
		return signal_priv_send(task_get_current(),
				killattr->id,
				killattr->si_signo,
				killattr->si_sigcode,
				killattr->si_sigvalue, 1);
	}
	errno = EINVAL;
	return -1;
}

int sys_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}

int sys_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int sys_close(const device_cfg_t * cfg){
	return 0;
}

int read_task(sys_taskattr_t * task){
	int ret;
	if ( task->tid < task_get_total() ){
		//hwpl_priv_debug("task:%d 0x%X\n", task->tid, task);
		if ( task_enabled( task->tid )){
			task->is_enabled = 1;
			task->pid = task_get_pid( task->tid );
			task->timer = task_priv_gettime(task->tid);
			task->mem_loc = (uint32_t)sched_table[task->tid].attr.stackaddr;
			task->mem_size = sched_table[task->tid].attr.stacksize;
			task->stack_ptr = (uint32_t)task_table[task->tid].sp;
			task->prio = sched_table[task->tid].priority;
			task->prio_ceiling = sched_table[task->tid].attr.schedparam.sched_priority;
			task->is_active = sched_active_asserted(task->tid);
			task->is_thread = task_isthread_asserted( task->tid );

			strncpy(task->name, ((struct _reent*)task_table[ task->tid ].global_reent)->procmem_base->proc_name, NAME_MAX);

			if ( !task->is_thread && ( task_table[task->tid].reent != NULL) ){
				task->malloc_loc = (uint32_t)&((struct _reent*)task_table[task->tid].reent)->procmem_base
						+ ((struct _reent*)task_table[task->tid].reent)->procmem_base->size;
			} else {
				task->malloc_loc = 0;
			}


			ret = 1;

		} else {
			task->is_enabled = 0;
			ret = 0;
		}
		errno = 0;
	} else {
		errno = ESRCH;
		ret = -1;
	}

	return ret;
}


/*! @} */
