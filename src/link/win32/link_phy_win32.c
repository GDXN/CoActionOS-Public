/*
 * link_phy_posix.c
 *
 *  Created on: Oct 6, 2012
 *      Author: tgil
 */


#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <windows.h>
#include "../link_flags.h"

typedef struct {
	HANDLE handle;
	char name[32];
} link_phydata_t;


#define COM_PORT_NAME "\\\\.\\COM"
#define COM_PORT_MAX 128


int link_phy_getname(char * dest, const char * last, int len){
	int com_port;
	char buffer[12];


	if( strlen(last) == 0 ){
		com_port = 0;
		sprintf(dest, "%s%d", COM_PORT_NAME, com_port);
		return 0;
	} else {
		com_port = 0;
		do {
			sprintf(buffer, "%s%d", COM_PORT_NAME, com_port++);
			if( strcmp(buffer, last) == 0 ){
				break;
			}
		} while(com_port < COM_PORT_MAX);
		if( com_port > (COM_PORT_MAX-1) ){
			return LINK_PHY_ERROR;
		}
	}


	sprintf(buffer, "%s%d", COM_PORT_NAME, com_port);
	strcpy(dest, buffer);

	return 0;
}

int link_phy_status(link_phy_t handle){

	//TODO This needs to check to see if the port still exists
	return 0;
}


link_phy_t link_phy_open(const char * name, int baudrate){

	link_phydata_t * handle;
	DCB params;

	handle = malloc(sizeof(link_phydata_t));
	if( handle == 0 ){
		return LINK_PHY_OPEN_ERROR;
	}

	memset(((link_phydata_t*)handle)->name, 0, 32);
	strncpy(handle->name, name, 31);

	handle->handle = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if( handle->handle == INVALID_HANDLE_VALUE ){
		free(handle);
		return LINK_PHY_OPEN_ERROR;
	}

	if( !GetCommState(handle->handle, &params) ){
		CloseHandle(handle->handle);
		free(handle);
		return LINK_PHY_OPEN_ERROR;
	}


	params.BaudRate = 460800;
	params.ByteSize = 8;
	params.StopBits = TWOSTOPBITS;
	params.Parity = EVENPARITY;
	params.fBinary=1;


	if ( !SetCommState(handle->handle, &params)){
		CloseHandle(handle->handle);
		link_error("Failed set COMM state\n");
		return LINK_PHY_OPEN_ERROR;
	}



	COMMTIMEOUTS timeouts={0};
	timeouts.ReadIntervalTimeout=MAXDWORD;
	timeouts.ReadTotalTimeoutConstant=1;
	timeouts.ReadTotalTimeoutMultiplier=0;
	timeouts.WriteTotalTimeoutConstant=0;
	timeouts.WriteTotalTimeoutMultiplier=0;
	if(!SetCommTimeouts(handle->handle, &timeouts)){
		//error occurred. Inform user
	}

	return handle;
}

int link_phy_write(link_phy_t handle, const void * buf, int nbyte){
	DWORD bytes_written;
	if( !WriteFile(((link_phydata_t*)handle)->handle, buf, nbyte, &bytes_written, NULL) ){
		link_error("Failed to write %d bytes from handle:%d\n", nbyte, (int)handle);
		return LINK_PHY_ERROR;
	}
	return bytes_written;
}

int link_phy_read(link_phy_t handle, void * buf, int nbyte){
	DWORD bytes_read;
	if( !ReadFile(((link_phydata_t*)handle)->handle, buf, nbyte, &bytes_read, NULL) ){
		link_error("Failed to read %d bytes from handle:%d\n", nbyte, (int)handle);
		return LINK_PHY_ERROR;
	}
	return bytes_read;
}

int link_phy_close(link_phy_t handle){

	if( CloseHandle(((link_phydata_t*)handle)->handle) == 0 ){
		link_error("Failed to close handle\n");
	}
	free(handle);
	return 0;
}

void link_phy_wait(int msec){
	Sleep(msec);
}

void link_phy_flush(link_phy_t handle){
	char c;
	while( link_phy_read(handle, &c, 1) == 1 ){
		;
	}
}


