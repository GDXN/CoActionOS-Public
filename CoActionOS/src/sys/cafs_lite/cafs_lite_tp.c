/*
 * cl_tp.c
 *
 *  Created on: Apr 17, 2012
 *      Author: tgil
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cafs_lite_local.h"
#include "cafs_lite_tp.h"

#define FILE_LEN 128
#define LINE_LEN 8
#define FUNC_LEN 128
#define DESC_LEN 256

typedef struct {
	char file[FILE_LEN];
	int line;
	char func[FUNC_LEN];
	char desc[DESC_LEN];
	int count;
	int failed;
} cl_tp_t;


#define UPDATE_TABLE_JUMP 64

static cl_tp_t * add_tp(const char * file, int line, const char * func, const char * desc);
static cl_tp_t * find_tp(const char * file, int line);

static int rand_seed = 0;
static cl_tp_t * tp_table;
static int tp_total;
static void (*fail_routine)(void);

static int scan_report(const char * name){
	FILE * f;
	cl_tp_t entry;
	cl_tp_t * update_entry;

	char line[512];
	char buf[512];
	char * p;
	char * last;


	f = fopen(name, "r");
	if ( f != NULL ){

		fgets(line, 512, f); //scan the header

		while( fgets(line, 512, f) != NULL ){

			strcpy(buf, line);

			p = strtok_r(buf, ",", &last);
			strcpy(entry.func, p);
			p = strtok_r(NULL, ",", &last);
			strcpy(entry.file, p);
			p = strtok_r(NULL, ",", &last);
			entry.line = atoi(p);
			p = strtok_r(NULL, ",", &last);
			strcpy(entry.desc, p);
			p = strtok_r(NULL, ",", &last);
			entry.count = atoi(p);
			p = strtok_r(NULL, ",", &last);
			entry.failed = atoi(p);

			update_entry = find_tp(entry.file, entry.line);
			if ( update_entry == NULL ){
				if ( (update_entry = add_tp(entry.file, entry.line, entry.func, entry.desc)) == NULL ){
					return -1;
				}
				update_entry->count = entry.count;
				update_entry->failed = entry.failed;

			} else {
				update_entry->count += entry.count;
				update_entry->failed += entry.failed;
			}
		}

		fclose(f);
	}

	//create the file with the headers
	f = fopen(name, "w");
	if (f == NULL ){
		cl_error("Failed to create %s\n", name);
		return -1;
	}

	fprintf(f, "func,file,line,desc,count,failed\n");
	fclose(f);
	return 0;

}


static int create_report(const char * name){
	FILE * f;
	int i;

	if ( scan_report(name) < 0 ){
		return -1;
	}


	f = fopen(name, "a");
	if( f == NULL ){
		return -1;
	}
	for(i=0; i < tp_total; i++){
		if ( tp_table[i].file[0] == 0 ){
			break;
		}

		fprintf(f, "%s,%s,%d,%s,%d,%d\n",
				tp_table[i].func,
				tp_table[i].file,
				tp_table[i].line,
				tp_table[i].desc,
				tp_table[i].count,
				tp_table[i].failed);
	}

	fclose(f);


	return 0;
}

static int update_table(void){
	int size;
	cl_tp_t * tmp;
	if ( tp_table == NULL ){
		size = sizeof(cl_tp_t) * UPDATE_TABLE_JUMP;
		tp_table = malloc(size);
		if ( tp_table == NULL ){
			return -1;
		}
		tp_total = UPDATE_TABLE_JUMP;
		memset(tp_table, 0, size);
	} else {
		tp_total += UPDATE_TABLE_JUMP;
		size = sizeof(cl_tp_t) * tp_total;
		tp_table = realloc(tp_table, size);
		if  (tp_table == NULL ){
			tmp = malloc(size);
			if ( tmp == NULL ){
				return -1;
			}
			memcpy(tmp, tp_table, sizeof(cl_tp_t)*(tp_total - UPDATE_TABLE_JUMP));
			free(tp_table);
			tp_table = tmp;
		}

		memset(&(tp_table[tp_total - UPDATE_TABLE_JUMP]), 0, sizeof(cl_tp_t)*UPDATE_TABLE_JUMP);
	}
	return 0;
}

static cl_tp_t * add_tp(const char * file, int line, const char * func, const char * desc){
	int i;
	if ( tp_table == NULL ){
		if ( update_table() < 0 ){
			return NULL; //could not add any more entries
		}
	}

	for(i=0; i < tp_total; i++){
		if ( tp_table[i].file[0] == 0 ){
			strncpy(tp_table[i].file, file, FILE_LEN);
			tp_table[i].line = line;
			strncpy(tp_table[i].func, func, FUNC_LEN);
			if ( desc != NULL ){
			strncpy(tp_table[i].desc, desc, DESC_LEN);
			} else {
				strncpy(tp_table[i].desc, "none", DESC_LEN);
			}
			tp_table[i].count = 0;
			tp_table[i].failed = 0;
			return &(tp_table[i]);
		}
	}
	return NULL;
}


static cl_tp_t * find_tp(const char * file, int line){
	int i;
	if ( tp_table == NULL ){
		return NULL;
	}
	for(i=0; i < tp_total; i++){
		if ( !strcmp(file, tp_table[i].file) && (tp_table[i].line == line)){
			return &(tp_table[i]);
		}
	}
	return NULL;
}


int cl_tp_createreport(const char * name){
	return create_report(name);
}

void cl_tp_setfailroutine(void (*routine)(void)){
	fail_routine = routine;
}


static cl_tp_t * find_tp_desc(const char * desc){
	int i;
	if ( tp_table == NULL ){
		return NULL;
	}
	for(i=0; i < tp_total; i++){
		if ( !strcmp(desc, tp_table[i].desc) ){
			return &(tp_table[i]);
		}
	}
	return NULL;
}


int cl_getcount(const char * desc){
	cl_tp_t * tp;

	tp = find_tp_desc(desc);
	if ( tp == NULL ){
		return -1;
	}

	return tp->count;
}

int cl_tp(const char * file, int line, const char * func, float failrate, const char * desc){
	cl_tp_t * tp;
	int num;
	//look for the entry
	tp = find_tp(file, line);
	if ( tp == NULL ){
		tp = add_tp(file, line, func, desc);
		if ( tp == NULL ){
			return -1;
		}
	}
	tp->count++;

	if ( rand_seed == 0 ){
		rand_seed = (int)time(NULL);
		srand(rand_seed);
	}

	num = rand();

	if ( num > (int)((1.0 - failrate) * RAND_MAX) ){
		tp->failed++;
		create_report("failreport.txt");
		if ( fail_routine != NULL ){
			fail_routine();
		}
		return 1;
	}
	return 0;
}



