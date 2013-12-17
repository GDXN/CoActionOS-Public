/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef TASK_TABLE_H_
#define TASK_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_FLAGS_USED (1<<8)
#define TASK_FLAGS_EXEC (1<<9)
#define TASK_FLAGS_PRIV (1<<10)
#define TASK_FLAGS_IS_THREAD (1<<11)
#define TASK_FLAGS_IS_FIFO (1<<12)
#define TASK_FLAGS_YIELD (1<<13)
#define TASK_FLAGS_PARENT(x) ( (x & 0xFFFF) << 16 )

#define TASK_FLAGS_GET_PARENT(flags) ( flags >> 16 )
#define TASK_FLAGS_PARENT_MASK 0xFFFF0000


extern task_t * task_table;
extern int task_total;
extern int task_rr_reload;
extern volatile task_timer_t task_clock;
extern volatile int task_current;

static inline int task_get_rr_reload(void) HWPL_ALWAYS_INLINE;
int task_get_rr_reload(void){
	return task_rr_reload;
}

static inline void task_set_reent(int id, struct _reent * reent, struct _reent * global_reent) HWPL_ALWAYS_INLINE;
void task_set_reent(int id, struct _reent * reent, struct _reent * global_reent){
	task_table[id].reent = reent;
	task_table[id].global_reent = global_reent;
}

static inline void task_deassert_exec(int id) HWPL_ALWAYS_INLINE;
void task_deassert_exec(int id){
	task_table[id].flags &= ~(TASK_FLAGS_EXEC);
}

static inline void task_assert_exec(int id) HWPL_ALWAYS_INLINE;
void task_assert_exec(int id){
	task_table[id].flags |= (TASK_FLAGS_EXEC);
}

static inline int task_exec_asserted(int id) HWPL_ALWAYS_INLINE;
int task_exec_asserted(int id){
	return ( (task_table[id].flags & (TASK_FLAGS_EXEC)) ==  TASK_FLAGS_EXEC);
}

static inline void task_deassert_yield(int id) HWPL_ALWAYS_INLINE;
void task_deassert_yield(int id){
	task_table[id].flags &= ~(TASK_FLAGS_YIELD);
}

static inline void task_assert_yield(int id) HWPL_ALWAYS_INLINE;
void task_assert_yield(int id){
	task_table[id].flags |= (TASK_FLAGS_YIELD);
}

static inline int task_yield_asserted(int id) HWPL_ALWAYS_INLINE;
int task_yield_asserted(int id){
	return ( (task_table[id].flags & (TASK_FLAGS_YIELD)) ==  TASK_FLAGS_YIELD);
}

static inline int task_isthread_asserted(int id) HWPL_ALWAYS_INLINE;
int task_isthread_asserted(int id){
	return ( (task_table[id].flags & (TASK_FLAGS_IS_THREAD)) ==  TASK_FLAGS_IS_THREAD);
}

static inline void task_deassert_priv(int id) HWPL_ALWAYS_INLINE;
void task_deassert_priv(int id){
	task_table[id].flags &= ~(TASK_FLAGS_PRIV);
}

static inline void task_assert_priv(int id) HWPL_ALWAYS_INLINE;
void task_assert_priv(int id){
	task_table[id].flags |= (TASK_FLAGS_PRIV);
}

static inline int task_priv_asserted(int id) HWPL_ALWAYS_INLINE;
int task_priv_asserted(int id){
	return ( (task_table[id].flags & (TASK_FLAGS_PRIV)) ==  TASK_FLAGS_PRIV);
}

static inline void task_deassert_isfifo(int id) HWPL_ALWAYS_INLINE;
void task_deassert_isfifo(int id){
	task_table[id].flags &= ~(TASK_FLAGS_IS_FIFO);
}

static inline void task_assert_isfifo(int id) HWPL_ALWAYS_INLINE;
void task_assert_isfifo(int id){
	task_table[id].flags |= (TASK_FLAGS_IS_FIFO);
}

static inline int task_isfifo_asserted(int id) HWPL_ALWAYS_INLINE;
int task_isfifo_asserted(int id){
	return ( (task_table[id].flags & (TASK_FLAGS_IS_FIFO)) ==  TASK_FLAGS_IS_FIFO);
}


static inline void task_set_parent(int id, int parent) HWPL_ALWAYS_INLINE;
void task_set_parent(int id, int parent){
	task_table[id].flags &= ~TASK_FLAGS_PARENT_MASK;
	task_table[id].flags |= parent;
}

static inline int task_get_parent(int id) HWPL_ALWAYS_INLINE;
int task_get_parent(int id){
	return TASK_FLAGS_GET_PARENT(task_table[id].flags);
}

static inline int task_used_asserted(int id) HWPL_ALWAYS_INLINE;
int task_used_asserted(int id){
	return ((task_table[id].flags & (TASK_FLAGS_USED)) == TASK_FLAGS_USED);
}

static inline void task_assert_used(int id) HWPL_ALWAYS_INLINE;
void task_assert_used(int id){
	task_table[id].flags |= (TASK_FLAGS_USED);
}

static inline void task_deassert_used(int id) HWPL_ALWAYS_INLINE;
void task_deassert_used(int id){
	task_table[id].flags &= ~(TASK_FLAGS_USED);
}

static inline int task_enabled(int id) HWPL_ALWAYS_INLINE;
int task_enabled(int id){
	return ((task_table[id].flags & (TASK_FLAGS_USED)) == TASK_FLAGS_USED);
}

static inline int task_get_current(void) HWPL_ALWAYS_INLINE;
int task_get_current(void){
	return task_current;
}

static inline int task_get_pid(int id) HWPL_ALWAYS_INLINE;
int task_get_pid(int id){
	return task_table[id].pid;
}

static inline int task_get_total(void) HWPL_ALWAYS_INLINE;
int task_get_total(void){
	return task_total;
}

static inline void task_get_timer(uint32_t * dest, int id) HWPL_ALWAYS_INLINE;
void task_get_timer(uint32_t * dest, int id){
	dest[1] = task_table[id].timer.t_atomic[1];
	dest[0] = task_table[id].timer.t_atomic[0];
}

#ifdef __cplusplus
}
#endif

#endif /* TASK_TABLE_H_ */
