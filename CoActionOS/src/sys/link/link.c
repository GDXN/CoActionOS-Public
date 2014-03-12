/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup LINK
 * @{
 *
 */

//#include "config.h"

#include <stdbool.h>
#include <sys/fcntl.h> //Defines the flags
#include <errno.h>
#include <dirent.h>
#include <pthread.h>
#include "caos.h"
#include "hwpl/flash.h"
#include "hwpl/debug.h"

#include "caos.h"
#include "link.h"
#include "link_local.h"

#include "../syscalls/process/syscalls_process.h"
#include "../sched/sched_flags.h"

static link_phy_t phy_handle;

#define FLASH_FILDES 0xFF778833

#define SERIAL_NUM_WIDTH 3

#define LINK_DEBUG 0
#define link_debug(...) do { if ( LINK_DEBUG == 1 ){ hwpl_debug("%s:", __func__); hwpl_debug(__VA_ARGS__); } } while(0)

static int read_device(int fildes, int size);
static int write_device(int fildes, int size);
static int read_device_callback(void * context, void * buf, int nbyte);
static int write_device_callback(void * context, void * buf, int nbyte);
static void translate_link_stat(struct link_stat * dest, struct stat * src);

typedef struct {
	link_op_t op;
	link_reply_t reply;
	//char path[PATH_MAX];
	//char path_new[PATH_MAX];  //Used only with LINK_CMD_LINK (Symbolic link creation)
} link_data_t;


static void link_cmd_none(link_data_t * args);
static void link_cmd_readserialno(link_data_t * args);
static void link_cmd_ioctl(link_data_t * args);
static void link_cmd_read(link_data_t * args);
static void link_cmd_write(link_data_t * args);
static void link_cmd_open(link_data_t * args);
static void link_cmd_close(link_data_t * args);
static void link_cmd_link(link_data_t * args);
static void link_cmd_unlink(link_data_t * args);
static void link_cmd_lseek(link_data_t * args);
static void link_cmd_stat(link_data_t * args);
static void link_cmd_fstat(link_data_t * args);
static void link_cmd_mkdir(link_data_t * args);
static void link_cmd_rmdir(link_data_t * args);
static void link_cmd_opendir(link_data_t * args);
static void link_cmd_readdir(link_data_t * args);
static void link_cmd_closedir(link_data_t * args);
static void link_cmd_rename(link_data_t * args);
static void link_cmd_chown(link_data_t * args);
static void link_cmd_chmod(link_data_t * args);
static void link_cmd_exec(link_data_t * args);
static void link_cmd_mkfs(link_data_t * args);


void (* const link_cmd_func_table[LINK_CMD_TOTAL])(link_data_t*) = {
		link_cmd_none,
		link_cmd_readserialno,
		link_cmd_ioctl,
		link_cmd_read,
		link_cmd_write,
		link_cmd_open,
		link_cmd_close,
		link_cmd_link,
		link_cmd_unlink,
		link_cmd_lseek,
		link_cmd_stat,
		link_cmd_fstat,
		link_cmd_mkdir,
		link_cmd_rmdir,
		link_cmd_opendir,
		link_cmd_readdir,
		link_cmd_closedir,
		link_cmd_rename,
		link_cmd_chown,
		link_cmd_chmod,
		link_cmd_exec,
		link_cmd_mkfs
};


int link_init(void){
	if( (phy_handle = link_phy_open(NULL, 0)) == LINK_PHY_ERROR){
		//hwpl_debug("failed to init phy\n");
		return -1;
	}
	return 0;
}

void * link_update(void * arg){
	int err;
	link_data_t data;
	char packet_data[LINK_MAX_TRANSFER_SIZE];
	data.op.cmd = 0;
	err = 0;


	hwpl_debug("start link update\n");
	while(1){

		//Wait for data to arrive on the link transport device
		while( 1 ){

			if ( (err = link_protocol_slaveread(phy_handle, &packet_data, LINK_MAX_TRANSFER_SIZE, NULL, NULL)) <= 0 ){
				if ( err == 0 ){
					//hwpl_debug("zero len packet\n");
				}
				//hwpl_debug("recv err %d\n", err);
				continue;
			}

			memcpy(&data.op, packet_data, sizeof(data.op));


			if( err > 0 ){
				break;
			}

		}

		if ( data.op.cmd < LINK_CMD_TOTAL ){
			link_cmd_func_table[data.op.cmd](&data);
		} else {
			data.reply.err = -1;
			data.reply.err_number = EINVAL;
		}

		//send the reply
		if( data.op.cmd != 0 ){
			link_protocol_slavewrite(phy_handle, &data.reply, sizeof(data.reply), NULL, NULL);
			data.op.cmd = 0;
		}

	}
	return NULL;
}

void link_cmd_none(link_data_t * args){
}

void link_cmd_isbootloader(link_data_t * args){
	args->reply.err = 0; //this is not the bootloader
}

static void priv_get_serialno(void * dest){
	core_attr_t attr;
	int i, j;
	char * p = dest;
	hwpl_core_getattr(0, &attr);
	for(j=SERIAL_NUM_WIDTH; j >= 0; j--){
		for(i=0; i < 8; i++){
			*p++ = htoc((attr.serial_number[j] >> 28) & 0x0F);
			attr.serial_number[j] <<= 4;
		}
	}
}

void link_cmd_readserialno(link_data_t * args){
	char serialno[LINK_PACKET_DATA_SIZE];
	memset(serialno, 0, LINK_PACKET_DATA_SIZE);
	hwpl_core_privcall(priv_get_serialno, serialno);

	args->reply.err = strlen(serialno);
	args->reply.err_number = 0;

	if( link_protocol_slavewrite(phy_handle, &args->reply, sizeof(args->reply), NULL, NULL) < 0 ){
		return;
	}

	if( link_protocol_slavewrite(phy_handle, serialno, args->reply.err, NULL, NULL) < 0 ){
		return;
	}

	args->op.cmd = 0; //reply was already sent
}

void link_cmd_open(link_data_t * args){
	char path[PATH_MAX];
	args->reply.err = link_protocol_slaveread(phy_handle, path, args->op.open.path_size, NULL, NULL);
	args->reply.err = open(path, args->op.open.flags, args->op.open.mode);
	if ( args->reply.err < 0 ){
		link_debug("Failed to open %s (%d)\n", path, errno);
		args->reply.err_number = errno;
	}
}

void link_cmd_link(link_data_t * args){
	char path[PATH_MAX];
	char path_new[PATH_MAX];
	args->reply.err = link_protocol_slaveread(phy_handle, path, args->op.symlink.path_size_old, NULL, NULL); //send final ack

	if ( args->reply.err == args->op.symlink.path_size_old ){
		args->reply.err = link_protocol_slaveread(phy_handle, path_new, args->op.symlink.path_size_new, NULL, NULL); //don't send final ack
		args->reply.err = link(path, path_new);
		args->reply.err = 0;
		if ( args->reply.err < 0 ){
			link_debug("Failed to link %s (%d)\n", path, errno);
			args->reply.err_number = errno;
		}
	}
}

void link_cmd_ioctl(link_data_t * args){
	int err;
	int size;
	size = _IOCTL_SIZE(args->op.ioctl.request);
	void * io_buf;
	io_buf = malloc(size);
	if ( _IOCTL_IOCTLW(args->op.ioctl.request) != 0 ){ //this means data is being sent over the bulk interrupt
		if( link_protocol_slaveread(phy_handle, io_buf, size, NULL, NULL) == -1 ){
			args->op.cmd = 0;
			args->reply.err = -1;
			return;
		}
	}

	if( args->op.ioctl.fildes != phy_handle ){
		if ( _IOCTL_IOCTLRW(args->op.ioctl.request) == 0 ){
			//This means the third argument is just an integer
			args->reply.err = ioctl(args->op.ioctl.fildes, args->op.ioctl.request, args->op.ioctl.arg);
		} else {
			//This means a read or write is happening and the pointer should be passed
			if( io_buf != NULL ){
				args->reply.err = ioctl(args->op.ioctl.fildes, args->op.ioctl.request, io_buf);
			} else {
				args->reply.err = -1;
				args->reply.err_number = ENOMEM;
			}
		}
	} else {
		args->reply.err = -1;
		args->reply.err_number = EBADF;
	}

	//Check to see if this is a read operation and data must be sent back to the host
	if ( _IOCTL_IOCTLR(args->op.ioctl.request) != 0 ){
		//If the ioctl function wrote data to the ctl argument, pass the data over the link
		err = link_protocol_slavewrite(phy_handle, io_buf, size, NULL, NULL);
		if ( err == -1 ){
			args->op.cmd = 0;
			args->reply.err = -1;
		}
	}

	if ( args->reply.err < 0 ){
		link_debug("Failed to ioctl (%d)\n", errno);
		args->reply.err_number = errno;
	}

	if( io_buf != NULL ){
		free(io_buf);
	}

}

void link_cmd_read(link_data_t * args){
	if( args->op.ioctl.fildes != phy_handle ){
		args->reply.err = read_device(args->op.read.fildes, args->op.read.nbyte);
	}  else {
		args->reply.err = -1;
		args->reply.err_number = EBADF;
	}
	if ( args->reply.err < 0 ){
		link_debug("Failed to read (%d)\n", errno);
		args->reply.err_number = errno;
	}
}

void link_cmd_write(link_data_t * args){
	if( args->op.ioctl.fildes != phy_handle ){
		args->reply.err = write_device(args->op.write.addr, args->op.write.nbyte);
	}  else {
		args->reply.err = -1;
		args->reply.err_number = EBADF;
	}
	if ( args->reply.err < 0 ){
		link_debug("Failed to write (%d)\n", errno);
		args->reply.err_number = errno;
	}
}

void link_cmd_read_flash(link_data_t * args){
}

void link_cmd_write_flash(link_data_t * args){
}

void link_cmd_erase_flash(link_data_t * args){
}

void link_cmd_close(link_data_t * args){
	if( args->op.ioctl.fildes != phy_handle ){
		args->reply.err = close(args->op.close.fildes);
	}  else {
		args->reply.err = -1;
		args->reply.err_number = EBADF;
	}
	if ( args->reply.err < 0 ){
		link_debug("Failed to close (%d)\n", errno);
		args->reply.err_number = errno;
	}
}

void link_cmd_unlink(link_data_t * args){
	char path[PATH_MAX];
	if( link_protocol_slaveread(phy_handle, path, args->op.unlink.path_size, NULL, NULL) < 0 ){
		return;
	}
	args->reply.err = unlink(path);
	if ( args->reply.err < 0 ){
		link_debug("Failed to unlink (%d)\n", errno);
		args->reply.err_number = errno;
	}

}

void link_cmd_lseek(link_data_t * args){
	args->reply.err = lseek(args->op.lseek.fildes, args->op.lseek.offset, args->op.lseek.whence);
	if ( args->reply.err < 0 ){
		args->reply.err_number = errno;
	}
}

void link_cmd_stat(link_data_t * args){
	char path[PATH_MAX];
	int err;
	struct stat st;
	struct link_stat lst;

	link_protocol_slaveread(phy_handle, path, args->op.stat.path_size, NULL, NULL);

	args->reply.err = stat(path, &st);
	if (args->reply.err < 0 ){
		link_debug("Failed to stat (%d)\n", errno);
		args->reply.err_number = errno;
	}

	translate_link_stat(&lst, &st);
	args->op.cmd = 0;


	//Send the reply
	link_protocol_slavewrite(phy_handle, &args->reply, sizeof(link_reply_t), NULL, NULL);
	if ( args->reply.err < 0 ){
		return;
	}

	//now send the data
	err = link_protocol_slavewrite(phy_handle, &lst, sizeof(struct link_stat), NULL, NULL);
	if ( err < -1 ){
		return;
	}

}

void link_cmd_fstat(link_data_t * args){
	int err;
	struct stat st;
	struct link_stat lst;
	args->reply.err = fstat(args->op.fstat.fildes, (struct stat*)&st);
	if ( args->reply.err == -1 ){
		args->reply.err_number = errno;
	}

	translate_link_stat(&lst, &st);
	args->op.cmd = 0;

	//Send the reply
	link_protocol_slavewrite(phy_handle, &args->reply, sizeof(link_reply_t), NULL, NULL);
	if ( args->reply.err < -1 ){
		return;
	}


	//now send the data
	err = link_protocol_slavewrite(phy_handle, &lst, sizeof(struct link_stat), NULL, NULL);
	if ( err < -1 ){
		return;
	}

}

void link_cmd_mkdir(link_data_t * args){
	char path[PATH_MAX];
	link_protocol_slaveread(phy_handle, path, args->op.mkdir.path_size, NULL, NULL);
	args->reply.err = mkdir(path, args->op.mkdir.mode);
	if ( args->reply.err == -1 ){
		args->reply.err_number = errno;
	}

}

void link_cmd_rmdir(link_data_t * args){
	char path[PATH_MAX];
	link_protocol_slaveread(phy_handle, path, args->op.rmdir.path_size, NULL, NULL);
	args->reply.err = rmdir(path);
	if ( args->reply.err == -1 ){
		args->reply.err_number = errno;
	}

}

void link_cmd_opendir(link_data_t * args){
	char path[PATH_MAX];
	link_protocol_slaveread(phy_handle, path, args->op.opendir.path_size, NULL, NULL);
	args->reply.err = (int)opendir(path);
	if ( args->reply.err  == 0 ){
		link_debug("Failed to open dir %s (%d)\n", path, errno);
		args->reply.err_number = errno;
	}
}

void link_cmd_readdir(link_data_t * args){
	struct dirent de;
	struct link_dirent lde;

	args->reply.err = readdir_r((DIR*)args->op.readdir.dirp, &de, NULL);

	if ( args->reply.err < 0 ){
		args->reply.err = -1;
		args->reply.err_number = errno;
		if ( errno != 2 ){
			link_debug("Failed to read dir (%d)\n", errno);
		}
	}

	args->op.cmd = 0;

	lde.d_ino = de.d_ino;
	strcpy(lde.d_name, de.d_name);

	if( link_protocol_slavewrite(phy_handle, &args->reply, sizeof(link_readdir_reply_t), NULL, NULL) < 0 ){
		return;
	}

	if( args->reply.err < 0 ){
		return;
	}

	link_protocol_slavewrite(phy_handle, &lde, sizeof(struct link_dirent), NULL, NULL);
}

void link_cmd_closedir(link_data_t * args){
	args->reply.err = closedir((DIR*)args->op.closedir.dirp);
	if ( args->reply.err < 0 ){
		args->reply.err_number = errno;
	}
}


void link_cmd_rename(link_data_t * args){
	char path_new[PATH_MAX];
	char path[PATH_MAX];
	args->reply.err = 0;
	if( link_protocol_slaveread(phy_handle, path, args->op.rename.old_size, NULL, NULL) != args->op.rename.old_size){
		args->reply.err = -1;
	}
	if( link_protocol_slaveread(phy_handle, path_new, args->op.rename.new_size, NULL, NULL) != args->op.rename.new_size){
		args->reply.err = -1;
	}
	if( args->reply.err < 0 ){
		return;
	}
	args->reply.err = rename(path, path_new);
	if ( args->reply.err < 0 ){
		args->reply.err_number = errno;
	}

}

void link_cmd_chown(link_data_t * args){
	char path[PATH_MAX];
	if( link_protocol_slaveread(phy_handle, path, args->op.chown.path_size, NULL, NULL) != args->op.chown.path_size){
		args->reply.err = -1;
	}
	if( args->reply.err < 0 ){
		return;
	}
	args->reply.err = chown(path, args->op.chown.uid, args->op.chown.gid);
	if ( args->reply.err == -1 ){
		args->reply.err_number = errno;
	}

}

void link_cmd_chmod(link_data_t * args){
	char path[PATH_MAX];
	if( link_protocol_slaveread(phy_handle, path, args->op.chmod.path_size, NULL, NULL) != args->op.chmod.path_size){
		args->reply.err = -1;
	}
	if( args->reply.err < 0 ){
		return;
	}
	args->reply.err = chmod(path, args->op.chmod.mode);
	if ( args->reply.err == -1 ){
		args->reply.err_number = errno;
	}
}


void link_cmd_exec(link_data_t * args){
	char path[PATH_MAX];
	memset(path, 0, PATH_MAX);
	args->reply.err = link_protocol_slaveread(phy_handle, path, args->op.exec.path_size, NULL, NULL);
	args->reply.err = process_start(path, NULL, NULL);
	if ( args->reply.err < 0 ){
		link_debug("Failed to exec %s (%d)\n", path, errno);
		args->reply.err_number = errno;
	}
}

void link_cmd_mkfs(link_data_t * args){
	char path[PATH_MAX];
	memset(path, 0, PATH_MAX);
	args->reply.err = link_protocol_slaveread(phy_handle, path, args->op.exec.path_size, NULL, NULL);
	args->reply.err = mkfs(path);
	if ( args->reply.err < 0 ){
		link_debug("Failed to exec %s (%d)\n", path, errno);
		args->reply.err_number = errno;
	}
}

int read_device_callback(void * context, void * buf, int nbyte){
	int * fildes;
	int ret;
	fildes = context;
	ret = read(*fildes, buf, nbyte);
	return ret;
}

int write_device_callback(void * context, void * buf, int nbyte){
	int * fildes;
	int ret;
	fildes = context;
	ret =  write(*fildes, buf, nbyte);
	return ret;
}

int read_device(int fildes, int nbyte){
	return link_protocol_slavewrite(phy_handle, NULL, nbyte, read_device_callback, &fildes);
}

int write_device(int fildes, int nbyte){
	return link_protocol_slaveread(phy_handle, NULL, nbyte, write_device_callback, &fildes);
}

void translate_link_stat(struct link_stat * dest, struct stat * src){
	dest->st_dev = src->st_dev;
	dest->st_ino = src->st_ino;
	dest->st_mode = src->st_mode;
	dest->st_uid = src->st_uid;
	dest->st_gid = src->st_gid;
	dest->st_rdev = src->st_rdev;
	dest->st_size = src->st_size;
	dest->st_mtime_ = src->st_mtime;
	dest->st_ctime_ = src->st_ctime;
	dest->st_blksize = src->st_blksize;
	dest->st_blocks = src->st_blocks;
}



/*! @} */

