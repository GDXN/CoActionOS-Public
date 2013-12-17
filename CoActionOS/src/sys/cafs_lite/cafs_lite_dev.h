/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#ifndef CAFS_DEV_H_
#define CAFS_DEV_H_

#include <pthread.h>

int cl_dev_open(const void * cfg);
int cl_dev_write(const void * cfg, int loc, const void * buf, int nbyte);
int cl_dev_read(const void * cfg, int loc, void * buf, int nbyte);
int cl_dev_close(const void * cfg);

int cl_dev_getsize(const void * cfg);
int cl_dev_geterasesize(const void * cfg);

int cl_dev_erase(const void * cfg);
int cl_dev_erasesection(const void * cfg, int loc);

int sysfs_access(int file_mode, int file_uid, int file_gid, int amode);
int cl_sys_geteuid(void);
int cl_sys_getegid(void);

int cl_dev_getlist_block(const void * cfg);
void cl_dev_setlist_block(const void * cfg, int list_block);
int cl_dev_getserialno(const void * cfg);
void cl_dev_setserialno(const void * cfg, int serialno);

void cl_dev_setdelay_mutex(pthread_mutex_t * mutex);

#endif /* CAFS_DEV_H_ */
