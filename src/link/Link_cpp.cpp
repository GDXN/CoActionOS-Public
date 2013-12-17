/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <string>

#define MAX_TRIES 3

#include "Link.h"
#include "link_flags.h"

/*
static int atoh(string s){
	stringstream ss;
	int x;
	ss << hex << s;
	ss >> x;
	return x;
}
 */

static string genError(string msg, int err_number){
	string s;
	char buffer[32];
	sprintf(buffer, " (%d)", err_number);
	s = msg + string(buffer);
	return s;
}

vector<string> Link::listDevices(int max){
	vector<string> devices;
	char * snList;
	int i;
	char * sn;
	snList = link_new_device_list( max );
	devices.clear();
	if ( snList ){
		i = 0;
		while( ( strlen( (sn = link_device_list_entry(snList, i)) ) > 0 ) && (i < max) ){
			devices.push_back(sn);
			i++;
		}
		link_del_device_list(snList);
	}
	return devices;
}

Link::Link(){
	//check to see if the device is connected -- if it is not connected, connect to it
	stdoutFd = -1;
	stdinFd = -1;
	phyHandle = LINK_PHY_OPEN_ERROR;
	lock = 0;
	isBoot = false;
	statusMessage = "";
	errMsg = "";
	lastsn = "";
}

Link::~Link(){}


link_phy_t Link::handle(void){
	return phyHandle;
}

int Link::lockDevice(void){
	return 0;
}

int Link::unlockDevice(void){
	return 0;
}

int Link::checkError(int err){
	switch(err){
	case LINK_PHY_ERROR:
		errMsg = "Physical Connection Error";
		this->exit();
		return LINK_PHY_ERROR;
	case LINK_PROT_ERROR:
		errMsg = "Protocol Error";
		return LINK_PROT_ERROR;
	}
	return err;
}

int Link::getProgress(void){
	return progress;
}

int Link::getProgessMax(void){
	return progressMax;
}

void Link::resetProgress(void){
	progress = 0;
	progressMax = 0;
}

string Link::getStatusMessage(void){
	return statusMessage;
}

int Link::init(string sn){
	int err;

	resetProgress();

	if ( phyHandle == LINK_PHY_OPEN_ERROR ){
		link_debug(4, "connect to %s\n", sn.c_str());
		phyHandle = link_connect(sn.c_str());
		link_debug(4, "Link connect handle 0x%X (0x%X)\n", (int)phyHandle, (int)LINK_PHY_OPEN_ERROR);

		if ( phyHandle == LINK_PHY_OPEN_ERROR ){
			errMsg = "Failed to Connect to Device";
			return -1;
		}

		lastsn = sn;

	} else {
		errMsg = genError("Already Connected", (int)phyHandle);
		return -1;
	}

	link_debug(4, "Check for bootloader\n");
	err = link_isbootloader(phyHandle);

	if ( err > 0 ){
		isBoot = true;
	} else if ( err == 0){
		isBoot = false;
	} else {
		errMsg = genError("Failed to check for Bootloader status", link_errno);
		return -1;
	}

	return 0;
}

int Link::open(string file, int flags, link_mode_t mode){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_open(phyHandle, file.c_str(), flags, mode);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to open file: " + file, link_errno);
	}
	return checkError(err);
}

int Link::close(int fd){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_close(phyHandle, fd);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to close file", link_errno);
	}
	return checkError(err);
}


int Link::read(int fd, void * buf, int nbyte){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err =  link_read(phyHandle, fd, buf, nbyte);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to read", link_errno);
	}
	return checkError(err);
}

int Link::write(int fd, const void * buf, int nbyte){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err =  link_write(phyHandle, fd, buf, nbyte);
		if(err != LINK_PROT_ERROR) break;
	}
	if ( err < 0 ){
		errMsg = genError("Failed to write", link_errno);
	}
	unlockDevice();
	return checkError(err);

}

int Link::readFlash(int addr, void * buf, int nbyte){
	int err;
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err =  link_readflash(phyHandle, addr, buf, nbyte);
		if(err != LINK_PROT_ERROR) break;
	}
	if ( err < 0 ){
		errMsg = genError("Failed to read flash", link_errno);
	}
	unlockDevice();
	return checkError(err);

}

int Link::isExecuting(string name){
	sys_taskattr_t task;
	int id;
	int err;
	int fd;

	if ( this->connected() == false ){
		return -1;
	}

	fd = open("/dev/sys", LINK_O_RDWR);
	if( fd < 0 ){
		this->errMsg = "Failed to Open /dev/sys";
		return -1;
	}

	id = 0;

	do {
		task.is_enabled = 0;
		task.tid = id;
		memset(task.name, 0, 24);
		err = this->ioctl(fd, I_SYS_GETTASK, &task);

		if( err > 0 ){
			if( task.is_enabled != 0 ){
				if( string(task.name) == name ){
					close(fd);
					return task.pid;
				}
			}
		}
		id++;
	} while( err != -1 );

	close(fd);

	return -1;

}

int Link::writeFlash(int addr, const void * buf, int nbyte){
	int err;
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err =  link_writeflash(phyHandle, addr, buf, nbyte);
		if(err != LINK_PROT_ERROR) break;
	}
	if ( err < 0 ){
		errMsg = genError("Failed to write flash", link_errno);
	}
	unlockDevice();
	return checkError(err);
}


int Link::lseek(int fd, int offset, int whence){
	int err;
	if ( isBoot ){
		return -1;
	}

	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_lseek(phyHandle, fd, offset, whence);
		if(err != LINK_PROT_ERROR) break;
	}
	if ( err < 0 ){
		errMsg = genError("Failed to lseek", link_errno);
	}
	unlockDevice();
	return checkError(err);

}


int Link::ioctl(int fd, int request, void * ctl){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_ioctl(phyHandle, fd, request, ctl);
		if(err != LINK_PROT_ERROR) break;
	}
	if ( err < 0 ){
		errMsg = genError("Failed to ioctl", link_errno);
	}
	unlockDevice();
	return checkError(err);

}

int Link::exit(void){
	lockDevice();
	if ( phyHandle != LINK_PHY_OPEN_ERROR ){
		link_disconnect(phyHandle);
		phyHandle = LINK_PHY_OPEN_ERROR;
	}
	unlockDevice();
	return 0;

}

bool Link::connected(void){

	if( link_phy_status(phyHandle) == LINK_PHY_ERROR){
		phyHandle = LINK_PHY_OPEN_ERROR;
	}
	return (phyHandle != LINK_PHY_OPEN_ERROR);
}

int Link::openStdio(void){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	err = link_open_stdio(phyHandle);
	unlockDevice();
	if ( err < 0 ){
		if ( err == LINK_TRANSFER_ERR ){
			errMsg = "Connection Failed";
			this->exit();
			return -2;
		} else {
			errMsg = "Failed to Open Stdio ";
			return -1;
		}
	}
	return err;
}

int Link::closeStdio(void){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	err = link_close_stdio(phyHandle);
	unlockDevice();
	if ( err < 0 ){
		if ( err == LINK_TRANSFER_ERR ){
			errMsg = "Connection Failed";
			this->exit();
			return -2;
		} else {
			errMsg = genError("Failed to Close Stdio", link_errno);
			return -1;
		}
	}
	return err;
}

int Link::readStdout(void * buf, int nbyte, volatile bool * abort){
	int err;
	if ( isBoot ){
		return -1;
	}
	//lockDevice();
	link_errno = 0;
	err = link_read_stdout(phyHandle, buf, nbyte);
	//unlockDevice();
	if ( err < 0 ){
		if ( link_errno == 0 ){
			if ( abort == false ){
				errMsg = "Connection Failed";
				this->exit();
			}
			return -2;
		} else {
			errMsg = genError("Failed to Read Stdout", link_errno);
			return -1;
		}
	}
	return err;

}

int Link::writeStdin(const void * buf, int nbyte){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	err = link_write_stdin(phyHandle, buf, nbyte);
	unlockDevice();
	if ( err < 0 ){
		if ( err == LINK_TRANSFER_ERR ){
			errMsg = "Connection Failed";
			return -1;
		} else {
			errMsg = genError("Failed to Write Stdin", link_errno);
			return -1;
		}
	}
	return err;
}

int Link::stat(string path, struct link_stat * st){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_stat(phyHandle, path.c_str(), st);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if( err < 0 ){
		errMsg = genError("Failed to Get Stat", link_errno);
	}
	return checkError(err);
}

int Link::getTime(struct tm * gt){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_gettime(phyHandle, gt);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to Get Time", link_errno);
	}
	return checkError(err);
}

int Link::setTime(struct tm * gt){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_settime(phyHandle, gt);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to Set Time", link_errno);
	}
	return checkError(err);
}

//Operations on the device
int Link::mkdir(string directory, link_mode_t mode){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_mkdir(phyHandle, directory.c_str(), mode);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to create " + directory, link_errno);
	}
	return checkError(err);
}

int Link::rmdir(string directory){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_rmdir(phyHandle, directory.c_str());
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to remove " + directory, link_errno);
	}
	return checkError(err);
}

int Link::opendir(string directory){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_opendir(phyHandle, directory.c_str());
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to open " + directory, link_errno);
	}
	return checkError(err);
}

int Link::readdir_r(int dirp, struct link_dirent * entry, struct link_dirent ** result){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_readdir_r(phyHandle, dirp, entry, result);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to read directory", link_errno);
		return -1;
	}
	return checkError(err);
}

int Link::closedir(int dirp){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_closedir(phyHandle, dirp);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to close directory", link_errno);
	}
	return checkError(err);
}

int Link::symlink(string oldPath, string newPath){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_symlink(phyHandle, oldPath.c_str(), newPath.c_str());
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to create symlink " + newPath, link_errno);
	}
	return checkError(err);
}

int Link::unlink(string filename){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_unlink(phyHandle, filename.c_str());
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to remove " + filename, link_errno);
	}
	return checkError(err);
}

int Link::cp(string src, string dest, link_mode_t mode, bool toDevice, bool (*update)(int, int)){
	FILE * hostFile;
	int err;
	int deviceFile;
	int flags;
	int bytesRead;
	const int bufferSize = 128;
	char buffer[bufferSize];
	struct link_stat st;

	if ( isBoot ){
		return -1;
	}

	err = 0;

	if ( toDevice == true ){

		progressMax = 0;
		progress = 0;

		//Open the host file
		hostFile = fopen(src.c_str(), "rb");
		if ( hostFile == NULL ){
			errMsg = "Could not find file " + src + " on host.";
			return -1;
		}

		//Create the device file
		flags = LINK_O_TRUNC | LINK_O_CREAT | LINK_O_WRONLY; //The create new flag settings
		lockDevice();
		deviceFile = link_open(phyHandle, dest.c_str(), flags, mode);

		fseek(hostFile, 0, SEEK_END);
		progressMax = ftell(hostFile);
		rewind(hostFile);

		errMsg = "";

		if ( deviceFile > 0 ){
			while( (bytesRead = fread(buffer, 1, bufferSize, hostFile)) > 0 ){
				if ( (err = link_write(phyHandle, deviceFile, buffer, bytesRead)) != bytesRead ){
					errMsg = genError("Failed to write to Link device file", link_errno);
					if ( err > 0 ){
						err = -1;
					}
					break;
				} else {
					progress += bytesRead;
					if( update != 0 ){
						if( update(progress, progressMax) == true ){
							//update progress and check for abort
							break;
						}
					}
					err = 0;
				}
			}
		} else {
			unlockDevice();
			fclose(hostFile);

			if ( deviceFile == LINK_TRANSFER_ERR ){
				errMsg = "Connection Failed";
				this->exit();
				return -2;
			}

			errMsg = genError("Failed to create file " + dest + " on Link device", link_errno);
			return -1;
		}

		fclose(hostFile);

		if ( err == LINK_TRANSFER_ERR ){
			unlockDevice();
			errMsg = "Connection Failed";
			this->exit();
			return -2;
		}

		if ( link_close(phyHandle, deviceFile) ){
			errMsg = genError("Failed to close Link device file", link_errno);
			unlockDevice();
			return -1;
		}
		unlockDevice();

		return err;

	} else {


		if ( link_stat(phyHandle, src.c_str(), &st) < 0 ){
			errMsg = "Failed to get target file size";
			return -1;
		}

		progress = 0;
		progressMax = 0;
		//Copy the source file from the device to the host
		hostFile = fopen(dest.c_str(), "wb");
		if ( hostFile == NULL ){
			errMsg = "Failed to open file " + dest + " on host.";
			return -1;
		}


		//Open the device file
		flags = LINK_O_RDONLY; //Read the file only
		lockDevice();
		deviceFile = link_open(phyHandle, src.c_str(), flags, 0);
		if ( deviceFile > 0 ){
			progressMax = st.st_size;
			while( (bytesRead = link_read(phyHandle, deviceFile, buffer, bufferSize)) > 0 ){
				fwrite(buffer, 1, bytesRead, hostFile);
				if ( bytesRead < bufferSize ){
					break;
				}
				progress += bytesRead;
				if( update != 0 ){
					if( update(progress, progressMax) == true ){
						//update progress and check for abort
						break;
					}
				}
			}
		} else {
			if ( deviceFile == LINK_TRANSFER_ERR ){
				errMsg = "Connection Failed";
				unlockDevice();
				this->exit();
				return -2;
			} else {
				errMsg = genError("Failed to open file " + src + " on Link device", link_errno);
				fclose(hostFile);
				unlockDevice();
				return -1;
			}
		}

		fclose(hostFile);

		if ( (err = link_close(phyHandle, deviceFile)) ){
			if ( err == LINK_TRANSFER_ERR ){
				errMsg = "Connection Failed";
				unlockDevice();
				this->exit();
				return -2;
			} else {
				errMsg = genError("Failed to close Link file", link_errno);
				unlockDevice();
				return -1;
			}
		}
		unlockDevice();
	}
	return 0;
}

int Link::runApp(string path){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_exec(phyHandle, path.c_str());
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		if ( err == LINK_TRANSFER_ERR ){
			errMsg = "Connection Failed";
			this->exit();
			return -2;
		} else {
			errMsg = genError("Failed to run program: " + path, link_errno);
			return -1;
		}
	}
	return 0;
}

int Link::format(string path){
	int err;
	if ( isBoot ){
		return -1;
	}
	errMsg = "";
	//Format the filesystem
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_mkfs(phyHandle, path.c_str());
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to Format Filesystem", link_errno);
	}
	return checkError(err);
}

int Link::killPid(int pid, int signo){
	int err;
	stringstream ss;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_kill_pid(phyHandle, pid, signo);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		ss << "Failed to kill process " << pid;
		errMsg = genError(ss.str(), link_errno);
	}
	return checkError(err);
}

int Link::reset(void){
	lockDevice();
	link_reset(phyHandle);
	unlockDevice();
	phyHandle = LINK_PHY_OPEN_ERROR;
	return 0;
}

int Link::resetBootloader(void){
	lockDevice();
	link_resetbootloader(phyHandle);
	link_phy_close(phyHandle);
	unlockDevice();
	phyHandle = LINK_PHY_OPEN_ERROR;
	return 0;
}

bool Link::isBootloader(void){
	return isBoot;
}

int Link::getSecurityAddr(uint32_t * addr){
	return -1;
}

int Link::rename(string old_path, string new_path){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_rename(phyHandle, old_path.c_str(), new_path.c_str());
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if( err < 0 ){
		errMsg = genError("Failed to rename file", link_errno);
	}
	return checkError(err);
}

int Link::chown(string path, int owner, int group){
	int err;
	if ( isBoot ){
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_chown(phyHandle, path.c_str(), owner, group);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to chown file", link_errno);
	}
	return checkError(err);
}

int Link::chmod(string path, int mode){
	int err;
	if ( isBoot ){
		errMsg = "Target is a bootloader";
		return -1;
	}
	lockDevice();
	for(int tries = 0; tries < MAX_TRIES; tries++){
		err = link_chmod(phyHandle, path.c_str(), mode);
		if(err != LINK_PROT_ERROR) break;
	}
	unlockDevice();
	if ( err < 0 ){
		errMsg = genError("Failed to chmod file", link_errno);
	}
	return checkError(err);

}

int Link::updateOS(string path, bool verify, bool (*update)(int,int)){
	int err;
	uint32_t loc;
	int bytesRead;
	FILE * hostFile;
	char stackaddr[256];
	const int bufferSize = 1024;
	unsigned char buffer[bufferSize];
	unsigned char cmpBuffer[bufferSize];
	int i;
	bootloader_attr_t attr;
	uint32_t startAddr;

	if ( isBoot == false ){
		errMsg = "Target is not a bootloader";
		return -1;
	}


	lockDevice();
	//first erase the flash
	err = link_eraseflash(phyHandle);
	unlockDevice();

	if ( err < 0 ){
		errMsg = "Failed to erase flash";
		unlockDevice();
		return checkError(err);
	}


	//now write the OS to the device using link_writeflash()
	progressMax = 0;
	progress = 0;

	//Open the host file
	hostFile = fopen(path.c_str(), "rb");
	if ( hostFile == NULL ){
		errMsg = "Could not find file " + path + " on host.";
		unlockDevice();
		return -1;
	}


	fseek(hostFile, 0, SEEK_END);
	progressMax = ftell(hostFile);
	rewind(hostFile);

	errMsg = "";
	statusMessage = "Writing OS to Target...";

	err = link_ioctl(phyHandle, LINK_BOOTLOADER_FILDES, I_BOOTLOADER_GETATTR, &attr);
	if( err < 0 ){
		unlockDevice();
		errMsg = "Failed to read attributes";
		return checkError(err);
	}

	startAddr = attr.startaddr;
	loc = startAddr;


	while( (bytesRead = fread(buffer, 1, bufferSize, hostFile)) > 0 ){

		if( loc == startAddr ){
			memcpy(stackaddr, buffer, 256);
			memset(buffer, 0xFF, 256);
		}

		if ( (err = link_writeflash(phyHandle, loc, buffer, bytesRead)) != bytesRead ){
			errMsg = genError("Failed to write to link flash", link_errno);
			if ( err > 0 ){
				err = -1;
			}
			break;
		}

		loc += bytesRead;
		progress += bytesRead;
		if( update(progress, progressMax) == true ){
			//update progress and check for abort
			break;
		}
		err = 0;
	}

	if ( err == 0 ){

		if ( verify == true ){

			rewind(hostFile);
			loc = startAddr;
			progress = 0;

			statusMessage = "Verifying...";


			while( (bytesRead = fread(buffer, 1, bufferSize, hostFile)) > 0 ){

				if ( (err = link_readflash(phyHandle, loc, cmpBuffer, bytesRead)) != bytesRead ){
					errMsg = genError("Failed to read flash memory", link_errno);
					if ( err > 0 ){
						err = -1;
					}
					break;
				} else {

					if( loc == startAddr ){
						memset(buffer, 0xFF, 256);
					}

					if ( memcmp((void*)cmpBuffer, buffer, bytesRead) != 0 ){
						for(i=0; i < bytesRead; i++){
							if( buffer[i] != cmpBuffer[i] ){
								printf("0x%X targ:0x%02X actual:0x%02X\n", loc + i, buffer[i], cmpBuffer[i]);
							}
						}
						errMsg = genError("Failed to verify program installation", link_errno);
						fclose(hostFile);

						//erase the flash
						//link_eraseflash(handle);

						return -1;
					}

					loc += bytesRead;
					progress += bytesRead;
					printf("read %d of %d bytes\n", progress, progressMax);
					err = 0;
				}
			}
		}

		//write the stack address
		if( (err = link_writeflash(phyHandle, startAddr, stackaddr, 256)) != 256 ){
			errMsg = genError("Failed to write stack addr", err);
			return -1;
		}


		if( verify == true ){
			//verify the stack address
			if( (err = link_readflash(phyHandle, startAddr, buffer, 256)) != 256 ){
				errMsg = genError("Failed to write stack addr", err);
				return -1;
			}

			if( memcmp(buffer, stackaddr, 256) != 0 ){
				link_eraseflash(phyHandle);
				errMsg = "Failed to verify stack address";
				return -1;
			}
		}

		statusMessage = "Finalizing...";

	}

	statusMessage = "Done";

	fclose(hostFile);
	unlockDevice();

	if( err < 0 ){
		link_eraseflash(phyHandle);
	}

	return checkError(err);

}



