/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



/*! \addtogroup AIO Asynchronous IO (AIO)
 * @{
 *
 * \ingroup POSIX
 *
 */

/*! \file */

#include "hwpl/debug.h"

#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "aio.h"
#include "../unistd/unistd_flags.h"
#include "../sched/sched_flags.h"
#include "../signal/sig_local.h"

typedef struct {
	struct aiocb *const * list;
	int nent;
	bool block_on_all;
	struct sched_timeval abs_timeout;
	int ret;
	struct sigevent * event;
} priv_suspend_t;

static void priv_suspend(void * args);

typedef struct {
	const device_t * device;
	struct aiocb * aiocbp;
	int read;
	int ret;
} priv_aio_transfer_t;

static int aio_data_transfer(struct aiocb * aiocbp);

/*! \details This function is not supported this version.
 *
 * \return -1 with errno set to ENOTSUP
 */
int aio_cancel(int fildes /*! the file descriptor */,
		struct aiocb * aiocbp /*! a pointer to the AIO data struture */){

	//this needs a special ioctl request to cancel current operations

	errno = ENOTSUP;
	return -1;
}

/*! \details This function gets the error value for \a aiocbp.
 *
 * \return The error value for the operation or zero for no errors:
 *  - EINPROGRESS:  The operation is still in progress
 *  - Other interface specific error
 */
int aio_error(const struct aiocb * aiocbp /*! a pointer to the AIO data struture */){
	if ( aiocbp->op.buf != NULL ){
		return EINPROGRESS;
	} else {
		return aiocbp->op.nbyte; //this is where the error value is stored in case of failure
	}
}

/*! \details This function is not supported this version.
 * \return -1 with errno set to ENOTSUP
 */
int aio_fsync(int op, struct aiocb * aiocbp){
	//all data is already synced
	errno = ENOTSUP;
	return -1;
}

/*! \details The function initiates an asynchronous read using the data specified by \a aiocbp.
 *
 * \return 0 on success or -1 with errno (see \ref ERRNO) set to:
 *  - EIO:  failed to start the transfer
 */
int aio_read(struct aiocb * aiocbp /*! a pointer to the AIO data struture */){
	aiocbp->aio_lio_opcode = LIO_READ;
	return aio_data_transfer(aiocbp);
}

/*! \details The function initiates an asynchronous write using the data specified by \a aiocbp.
 *
 * \return 0 on success or -1 with errno (see \ref ERRNO) set to:
 *  - EIO:  failed to start the transfer
 */
int aio_write(struct aiocb * aiocbp /*! a pointer to the AIO data struture */){
	aiocbp->aio_lio_opcode = LIO_WRITE;
	return aio_data_transfer(aiocbp);
}

/*! \details This function returns the number of bytes read/written for the asynchronous operation.
 *
 * \return The number of bytes transferred or -1 if there was an error
 * (or if the operation is not yet complete).  \ref aio_error() can be used to determine the error.
 */
ssize_t aio_return(struct aiocb * aiocbp /*! a pointer to the AIO data struture */){
	if ( aiocbp->op.buf != NULL ){
		return -1;
	} else {
		return aiocbp->aio_nbytes; //this is the number of bytes that was read or written
	}
}

void priv_suspend(void * args){
	int i;
	bool suspend;
	priv_suspend_t * p = (priv_suspend_t*)args;
	//disable interrupts
	//! \todo See if an operation is in progress
	if ( p->block_on_all == false ){
		suspend = true;
	} else {
		suspend = false;
	}
	_hwpl_core_priv_disable_interrupts(NULL); //no switching until the transfer is started
	for(i = 0; i < p->nent; i++ ){
		if (p->list[i] != NULL ){

			//first check to see if we block on aio suspend (if anything is complete don't block)
			if ( (p->list[i]->op.buf == NULL) && (p->block_on_all == false) ){ //if op.buf is NULL the operation is complete
				suspend = false;
				break;
			}

			//now check to see if we block on listio suspend (if anything is incomplete block)
			if ( (p->list[i]->op.buf != NULL) && (p->block_on_all == true) ){
				suspend = true;
			}

		}
	}

	if ( suspend == true ){
		sched_priv_assert_aiosuspend(task_get_current());
		sched_priv_timedblock(args, &p->abs_timeout);
	} else {
		p->nent = -1;
	}

	//enable interrupts
	_hwpl_core_priv_enable_interrupts(NULL);

}


static int suspend(struct aiocb *const list[], int nent, const struct timespec * timeout, bool block_on_all){
	priv_suspend_t args;

	//suspend until an AIO operation completes or until timeout is exceeded
	args.list = list;
	args.nent = nent;
	args.block_on_all = block_on_all; //only block on one or block on all
	sched_convert_timespec(&args.abs_timeout, timeout);
	hwpl_core_privcall(priv_suspend, &args);

	if( args.nent == -1 ){
		return 0; //one of the AIO's in the list has already completed
	}

	//Check the unblock type
	if ( sched_get_unblock_type( task_get_current() ) == SCHED_UNBLOCK_SLEEP ){
		errno = EAGAIN;
		return -1;
	} else if ( sched_get_unblock_type( task_get_current() ) == SCHED_UNBLOCK_SIGNAL ){
		//check to see if a signal interrupted the sleeping period
		errno = EINTR;
		return -1;
	}

	return args.ret;
}

/*! \details This function will suspend the currently executing thread until an AIO operation
 * in \a list completes or until the \a timeout value is surpassed.  If \a timeout is NULL, it is
 * ignored.
 *
 * \return 0 on success or -1 with errno (see \ref ERRNO) set to:
 *  - EAGAIN:  \a timeout was exceeded before any operations completed.
 *  - EINTR:  the thread received a signal before any operations completed.
 */
int aio_suspend(struct aiocb *const list[] /*! a list of AIO transfer structures */,
		int nent /*! the number of transfer in \a list */,
		const struct timespec * timeout /*! the absolute timeout value */){
	return suspend(list, nent, timeout, false);
}

/*! \details This function initiates a list of asynchronous transfers.
 *
 * \note Asynchronous notification is not supported in this version.  \a ENOTSUP
 * is returned if an attempt is made to invoke an asynchronous notification.
 *
 * \note This function is experimental in this version.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - EINTR:  the thread received an signal before completed all transactions
 * - ENOTSUP: \a mode is LIO_NOWAIT and \a sig is not null or sigev_notify is not set to SIGEV_NONE
 * - EINVAL:  \a mode is not set to LIO_NOWAIT or LIO_WAIT
 */
int lio_listio(int mode /*! The mode:  \a LIO_WAIT or \a LIO_NOWAIT */,
		struct aiocb * const list[] /*! The list of AIO transfers */,
		int nent /*! The number of transfers in \a list */,
		struct sigevent * sig /*! The sigevent structure (use NULL in this version) */){
	int i;

	switch(mode){
	case LIO_NOWAIT:
		if ( sig != NULL ){
			if ( sig->sigev_notify != SIGEV_NONE ){
				errno = ENOTSUP;
				return -1;
			}
		}
	case LIO_WAIT:
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	for(i=0; i < nent; i++){
		//start all of the operations in the list
		if( list[i] != NULL ){  //ignore NULL entries
			aio_data_transfer(list[i]);
		}
	}

	if ( mode == LIO_WAIT ){
		return suspend(list, nent, NULL, true);
	}

	return 0;
}

static int aio_data_transfer_callback(struct aiocb * aiocbp, const void * ignore){
	unsigned int tid;
	int i;
	bool wakeup;
	priv_suspend_t * p;
	aiocbp->aio_nbytes = aiocbp->op.nbyte;
	aiocbp->op.buf = NULL;
	aiocbp->op.nbyte = 0;

	//Check to see if the thread is suspended on aio -- the block object is a list of aiocb -- check if aiocbp is in list
	tid = aiocbp->op.tid;
	if( tid >= task_get_total() ){
		//This is not a valid task id
		return 0;
	}

	if( task_enabled(tid) ){ //if task is no longer enabled (don't do anything)
		if ( sched_aiosuspend_asserted(tid) ){
			p = (priv_suspend_t*)sched_table[tid].block_object;

			if ( p->block_on_all == false ){
				for(i=0; i < p->nent; i++){
					if ( aiocbp == p->list[i] ){ //If this is true the thread is blocked on the operation that is currently completing
						sched_priv_assert_active(tid, SCHED_UNBLOCK_AIO);
						sched_priv_update_on_wake(sched_table[tid].priority);
						break;
					}
				}
			} else {
				wakeup = true;
				for(i=0; i < p->nent; i++){
					if ( p->list[i]->op.buf != NULL ){ //operation is not complete
						//don't wakeup because this operation is not complete
						wakeup = false;
					}
				}

				if( wakeup == true ){
					sched_priv_assert_active(tid, SCHED_UNBLOCK_AIO);
					sched_priv_update_on_wake(sched_table[tid].priority);
				}
			}
		}

		//Need to send an asynchronous notification if a lio_listio call was made -- no limit to number of lio_listio calls
		if( aiocbp->aio_sigevent.sigev_notify == SIGEV_SIGNAL ){
			//send a signal
			signal_priv_send(0,
					tid,
					aiocbp->aio_sigevent.sigev_signo,
					SI_ASYNCIO,
					aiocbp->aio_sigevent.sigev_value.sival_int,
					1);
		}

		//This needs to check if all operations in a list have complete and then use SIGEV_NONE, SIGEV_SIGNAL, or SIGEV_THREAD to notify
		return 0;
	}

	return 0;
}

static void priv_device_data_transfer(void * args){
	priv_aio_transfer_t * p = (priv_aio_transfer_t*)args;

	_hwpl_core_priv_disable_interrupts(NULL); //no switching until the transfer is started
	//set the device callback for the read/write op
	if ( p->read == true ){
		//Read operation
		p->ret = p->device->driver.read(&p->device->cfg, &p->aiocbp->op);
	} else {
		p->ret = p->device->driver.write(&p->device->cfg, &p->aiocbp->op);
	}

	sched_table[task_get_current()].block_object = NULL;

	_hwpl_core_priv_enable_interrupts(NULL);

	if ( p->ret == 0 ){
		if( p->aiocbp->op.nbyte > 0 ){
			//AIO is in progress
		}
	} else if ( p->ret < 0 ){
		//AIO was not started -- set the errno
		errno = EIO;
		p->ret = -1;
	} else if ( p->ret > 0 ){
		//The transfer happened synchronously -- call the callback manually
		aio_data_transfer_callback(p->aiocbp, 0);
		p->ret = 0;
	}
}

int aio_data_transfer(struct aiocb * aiocbp){
	priv_aio_transfer_t args;
	open_file_t * open_file;

	//! \todo make sure the fildes is valid

	open_file = get_open_file(aiocbp->aio_fildes);
	args.device = (device_t *)open_file->handle;
	args.aiocbp = aiocbp;
	if ( aiocbp->aio_lio_opcode == LIO_READ ){
		args.read = true;
	} else {
		args.read = false;
	}
	args.aiocbp->op.loc = aiocbp->aio_offset;
	args.aiocbp->op.flags = 0; //this is never a blocking call
	args.aiocbp->op.nbyte = aiocbp->aio_nbytes;
	args.aiocbp->op.buf = (void*)aiocbp->aio_buf;
	args.aiocbp->op.callback = (hwpl_callback_t)aio_data_transfer_callback;
	args.aiocbp->op.tid = task_get_current();
	args.aiocbp->op.context = aiocbp;
	args.aiocbp->aio_nbytes = -1; //means status is in progress
	hwpl_core_privcall(priv_device_data_transfer, &args);
	return args.ret;
}

/*! @} */

