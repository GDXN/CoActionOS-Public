/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "link_flags.h"


int link_exec(link_phy_t handle, const char * file){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;

	link_debug(5, "exec %s\n", file);

	op.exec.cmd = LINK_CMD_EXEC;
	op.exec.path_size = strlen(file) + 1;


	link_debug(4, "Write op (0x%lX)\n", (long unsigned int)link_dev);
	err = link_protocol_masterwrite(handle, &op, sizeof(link_open_t));
	if ( err < 0 ){
		return err;
	}

	//Send the path on the bulk out endpoint
	link_debug(4, "Write exec path (%d bytes)\n", op.exec.path_size);
	len = link_protocol_masterwrite(handle, file, op.exec.path_size);
	if ( len < 0 ){
		link_debug(1, "Failed to write bulk output\n");
		return LINK_TRANSFER_ERR;
	}

	//read the reply to see if the file opened correctly
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		link_debug(1, "Failed to read the reply\n");
		return err;
	}

	link_errno = reply.err_number;
	if ( reply.err < 0 ){
		link_debug(1, "Failed to exec file (%d)\n", link_errno);
	}
	return reply.err;

	return 0;
}

int link_kill_pid(link_phy_t handle, int pid, int signo){
	int fd;
	int err_ioctl;
	int err;
	sys_killattr_t killattr;

	fd = link_open(handle, "/dev/sys", LINK_O_RDWR);
	if ( fd < 0 ){
		link_error("failed to open /dev/sys\n");
		return link_handle_err(handle, fd);
	}

	killattr.id = pid;
	killattr.si_signo = signo;
	killattr.si_sigcode = LINK_SI_USER;
	killattr.si_sigvalue = 0;

	err_ioctl = link_ioctl(handle, fd, I_SYS_KILL, &killattr);
	if( err_ioctl == LINK_PHY_ERROR ){
		link_error("failed to I_SYS_KILL\n");
		return err_ioctl;
	}

	if( err_ioctl == LINK_PROT_ERROR ){
		if( link_handle_err(handle, err_ioctl) == LINK_PHY_ERROR ){
			return LINK_PHY_ERROR;
		}
	}

	if( (err = link_close(handle, fd)) < 0 ){
		link_error("failed to close fd\n");
		return err;
	}

	return err_ioctl;
}







/*! @} */
