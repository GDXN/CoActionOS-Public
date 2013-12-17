/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <sys/unistd.h>
#include <unistd.h>
#include "malloc_local.h"
#include "hwpl/debug.h"
#include "hwpl/core.h"



static void set_last_chunk(malloc_chunk_t * chunk);
static void cleanup_memory(malloc_chunk_t * chunk);
static int get_more_memory(struct _reent * reent_ptr);
static malloc_chunk_t * find_free_chunk(malloc_chunk_t * chunk, uint32_t num_chunks);
static int is_memory_corrupt(struct _reent * reent_ptr);
static void show_seg_fault(void);

uint16_t malloc_calc_num_chunks(uint32_t size){
	int num_chunks;
	if ( size > MALLOC_DATA_SIZE ){
		num_chunks = ((size - MALLOC_DATA_SIZE) + MALLOC_CHUNK_SIZE - 1) / MALLOC_CHUNK_SIZE + 1;
	} else {
		num_chunks = 1;
	}
	return num_chunks;
}

malloc_chunk_t * find_free_chunk(malloc_chunk_t * chunk, uint32_t num_chunks){
	int loop_count = 0;
	int is_free;

	while( chunk->num_chunks != 0 ){
		is_free = malloc_chunk_is_free(chunk);

		/*
		hwpl_debug("C:(0x%X %d 0x%X %d)\n",
				(int)chunk,
				chunk->num_chunks,
				chunk->signature,
				is_free);
		 */


		if ( is_free == -1 ){
			return NULL;
		}

		if ( (is_free == 1)  && ( chunk->num_chunks >= num_chunks ) ){
			return chunk;
		}
		loop_count++;
		chunk = chunk + chunk->num_chunks;
	}


	//No block found to fit size
	//hwpl_debug("No chunk to fit (0x%X %d 0x%X)\n", chunk, chunk->num_chunks, chunk->signature);
	return NULL;
}

int is_memory_corrupt(struct _reent * reent_ptr){
	int is_free;
	malloc_chunk_t * start_chunk = (malloc_chunk_t *)&(reent_ptr->procmem_base->base);
	malloc_chunk_t * chunk = start_chunk;

	while( chunk->num_chunks != 0 ){
		is_free = malloc_chunk_is_free(chunk);
		if ( is_free == -1 ){
			/*
			hwpl_debug("Corrupt Memory %d 0x%X 0x%X\n",
					task_get_current(),
					(int)stackaddr,
					(uint32_t)(&(reent_ptr->procmem_base->base)) + (uint32_t)reent_ptr->procmem_base->size);
			 */

			//Is the corruption caused by a collision with the stack?
			/*
			if ( stackaddr != NULL ){
				if ( (uint32_t)stackaddr < ((uint32_t)(&(reent_ptr->procmem_base->base)) + (uint32_t)reent_ptr->procmem_base->size) ){

					hwpl_debug("Stack Collision 0x%X < 0x%X\n",
							(int)stackaddr,
							(uint32_t)(&(reent_ptr->procmem_base->base)) + (uint32_t)reent_ptr->procmem_base->size);

				}
			}
			 */

			//Kill the process with the faulty memory
			show_seg_fault();
#if SINGLE_TASK == 0
			kill( getpid(), SIGKILL);
#endif
			return -1;
		}
		chunk += chunk->num_chunks;
	}
	return 0;
}

void cleanup_memory(malloc_chunk_t * chunk){
	malloc_chunk_t * current;
	malloc_chunk_t * next;
	int next_free;
	int current_free;
	current = chunk;
	next = current + (current->num_chunks);
	while( next->num_chunks != 0 ){
		current_free = malloc_chunk_is_free(current);
		next_free = malloc_chunk_is_free(next);
		//hwpl_debug("CC:0x%X %d %d\n", (int)current, current->num_chunks, current_free);
		//hwpl_debug("NC:0x%X %d %d\n", (int)next, next->num_chunks, next_free);

		if ( next_free == -1 ){
			return;
		}

		if ( (current_free == 1) &&
				(next_free == 1) ){ //both blocks are free
			current->num_chunks += next->num_chunks;
			malloc_set_chunk_free(current);
		} else {
			current = next;
		}
		next = next + next->num_chunks;
	}
}

malloc_chunk_t * malloc_chunk_from_addr(void * addr){
	malloc_chunk_t * chunk;
	chunk = (malloc_chunk_t *)((char*)addr - (sizeof(malloc_chunk_t) - MALLOC_DATA_SIZE));
	return chunk;
}


void malloc_free_task_r(struct _reent * reent_ptr, int task_id){
	malloc_chunk_t * chunk;
	malloc_chunk_t * next;
	if ( reent_ptr->procmem_base == NULL ){
		return;
	}

	chunk = (malloc_chunk_t *) &(reent_ptr->procmem_base->base);

	while( chunk->num_chunks != 0 ){
		next = chunk + chunk->num_chunks;
		if ( chunk->task_id == task_id ){
			_free_r(reent_ptr, chunk->memory);
		}
		chunk = next;
	}
}



void _free_r(struct _reent * reent_ptr, void * addr){
	int tmp;
	malloc_chunk_t * chunk;
	int is_free;
	proc_mem_t * base;

	if ( reent_ptr->procmem_base == NULL ){
		return;
	}

	if ( addr == NULL ){
		return;
	}

	chunk = malloc_chunk_from_addr(addr);

	/*
	hwpl_debug("Freeing:%d bytes (%d chunks) at 0x%X for %d signed 0x%X\n",
			chunk->actual_size,
			chunk->num_chunks,
			(int)chunk->memory,
			_getpid(),
			chunk->signature);
	 */

	base = reent_ptr->procmem_base;

	__malloc_lock(reent_ptr);
	tmp = (unsigned int)chunk - (unsigned int)(&(base->base));
	if ( tmp % MALLOC_CHUNK_SIZE ){
		__malloc_unlock(reent_ptr);
		return;
	}

	is_free = malloc_chunk_is_free(chunk);

	if ( is_free != 0 ){  //Is the chunk in use (able to be freed)
		//This is not a valid memory allocation location
		__malloc_unlock(reent_ptr);
		return;
	}

	//hwpl_debug("f:%d 0x%X\n", _getpid(), addr);
	malloc_set_chunk_free(chunk);
	cleanup_memory((malloc_chunk_t *)&(reent_ptr->procmem_base->base));
	__malloc_unlock(reent_ptr);
}


int get_more_memory(struct _reent * reent_ptr){
	malloc_chunk_t * chunk;
	chunk = (malloc_chunk_t *)_sbrk_r(reent_ptr, MALLOC_SBRK_JUMP_SIZE);
	if ( chunk == NULL ){
		return -1;
	} else {
		chunk->num_chunks = MALLOC_SBRK_JUMP_SIZE / MALLOC_CHUNK_SIZE;
		//hwpl_debug("MC:0x%X (%d)\n", (int)chunk, chunk->num_chunks);
		malloc_set_chunk_free(chunk);
		set_last_chunk(chunk + chunk->num_chunks); //mark the last block
		//hwpl_debug("LC:%d:0x%X\n", chunk_is_free(chunk + chunk->num_chunks), chunk + chunk->num_chunks);
	}
	return 0;
}

void * _malloc_r(struct _reent * reent_ptr, size_t size){
	void * alloc;
	uint16_t num_chunks;
	malloc_chunk_t * chunk;
	malloc_chunk_t * next;
	alloc = NULL;

	if ( reent_ptr == NULL ){
		errno = EINVAL;
		return NULL;
	}

	__malloc_lock(reent_ptr);

	if ( reent_ptr->procmem_base->size == 0 ){

		if ( get_more_memory(reent_ptr) < 0 ){
			__malloc_unlock(reent_ptr);
			errno = ENOMEM;
			return NULL;
		}
	}

	num_chunks = malloc_calc_num_chunks(size);

	//Find a free chunk that fits size -- add memory using get_more_memory() as necessary
	do {

		chunk = find_free_chunk((malloc_chunk_t *) &(reent_ptr->procmem_base->base), num_chunks);
		if ( chunk == NULL ){

			//See if the memory is corrupt
			if ( is_memory_corrupt(reent_ptr) ){
				__malloc_unlock(reent_ptr);
				errno = ENOMEM; //! \todo This should be a different error -- fatal fault
				return NULL;
			}

			//Try to get more memory
			if ( get_more_memory(reent_ptr) < 0 ){
				__malloc_unlock(reent_ptr);
				errno = ENOMEM;
				return NULL;
			}

			//Cleanup the memory
			cleanup_memory((malloc_chunk_t *)&(reent_ptr->procmem_base->base) );

		} else {

			//See if the memory will fit in this chunk
			if ( chunk->num_chunks > num_chunks ){
				next = chunk + (num_chunks);
				next->num_chunks = (chunk->num_chunks) - num_chunks;
				malloc_set_chunk_free(next);
			} else if ( chunk->num_chunks < num_chunks ){
				__malloc_unlock(reent_ptr);
				errno = ENOMEM;
				return NULL;
			}
			malloc_set_chunk_used(reent_ptr, chunk, num_chunks, size);
			alloc = chunk->memory;
		}
	} while(alloc == NULL);

	__malloc_unlock(reent_ptr);

	//hwpl_debug("a:%d 0x%X %d 0x%X 0x%X\n", _getpid(), alloc, size, reent_ptr, _GLOBAL_REENT);


	return alloc;
}

void malloc_set_chunk_used(struct _reent * reent, malloc_chunk_t * chunk, uint16_t num_chunks, uint32_t actual_size){
	chunk->num_chunks = num_chunks;
	chunk->actual_size = actual_size;
	chunk->signature = ~(num_chunks ^ actual_size);
#if SINGLE_TASK == 0
	if( (reent == _REENT) && ( task_isthread_asserted( task_get_current() )) ){
		chunk->task_id = task_get_current();
	} else {
		chunk->task_id = 0;
	}
#else
	chunk->task_id = 0;
#endif
}

void malloc_set_chunk_free(malloc_chunk_t * chunk){
	chunk->actual_size = 0;
	chunk->signature = ~(chunk->num_chunks ^ 0);
}

void set_last_chunk(malloc_chunk_t * chunk){
	chunk->num_chunks = 0;
	chunk->actual_size = 0;
	chunk->signature = ~(0 ^ 0);
}


int malloc_chunk_is_free(malloc_chunk_t * chunk){
	if ( chunk->signature != (uint8_t)~(chunk->num_chunks ^ chunk->actual_size) ){
		//This chunk is corrupt
		hwpl_debug("Corrupt Chunk %d 0x%X 0x%X 0x%X\n", getpid(), (int)chunk, chunk->signature, (uint16_t)~(chunk->num_chunks ^ chunk->actual_size));
		show_seg_fault();
#if SINGLE_TASK == 0
		kill( getpid(), SIGKILL);
		while(1){}
#endif
		return -1;
	}

	if ( chunk->actual_size == 0 ){ //Is the chunk free
		return 1;
	}

	//Chunk is in use
	return 0;
}

void show_seg_fault(void){
	//hwpl_debug("\nSegmentation Fault\n");
#if SINGLE_TASK == 0
	if ( task_get_current() != 0 ){
#endif

#if USE_STDIO != 0
		if ( (stderr != NULL) && (stderr != (FILE*)&__sf_fake_stderr) ){
			write(stderr->_file, hwpl_debug_buffer, strlen(hwpl_debug_buffer));
		}
#endif

#if SINGLE_TASK == 0
	}
#endif


}
