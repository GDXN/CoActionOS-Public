/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include "dev/disk.h"
#include "../unistd/unistd_flags.h"
#include "../sched/sched_flags.h"

#include "cafs_lite.h"
#include "cafs_lite_dev.h"

#include "hwpl/debug.h"


static void set_delay_mutex(void * args){
	sched_table[ task_get_current() ].signal_delay_mutex = args;
}

int cl_dev_getlist_block(const void * cfg){
	const cafs_lite_cfg_t * cfgp = cfg;
	return cfgp->state->list_block;
}

void cl_dev_setlist_block(const void * cfg, int list_block){
	const cafs_lite_cfg_t * cfgp = cfg;
	cfgp->state->list_block = list_block;
}

void cl_dev_setdelay_mutex(pthread_mutex_t * mutex){
	hwpl_core_privcall(set_delay_mutex, mutex);
}

int cl_dev_getserialno(const void * cfg){
	const cafs_lite_cfg_t * cfgp = cfg;
	return cfgp->state->serialno;
}

void cl_dev_setserialno(const void * cfg, int serialno){
	const cafs_lite_cfg_t * cfgp = cfg;
	cfgp->state->serialno = serialno;
}

int cl_dev_open(const void * cfg){
	const cafs_lite_cfg_t * cfgp = cfg;

	cfgp->open_file->flags = O_RDWR;
	cfgp->open_file->loc = 0;
	cfgp->open_file->fs = cfgp->devfs;
	cfgp->open_file->handle = NULL;

	return cfgp->devfs->open(
			cfgp->devfs->cfg,
			&(cfgp->open_file->handle),
			cfgp->name,
			O_RDWR,
			0);
}

int cl_dev_write(const void * cfg, int loc, const void * buf, int nbyte){
	int ret;
	//int i;
	char buffer[nbyte];
	const cafs_lite_cfg_t * cfgp = cfg;
	if ( cfgp->open_file->fs == NULL ){
		errno = EIO;
		return -1;
	}
	cfgp->open_file->loc = loc;
	ret = unistd_device_write(cfgp->open_file, buf, nbyte);
	if( ret != nbyte ){
		//hwpl_debug("Only wrote %d bytes\n", ret);
		return -1;
	}
	memset(buffer, 0, nbyte);
	cfgp->open_file->loc = loc;
	unistd_device_read(cfgp->open_file, buffer, nbyte);
	if ( memcmp(buffer, buf, nbyte) != 0 ){
		/*
		hwpl_debug("bad verify\n");
		for(i=0; i < nbyte; i++){
			hwpl_debug("0x%X != 0x%X?\n", buffer[i], ((char*)buf)[i]);
		}
		*/
		return -1;
	}

	return ret;
}

int cl_dev_read(const void * cfg, int loc, void * buf, int nbyte){
	const cafs_lite_cfg_t * cfgp = cfg;
	if ( cfgp->open_file->fs == NULL ){
		errno = EIO;
		return -1;
	}
	cfgp->open_file->loc = loc;
	return unistd_device_read(cfgp->open_file, buf, nbyte);
}


int cl_dev_erase(const void * cfg){
	const cafs_lite_cfg_t * cfgp = cfg;
	if ( cfgp->open_file->fs == NULL ){
		errno = EIO;
		return -1;
	}
	int usec;
	if( unistd_device_ioctl(cfgp->open_file, I_DISK_ERASEDEVICE, NULL) < 0 ){
		return -1;
	}

	if( (usec = unistd_device_ioctl(cfgp->open_file, I_DISK_GETDEVICEERASETIME, NULL)) < 0 ){
		return -1;
	}
	usleep(usec);
	return 0;
}

int cl_dev_erasesection(const void * cfg, int loc){
	const cafs_lite_cfg_t * cfgp;
	int usec;
	cfgp = cfg;
	if ( cfgp->open_file->fs == NULL ){
		errno = EIO;
		return -1;
	}

	if( unistd_device_ioctl(cfgp->open_file, I_DISK_ERASEBLOCK, (void*)loc) < 0 ){
		return -1;
	}

	if( (usec = unistd_device_ioctl(cfgp->open_file, I_DISK_GETBLOCKERASETIME, NULL)) < 0 ){
		return -1;
	}

	usleep(usec);
	return 0;
}

int cl_dev_close(const void * cfg){
	return 0;
}

int cl_dev_getsize(const void * cfg){
	const cafs_lite_cfg_t * cfgp = cfg;
	if ( cfgp->open_file->fs == NULL ){
		errno = EIO;
		return -1;
	}
	return unistd_device_ioctl(cfgp->open_file, I_DISK_GETSIZE, NULL);
}

int cl_dev_geterasesize(const void * cfg){
	const cafs_lite_cfg_t * cfgp = cfg;
	if ( cfgp->open_file->fs == NULL ){
		errno = EIO;
		return -1;
	}
	return unistd_device_ioctl(cfgp->open_file, I_DISK_GETBLOCKSIZE, NULL);
}

