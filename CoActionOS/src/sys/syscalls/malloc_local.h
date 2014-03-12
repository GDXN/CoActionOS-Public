/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef MALLOC_LOCAL_H_
#define MALLOC_LOCAL_H_


#include "config.h"
#include <_ansi.h>
#include <reent.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include "hwpl.h"
#include "hwpl/task.h"


#define MALLOC_DATA_SIZE (MALLOC_CHUNK_SIZE - (sizeof(uint32_t)*2))

typedef struct {
	uint8_t task_id;
	uint8_t signature;
	uint16_t num_chunks;
	uint32_t actual_size;
	char memory[MALLOC_DATA_SIZE];
} malloc_chunk_t;

void malloc_set_chunk_used(struct _reent * reent, malloc_chunk_t * chunk, uint16_t num_chunks, uint32_t actual_size);
void malloc_set_chunk_free(malloc_chunk_t * chunk);
int malloc_chunk_is_free(malloc_chunk_t * chunk);
uint16_t malloc_calc_num_chunks(uint32_t size);
malloc_chunk_t * malloc_chunk_from_addr(void * addr);

void malloc_free_task_r(struct _reent * reent_ptr, int task_id);

void __malloc_lock(struct _reent *ptr);
void __malloc_unlock(struct _reent *ptr);

#endif /* MALLOC_LOCAL_H_ */
