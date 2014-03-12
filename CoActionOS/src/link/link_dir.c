/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hwpl.h"
#include "link_flags.h"


//Access to directories
int link_mkdir(link_phy_t handle, const char * path, link_mode_t mode){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;

	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	link_debug(5, "mkdir %s 0%o\n", path, mode);

	op.mkdir.cmd = LINK_CMD_MKDIR;
	op.mkdir.mode = mode;
	op.mkdir.path_size = strlen(path) + 1;

	link_debug(3, "Write op\n");
	err = link_protocol_masterwrite(handle, &op, sizeof(op));
	if ( err < 0 ){
		return err;
	}

	//Send the path on the bulk out endpoint
	link_debug(3, "Write path\n");
	len = link_protocol_masterwrite(handle, path, op.mkdir.path_size);
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

int link_rmdir(link_phy_t handle, const char * path){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;

	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	link_debug(5, "rmdir %s\n", path);

	op.rmdir.cmd = LINK_CMD_RMDIR;
	op.rmdir.path_size = strlen(path) + 1;

	err = link_protocol_masterwrite(handle, &op, sizeof(op));
	if ( err < 0 ){
		return err;
	}

	//Send the path on the bulk out endpoint
	len = link_protocol_masterwrite(handle, path, op.rmdir.path_size);
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

int link_opendir(link_phy_t handle, const char * dirname){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;

	if ( link_dev == NULL ){
		link_error("No device\n");
		return 0;
	}

	op.opendir.cmd = LINK_CMD_OPENDIR;
	op.opendir.path_size = strlen(dirname) + 1;

	if ( dirname == NULL ){
		link_error("Directory name is NULL\n");
		return 0;
	}

	link_debug(3, "Write op\n");
	err = link_protocol_masterwrite(handle, &op, sizeof(link_opendir_t));
	if ( err < 0 ){
		link_error("Failed to transfer command\n");
		return 0;
	}

	//Send the path on the bulk out endpoint
	link_debug(3, "Write path %s\n", dirname);
	len = link_protocol_masterwrite(handle, dirname, op.opendir.path_size);
	if ( len < 0 ){
		link_error("Failed to write bulk out\n");
		return 0;
	}

	link_debug(3, "Write path len is %d 0x%X\n", len, reply.err);
	//read the reply to see if the file opened correctly
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		link_error("Failed to read bulk in\n");
		return 0;
	}

	link_errno = reply.err_number;

	link_debug(3, "Opened directory 0x%X\n", reply.err);

	return reply.err;
}

int link_readdir_r(link_phy_t handle, int dirp, struct link_dirent * entry, struct link_dirent ** result){
	link_op_t op;
	link_reply_t reply;
	int len;

	if ( link_dev == NULL ){
		return -1;
	}

	op.readdir.cmd = LINK_CMD_READDIR;
	op.readdir.dirp = dirp;


	if ( result != NULL ){
		*result = NULL;
	}

	link_debug(5, "Write op\n");
	if (link_protocol_masterwrite(handle, &op, sizeof(link_readdir_t)) < 0){
		return -1;
	}

	link_debug(5, "Read reply\n");
	if (link_protocol_masterread(handle, &reply, sizeof(reply)) < 0){
		return -1;
	}

	if( reply.err < 0 ){
		link_errno = reply.err_number;
		return reply.err;
	}

	//Read the bulk in buffer for the result of the read
	link_debug(5, "Read link dirent\n");
	len = link_protocol_masterread(handle, entry, sizeof(struct link_dirent));
	if ( len < 0 ){
		return -1;
	}

	if ( result != NULL ){
		*result = entry;
	}

	return 0;
}

int link_closedir(link_phy_t handle, int dirp){
	link_op_t op;
	link_reply_t reply;
	int err;

	if ( link_dev == NULL ){
		return LINK_TRANSFER_ERR;
	}

	op.closedir.cmd = LINK_CMD_CLOSEDIR;
	op.closedir.dirp = dirp;

	link_debug(3, "Send op\n");
	err = link_protocol_masterwrite(handle, &op, sizeof(link_closedir_t));
	if ( err < 0 ){
		return err;
	}

	link_debug(3, "Get Reply\n");
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		return err;
	}

	link_errno = reply.err_number;

	return reply.err;

}
