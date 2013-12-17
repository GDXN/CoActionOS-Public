/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "link_flags.h"

#define MAX_TRIES 3

int link_debug_level = 0;
link_dev_t link_dev = (void*)1;
link_phy_t link_phy;

int link_errno;

void link_set_debug(int debug_level){
	link_debug_level = debug_level;
}


int link_init(void){
	return 0;
}


void link_exit(void){

}

int link_disconnect(link_phy_t handle){
	return link_phy_close(handle);
}

link_phy_t link_connect(const char * sn){
	char name[LINK_PHY_NAME_MAX];
	char serialno[256];
	char last[LINK_PHY_NAME_MAX];
	link_phy_t handle;
	int err;
	int len;

	memset(last, 0, LINK_PHY_NAME_MAX);

	while( (err = link_phy_getname(name, last, LINK_PHY_NAME_MAX)) == 0 ){
		//success in getting new name
		handle = link_phy_open(name, 0);
		if( handle != LINK_PHY_OPEN_ERROR ){
			if( link_readserialno(handle, serialno, 256) == 0 ){
				//check for NULL sn, zero length sn or matching sn
				if( (sn == NULL) || (strlen(sn) == 0) || (strcmp(sn, serialno) == 0) ){
					return handle;
				}

				//check for half the serial number for compatibility to old serial number format
				len = strlen(sn);
				if( strcmp(&(sn[len/2]), serialno) == 0 ){
					return handle;
				}

				len = strlen(serialno);
				if( strcmp(sn, &(serialno[len/2])) == 0 ){
					return handle;
				}
			}
			link_debug(2, "Close Handle\n");
			link_phy_close(handle);
		}
		strcpy(last, name);
	}


	//no device was found
	link_error("Device not found\n");
	return LINK_PHY_OPEN_ERROR;
}

int link_readserialno(link_phy_t handle, char * serialno, int len){
	link_op_t op;
	link_reply_t reply;
	int err;

	op.cmd = LINK_CMD_READSERIALNO;

	link_debug(2, "Send command (%d) to read serial number\n", op.cmd);
	err = link_protocol_masterwrite(handle, &op, sizeof(link_cmd_t));
	if ( err < 0 ){
		link_error("failed to write op\n");
		return link_handle_err(handle, err);
	}

	link_debug(2, "Read the reply\n");
	//Get the reply
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		link_error("failed to read reply\n");
		return link_handle_err(handle, err);
	}

	link_debug(2, "Read the serial number (%d bytes)\n", reply.err);
	if ( reply.err > 0 ){
		//Read bulk in as the size of the the new data
		if( reply.err <= len ){
			memset(serialno, 0, len);
			err = link_protocol_masterread(handle, serialno, reply.err);
		} else {
			return -1;
		}
		if ( err < 0 ){
			return err;
		}

	} else {
		link_errno = reply.err_number;
	}
	return 0;
}


int link_mkfs(link_phy_t handle, const char * path){
	link_op_t op;
	link_reply_t reply;
	int len;
	int err;

	link_debug(3, "format %s\n", path);

	op.mkfs.cmd = LINK_CMD_MKFS;
	op.mkfs.path_size = strlen(path) + 1;


	link_debug(4, "Write op (0x%d)\n", op.mkfs.cmd);
	err = link_protocol_masterwrite(handle, &op, sizeof(link_open_t));
	if ( err < 0 ){
		return err;
	}

	//Send the path on the bulk out endpoint
	link_debug(4, "Write mkfs path (%d bytes)\n", op.exec.path_size);
	len = link_protocol_masterwrite(handle, path, op.exec.path_size);
	if ( len < 0 ){
		link_debug(1, "Failed to write bulk output\n");
		return LINK_TRANSFER_ERR;
	}

	link_phy_wait(100);

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

char * link_new_device_list(int max){
	char name[LINK_PHY_NAME_MAX];
	char serialno[256];
	char last[LINK_PHY_NAME_MAX];
	link_phy_t handle;
	char * sn_list;
	int cnt;
	int err;

	sn_list = malloc(max*256);
	if( sn_list == NULL ){
		return NULL;
	}

	memset(sn_list, 0, max*256);

	//count the devices
	cnt = 0;
	memset(last, 0, LINK_PHY_NAME_MAX);

	while ( (err = link_phy_getname(name, last, LINK_PHY_NAME_MAX)) == 0 ){
		//success in getting new name
		handle = link_phy_open(name, 0);
		if( handle != LINK_PHY_OPEN_ERROR ){
			if( link_readserialno(handle, serialno, 256) == 0 ){
				strcpy(&(sn_list[256*cnt]), serialno);
				cnt++;
				if( cnt == max ){
					return sn_list;
				}
			}
			link_phy_close(handle);
		}
		strcpy(last, name);
	}
	return sn_list;
}


/*! \details This function deletes a device list previously
 * created with usbtools_new_device_list().
 */
void link_del_device_list(char * sn_list){
	free(sn_list);
}

char * link_device_list_entry(char * list, int entry){
	return &(list[entry*256]);
}

int link_handle_err(link_phy_t handle, int err){
	int tries;
	int err2;
	link_phy_flush(handle);
	switch(err){
	case LINK_PHY_ERROR:
		break;
	case LINK_PROT_ERROR:
		//send zero length packets until device is ready
		tries = 0;
		link_debug(4, "Try to overcome PROT error\n");
		do {
			err2 = link_protocol_masterwrite(handle, NULL, 0);
			if( err2 == LINK_PHY_ERROR ){
				return LINK_PHY_ERROR; //failure
			}

			if( err2 == 0 ){
				link_debug(4, "Successfully overcame PROT error\n");
				link_phy_wait(10);
				link_phy_flush(handle);
				return LINK_PROT_ERROR; //try the operation again
			}

			tries++;
		} while( tries < MAX_TRIES );
		break;
	case LINK_DEV_ERROR:
		return err;
	default:
		break;
	}

	link_error("PHY error\n");
	return LINK_PHY_ERROR;  //failure
}



/*! @} */
