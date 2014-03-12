/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup MQUEUE
 * @{
 *
 * \details This interface allows processes to share data by sending and receiving messages.
 *
 * Here is an example of how to create a new message queue and put a message in the new queue:
 *
 * \code
 * #include <mqueue.h>
 * #include <stdio.h>
 *
 * void create_queue_and_send_msg(void){
 * 	mqd_t mdes;
 * 	struct mq_attr attr;
 * 	char msg[32];
 *
 * 	attr.mq_maxmsg = 4;
 * 	attr.mq_msgsize = 32;
 *
 * 	mdes = mq_open("/path/to/queue", O_CREAT|O_EXCL|O_RDWR, 0666, &attr);
 * 	if ( mdes == (mqd_t)-1 ){
 * 		perror("failed to create queue");
 * 		return;
 * 	}
 *
 * 	//now send a message in the queue
 * 	strcpy(msg, "this is the message");
 * 	if ( mq_send(mdes, msg, strlen(msg), 0) < 0 ){
 * 		perror("failed to send message");
 * 	}
 *
 * }
 *
 * \endcode
 *
 * Another process can read the message in the queue my using the following code:
 *
 * \code
 * #include <mqueue.h>
 * #include <stdio.h>
 *
 * void read_msg(void){
 * 	mqd_t mdes;
 * 	char msg[32];
 * 	unsigned msg_prio;
 *
 * 	mdes = mq_open("/path/to/queue", O_RDWR);
 * 	if ( mdes == (mqd_t)-1 ){
 * 		perror("failed to create queue");
 * 		return;
 * 	}
 *
 * 	//now send a message in the queue
 * 	if ( mq_receive(mdes, msg, strlen(msg), &msg_prio) < 0 ){
 * 		perror("failed to send message");
 * 	}
 *
 * 	printf("received: %s\n", msg);
 *
 * }
 * \endcode
 */

/*! \file */

#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syslimits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>

#include "sysfs.h"

#include "mqueue.h"
#include "../sched/sched_flags.h"

#define MSG_RD_ONLY 0
#define MSG_RDWR 1
#define MSG_NONBLOCK 2
#define MSG_FLAGS_MASK 0x03

#define MSG_FILE_HDR_SIGNATURE 0x0A5518664
#define MSG_FILE_HDR_NOT_SIGNATURE (~MSG_FILE_HDR_SIGNATURE)

struct message {
	int prio;
	int size;
	int age;
	//! \todo Add a checksum to the message -- generate on send and check on receive
};

typedef struct {
	int max_size; //maximum message size
	int max_msgs; //maximum number of messages
	int cur_msgs; //current number of messages
	int refs; //how many tasks are accessing the message queue
	int age; //The running age -- 0 is the oldest message
	int mode;
	char name[NAME_MAX]; //The name of the queue
	struct message * msg_table; //a pointer to the message table
	int unlink_on_close;
} mq_t;

typedef struct {
	mq_t mq;
	void * next;
} mq_list_t;

static mq_list_t * mq_first = 0;

static mq_t * mq_find_named(const char * name){
	mq_list_t * entry;
	for(entry = mq_first; entry != 0; entry = entry->next){
		if( entry->mq.msg_table != 0 ){
			if( strncmp(entry->mq.name, name, NAME_MAX) == 0 ){
				return &entry->mq;
			}
		}
	}
	return 0;
}

static mq_t * mq_find_free(void){
	mq_list_t * entry;
	mq_list_t * new_entry;
	mq_list_t * last_entry;
	last_entry = 0;
	for(entry = mq_first; entry != 0; entry = entry->next){
		last_entry = entry;
		if( entry->mq.msg_table == 0 ){
			return &entry->mq;
		}
	}

	//no free semaphores
	new_entry = _malloc_r(task_table[0].global_reent, sizeof(mq_list_t));
	if( new_entry == 0 ){
		return 0;
	}
	if( last_entry == 0 ){
		mq_first = new_entry;
	} else {
		last_entry->next = new_entry;
	}
	new_entry->next = 0;
	return &new_entry->mq;
}

#define GET_MQ_PTR(mqdes) ( (mq_t*)(mqdes & ~MSG_FLAGS_MASK) )

typedef struct {
	uint32_t signature;
	uint32_t not_signature;
	mq_t * mq;
} msg_file_hdr_t;

static void check_for_blocked_task(void * block);
static int block_on_mq(void * block, const struct timespec * abs_timeout);

typedef struct {
	void * block;
	struct sched_timeval abs_timeout;
} priv_block_on_mq_t;
static void priv_block_on_mq(void * args);

typedef struct {
	mq_t * mq;
	size_t msg_len;
	int size;
	int entry_size;
	struct message * new_msg;
} priv_send_receive_t;
static void priv_receive(void * args);

static void priv_wake_blocked(void * args);

/*! \details This function gets the message queue attributes and stores them at \a mqstat.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - EBADF: \a mqdes is not a valid message queue descriptor
 *
 */
int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat){
	mq_t * mq = GET_MQ_PTR(mqdes);

	if ( mq == NULL ){
		errno = EBADF;
		return -1;
	}

	//read the mq in priv mode
	mqstat->mq_maxmsg = mq->max_msgs;
	mqstat->mq_msgsize = mq->max_size;
	mqstat->mq_curmsgs = mq->cur_msgs;

	mqstat->mq_flags = 0;

	if ( mqdes & MSG_NONBLOCK ){
		mqstat->mq_flags |= O_NONBLOCK;
	}

	if ( mqdes & MSG_RDWR ){
		mqstat->mq_flags |= O_RDWR;
	}

	return 0;
}

/*! \details This function is not supported.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - ENOTSUP
 *
 */
int mq_setattr(mqd_t mqdes, const struct mq_attr * mqstat, struct mq_attr * omqstat){
	errno = ENOTSUP;
	return -1;
}


/*! \details This function opens or creates a new message queue.
 *
 * \param name The name of the message queue
 * \param oflag The flags to use when opening (O_CREAT, O_EXCL, O_RDWR, O_RDONLY)
 * \param mode The access mode settings for the queue
 * \param attr The message queue attributes
 *
 * If using O_CREAT, the following prototype is used:
 * \code
 * mqd_t mq_open(const char * name, int oflag, mode_t mode, const struct mq_attr * attr);
 * \endcode
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - ENAMETOOLONG:  \a name length is greater than NAME_MAX
 * - EEXIST:  O_CREAT and O_EXCL are set in \a oflag but the queue already exists
 * - ENOENT:  O_CREAT is not set in \a oflag and the queue does not exist
 * - ENOMEM:  not enough memory for the queue
 * - EACCES:  permission to create \a name queue is denied
 * - EINVAL: O_CREAT is set and \a attr is not null but \a mq_maxmsg or \a mq_msgsize is less than or equal to zero
 *
 *
 */
mqd_t mq_open(const char * name /*! the full path to the message queue */,
		int oflag /*! the flags used for opening the message queue */,
		... /*! mode_t mode, const struct mq_attr * attr when O_CREAT is set in \a oflag */){
	int err;
	mq_t * new_mq;
	ssize_t mqdes;
	int action;
	mode_t mode;
	const struct mq_attr * attr;
	va_list ap;
	struct _reent * reent_ptr;

	if ( strnlen(name, NAME_MAX) == NAME_MAX ){
		errno = ENAMETOOLONG;
		return -1;
	}

	//Check to see if the  message queue exists
	new_mq = mq_find_named(name);

	//Check the flags to determine the appropriate action
	if ( oflag & O_CREAT ){
		if ( oflag & O_EXCL ){
			if ( new_mq == 0 ){
				//Create the new message queue
				action = 0;
			} else {
				errno = EEXIST;
				return -1;
			}
		} else {
			if ( new_mq == 0 ){
				//Create a new message queue
				action = 0;
			} else {
				//Read the existing message queue
				action = 1;
			}
		}
	} else {
		if ( new_mq == 0 ){
			errno = ENOENT;
			return -1;
		} else {
			//Read the existing message queue
			action = 1;
		}
	}

	switch(action){
	case 0:
		va_start(ap, oflag);
		mode = va_arg(ap, mode_t);
		attr = va_arg(ap, const struct mq_attr *);
		va_end(ap);

		//check for valid message attributes
		if ( (attr->mq_maxmsg <= 0) || (attr->mq_msgsize <= 0) ){
			errno = EINVAL;
			return -1;
		}

		//Create the new message queue
		reent_ptr = task_table[0].global_reent;
		new_mq = mq_find_free();
		if ( new_mq == NULL ){
			//errno is set by malloc
			return -1;
		}

		new_mq->mode = mode;
		new_mq->max_msgs = attr->mq_maxmsg;
		new_mq->max_size = attr->mq_msgsize & ~0x03; //make sure the size is word aligned
		new_mq->cur_msgs = 0;
		new_mq->refs = 1;
		new_mq->age = 0;
		new_mq->unlink_on_close = 0;
		new_mq->msg_table = _calloc_r(reent_ptr, new_mq->max_msgs , (new_mq->max_size + sizeof(struct message)) );
		strcpy(new_mq->name, name);
		if ( new_mq->msg_table == NULL ){
			return -1;
		}
		break;

	case 1:
		//increment the reference to this mq (priv mode)
		new_mq->refs++;
		break;
	}

	err = 0;
	if ( oflag & O_NONBLOCK ){
		err |= MSG_NONBLOCK;
	}

	if ( (oflag & O_WRONLY) || (oflag & O_RDWR) ){
		err |= MSG_RDWR;
	}

	mqdes = err | (int)new_mq;

	return mqdes;
}

/*! \details This function closes the message queue.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - EBADF:  \a mqdes is not a valid descriptor
 *
 */
int mq_close(mqd_t mqdes /*! the message queue handler */){
	mq_t * mq;

	if ( mqdes == -1 ){
		errno = EBADF;
		return -1;
	}

	mq = GET_MQ_PTR(mqdes);

	if( mq->refs > 0 ){
		mq->refs--;
	}

	if ( mq->refs == 0 ){
		//Should message queue be unlinked now?
		if ( mq->unlink_on_close == 1 ){
			_free_r(task_table[0].global_reent, mq->msg_table);
			mq->msg_table = 0;
		}
	}

	return 0;
}

/*! \details This function deletes a message queue if no other processes
 * have it open.  If another process has the queue open, the queue will be deleted when
 * said process closes the queue using \ref mq_close().
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - ENAMETOOLONG:  length of \a name exceeds NAME_MAX
 * - EACCES: write permission to \a name is denied
 * - ENOENT:  the queue does not exist
 * - EIO:  I/O error while deleting the queue
 *
 */
int mq_unlink(const char * name /*! the full path to the message queue */){
	mq_t * mq;

	if ( strnlen(name, NAME_MAX) == NAME_MAX ){
		errno = ENAMETOOLONG;
		return -1;
	}

	mq = mq_find_named(name);
	if( mq == 0 ){
		errno = ENOENT;
		return -1;
	}

	if ( mq->refs == 0 ){
		_free_r(task_table[0].global_reent, mq->msg_table);
		mq->msg_table = 0;
	} else {
		//Mark the message queue for deletion when all refs are done
		mq->unlink_on_close = 1;
	}

	return 0;
}

/*! \details This function is not supported.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - ENOTSUP:  feature is not supported
 *
 */
int mq_notify(mqd_t mqdes,
		const struct sigevent *notification){
	errno = ENOTSUP;
	return -1;
}

/*! \details This function removes a message from the queue and stores
 * the message at \a msg_ptr.
 *
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - EAGAIN:  no message on the queue and O_NONBLOCK is set in the descriptor flags
 * - EIO:  I/O error while accessing the queue
 * - EMSGSIZE:  \a msg_len is less than the message size of the queue
 * - EBADF: \a mqdes is not a valid message queue descriptor
 *
 */
ssize_t mq_receive(mqd_t mqdes /*! the message queue handle */,
		char * msg_ptr /*! a pointer to the destination memory for the message */,
		size_t msg_len /*! the number of bytes available in \a msg_ptr (must be at least the size of the
		message size) */,
		unsigned * msg_prio /*! if not NULL, the priority of the received message is stored here */){
	return mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, NULL);
}


void priv_receive(void * args){
	priv_send_receive_t * p = (priv_send_receive_t*)args;
	void * ptr;
	int current;
	int max_prio;
	int max_age;
	int i;

	max_prio = -1;
	max_age = INT_MAX;
	p->size = 0;

	if ( p->mq->cur_msgs != 0 ){
		ptr = p->new_msg;
		current = -1;
		//find the oldest, highest priority message from the queue
		for(i=0; i < p->mq->max_msgs; i++ ){
			if (p->new_msg->size != 0){
				if ( p->new_msg->prio > max_prio ){
					max_prio = p->new_msg->prio;
					max_age = p->new_msg->age;
					current = i;
				} else if ( p->new_msg->prio == max_prio ){
					if ( (p->new_msg->age < max_age) ){
						max_prio = p->new_msg->prio;
						max_age = p->new_msg->age;
						current = i;
					}
				}
			}
			ptr += p->entry_size;
			p->new_msg = ptr;
		}

		if ( current != -1 ){
			//calculate the pointer to the entry
			ptr = p->mq->msg_table;
			ptr += current * p->entry_size;
			p->new_msg = ptr;

			//Mark message as retrieved
			if ( p->msg_len < p->new_msg->size){
				//The target buffer is too small to hold the entire message
				errno = EMSGSIZE;
				p->size = -1;
			} else {
				//Remove the message from the queue
				p->mq->cur_msgs--;
				p->size = p->new_msg->size;
				p->new_msg->size = 0;  //Mark message as received in the table
			}
		} else {
			//This should never happen
			errno = EIO;
			p->size = -1;
		}
	}
}

/*! \details This function removes a message from the queue and stores
 * the message at \a msg_ptr.  If no messages are available, the thread is blocked
 * until either a messsage is available or the value of \a CLOCK_REALTIME is less then \a abs_timeout.
 *
 * If O_NONBLOCK is set in \a mqdes, the function returns immediately whether a message is ready
 * or not.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - EAGAIN:  no message on the queue and O_NONBLOCK is set in the descriptor flags
 * - EIO:  I/O error while accessing the queue
 * - ETIMEDOUT:  \a abs_timeout was exceeded by \a CLOCK_REALTIME
 * - EMSGSIZE:  \a msg_len is less than the message size of the queue
 * - EBADF: \a mqdes is not a valid message queue descriptor
 *
 */
ssize_t mq_timedreceive(mqd_t mqdes /*! see \ref mq_receive() */,
		char * msg_ptr /*! see \ref mq_receive() */,
		size_t msg_len /*! see \ref mq_receive() */,
		unsigned * msg_prio /*! see \ref mq_receive() */,
		const struct timespec * abs_timeout /*! the absolute timeout value */){
	priv_send_receive_t args;
	void * ptr;

	if ( mqdes == -1 ){
		errno = EBADF;
		return -1;
	}

	args.mq = GET_MQ_PTR(mqdes);
	args.msg_len = msg_len;
	args.entry_size =  args.mq->max_size + sizeof(struct message);

	do {

		args.new_msg = args.mq->msg_table;
		hwpl_core_privcall(priv_receive, &args); //this allows isolated access to the queue

		if ( args.size > 0 ){
			if ( msg_prio != NULL ){
				*(msg_prio) = args.new_msg->prio;
			}

			//Copy the message
			ptr = args.new_msg;
			memcpy(msg_ptr, ptr + sizeof(struct message), args.size);
			//Remove the message from the queue
		} else if ( args.size < 0 ){
			//errno has already been set
			return -1;
		} else {
			if( mqdes & MSG_NONBLOCK ){
				errno = EAGAIN;
				return -1;
			}
			//block this thread until a message arrives
			args.size = block_on_mq(args.mq, abs_timeout);
		}

	} while( args.size == 0 ); //wait for either a successful receive or an error

	//message was successfully received -- now see if any threads are blocked trying to send to this queue
	check_for_blocked_task(args.mq);

	return args.size;
}

/*! \details This function sends a message pointed to by \a msg_ptr.  If there is no room
 * in the queue (and O_NONBLOCK is not set in \a mqdes), the thread is blocked until
 * a message is removed from the queue.
 *
 * If O_NONBLOCK is set in \a mqdes, the function returns immediately whether a message is sent
 * or not.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - EAGAIN:  no room on the queue and O_NONBLOCK is set in the descriptor flags
 * - EIO:  I/O error while accessing the queue
 * - EBADF: \a mqdes is not a valid message queue descriptor
 *
 */
int mq_send(mqd_t mqdes /*! the message queue handle */,
		const char * msg_ptr /*! a pointer to the message to be sent */,
		size_t msg_len /*! the message size (must be less than or equal to \a mq_msgsize associated with \a mqdes */,
		unsigned msg_prio /*! the priority of the message (see \ref MQ_PRIO_MAX) */){
	return mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, NULL);
}


void priv_send(void * args){
	priv_send_receive_t * p = (priv_send_receive_t*)args;
	void * ptr;
	int i;

	p->size = 0;

	if ( p->mq->cur_msgs < p->mq->max_msgs ){
		//add the message in the first free spot
		ptr = p->new_msg;

		//Find an open spot
		for(i = 0; i < p->mq->max_msgs; i++){
			if ( p->new_msg->size == 0 ){
				//This slot is free
				p->new_msg->size = p->msg_len;
				p->size = p->msg_len;
				p->mq->cur_msgs++;
				return;
			}
			ptr += p->entry_size;
			p->new_msg = ptr;
		}

	}

}

/*! \details This function sends a message pointed to by \a msg_ptr.  If there is no room
 * in the queue (and O_NONBLOCK is not set in \a mqdes), the thread is blocked until
 * a message is removed from the queue or until the value of \a CLOCK_REALTIME exceeds
 * \a abs_timeout.
 *
 * If O_NONBLOCK is set in \a mqdes, the function returns immediately whether a message is sent
 * or not.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - EAGAIN:  no message on the queue and O_NONBLOCK is set in the descriptor flags
 * - EIO:  I/O error while accessing the queue
 * - ETIMEDOUT:  \a abs_timeout was exceeded by \a CLOCK_REALTIME
 * - EBADF: \a mqdes is not a valid message queue descriptor
 *
 */
int mq_timedsend(mqd_t mqdes /*! see \ref mq_send() */,
		const char * msg_ptr /*! see \ref mq_send() */,
		size_t msg_len /*! see \ref mq_send() */,
		unsigned msg_prio /*! see \ref mq_send() */,
		const struct timespec * abs_timeout /*! the absolute timeout value */){
	void * ptr;
	priv_send_receive_t args;

	if ( mqdes == -1 ){
		errno = EBADF;
		return -1;
	}

	if ( mqdes & MSG_RDWR ){

		args.mq = GET_MQ_PTR(mqdes);
		//Check the length of the message
		if ( args.mq->max_size < msg_len ){
			//The message to send is too big
			errno = EMSGSIZE;
			return -1;
		}

		args.entry_size = args.mq->max_size + sizeof(struct message);
		args.msg_len = msg_len;
		do {

			args.new_msg = args.mq->msg_table;
			hwpl_core_privcall(priv_send, &args); //this allows isolated access to the queue
			//This message slot is free
			if ( args.size > 0 ){
				ptr = args.new_msg;
				memcpy(ptr + sizeof(struct message), msg_ptr, args.size);
				args.new_msg->prio = msg_prio;
				args.new_msg->age = args.mq->age++;
			} else {
				if ( mqdes & MSG_NONBLOCK ){
					//Non-blocking mode:  return an error
					errno = EAGAIN;
					return -1;
				} else {
					//no slots are free (block until one is free)
					//block this thread until a message arrives
					args.size = block_on_mq(args.mq, abs_timeout);
				}
			}

		} while( args.size  == 0 );

		//message was successfully sent -- now see if any threads are blocked trying to send to this queue
		check_for_blocked_task(args.mq);
		return args.size;
	}

	errno = EACCES;
	return -1;
}

void priv_block_on_mq(void * args){
	priv_block_on_mq_t * argsp = (priv_block_on_mq_t*)args;
	sched_priv_timedblock(argsp->block, &argsp->abs_timeout);
}

int block_on_mq(void * block, const struct timespec * abs_timeout){
	priv_block_on_mq_t args;
	args.block = block;
	sched_convert_timespec(&args.abs_timeout, abs_timeout);
	hwpl_core_privcall(priv_block_on_mq, &args);
	if ( sched_get_unblock_type( task_get_current() ) == SCHED_UNBLOCK_SLEEP ){
		errno = ETIMEDOUT;
		return -1;
	}
	return 0;
}

void priv_wake_blocked(void * args){
	int * task = (int*)args;
	int id = *task;
	sched_priv_assert_active(id, SCHED_UNBLOCK_MQ);
	sched_priv_update_on_wake(sched_table[id].priority);
}

void check_for_blocked_task(void * block){
	int new_thread;
	new_thread = sched_get_highest_priority_blocked(block);
	if ( new_thread != -1 ){
		hwpl_core_privcall(priv_wake_blocked, &new_thread);
	} else {
		//See if any tasks need to be notified if a message was just sent
	}
}

/*! @} */


