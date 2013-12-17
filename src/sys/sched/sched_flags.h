/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#ifndef SCHED_FLAGS_H_
#define SCHED_FLAGS_H_

#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdint.h>
#include "hwpl/fault.h"
#include "hwpl/task.h"
#include "hwpl/debug.h"
#include "hwpl/core.h"


#include "sched.h"

#define SCHED_TASK_FLAGS_ACTIVE 4
#define SCHED_TASK_FLAGS_INUSE 5
#define SCHED_TASK_FLAGS_WAITCHILD 6
#define SCHED_TASK_FLAGS_SIGCAUGHT 7
#define SCHED_TASK_FLAGS_AIOSUSPEND 8
#define SCHED_TASK_FLAGS_ZOMBIE 9
#define SCHED_TASK_FLAGS_LISTIOSUSPEND 10
#define SCHED_TASK_FLAGS_STOPPED 11
#define SCHED_TASK_FLAGS_UNBLOCK_MASK 0x0F

typedef enum {
	SCHED_UNBLOCK_NONE,
	SCHED_UNBLOCK_MUTEX,
	SCHED_UNBLOCK_SEMAPHORE,
	SCHED_UNBLOCK_RWLOCK,
	SCHED_UNBLOCK_COND,
	SCHED_UNBLOCK_SLEEP,
	SCHED_UNBLOCK_WAIT,
	SCHED_UNBLOCK_SIGNAL,
	SCHED_UNBLOCK_TRANSFER,
	SCHED_UNBLOCK_MQ,
	SCHED_UNBLOCK_PTHREAD_JOINED,
	SCHED_UNBLOCK_PTHREAD_JOINED_THREAD_COMPLETE,
	SCHED_UNBLOCK_AIO
} sched_unblock_type_t;

typedef struct {
	bool logged;
	uint32_t tid;
	uint32_t pid;
	fault_t fault;
	time_t time;
	unsigned int useconds;
} sched_fault_t;

#define SCHED_TIMEVAL_SEC_INVALID UINT_MAX
#define SCHED_TIMEVAL_SECONDS 2048

struct sched_timeval {
	uint32_t tv_sec; //SCHED_TIMEVAL_SECONDS seconds each
	uint32_t tv_usec; //up to 1000000 * SCHED_TIMEVAL_SECONDS
};

#define SCHED_NUM_SIGNALS 32

typedef struct {
	pthread_attr_t attr /*! This holds the task's pthread attributes */;
	volatile void * block_object /*! The blocking object */;
	union {
		volatile int exit_status /*! The task's exit status */;
		void * (*init)(void*) /*! Task 0 init routine */;
	};
	pthread_mutex_t * signal_delay_mutex /*! The mutex to lock if the task cannot be interrupted */;
	volatile struct sched_timeval wake /*! When to wake the task */;
	volatile uint16_t flags /*! This indicates whether the process is active or not */;
	volatile uint16_t priority /*! The current priority */;
} sched_task_t;


//Scheduler timing
int sched_timing_init(void);

extern volatile int8_t sched_current_priority;
extern volatile int8_t sched_status_changed;
extern volatile sched_task_t sched_table[CHILD_MAX];
extern task_t sched_task_table[CHILD_MAX];
extern volatile sched_fault_t sched_fault;
extern const int caoslib_system_memory_size;
extern const int cpu_init_freq;

static inline int sched_active_asserted(int id) HWPL_ALWAYS_INLINE;
int sched_active_asserted(int id){ return sched_table[id].flags & (1<< SCHED_TASK_FLAGS_ACTIVE); };

static inline int sched_inuse_asserted(int id) HWPL_ALWAYS_INLINE;
int sched_inuse_asserted(int id){ return sched_table[id].flags & (1<< SCHED_TASK_FLAGS_INUSE); };

static inline int sched_waitchild_asserted(int id) HWPL_ALWAYS_INLINE;
int sched_waitchild_asserted(int id){ return sched_table[id].flags & (1<< SCHED_TASK_FLAGS_WAITCHILD); };

static inline int sched_sigcaught_asserted(int id) HWPL_ALWAYS_INLINE;
int sched_sigcaught_asserted(int id){ return sched_table[id].flags & (1<< SCHED_TASK_FLAGS_SIGCAUGHT); };

static inline int sched_aiosuspend_asserted(int id) HWPL_ALWAYS_INLINE;
int sched_aiosuspend_asserted(int id){ return sched_table[id].flags & (1<< SCHED_TASK_FLAGS_AIOSUSPEND); };

static inline int sched_listiosuspend_asserted(int id) HWPL_ALWAYS_INLINE;
int sched_listiosuspend_asserted(int id){ return sched_table[id].flags & (1<< SCHED_TASK_FLAGS_LISTIOSUSPEND); };

static inline int sched_stopped_asserted(int id) HWPL_ALWAYS_INLINE;
int sched_stopped_asserted(int id){ return sched_table[id].flags & (1<< SCHED_TASK_FLAGS_STOPPED); };

static inline int sched_zombie_asserted(int id) HWPL_ALWAYS_INLINE;
int sched_zombie_asserted(int id){ return sched_table[id].flags & (1<< SCHED_TASK_FLAGS_ZOMBIE); };

static inline int sched_get_priority(int id) HWPL_ALWAYS_INLINE;
int sched_get_priority(int id){ return sched_table[id].priority; };

static inline int sched_get_current_priority(void) HWPL_ALWAYS_INLINE;
int sched_get_current_priority(void){ return sched_current_priority; };

static inline int sched_status_change_asserted(void) HWPL_ALWAYS_INLINE;
inline int sched_status_change_asserted(void){
	return sched_status_changed;
}

static inline int sched_get_unblock_type(int id) HWPL_ALWAYS_INLINE;
int sched_get_unblock_type(int id){ return sched_table[id].flags & SCHED_TASK_FLAGS_UNBLOCK_MASK; };

static inline void sched_priv_set_unblock_type(int id, sched_unblock_type_t unblock_type) HWPL_ALWAYS_INLINE;
void sched_priv_set_unblock_type(int id, sched_unblock_type_t unblock_type){
	sched_table[id].flags &= ~SCHED_TASK_FLAGS_UNBLOCK_MASK;
	sched_table[id].flags |= unblock_type;
};

void sched_priv_update_on_sleep(void);
void sched_priv_update_on_stopped(void);
void sched_priv_update_on_wake(int new_priority);
void sched_priv_cleanup_tasks(void * args);

void sched_priv_assert(int id, int flag);
void sched_priv_deassert(int id, int flag);

void sched_priv_assert_active(int id, int unblock_type);
void sched_priv_deassert_active(int id);
void sched_priv_assert_status_change(void);
void sched_priv_deassert_status_change(void);
void sched_priv_set_current_priority(int priority);


static inline void sched_priv_assert_waitchild(int id);
void sched_priv_assert_waitchild(int id){
	sched_priv_assert(id, SCHED_TASK_FLAGS_WAITCHILD);
}
static inline void sched_priv_deassert_waitchild(int id);
void sched_priv_deassert_waitchild(int id){
	sched_priv_deassert(id, SCHED_TASK_FLAGS_WAITCHILD);
}

static inline void sched_priv_assert_inuse(int id);
void sched_priv_assert_inuse(int id){
	sched_priv_assert(id, SCHED_TASK_FLAGS_INUSE);
}
static inline void sched_priv_deassert_inuse(int id);
void sched_priv_deassert_inuse(int id){
	sched_priv_deassert(id, SCHED_TASK_FLAGS_INUSE);
}
static inline void sched_priv_assert_sigcaught(int id);
void sched_priv_assert_sigcaught(int id){
	sched_priv_assert(id, SCHED_TASK_FLAGS_SIGCAUGHT);
}
static inline void sched_priv_deassert_sigcaught(int id);
void sched_priv_deassert_sigcaught(int id){
	sched_priv_deassert(id, SCHED_TASK_FLAGS_SIGCAUGHT);
}
static inline void sched_priv_assert_aiosuspend(int id);
void sched_priv_assert_aiosuspend(int id){
	sched_priv_assert(id, SCHED_TASK_FLAGS_AIOSUSPEND);
}
static inline void sched_priv_deassert_aiosuspend(int id);
void sched_priv_deassert_aiosuspend(int id){
	sched_priv_deassert(id, SCHED_TASK_FLAGS_AIOSUSPEND);
}
static inline void sched_priv_assert_listiosuspend(int id);
void sched_priv_assert_listiosuspend(int id){
	sched_priv_assert(id, SCHED_TASK_FLAGS_LISTIOSUSPEND);
}
static inline void sched_priv_deassert_listiosuspend(int id);
void sched_priv_deassert_listiosuspend(int id){
	sched_priv_assert(id, SCHED_TASK_FLAGS_LISTIOSUSPEND);
}

static inline  void sched_priv_assert_stopped(int id);
void sched_priv_assert_stopped(int id){
	sched_priv_assert(id, SCHED_TASK_FLAGS_STOPPED);
}
static inline void sched_priv_deassert_stopped(int id);
void sched_priv_deassert_stopped(int id){
	sched_priv_deassert(id, SCHED_TASK_FLAGS_STOPPED);
}

int sched_check_tid(int id);
void sched_microsecond_match_event(int current_match);
void sched_second_match_event(void);

int sched_send_signal(int id, int sig);
int sched_send_task_signal(int tid, int sig);

/*! \details This function uses the task API to create a new thread.
 * \return Zero on success
 */
int sched_new_thread(void *(*p)(void*)  /*! The function to execute for the task */,
		void * arg /*! The thread's single argument */,
		void * mem_addr /*! The address for the new thread memory */,
		int mem_size /*! The stack size in bytes */,
		pthread_attr_t * attr);


int sched_new_process(void (*p)(int, char * const*),
		int argc,
		char * const argv[],
		task_memories_t * mem,
		void * reent);

int sched_update(void);

void sched_priv_abnormal_child_stop(int id);
void sched_priv_stop_task(int id);
void sched_priv_start_task(int id);
void sched_notify_parent_of_kill(int id);
int sched_set_param(int id, int priority, int policy);
int sched_get_param(int id, int * priority, int * policy);
int sched_switch_context(void * args);

int sched_get_highest_priority_blocked(void * block_object);
int sched_priv_unblock_all(void * block_object, int unblock_type);

void sched_priv_timedblock(void * block_object, struct sched_timeval * interval);

uint32_t sched_seconds_to_clocks(int seconds);
uint32_t sched_useconds_to_clocks(int useconds);
uint32_t sched_nanoseconds_to_clocks(int nanoseconds);
void sched_convert_timespec(struct sched_timeval * tv, const struct timespec * ts);
void sched_priv_get_realtime(struct sched_timeval * tv);
void sched_fault_build_string(char * dest);

int sched_prepare(void);
void scheduler(void);

#define SCHED_DEBUG 0
#define sched_debug(...) do { if ( SCHED_DEBUG == 1 ){ hwpl_debug("%s:", __func__); hwpl_debug(__VA_ARGS__); } } while(0)
#define sched_priv_debug(...) do { if ( SCHED_DEBUG == 1 ){ hwpl_priv_debug("%s:", __func__); hwpl_priv_debug(__VA_ARGS__); } } while(0)
extern void gled_priv_error(void * args);



#endif /* SCHED_FLAGS_H_ */
