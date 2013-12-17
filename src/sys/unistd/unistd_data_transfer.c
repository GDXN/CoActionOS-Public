/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNISTD
 * @{
 */

/*! \file */

#include <reent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>

#include "hwpl.h"
#include "unistd_fs.h"
#include "unistd_flags.h"
#include "../sched/sched_flags.h"

typedef struct {
	const sysfs_t * fs;
	void * handle;
	device_transfer_t op;
	int read;
	int ret;
} priv_device_data_transfer_t;

#if SINGLE_TASK != 0
volatile bool waiting;
#endif



static void priv_device_data_transfer(void * args);
static int priv_data_transfer_callback(void * context, const void * data);
static int device_data_transfer(open_file_t * open_file, void * buf, int nbyte, int read);
static int get_mode(const sysfs_t* fs, void * handle);

int priv_data_transfer_callback(void * context, const void * data){
	//activate all tasks that are blocked on this signal
#if SINGLE_TASK == 0
	int i;
	int new_priority;
	priv_device_data_transfer_t * args = (priv_device_data_transfer_t*)context;

	if( data == DEVICE_OP_CANCELLED ){
		args->op.nbyte = -1; //ignore any data transferred and return an error
	}

	new_priority = -1;
	if ( (uint32_t)args->op.tid < task_get_total() ){
		if ( sched_inuse_asserted(args->op.tid) ){ //check to see if the process terminated before arriving here
			new_priority = sched_table[args->op.tid].priority;
		}
	} else {
		hwpl_priv_debug("tid0x%X\n", args->op.tid); //an op completed after a task expired?
	}

	//check to see if any tasks are waiting for this device
	for(i = 1; i < task_get_total(); i++){
		if ( task_enabled(i) && sched_inuse_asserted(i) ){
			if ( sched_table[i].block_object == (args->handle + args->read) ){
				sched_priv_assert_active(i, SCHED_UNBLOCK_TRANSFER);
				if ( sched_table[i].priority > new_priority ){
					new_priority = sched_table[i].priority;
				}
			}
		}
	}
	sched_priv_update_on_wake(new_priority);

#else
	waiting = false;
#endif
	return 0;
}


void priv_device_data_transfer(void * args){
	priv_device_data_transfer_t * p = (priv_device_data_transfer_t*)args;
	_hwpl_core_priv_disable_interrupts(NULL); //no switching until the transfer is started

	if ( p->read != 0 ){
		//Read operation
		p->ret = p->fs->priv_read(p->fs->cfg, p->handle, &p->op);
	} else {
		p->ret = p->fs->priv_write(p->fs->cfg, p->handle, &p->op);
	}

	if ( p->ret == 0 ){
		if ( (p->op.nbyte >= 0) //wait for the operation to complete or for data to arrive
		){
#if SINGLE_TASK == 0
			//Block waiting for the operation to complete or new data to be ready
			sched_table[ task_get_current() ].block_object = (void*)p->handle + p->read;
			//switch tasks until a signal becomes available
			sched_priv_update_on_sleep();
#else
			waiting = true;
#endif
		}
	}

	_hwpl_core_priv_enable_interrupts(NULL);
}


int unistd_device_read(open_file_t * open_file, void * buf, int nbyte){
	return device_data_transfer(open_file, buf, nbyte, 1);
}

int unistd_device_write(open_file_t * open_file, const void * buf, int nbyte){
	return device_data_transfer(open_file, (void*)buf, nbyte, 0);
}

int device_data_transfer(open_file_t * open_file, void * buf, int nbyte, int read){
	int tmp;
	int mode;
	priv_device_data_transfer_t args;
	hwpl_action_t action;

	if ( nbyte == 0 ){
		return 0;
	}

	args.fs = (const sysfs_t*)open_file->fs;
	args.handle = (device_t *)open_file->handle;
	args.read = read;
	args.op.loc = open_file->loc;
	args.op.flags = open_file->flags;
	args.op.buf = buf;
	args.op.callback = priv_data_transfer_callback;
	args.op.context = &args;
#if SINGLE_TASK == 0
	args.op.tid = task_get_current();
#else
	args.op.tid = 0;
#endif

	if ( (mode = get_mode(args.fs, args.handle)) < 0 ){
		return -1;
	}

	//privilege call for the operation
	do {

#if SINGLE_TASK > 0
		waiting = false;
#endif
		args.op.nbyte = nbyte;
		args.ret = -101010;

		//This transfers the data
		hwpl_core_privcall(priv_device_data_transfer, &args);
		//We arrive here if the data is done transferring or there is no data to transfer and O_NONBLOCK is set
		//or if there was an error
		if( sched_get_unblock_type(task_get_current()) == SCHED_UNBLOCK_SIGNAL ){

			//This needs to make sure the device is freed -- ie no longer waiting for this thread
			action.callback = 0;
			unistd_device_ioctl(open_file, I_GLOBAL_SETACTION, &action);

			errno = EINTR;
			return -1;
		}

#if SINGLE_TASK != 0
		while ( waiting == true ){
			core_sleep(CORE_SLEEP);
		}
#endif
		if ( args.ret > 0 ){
			//The operation happened synchronously
			tmp = args.ret;
			break;
		} else if ( args.ret == 0 ){
			//the operation happened asynchronously
			if ( args.op.nbyte > 0 ){
				//The operation has completed and transferred args.op.nbyte bytes
				tmp = args.op.nbyte;
				break;
			} else if ( args.op.nbyte == 0 ){
				//There was no data to read/write -- try again
				if (args.op.flags & O_NONBLOCK ){
					errno = ENODATA;
					return -1;
				}
			} else if ( args.op.nbyte < 0 ){
				//there was an error executing the operation (or the operation was cancelled)
				return -1;
			}
		} else if ( args.ret < 0 ){
			//there was an error starting the operation (such as EAGAIN)
			if( args.ret == -101010 ){
				errno = ENXIO; //this is a rare/strange error where hwpl_core_privcall fails to run properly
			}
			return -1;
		}
	} while ( args.ret == 0 );


	if ( ((mode & S_IFMT) != S_IFCHR) && (tmp > 0) ){
		open_file->loc += tmp;
	}

	return tmp;
}

int get_mode(const sysfs_t* fs, void * handle){
	struct stat st;
	if ( fs->fstat(fs->cfg, handle, &st) < 0){
		return -1;
	}
	return st.st_mode;
}

/*! @} */


