/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*!
 * \addtogroup USBLINK USB Link (link)
 * @{
 * \details USB Link is a desktop API that allows desktop programs to access MCU
 * peripherals.  The target MCU must be running CoActionOS in order for the USB Link
 * technology to work.  CoActionOS-Link uses this API to access the file
 * system on CoActionOS microcomputers.
 *
 * \ingroup ARCHITECTURES
 *
 */

/*! \file
 * \brief USB Link Protocol Header File
 *
 */

#ifndef LINK_PROTOCOL_H_
#define LINK_PROTOCOL_H_


#include <sys/stat.h>
#include <time.h>

#include "dev/adc.h"
#include "dev/bootloader.h"
#include "dev/core.h"
#include "dev/gpio.h"
#include "dev/pio.h"
#include "dev/dac.h"
#include "dev/eint.h"
#include "dev/flash.h"
#include "dev/i2c.h"
#include "dev/pwm.h"
#include "dev/qei.h"
#include "dev/rtc.h"
#include "dev/spi.h"
#include "dev/tmr.h"
#include "dev/uart.h"
#include "dev/usb.h"
#include "dev/sys.h"


#if defined( __cplusplus )
extern "C" {
#endif

//This is for POSIX -- for WINDOWS it is different
#if defined __win32 || __win64
typedef void * link_phy_t;
#define LINK_PHY_OPEN_ERROR ((void*)0)
#else
typedef int link_phy_t;
#define LINK_PHY_OPEN_ERROR -1
#endif


/*! \details There was an error on the physical layer trying to send/receive data
 * to/from the device.  This error can happen if the device is unexpectedly unplugged.
 */
#define LINK_PHY_ERROR (-3)


/*! \details The physical layer successfully sends and receives data, but
 * the device does not ack correctly.  This usually happens if a command
 * was partially completed before the host aborted.  The protocol needs to
 * be reset with link_resetprot().
 */
#define LINK_PROT_ERROR (-2)

/*! \details The physical and protocol layers successfully transferred
 * data to/from the device and the command was correctly received.  However,
 * there was an error when the device executed the command.  For example,
 * calling link_open() on a file that does not exist on the target will
 * return this error.
 */
#define LINK_DEV_ERROR (-1)



#define LINK_MAX_TRANSFER_SIZE 64
#define LINK_MAX_PACKET_SIZE (63)
#define LINK_PACKET_HEADER_SIZE (3)  //start, size and checksum
#define LINK_PACKET_DATA_SIZE (LINK_MAX_PACKET_SIZE - LINK_PACKET_HEADER_SIZE)


#define LINK_PACKET_STDIO_FLAG (1<<15)

#define LINK_PACKET_START (16)  //DLE character
#define LINK_PACKET_ACK (0x06)
#define LINK_PACKET_NACK (0x53)


typedef struct HWPL_PACK {
	uint8_t ack;
	uint8_t checksum;
} link_ack_t;

typedef struct HWPL_PACK {
	uint8_t start;
	uint8_t size;
	uint8_t data[LINK_PACKET_DATA_SIZE+1];
} link_pkt_t;



#define APPFS_PAGE_SIZE 256
typedef struct {
	uint32_t loc;
	uint32_t nbyte;
	uint8_t buffer[APPFS_PAGE_SIZE];
} appfs_installattr_t;

typedef appfs_installattr_t appfs_createattr_t;

//Install an executable in RAM or Flash
#define I_APPFS_INSTALL _IOCTLW('a', 0, appfs_installattr_t)

//Create a data file in RAM or flash
#define I_APPFS_CREATE _IOCTLW('a', 1, appfs_createattr_t)

#define APPFS_CREATE_SIGNATURE 0x12345678


typedef uint8_t link_cmd_t;

#define LINK_TRANSFER_ERR -701

#define LINK_SIGABRT 1
#define LINK_SIGALRM 2
#define LINK_SIGBUS 3
#define LINK_SIGCHLD 4
#define LINK_SIGCONT 5
#define LINK_SIGFPE 6
#define LINK_SIGHUP 7
#define LINK_SIGILL 8
#define LINK_SIGINT 9
#define LINK_SIGKILL 10
#define LINK_SIGPIPE 11
#define LINK_SIGQUIT 12
#define LINK_SIGSEGV 13
#define LINK_SIGSTOP 14
#define LINK_SIGTERM 15
#define LINK_SIGTSTP 16
#define LINK_SIGTTIN 17
#define LINK_SIGTTOU 18
#define LINK_SIGUSR1 19
#define LINK_SIGUSR2 20
#define LINK_SIGPOLL 21
#define LINK_SIGPROF 22
#define LINK_SIGSYS 23
#define LINK_SIGTRAP 24
#define LINK_SIGURG 25
#define LINK_SIGVTALRM 26
#define LINK_SIGXCPU 27
#define LINK_SIGXFSZ 28
#define LINK_SIGRTMIN 29
#define LINK_SIGRTMAX 31
#define LINK_RTSIG_MAX (SIGRTMAX - SIGRTMIN + 1)
#define LINK_NSIG	32      /* signal 0 implied */

#define LINK_SI_USER    1    /* Sent by a user. kill(), abort(), etc */
#define LINK_SI_QUEUE   2    /* Sent by sigqueue() */
#define LINK_SI_TIMER   3    /* Sent by expiration of a timer_settime() timer */
#define LINK_SI_ASYNCIO 4    /* Indicates completion of asycnhronous IO */
#define LINK_SI_MESGQ   5    /* Indicates arrival of a message at an empty queue */

#define LINK_DEV_MAX_STRING 16

#define	LINK_SEEK_SET 0
#define	LINK_SEEK_CUR 1
#define	LINK_SEEK_END 2

#define	LINK__IFMT		0170000	/* type of file */
#define	LINK__IFDIR	0040000	/* directory */
#define	LINK__IFCHR	0020000	/* character special */
#define	LINK__IFBLK	0060000	/* block special */
#define	LINK__IFREG	0100000	/* regular */
#define	LINK__IFLNK	0120000	/* symbolic link */
#define	LINK__IFSOCK	0140000	/* socket */
#define	LINK__IFIFO	0010000	/* fifo */

/*! \details S_IFMT equivalent.
 * \hideinitializer
 */
#define	LINK_S_IFMT		LINK__IFMT
/*! \details S_IFDIR equivalent.
 * \hideinitializer
 */
#define	LINK_S_IFDIR		LINK__IFDIR
/*! \details S_IFCHR equivalent.
 * \hideinitializer
 */
#define	LINK_S_IFCHR		LINK__IFCHR
/*! \details S_IFBLK equivalent.
 * \hideinitializer
 */
#define	LINK_S_IFBLK		LINK__IFBLK
/*! \details S_IFREG equivalent.
 * \hideinitializer
 */
#define	LINK_S_IFREG		LINK__IFREG
/*! \details S_IFLNK equivalent.
 * \hideinitializer
 */
#define	LINK_S_IFLNK		LINK__IFLNK
/*! \details S_IFSOCK equivalent.
 * \hideinitializer
 */
#define	LINK_S_IFSOCK	LINK__IFSOCK
/*! \details S_IFIFO equivalent.
 * \hideinitializer
 */
#define	LINK_S_IFIFO		LINK__IFIFO

/*! \details S_IRWXU equivalent.
 * \hideinitializer
 */
#define	LINK_S_IRWXU 	(LINK_S_IRUSR | LINK_S_IWUSR | LINK_S_IXUSR)
/*! \details S_IRUSR equivalent.
 * \hideinitializer
 */
#define		LINK_S_IRUSR 0000400	/* read permission, owner */
/*! \details S_IWUSR equivalent.
 * \hideinitializer
 */
#define		LINK_S_IWUSR 0000200	/* write permission, owner */
/*! \details S_IXUSR equivalent.
 * \hideinitializer
 */
#define		LINK_S_IXUSR 0000100/* execute/search permission, owner */
/*! \details S_IRWXG equivalent.
 * \hideinitializer
 */
#define	LINK_S_IRWXG		(LINK_S_IRGRP | LINK_S_IWGRP | LINK_S_IXGRP)
/*! \details S_IRGRP equivalent.
 * \hideinitializer
 */
#define		LINK_S_IRGRP 0000040	/* read permission, group */
/*! \details S_IWGRP equivalent.
 * \hideinitializer
 */
#define		LINK_S_IWGRP 0000020	/* write permission, grougroup */
/*! \details S_IXGRP equivalent.
 * \hideinitializer
 */
#define		LINK_S_IXGRP 0000010/* execute/search permission, group */
/*! \details S_IRWXO equivalent.
 * \hideinitializer
 */
#define	LINK_S_IRWXO		(LINK_S_IROTH | LINK_S_IWOTH | LINK_S_IXOTH)
/*! \details S_IROTH equivalent.
 * \hideinitializer
 */
#define		LINK_S_IROTH 0000004	/* read permission, other */
/*! \details S_IWOTH equivalent.
 * \hideinitializer
 */
#define		LINK_S_IWOTH 0000002	/* write permission, other */
/*! \details S_IXOTH equivalent.
 * \hideinitializer
 */
#define		LINK_S_IXOTH 0000001 /* execute/search permission, other */
/*! \details S_ISUID equivalent.
 * \hideinitializer
 */
#define	LINK_S_ISUID 0004000	/* set user id on execution */
/*! \details S_ISGID equivalent.
 * \hideinitializer
 */
#define	LINK_S_ISGID 0002000	/* set group id on execution */
/*! \details S_ISVTX equivalent.
 * \hideinitializer
 */
#define	LINK_S_ISVTX 0001000	/* save swapped text even after use */


#define LINK_NAME_MAX 24
#define LINK_PATH_MAX 64

/*! \details This is set to the errno value
 * reported on the target device.
 */
extern int link_errno;

typedef uint32_t link_mode_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size;
	uint32_t flags;
	uint32_t mode;
} link_open_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size_old;
	uint32_t path_size_new;
} link_symlink_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	int32_t fildes;
	int32_t request;
	uint32_t arg;
} link_ioctl_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	union {
		int32_t fildes;
		uint32_t addr;
	};
	uint32_t nbyte;
} link_read_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	union {
		int32_t fildes;
		uint32_t addr;
	};
	uint32_t nbyte;
} link_write_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	int32_t fildes;
} link_close_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size;
} link_unlink_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	int32_t fildes;
	int32_t offset;
	int32_t whence;
} link_lseek_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size;
} link_stat_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	int32_t fildes;
} link_fstat_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size;
	int32_t mode;
} link_mkdir_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size;
} link_rmdir_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size;
} link_opendir_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	int32_t dirp;
} link_readdir_t;

typedef struct HWPL_PACK {
	int32_t err;
	int32_t err_number;
	int32_t d_ino;
	uint32_t d_name_size;
} link_readdir_reply_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	int32_t dirp;
} link_closedir_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	int32_t dirp;
} link_rewinddir_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	int32_t dirp;
} link_telldir_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	int32_t dirp;
	int32_t loc;
} link_seekdir_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t size;
} link_malloc_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	hwpl_target_ptr_t ptr;
} link_free_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size;
} link_exec_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
} link_time_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t pid;
	uint32_t tid;
} link_task_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t old_size;
	uint32_t new_size;
} link_rename_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size;
	uint32_t uid;
	uint32_t gid;
} link_chown_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size;
	uint32_t mode;
} link_chmod_t;

typedef struct HWPL_PACK {
	link_cmd_t cmd;
	uint32_t path_size;
} link_mkfs_t;


typedef struct HWPL_PACK {
	uint32_t pid;
	uint32_t tid;
	uint64_t timer;
	uint32_t mem_loc /*! location of data memory */;
	uint32_t mem_size /*! size of data memory */;
	uint32_t malloc_loc /*! malloc base + size */;
	uint32_t stack_ptr /*! location of stack pointer */;
	uint8_t prio;
	uint8_t prio_ceiling;
	uint8_t is_active;
	uint8_t is_thread;
	uint8_t is_enabled;
	char name[LINK_NAME_MAX];
} link_task_data_t;

struct link_dirent {
	uint32_t d_ino;
	char d_name[LINK_NAME_MAX];
};

struct link_stat {
	uint32_t st_dev;
	uint32_t st_ino;
	uint32_t st_mode;
	uint32_t st_uid;
	uint32_t st_gid;
	uint32_t st_rdev;
	uint32_t st_size;
	uint32_t st_mtime_;
	uint32_t st_ctime_;
	uint32_t st_blksize;
	uint32_t st_blocks;
};

#define LINK_INTERRUPT_ENDPOINT_SIZE 32
#define LINK_BULK_ENDPOINT_SIZE 64

#define LINK_ENDPOINT_DATA_SIZE (LINK_BULK_ENDPOINT_SIZE)

/*! \brief The USB Link Operation Data Structure (Interrupt Out)
 * \details This data structure defines the data unions
 */
typedef union {
		link_cmd_t cmd;
		link_open_t open;
		link_symlink_t symlink;
		link_ioctl_t ioctl;
		link_write_t write;
		link_read_t read;
		link_close_t close;
		link_unlink_t unlink;
		link_lseek_t lseek;
		link_stat_t stat;
		link_fstat_t fstat;
		link_mkdir_t mkdir;
		link_rmdir_t rmdir;
		link_opendir_t opendir;
		link_readdir_t readdir;
		link_closedir_t closedir;
		link_rewinddir_t rewinddir;
		link_telldir_t telldir;
		link_seekdir_t seekdir;
		link_malloc_t malloc;
		link_free_t free;
		link_exec_t exec;
		link_time_t time;
		link_task_t task;
		link_rename_t rename;
		link_chown_t chown;
		link_chmod_t chmod;
		link_mkfs_t mkfs;
} link_op_t;


/*! \details This defines the data structure
 * of the reply received from the device.
 */
typedef struct HWPL_PACK {
	int32_t err;
	int32_t err_number;
} link_reply_t;

#define LINK_VID 0x20A0
#define LINK_PID 0x413B

/*! \details Link read-only flag when opening a file/device.
 * \hideinitializer
 */
#define	LINK_O_RDONLY	0
/*! \details Link write-only flag when opening a file/device.
 * \hideinitializer
 */
#define	LINK_O_WRONLY	1
/*! \details Link read-write flag when opening a file/device.
 * \hideinitializer
 */
#define	LINK_O_RDWR		2
/*! \details Link append flag when opening a file/device.
 * \hideinitializer
 */
#define	LINK_O_APPEND	0x0008
/*! \details Link create flag when opening a file/device.
 * \hideinitializer
 */
#define	LINK_O_CREAT	0x0200
/*! \details Link trunc flag when opening a file/device.
 * \hideinitializer
 */
#define	LINK_O_TRUNC	0x0400
/*! \details Link exclude flag when opening a file/device.
 * \hideinitializer
 */
#define	LINK_O_EXCL		0x0800

/*! \details Link non-blocking flag when opening a file/device.
 * \hideinitializer
 */
#define	LINK_O_NONBLOCK	0x4000


//Commands
enum {
	LINK_CMD_NONE,
	LINK_CMD_READSERIALNO,
	LINK_CMD_IOCTL,
	LINK_CMD_READ,
	LINK_CMD_WRITE,
	LINK_CMD_OPEN,
	LINK_CMD_CLOSE,
	LINK_CMD_LINK,
	LINK_CMD_UNLINK,
	LINK_CMD_LSEEK,
	LINK_CMD_STAT,
	LINK_CMD_FSTAT,
	LINK_CMD_MKDIR,
	LINK_CMD_RMDIR,
	LINK_CMD_OPENDIR,
	LINK_CMD_READDIR,
	LINK_CMD_CLOSEDIR,
	LINK_CMD_RENAME,
	LINK_CMD_CHOWN,
	LINK_CMD_CHMOD,
	LINK_CMD_EXEC,
	LINK_CMD_MKFS,
	LINK_CMD_TOTAL
};

#define LINK_BOOTLOADER_CMD_TOTAL LINK_CMD_WRITE
#define LINK_BOOTLOADER_FILDES (-125)


#define LINK_APPFS_EXEC_OPTIONS_FLASH (1<<0)
#define LINK_APPFS_EXEC_OPTIONS_STARTUP (1<<1) //if set executes on boot

typedef struct {
	uint32_t startup /*! The startup routine */;
	uint32_t code_start /*! The start of memory */;
	uint32_t code_size /*! The size of the code section */;
	uint32_t ram_start /*! The start of memory */;
	uint32_t ram_size /*! The size of  memory (total RAM for process) */;
	uint32_t data_size /*! Size of "data" section */;
	uint32_t options /*! A pointer to the re-entrancy structure */;
	uint32_t signature /*! must be valid to execute the file */;
} link_appfs_exec_t; //32 bytes

typedef struct {
	char name[LINK_NAME_MAX] /*! The name of the process or file (must be specially written to the binary) */;
	//uint16_t uid;
	//uint16_t gid;
	uint32_t mode;
} link_appfs_hdr_t;  //28 bytes

typedef struct {
	link_appfs_hdr_t hdr;
	link_appfs_exec_t exec;
} link_appfs_file_t;



/*! \addtogroup USB_HANDLE Device Handling
 * @{
 */

/*! \details This defines the opaque type
 * used to select different USB Link devices.
 */
typedef volatile void * link_dev_t;

/*! \details This initializes the link framework.  This
 * will be called automatically by link_new_dev() if
 * it is not called explicitly.
 * \return Zero on success.
 */
int link_init(void);

/*! \details This shuts down the link framework.
 */
void link_exit(void);

/*! \details This opens a USB Link device based on
 * the serial number.
 * \return The new device or NULL if it could not be opened
 */

link_phy_t link_connect(const char * sn);

/*! \details This closes the specified USB Link device.
 *
 */
int link_disconnect(link_phy_t handle /*! The device to close */);


/*! \details This generates a list of available Link
 * devices.
 * \return A pointer to the new string array.
 */
char * link_new_device_list(int max);

/*! \details This frees a list of USB Link devices
 * (No USB transactions are performed).
 */
void link_del_device_list(char * sn_list /*! The list to free */);

char * link_device_list_entry(char * list, int entry);

/*! \details This function checks to see if there was an error
 * on the last transaction.
 */
int link_get_err(void);

/*! \details This function sets the debugging level for the
 * library (0 for no debugging output).
 */
void link_set_debug(int debug_level);


/*! @} */


//Access to files and devices
/*! \details This function opens a file or device on the
 * target.
 * \return The file handle used to read/write/ioctl the file or device; less
 * than zero on an error.
 */
int link_open(link_phy_t handle, const char * path, int flags, ...);

/*! \details This function performs ioctl control on device associated
 * with the file descriptor.
 * \return Zero on success or an error code.
 */
int link_ioctl(link_phy_t handle, int fildes, int request, ...);

/*! \details This function reads the device (or regular file) associated
 * with the file descriptor.
 *
 * \return The number of bytes read or an error code (less than zero).
 */
int link_read(link_phy_t handle, int fildes, void * buf, int nbyte);

/*! \details This function writes the device (or regular file) associated
 * with the file descriptor.
 *
 * \return The number of bytes writte or an error code (less than zero).
 */
int link_write(link_phy_t handle, int fildes, const void * buf, int nbyte);

/*! \details This function closes the device (or regular file) associated
 * with the file descriptor.
 *
 * \return Zero on success or an error code.
 */
int link_close(link_phy_t handle, int fildes);

/*! \details This function deletes a file from the
 * device's filesystem.
 *
 * \return Zero on success or an HWPL error code.
 */
int link_unlink(link_phy_t handle /*! Device handle */, const char * path /*! The full path to the file to delete */);

//For block devices and files
/*! \details For block devices, this function
 * moves the access pointer.
 *
 * \return Zero on success or an error code.
 */
int link_lseek(link_phy_t handle, int fildes, off_t offset, int whence);

//For files only
int link_stat(link_phy_t handle, const char * path, struct link_stat * buf);
int link_fstat(link_phy_t handle, int fildes, struct link_stat * buf);

//Access to directories
/*! \details This function creates a new directory on
 * the device filesystem.
 * \return 0 on success or an HWPL error code.
 */
int link_mkdir(link_phy_t handle, const char * path /*! The full path to the directory to create */,
		link_mode_t mode /*! Directory mode */);

/*! \details This function removes a directory from
 * the device filesystem.
 * \return 0 on success or an HWPL error code.
 */
int link_rmdir(link_phy_t handle, const char * path /*! The full path to the directory to remove */);

/*! \details This function opens a directory on the device
 * filesystem.  link_readdir() may then be used to
 * read the entries in the directory.
 * \return 0 on success or an HWPL error code.
 */
int link_opendir(link_phy_t handle, const char * dirname /*! The full path to the directory to open */);

/*! \details This function closes a directory that
 * was previously opened with link_opendir().
 *
 * \return 0 on success or an HWPL error code.
 */
int link_closedir(link_phy_t handle, int dirp /*! The handle for the open directory */);

/*! \details This function reads an entry from
 * a the directory that was previously opened with link_opendir().
 * \return 0 on success or an HWPL error code.
 */
int link_readdir_r(link_phy_t handle, int dirp /*! The directory handle */,
		struct link_dirent * entry /*! A pointer to the destination entry */,
		struct link_dirent ** result /*! Points to \a entry on success */);

/*! \details This function formats the filesystem.  All
 * data is erased from the filesystem leaving an
 * emtpy root directory.
 * \return 0 on success or an HWPL error code.
 */
int link_mkfs(link_phy_t handle, const char * path);

/*! \details This function starts a new process on the device.
 *
 * \return Zero on success or an HWPL error code
 */
int link_exec(link_phy_t handle, const char * file);

/*! \details This function creates a new symbolic link.
 *
 * \return Zero on success or an error code.
 */
int link_symlink(link_phy_t handle, const char * old_path, const char * new_path);

/*! \details This function opens the stdio.
 * return Zero on success or an error code.
 */
int link_open_stdio(link_phy_t handle);

/*! \details This function closes the stdio.
 * return Zero on success or an error code.
 */
int link_close_stdio(link_phy_t handle);

/*! \details This function reads the standard output from the device.
 *
 */
int link_read_stdout(link_phy_t handle, void * buf, int nbyte);

/*! \details This function writes the standard input to the device.
 *
 */
int link_write_stdin(link_phy_t handle, const void * buf, int nbyte);

/*! \details This function sets the time on the target device according to the
 * specified struct tm.
 * \return Zero on success or an error code
 */
int link_settime(link_phy_t handle, struct tm * t);

/*! \details This function gets the time on the target device according and
 * stores it in t.
 * \return Zero on success or an error code
 */
int link_gettime(link_phy_t handle, struct tm * t);

/*! \details This function sends the specified signal to the specified pid.
 */
int link_kill_pid(link_phy_t handle, int pid, int signo);




/*! \details This function reads the address of the security word.
 *
 */
int link_get_security_addr(uint32_t * addr);

/*! \details This renames a file on the filesystem.
 *
 */
int link_rename(link_phy_t handle, const char * old_path, const char * new_path);

/*! \details This changes file ownership.
 *
 */
int link_chown(link_phy_t handle, const char * path, int owner, int group);

/*! \details This changes a file mode.
 *
 */
int link_chmod(link_phy_t handle, const char * path, int mode);

/*! \addtogroup BOOTLOADER Bootloader Functions
 * @{
 *
 * \details These functions are supported when the device is in bootloader mode as well as normal mode.
 * The functions link_readflash(), link_writeflash(), and link_erase_flash() are only available in bootloader
 * mode.
 */

/*! \details
 *
 */
int link_isbootloader(link_phy_t handle);


/*! \details
 *
 */
int link_readserialno(link_phy_t handle, char * serialno, int len);


/*! \details This function resets the device
 * \return 0 on success
 */
int link_reset(link_phy_t handle);

/*! \details This function resets the device and starts the DFU bootloader.
 * \return 0 on success
 */
int link_resetbootloader(link_phy_t handle);

/*! \details This function reads the flash memory from the device.
 *
 */
int link_readflash(link_phy_t handle, int addr, void * buf, int nbyte);

/*! \details This function writes the flash memory of the device.
 *
 */
int link_writeflash(link_phy_t handle, int addr, const void * buf, int nbyte);

/*! \details This erases everything in the flash except the bootloader.
 *
 */
int link_eraseflash(link_phy_t handle);

/*! @} */

extern int link_phy_getname(char * dest, const char * last, int len);
extern link_phy_t link_phy_open(const char * name, int baudrate);
extern int link_phy_write(link_phy_t handle, const void * buf, int nbyte);
extern int link_phy_read(link_phy_t handle, void * buf, int nbyte);
extern int link_phy_close(link_phy_t handle);
extern void link_phy_wait(int msec);
extern void link_phy_flush(link_phy_t handle);
extern int link_phy_lock(link_phy_t handle);
extern int link_phy_unlock(link_phy_t handle);
extern int link_phy_status(link_phy_t handle);

int link_protocol_masterwrite(link_phy_t handle, const void * buf, int nbyte);
int link_protocol_masterread(link_phy_t handle, void * buf, int nbyte);
int link_protocol_slavewrite(link_phy_t handle, const void * buf, int nbyte, int (*callback)(void*,void*,int), void * context);
int link_protocol_slaveread(link_phy_t handle, void * buf, int nbyte, int (*callback)(void*,void*,int), void * context);
void link_protocol_insert_checksum(link_pkt_t * pkt);
bool link_protocol_checksum_isok(link_pkt_t * pkt);
int link_protocol_wait_packet(link_phy_t handle, link_pkt_t * pkt, int timeout);
int link_protocol_wait_start(link_phy_t handle, link_pkt_t * pkt, int timeout);

#if defined( __cplusplus )
}
#endif


#endif /* LINK_PROTOCOL_H_ */

/*! @} */
