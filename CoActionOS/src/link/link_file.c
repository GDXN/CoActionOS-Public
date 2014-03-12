/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "link_flags.h"

#include <stdarg.h>

int link_open(link_phy_t handle, const char * path, int flags, ...){
	link_op_t op;
	link_reply_t reply;
	link_mode_t mode;
	int err;
	va_list ap;

	link_debug(5, "open %s 0%o\n", path, flags);

	if ( flags & LINK_O_CREAT ){
		va_start(ap, flags);
		mode = va_arg(ap, link_mode_t);
		va_end(ap);
	} else {
		mode = 0;
	}

	op.open.cmd = LINK_CMD_OPEN;
	op.open.path_size = strlen(path) + 1;
	op.open.flags = flags;
	op.open.mode = mode;

	link_debug(4, "Write open op (0x%lX)\n", (long unsigned int)link_dev);
	err = link_protocol_masterwrite(handle, &op, sizeof(link_open_t));
	if ( err < 0 ){
		link_error("failed to write open op with handle %d\n", handle);
		return link_handle_err(handle, err);
	}

	//Send the path on the bulk out endpoint
	link_debug(4, "Write open path (%d bytes)\n", op.open.path_size);
	err = link_protocol_masterwrite(handle, path, op.open.path_size);
	if ( err < 0 ){
		link_error("failed to write path\n");
		return link_handle_err(handle, err);
	}



	//read the reply to see if the file opened correctly
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		link_error("failed to read the reply\n");
		return link_handle_err(handle, err);
	}

	link_errno = reply.err_number;
	if ( reply.err < 0 ){
		link_debug(1, "Failed to open file (%d) (%d)\n", link_errno, reply.err);
		return LINK_DEV_ERROR;
	}
	return reply.err;
}


int link_ioctl(link_phy_t handle, int fildes, int request, ...){
	void * argp;
	int arg;
	va_list ap;
	if ( _IOCTL_IOCTLRW(request) ){
		va_start(ap, request);
		argp = va_arg(ap, void*);
		va_end(ap);
		arg = 0;
	} else {
		va_start(ap, request);
		arg = va_arg(ap, int);
		va_end(ap);
		argp = NULL;
	}

	return link_ioctl_delay(handle, fildes, request, argp, arg, 0);
}


int link_ioctl_delay(link_phy_t handle, int fildes, int request, void * argp, int arg, int delay){
	int rw_size;
	link_op_t op;
	link_reply_t reply;

	int err;

	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	link_debug(3, "Request is 0x%X (0x%X)\n", (unsigned int)request, (unsigned int)I_BOOTLOADER_GETATTR);

	rw_size = _IOCTL_SIZE(request);

	link_debug(2, "Sending IOCTL request %c %d r:%X w:%X\n", _IOCTL_IDENT(request),
			_IOCTL_SIZE(request),
			_IOCTL_IOCTLR(request) != 0,
			_IOCTL_IOCTLW(request) != 0);

	//execute the request
	op.ioctl.fildes = fildes;
	op.ioctl.cmd = LINK_CMD_IOCTL;
	op.ioctl.request = request;
	op.ioctl.arg = arg;
	err = link_protocol_masterwrite(handle, &op, sizeof(link_ioctl_t));
	if ( err < 0 ){
		link_error("failed to write op\n");
		return link_handle_err(handle, err);
	}

	if( _IOCTL_IOCTLW(request) ){
		//need to write data to the bulk endpoint (argp)
		link_debug(3, "Sending IOW data\n");
		err = link_protocol_masterwrite(handle, argp, rw_size);
		if ( err < 0 ){
			link_error("failed to write IOW data\n");
			return link_handle_err(handle, err);
		}
	}

	if( delay > 0 ){
		link_debug(3, "Delay for %dms\n", delay);
		link_phy_wait(delay);
	}

	if( _IOCTL_IOCTLR(request) ){
		//need to read data from the bulk endpoint
		link_debug(3, "Getting IOR data %d bytes\n", rw_size);
		err = link_protocol_masterread(handle, argp, rw_size);
		link_debug(3, "Getting IOR data done (%d)\n", err);

		if ( err < 0 ){
			link_error("failed to read IOR data\n");
			return link_handle_err(handle, err);
		}
	}

	//Get the reply
	link_debug(3, "Read reply\n");
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		link_error("failed to read reply\n");
		return link_handle_err(handle, err);
	}

	link_errno = reply.err_number;
	link_debug(3, "Replied with %d (%d)\n", reply.err, link_errno);
	if ( reply.err < 0 ){
		link_debug(1, "Failed to ioctl file (%d)\n", link_errno);
		return LINK_DEV_ERROR;
	}
	return reply.err;
}


int link_read(link_phy_t handle, int fildes, void * buf, int nbyte){
	link_op_t op;
	link_reply_t reply;
	int err;
	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	op.read.cmd = LINK_CMD_READ;
	op.read.fildes = fildes;
	op.read.nbyte = nbyte;


	link_debug(4, "write read op\n");
	err = link_protocol_masterwrite(handle, &op, sizeof(link_read_t));
	if ( err < 0 ){
		link_error("failed to write op\n");
		return link_handle_err(handle, err);
	}

	link_debug(4, "read data from the file %d\n", nbyte);
	err = link_protocol_masterread(handle, buf, nbyte);
	if ( err < 0 ){
		link_error("failed to read data\n");
		return link_handle_err(handle, err);
	}

	link_debug(4, "read %d of %d bytes\n", err, nbyte);
	link_debug(4, "read reply\n");
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		link_error("failed to read reply\n");
		return link_handle_err(handle, err);
	}

	if ( reply.err < 0 ){
		link_errno = reply.err_number;
	}

	link_debug(4, "Read %d bytes from device\n", reply.err);

	return reply.err;
}

int link_write(link_phy_t handle, int fildes, const void * buf, int nbyte){
	link_op_t op;
	link_reply_t reply;
	int err;
	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	op.write.cmd = LINK_CMD_WRITE;
	op.write.fildes = fildes;
	op.write.nbyte = nbyte;

	link_debug(4, "Send op (fildes %d nbyte %d)\n", op.write.fildes, op.write.nbyte);
	err = link_protocol_masterwrite(handle, &op, sizeof(link_write_t));
	if ( err < 0 ){
		link_error("failed to write op\n");
		return link_handle_err(handle, err);
	}

	link_debug(4, "Write data\n");
	err = link_protocol_masterwrite(handle, buf, nbyte);
	if ( err < 0 ){
		link_error("failed to write data\n");
		return link_handle_err(handle, err);
	}

	//read the reply to see if the file wrote correctly
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		link_error("failed to read reply\n");
		return link_handle_err(handle, err);
	}

	if ( reply.err < 0 ){
		link_error("write failed (%d)\n", reply.err_number);
		link_errno = reply.err_number;
		return LINK_DEV_ERROR;
	}
	return reply.err;
}

int link_close(link_phy_t handle, int fildes){
	link_op_t op;
	link_reply_t reply;
	int err;
	op.close.cmd = LINK_CMD_CLOSE;
	op.close.fildes = fildes;
	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}
	link_debug(4, "Send Op to close fildes:%d\n", fildes);
	err = link_protocol_masterwrite(handle, &op, sizeof(link_close_t));
	if ( err < 0 ){
		link_error("failed to write op\n");
		return link_handle_err(handle, err);
	}

	link_debug(4, "Read Reply\n");
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		link_error("failed to read reply\n");
		return link_handle_err(handle, err);
	}

	if ( reply.err < 0 ){
		link_errno = reply.err_number;
		return LINK_DEV_ERROR;
	}

	return reply.err;
}

int link_symlink(link_phy_t handle, const char * old_path, const char * new_path){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;
	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}
	op.symlink.cmd = LINK_CMD_LINK;
	op.symlink.path_size_old = strlen(old_path) + 1;
	op.symlink.path_size_new = strlen(new_path) + 1;

	link_debug(4, "Write link op (0x%lX)\n", (long unsigned int)link_dev);
	err = link_protocol_masterwrite(handle, &op, sizeof(link_symlink_t));
	if ( err < 0 ){
		return err;
	}

	//Send the path on the bulk out endpoint
	link_debug(4, "Write open path (%d bytes)\n", op.symlink.path_size_old);
	len = link_protocol_masterwrite(handle, old_path, op.symlink.path_size_old);
	if (len != op.symlink.path_size_old ){
		return -1;
	}

	len = link_protocol_masterwrite(handle, new_path, op.symlink.path_size_new);
	if ( len < 0 ){
		return LINK_TRANSFER_ERR;
	}

	//read the reply to see if the file opened correctly
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		return err;
	}

	if ( reply.err < 0 ){
		link_errno = reply.err_number;
	}
	return reply.err;
}

int link_unlink(link_phy_t handle, const char * path){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;
	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	link_debug(5, "unlink %s\n", path);

	op.unlink.cmd = LINK_CMD_UNLINK;
	op.unlink.path_size = strlen(path) + 1;

	err = link_protocol_masterwrite(handle, &op, sizeof(link_unlink_t));
	if ( err < 0 ){
		return err;
	}

	//Send the path on the bulk out endpoint
	len = link_protocol_masterwrite(handle, path, op.unlink.path_size);
	if ( len < 0 ){
		return LINK_TRANSFER_ERR;
	}
	//read the reply to see if the file opened correctly
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		return err;
	}

	if ( reply.err < 0 ){
		link_errno = reply.err_number;
	}
	return reply.err;
}


int link_lseek(link_phy_t handle, int fildes, off_t offset, int whence){
	link_op_t op;
	link_reply_t reply;
	int err;

	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}
	op.lseek.cmd = LINK_CMD_LSEEK;
	op.lseek.fildes = fildes;
	op.lseek.offset = offset;
	op.lseek.whence = whence;

	link_errno = 0;
	link_debug(4, "seek command 0x%X (0x%lX)\n", fildes, (long unsigned int)link_dev);

	err = link_protocol_masterwrite(handle, &op, sizeof(link_lseek_t));
	if ( err < 0 ){
		return err;
	}

	link_debug(4, "wait for reply\n");
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		return err;
	}

	link_debug(4, "device returned %d %d\n", reply.err, reply.err_number);
	if ( reply.err < 0 ){
		link_errno = reply.err_number;
	}
	return reply.err;
}

int link_stat(link_phy_t handle, const char * path, struct link_stat * buf){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;

	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	link_debug(5, "stat %s\n", path);

	op.stat.cmd = LINK_CMD_STAT;
	op.stat.path_size = strlen(path) + 1;

	link_debug(6, "send op %d path size %d\n", op.stat.cmd, op.stat.path_size);
	err = link_protocol_masterwrite(handle, &op, sizeof(link_stat_t));
	if ( err < 0 ){
		return err;
	}

	//Send the path on the bulk out endpoint
	link_debug(6, "write stat path\n");
	len = link_protocol_masterwrite(handle, path, op.stat.path_size);
	if ( len < 0 ){
		return LINK_TRANSFER_ERR;
	}

	//Get the reply
	link_debug(6, "read stat reply\n");
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		link_error("failed to read reply\n");
		return err;
	}

	link_debug(8, "stat reply %d\n", reply.err);
	if ( reply.err == 0 ){
		//Read bulk in as the size of the the new data
		link_debug(6, "read stat data\n");
		err = link_protocol_masterread(handle, buf, sizeof(struct link_stat));
		if ( err < 0 ){
			return err;
		}
	} else {
		link_errno = reply.err_number;
	}

	return reply.err;

}

int link_fstat(link_phy_t handle, int fildes, struct link_stat * buf){
	link_op_t op;
	link_reply_t reply;
	int err;

	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	op.fstat.cmd = LINK_CMD_FSTAT;
	op.fstat.fildes = fildes;

	err = link_protocol_masterwrite(handle, &op, sizeof(link_fstat_t));
	if ( err < 0 ){
		return err;
	}

	//Get the reply
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		return err;
	}

	if ( reply.err == 0 ){
		//Read bulk in as the size of the the new data
		err = link_protocol_masterread(handle, buf, sizeof(struct link_stat));
		if ( err < 0 ){
			return err;
		}

	} else {
		link_errno = reply.err_number;
	}

	return reply.err;
}

int link_rename(link_phy_t handle, const char * old_path, const char * new_path){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;
	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	link_debug(5, "rename %s to %s\n", old_path, new_path);

	op.rename.cmd = LINK_CMD_RENAME;
	op.rename.old_size = strlen(old_path) + 1;
	op.rename.new_size = strlen(new_path) + 1;

	err = link_protocol_masterwrite(handle, &op, sizeof(link_rename_t));
	if ( err < 0 ){
		return err;
	}

	//Send the old path on the bulk out endpoint
	len = link_protocol_masterwrite(handle, old_path, op.rename.old_size);
	if ( len < 0 ){
		return LINK_TRANSFER_ERR;
	}

	//Send the new path on the bulk out endpoint
	len = link_protocol_masterwrite(handle, new_path, op.rename.new_size);
	if ( len < 0 ){
		return LINK_TRANSFER_ERR;
	}

	//read the reply to see if the file opened correctly
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		return err;
	}

	if ( reply.err < 0 ){
		link_errno = reply.err_number;
	}
	return reply.err;
}

int link_chown(link_phy_t handle, const char * path, int owner, int group){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;

	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	link_debug(5, "chown %s %d %d\n", path, owner, group);

	op.chown.cmd = LINK_CMD_CHOWN;
	op.chown.path_size = strlen(path) + 1;
	op.chown.uid = owner;
	op.chown.gid = group;

	link_debug(3, "Write op\n");
	err = link_protocol_masterwrite(handle, &op, sizeof(op));
	if ( err < 0 ){
		return err;
	}

	//Send the path on the bulk out endpoint
	link_debug(3, "Write path\n");
	len = link_protocol_masterwrite(handle, path, op.chown.path_size);
	if ( len < 0 ){
		return LINK_TRANSFER_ERR;
	}

	//read the reply to see if the file opened correctly
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		return err;
	}

	link_errno = reply.err_number;
	return reply.err;
}

int link_chmod(link_phy_t handle, const char * path, int mode){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;

	if ( link_dev == NULL ){
		return -1;
	}

	link_debug(5, "chmod %s %d\n", path, mode);

	op.chmod.cmd = LINK_CMD_CHMOD;
	op.chmod.path_size = strlen(path) + 1;
	op.chmod.mode = mode;

	link_debug(3, "Write op\n");
	err = link_protocol_masterwrite(handle, &op, sizeof(op));
	if ( err < 0 ){
		return err;
	}

	//Send the path on the bulk out endpoint
	link_debug(3, "Write path\n");
	len = link_protocol_masterwrite(handle, path, op.chown.path_size);
	if ( len < 0 ){
		return LINK_TRANSFER_ERR;
	}

	//read the reply to see if the file opened correctly
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		return err;
	}

	link_errno = reply.err_number;
	return reply.err;
}




/*! @} */

