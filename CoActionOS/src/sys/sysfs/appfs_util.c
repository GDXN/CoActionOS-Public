/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <errno.h>

#include "caos.h"
#include "hwpl.h"
#include "hwpl/mpu.h"
#include "hwpl/task.h"
#include "hwpl/core.h"
#include "appfs_local.h"
#include "hwpl/debug.h"

#include "../symbols.h"

#define APPFS_REWRITE_MASK 0xFF000000
#define APPFS_REWRITE_RAM_MASK (0x00800000)
#define APPFS_REWRITE_KERNEL_ADDR (0x00FF8000)
#define APPFS_REWRITE_KERNEL_ADDR_MASK (0x7FFF)

static void appfs_util_privloadfileinfo(void * args);
static int get_hdrinfo(appfs_file_t * file, int page, int type);
static int get_filesize(const device_t * dev, priv_load_fileinfo_t * args, int filetype);

static uint8_t calc_checksum(const char * name){
	int i;
	uint8_t checksum;
	checksum = 0;
	for(i=0; i < NAME_MAX-1; i++){
		checksum ^= name[i];
	}
	return checksum;
}


typedef struct {
	int ret;
	const device_t * dev;
	device_transfer_t op;
	int start_page;
	int end_page;
} priv_op_t;

static void priv_op_erase_pages(void * args){
	int i;
	priv_op_t * p = args;
	for(i=p->start_page; i <= p->end_page; i++){
		p->ret = p->dev->driver.ioctl(&(p->dev->cfg), I_MEM_ERASE_PAGE, (void*)i);
	}
}

int appfs_util_erasepages(const device_t * dev, int start_page, int end_page){
	priv_op_t args;
	args.dev = dev;
	args.start_page = start_page;
	args.end_page = end_page;
	hwpl_core_privcall(priv_op_erase_pages, &args);
	return 0;
}

int appfs_util_getpageinfo(const device_t * dev, mem_pageinfo_t * pageinfo){
	return dev->driver.ioctl(&(dev->cfg), I_MEM_GET_PAGEINFO, pageinfo);
}



static uint32_t translate_value(uint32_t addr, uint32_t mask, uint32_t code_start, uint32_t data_start, uint32_t total){
	//check if the value is an address
	uint32_t ret;
	ret = addr;
	if ( (addr & APPFS_REWRITE_MASK) == mask ){ //matches Text or Data
		ret = addr & ~(APPFS_REWRITE_MASK|APPFS_REWRITE_RAM_MASK);
		if( (addr & APPFS_REWRITE_KERNEL_ADDR) == APPFS_REWRITE_KERNEL_ADDR ){
			//This is a kernel value
			ret = (addr & APPFS_REWRITE_KERNEL_ADDR_MASK)>>2;
			if( ret < total ){
				//get the symbol location from the symbols table
				if( symbols_table[ret] == 0 ){
					errno = ENOEXEC;
				}
				return symbols_table[ret];
			} else {
				errno = ENOEXEC;
				return 0;
			}
		} else if (addr & APPFS_REWRITE_RAM_MASK ){
			ret |= data_start;
		} else {
			ret |= code_start;
		}
	}

	return ret;
}

static int find_protectable_addr(const device_t * dev, int size, int type, int * page){
	int i;
	uint32_t tmp_rbar;
	uint32_t tmp_rasr;
	mem_pageinfo_t pageinfo;
	int err;
	int mem_type;

	if ( type == MEM_PAGEINFO_TYPE_FLASH ){
		mem_type = MPU_MEMORY_FLASH;
	} else {
		mem_type = MPU_MEMORY_SRAM;
	}

	i = *page;
	do {
		//go through each page
		pageinfo.num = i;
		pageinfo.type = type;
		if ( dev->driver.ioctl(&(dev->cfg), I_MEM_GET_PAGEINFO, &pageinfo) < 0 ){
			return -1;
		}

		err = mpu_calc_region(
				6,
				(void*)pageinfo.addr,
				size,
				MPU_ACCESS_PR_UR,
				mem_type,
				true,
				&tmp_rbar,
				&tmp_rasr
		);

		if ( err == 0 ){
			*page = i;
			return pageinfo.addr;
		}


		i++;
	} while(1);

	return -1;
}

int check_for_free_space(const device_t * dev, int start_page, int type, int size){
	priv_load_fileinfo_t info;
	int free_size;
	int last_addr;
	int last_size;

	info.dev = dev;
	info.pageinfo.type = type;

	if ( type == MEM_PAGEINFO_TYPE_FLASH ){
		info.pageinfo.num = start_page;
		free_size = 0;
		last_addr = -1;
		last_size = -1;

		while( free_size < size ){
			appfs_util_privloadfileinfo(&info);

			if ( (last_addr != -1) &&  //make sure last_addr is initialized
					(last_addr + last_size != info.pageinfo.addr) ){
				//memory is not contiguous
				return -1;
			}

			last_addr = info.pageinfo.addr;
			last_size = info.pageinfo.size;

			if ( appfs_util_getflashpagetype(&info.fileinfo.hdr) == APPFS_MEMPAGETYPE_FREE ){
				free_size += info.pageinfo.size;
			} else {
				return -1;
			}
			info.pageinfo.num++;
		}
	} else {
		//check the RAM usage table to find free memory
		info.pageinfo.num = start_page;
		free_size = 0;
		last_addr = -1;
		last_size = -1;

		while( free_size < size ){
			appfs_util_getpageinfo(dev, &info.pageinfo);

			if ( (last_addr != -1) &&  //make sure last_addr is initialized
					(last_addr + last_size != info.pageinfo.addr) ){
				//memory is not contiguous
				return -1;
			}

			last_addr = info.pageinfo.addr;
			last_size = info.pageinfo.size;

			if ( appfs_ram_getusage(info.pageinfo.num) == APPFS_MEMPAGETYPE_FREE ){
				free_size += DEVICE_RAM_PAGE_SIZE;
			} else {
				//memory is not free
				return -1;
			}
			info.pageinfo.num++;

		}
	}

	return 0;
}

static int find_protectable_free(const device_t * dev, int type, int size, int * page){
	int start_addr;

	//find any area for the code
	*page = 0;
	//find an area of flash that is available to write
	do {
		start_addr = find_protectable_addr(dev,
				size,
				type,
				page);
		if ( start_addr < 0 ){
			//could not find any free space
			errno = ENOSPC;
			return -1;
		}

		if ( check_for_free_space(dev, *page, type, size) == 0 ){
			return start_addr;
		}

		(*page)++;

	} while(1);
	return -1;
}

static int find_free(const device_t * dev, int type, int size){
	mem_pageinfo_t pageinfo;
	//find any area for the code

	pageinfo.num = HWPL_LAST_BOOTLOADER_PAGE;
	//find an area of flash that is available to write
	do {

		pageinfo.type = type;
		if ( dev->driver.ioctl(&(dev->cfg), I_MEM_GET_PAGEINFO, &pageinfo) < 0 ){
			return -1;
		}

		if ( check_for_free_space(dev, pageinfo.num, type, size) == 0 ){
			return pageinfo.addr;
		}

		pageinfo.num++;

	} while(1);

	return -1;
}

int appfs_util_priv_create(const device_t * dev, appfs_handle_t * h, appfs_installattr_t * attr){
	int code_start_addr;
	int type;
	int len;
	appfs_file_t * dest;
	dest = (appfs_file_t*)attr->buffer;

	if ( attr->loc == 0 ){

		//This is the header data -- make sure it is complete
		if ( attr->nbyte < sizeof(appfs_file_t) ){
			errno = ENOTSUP;
			return -1;
		}


		if( dest->exec.signature != APPFS_CREATE_SIGNATURE ){
			errno = EINVAL;
			return -1;
		}

		//make sure the name is valid
		len = strnlen(dest->hdr.name, NAME_MAX-2);
		if ( len == (NAME_MAX-2) ){
			//truncate the name if it is too long
			dest->hdr.name[NAME_MAX-1] = 0;
			dest->hdr.name[NAME_MAX-2] = 0;
		}

		//add a checksum to the name
		dest->hdr.name[NAME_MAX-1] = calc_checksum(dest->hdr.name);

		//set the mode to read only
		dest->hdr.mode = 0444;

		//check the options
		dest->exec.options = APPFS_EXEC_OPTIONS_FLASH;
		type = MEM_PAGEINFO_TYPE_FLASH;

		//find space for the code
		code_start_addr = find_free(dev, type, dest->exec.code_size);
		if ( code_start_addr == -1 ){
			errno = ENOMEM;
			return -1;
		}

		h->type.install.code_start = (uint32_t)code_start_addr;
		h->type.install.code_size = dest->exec.code_size;
		h->type.install.data_start = 0;
		h->type.install.data_size = 0;

		h->type.install.rewrite_mask = 0;
		h->type.install.kernel_symbols_total = 0;

		dest->exec.startup = 0;
		dest->exec.code_start = (void*)code_start_addr;
		dest->exec.ram_start = (void*)0;
		dest->exec.ram_size = 0;
		dest->exec.data_size = 0;
		dest->exec.options = 0;
		dest->exec.startup = (void*)0;

	} else {
		if ( (attr->loc & 0x03) ){
			//this is not a word aligned write
			errno = ENOTSUP;
			return -1;
		}
	}

	//now write the buffer
	attr->loc += h->type.install.code_start;

	if( (attr->loc + attr->nbyte) > (h->type.install.code_start + h->type.install.code_size) ){
		errno = EINVAL;
		return -1;
	}

	return dev->driver.ioctl(&(dev->cfg), I_MEM_WRITEPAGE, attr);
}

int appfs_util_priv_writeinstall(const device_t * dev, appfs_handle_t * h, appfs_installattr_t * attr){
	union {
		const appfs_file_t * file;
		const uint32_t * ptr;
	} src;
	int i;
	int code_start_addr;
	int code_size;
	int data_start_addr;
	int ram_size;
	int type;
	int len;
	int page;

	union {
		appfs_file_t file;
		uint32_t buf[attr->nbyte>>2];
	} dest;

	src.ptr = (const uint32_t*)attr->buffer;

	if ( attr->loc == 0 ){
		//This is the header data -- make sure it is complete
		if ( attr->nbyte < sizeof(appfs_file_t) ){
			errno = ENOTSUP;
			return -1;
		}

		//make sure the name is valid
		memset(&dest.file, 0, sizeof(appfs_file_t));
		memcpy(&dest.file, src.file, attr->nbyte);
		len = strnlen(dest.file.hdr.name, NAME_MAX-2);
		if ( len == (NAME_MAX-2) ){
			//truncate the name if it is too long
			dest.file.hdr.name[NAME_MAX-1] = 0;
			dest.file.hdr.name[NAME_MAX-2] = 0; //! \todo this needs to be a checksum
		}


		//add a checksum to the name
		dest.file.hdr.name[NAME_MAX-1] = calc_checksum(dest.file.hdr.name);

		//set mode to read/exec
		dest.file.hdr.mode = 0555;

		//is signature correct
		if ( src.file->exec.signature != symbols_table[0] ){
			errno = ENOEXEC;
			return -1;
		}

		//check the options
		if ( (src.file->exec.options) & APPFS_EXEC_OPTIONS_FLASH ){
			//This should be installed in flash
			type = MEM_PAGEINFO_TYPE_FLASH;
		} else {
			//This should be install in RAM
			type = MEM_PAGEINFO_TYPE_RAM;
		}

		code_size = src.file->exec.code_size + src.file->exec.data_size; //code plus initialized data
		ram_size = src.file->exec.ram_size;

		//find space for the code
		code_start_addr = find_protectable_free(dev, type, code_size, &page);
		if ( code_start_addr == -1 ){
			return -1;
		}

		if ( !((src.file->exec.options) & APPFS_EXEC_OPTIONS_FLASH) ){ //for RAM app's mark the RAM usage
			//mark the range as SYS
			appfs_ram_setrange(appfs_ram_usagetable,
					page,
					code_size,
					APPFS_MEMPAGETYPE_SYS);

			//mark the first page as USER
			appfs_ram_setrange(appfs_ram_usagetable,
					page,
					DEVICE_RAM_PAGE_SIZE, //mark the first page as USER
					APPFS_MEMPAGETYPE_USER);
		}


		data_start_addr = find_protectable_free(dev, MEM_PAGEINFO_TYPE_RAM, ram_size, &page);
		if( data_start_addr == -1 ){
			return -1;
		}

		h->type.install.code_start = (uint32_t)code_start_addr;
		h->type.install.code_size = code_size;
		h->type.install.data_start = (uint32_t)data_start_addr;
		h->type.install.data_size = src.file->exec.data_size;

		h->type.install.rewrite_mask = (uint32_t)(src.file->exec.code_start) & APPFS_REWRITE_MASK;
		h->type.install.kernel_symbols_total = symbols_total();

		appfs_ram_setrange(appfs_ram_usagetable,
				page,
				ram_size,
				APPFS_MEMPAGETYPE_SYS);

		dest.file.exec.code_start = (void*)code_start_addr;
		dest.file.exec.ram_start = (void*)data_start_addr;
		dest.file.exec.startup = (void*)translate_value((uint32_t)dest.file.exec.startup,
				h->type.install.rewrite_mask,
				h->type.install.code_start,
				h->type.install.data_start,
				h->type.install.kernel_symbols_total);


		errno = 0;
		for(i=sizeof(appfs_file_t) >> 2; i < attr->nbyte >> 2; i++){
			dest.buf[i] = translate_value(src.ptr[i],
					h->type.install.rewrite_mask,
					h->type.install.code_start,
					h->type.install.data_start,
					h->type.install.kernel_symbols_total);
			if( (dest.buf[i] == 0) && (errno == ENOEXEC) ){
				return -1;
			}
		}

	} else {


		if ( (attr->loc & 0x03) ){
			//this is not a word aligned write
			errno = ENOTSUP;
			return -1;
		}
		for(i=0; i < attr->nbyte >> 2; i++){
			dest.buf[i] = translate_value(src.ptr[i],
					h->type.install.rewrite_mask,
					h->type.install.code_start,
					h->type.install.data_start,
					h->type.install.kernel_symbols_total);
			if( (dest.buf[i] == 0) && (errno == ENOEXEC) ){
				return -1;
			}
		}
	}

	//now write buffer
	attr->loc += h->type.install.code_start;
	memcpy(attr->buffer, &dest, attr->nbyte);

	if( (attr->loc + attr->nbyte) > (h->type.install.code_start + h->type.install.code_size + h->type.install.data_size) ){
		errno = EINVAL;
		return -1;
	}

	return dev->driver.ioctl(&(dev->cfg), I_MEM_WRITEPAGE, attr);
}


int appfs_ram_setusage(int page, int size, int type){
	uint32_t buf[APPFS_RAM_USAGE_WORDS];
	memcpy(buf, appfs_ram_usagetable, APPFS_RAM_USAGE_BYTES);
	appfs_ram_setrange(buf, page, size, type);
	hwpl_core_privcall(appfs_ram_priv_saveusage, buf);
	return 0;
}

int appfs_util_getflashpagetype(appfs_hdr_t * info){
	int len;
	int i;
	len = strnlen(info->name, NAME_MAX-2);
	if ( (len == NAME_MAX - 2) || //check if the name is short enough
			(len != strspn(info->name, sysfs_validset)) || //check if only valid characters are present
			(info->name[NAME_MAX-1] != calc_checksum(info->name)) || //check for the second terminating zero
			(len == 0)
	){
		for(i=0; i < NAME_MAX; i++){
			if ( info->name[i] != 0xFF){
				break;
			}
		}

		if (i == NAME_MAX ){
			return APPFS_MEMPAGETYPE_FREE;
		} else {
			return APPFS_MEMPAGETYPE_SYS;
		}
	}

	return APPFS_MEMPAGETYPE_USER;
}

int appfs_util_getpagetype(appfs_hdr_t * info, int page, int type){
	if ( type == MEM_PAGEINFO_TYPE_FLASH ){
		return appfs_util_getflashpagetype(info);
	}
	return appfs_ram_getusage(page);
}

int get_hdrinfo(appfs_file_t * file, int page, int type){
	char hex_num[9];
	int page_type;
	page_type = appfs_util_getpagetype(&file->hdr, page, type);
	htoa(hex_num, page);
	switch(page_type){
	case APPFS_MEMPAGETYPE_SYS:
		strcpy(file->hdr.name, ".sys");
		strcat(file->hdr.name, hex_num);
		//file->hdr.uid = 0;
		//file->hdr.gid = 0;
		file->hdr.mode = S_IFREG;
		memset(&(file->exec), 0, sizeof(appfs_exec_t));
		break;
	case APPFS_MEMPAGETYPE_FREE:
		strcpy(file->hdr.name, ".free");
		strcat(file->hdr.name, hex_num);
		//file->hdr.uid = 0;
		//file->hdr.gid = 0;
		file->hdr.mode = S_IFREG;
		memset(&(file->exec), 0, sizeof(appfs_exec_t));
		break;
	case APPFS_MEMPAGETYPE_USER:
		break;
	}

	return page_type;
}

bool appfs_util_isexecutable(const appfs_file_t * info){
	// do a priv read of the signature
	if ( info->exec.signature != symbols_table[0] ){
		return false;
	}
	return true;
}

void appfs_util_privloadfileinfo(void * args){
	priv_load_fileinfo_t * p = (priv_load_fileinfo_t*)args;
	device_transfer_t op;

	if ( p->dev->driver.ioctl(
			&(p->dev->cfg),
			I_MEM_GET_PAGEINFO,
			&p->pageinfo
	) < 0 ){
		p->ret = -1;
		return;
	}

	op.buf = &(p->fileinfo);
	op.nbyte = sizeof(appfs_file_t);
	op.context = NULL;
	op.loc = (int)p->pageinfo.addr;
	op.tid = task_get_current();

	if ( p->dev->driver.read(&(p->dev->cfg), &op) == sizeof(appfs_file_t) ){
		//read successfully
		p->ret = 0;
		return;
	}
	//this is an EIO error
	p->ret = -2;
}

int appfs_util_getfileinfo(priv_load_fileinfo_t * info, const device_t * dev, int page, int type, int * size){
	int filetype;
	info->dev = dev;
	info->pageinfo.num = page;
	info->pageinfo.type = type;
	hwpl_core_privcall(appfs_util_privloadfileinfo, info);
	if ( info->ret < 0 ){
		return -1;
	}

	//get the header info for free and sys files
	filetype = get_hdrinfo(&(info->fileinfo), page, type);

	if ( size != NULL ){
		*size = get_filesize(dev, info, filetype);
	}

	return filetype;
}

int get_filesize(const device_t * dev, priv_load_fileinfo_t * args, int filetype){
	//this will start at the end of the page and count backwards until it hits a non 0xFF value
	if ( filetype == APPFS_MEMPAGETYPE_USER ){
		return args->fileinfo.exec.code_size + args->fileinfo.exec.data_size;
	} else {
		return args->pageinfo.size;
	}
}

int appfs_util_lookupname(const void * cfg, const char * path, priv_load_fileinfo_t * args, int type, int * size){
	int i;

	i = 0;

	if ( strnlen(path, NAME_MAX-2) == NAME_MAX-2 ){
		errno = ENAMETOOLONG;
		return -1;
	}

	do {
		//go through each page
		if ( appfs_util_getfileinfo(args, cfg, i, type, size) < 0){
			return -1;
		}

		if ( strcmp(path, args->fileinfo.hdr.name) == 0 ){
			args->ret = i;
			return 0;
		}
		i++;
	} while(1);

	//name not found
	return -1;
}




