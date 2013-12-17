/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \mainpage
 *
 * \ref VERSION
 *
 * \section INTRO Introduction
 *
 * This is the CoActionOS API documentation.  This is recommended for users looking to write device
 * drivers or tweak the kernel.  If you are writing applications, see the
 * <a href="/redmine/projects/coactionos-applib/embedded/index.html">CoActionOS Applib API documentation</a>.
 *
 *
 * \subsection API POSIX API
 *
 * CoActionOS supports a subset of the \ref STDC and \ref POSIX APIs.  Many programmers are already familiar with these
 * APIs which eases the learning curve.  Additionally, reference material for these standards is widely
 * available on the web.
 *
 * \see \ref STDC, \ref POSIX
 *
 * \subsection MULTI Multi-tasking
 *
 * CoActionOS supports both threads and processes.  Threads are programmed using the \ref PTHREAD API.  Processes are
 * unique because CoActionOS does not use a memory management unit (MMU).  Instead, it uses a memory
 * protection unit (MPU) to prevent one process from reading or writing data from another process despite both processes
 * running in the same address space.  Interprocess communication is accomplished with the \ref SEMAPHORE,
 * \ref MQUEUE, and pthread APIs as well as with the filesystem (see \ref SYS_FIFO).
 *
 * \subsection FILESYSTEM Filesystem
 *
 * CoActionOS supports multiple filesystems.  There are three required filesystems and one optional filesystem included
 * in CoActionOS:
 *  - Root Filesystem
 *  - Application Filesystem
 *  - Device Filesystem
 *  - Flash Filesystem (optional)
 *
 *  Other filesystems (such as FAT) can also be added by the user.
 *
 * Files on CoActionOS are accessed using the \ref STDC or \ref UNISTD functions: \ref open(), \ref read(), \ref write(),
 * \ref close(), etc.  The root filesystem is located at "/" and only supports the \ref DIRENT API.  It is used for
 * browsing the child filesystems.
 *
 * The application filesystem manages data and code stored in the microcontroller's flash memory or RAM.  By default, it
 * is located at "/app" and includes the folders "/app/flash" and "/app/ram".  It is primarily used to install new programs
 * but may also be used in a limited way for both temporary and non-volatile data storage.
 *
 * The device filesystem comprises the hardware access API.  It is located at "/dev" and allows access to both
 * peripheral microcontroller hardware and external ICs.  The folder, per se, is read-only;
 * all available devices are fixed when the kernel is installed.  The devices in the folder usually support reading
 * as well as writing and are classified as either character special or block special files.
 *
 * The flash filesystem ("/home") is a fully-functional data-storage filesystem.  It requires an external IC (such as a
 * serial flash chip).
 *
 * \section DESIGN Design
 *
 * CoActionOS is divided into three layers (from the top down):
 * - OS Layer (scheduler, filesystem, etc)
 * - Hardware Device Library (external and system device level abstraction)
 * - Hardware Peripheral Library (MCU level abstraction)
 *
 * \subsection OS_DESIGN OS
 *
 * The OS layer comprises the C standard library as well as a subset of the POSIX standard including
 * the scheduler, threads, processes, and hardware abstraction.
 *
 * \subsection HWDL_DESIGN Hardware Device Library (HWDL)
 *
 * HWDL is the hardware device abstraction layer and includes drivers for a growing collection of ICs.
 * These ICs interface with the microcontroller using any combination of peripherals (SPI, UART, I2C, etc)
 * and GPIO pins.  HWDL drivers make direct calls to the HWPL library functions.
 *
 * \subsection HWPL_DESIGN Hardware Peripheral Library (HWPL)
 *
 * The HWPL comprises all the drivers to access the on-chip micrcontroller features
 * and peripherals including low-level task management.  For example, this layer includes drivers for the on-chip UART and the on-chip clock control.
 * It is the only layer that is dependent on the microcontroller.
 *
 * \subsubsection ARCHITECTURES Architectures
 *
 * HWPL currently supports:
 * - ARM Cortex M3 Core:  CMSIS, Fault Handling, Memory Protection, SysTick, NVIC, Tasks including Context Switching
 * - NXP LPC17xx:  Cortex M3 Core, ADC, DAC (with DMA) Flash Memory, GPIO, USB Device, UART (with DMA), SPI (and SSP), I2C,
 * Timers, RTC, PWM, QEI, WDT; support planned for: CAN; see \ref LPC17XXDEV.
 * - USB Link:  USB Link technology is a desktop API for accessing MCU peripherals which currently supports
 * peripheral IO operations on devices running CoActionOS; see \ref USBLINK.
 *
 *
 */

/*! \file */

/*! \addtogroup CAOS CoActionOS
 * @{
 *
 * \details CoActionOS includes two main APIs for users: the \ref STDC and the \ref POSIX API.
 *
 */

/*! \addtogroup LINK USB Link
 * @{
 */

/*! @} */

/*!
 *
 * \addtogroup STDC C Standard Library
 * @{
 * \details CoActionOS comes pre-installed with the majority of the C Standard library including:
 * - <assert.h>
 * - <ctype.h>
 * - <errno.h>
 * - <float.h>
 * - <inttypes.h>
 * - <iso646.h>
 * - <limits.h>
 * - <locale.h>
 * - <math.h>
 * - <setjmp.h>
 * - <signal.h>
 * - <stdarg.h>
 * - <stdbool.h>
 * - <stddef.h>
 * - <stdint.h>
 * - <stdio.h>
 * - <stdlib.h>
 * - <string.h>
 * - <tgmath.h>
 * - <time.h>
 * - <wchar.h>
 * - <wctype.h>
 *
 * The following C standard library components are not available:
 * - <complex.h>
 * - <fenv.h>
 *
 * If you need to familiarize yourself with the details of the C Standard Library, try
 * the <a href="http://en.wikipedia.org/wiki/C_standard_library">Wikipedia page</a>.
 *
 *
 */

/*! @} */

/*! \addtogroup POSIX POSIX
 * @{
 *
 * \details The POSIX API contains functions that allow users to create new threads, access files as well as hardware, and
 * perform interprocess communication.
 */

/*! \addtogroup TIME Time
 * @{
 *
 * \details Unix style time functions are available in CoActionOS as documented herein.  The C standard library
 * time functions are also available (see \ref STDC).
 *
 */

/*! @} */

/*! \addtogroup UNISTD Unix Standard (unistd)
 * @{
 *
 */

/*! \addtogroup UNI_FS Filesystem
 * @{
 */

/*! \addtogroup UNI_PERMS Permissions
 * @{
 */

/*! @} */

/*! \addtogroup UNI_FILE_ACCESS Access
 * @{
 */

/*! @} */

/*! \addtogroup UNI_FILDES Descriptor Handling
 * @{
 */

/*! @} */

/*! @} */

/*! \addtogroup UNI_PROCESS Processes
 * @{
 */

/*! @} */

/*! \addtogroup UNI_SLEEP Sleep Funcions
 * @{
 */

/*! @} */



/*! @} */

/*! \addtogroup PTHREAD Posix Threads (pthread)
 * @{
 */

/*! @} */

/*! \addtogroup SIGNAL Signal Handling
 * @{
 */

/*! @} */

/*! \addtogroup GRP Groups
 * @{
 */

/*! @} */

/*! \addtogroup PWD Password
 * @{
 */

/*! @} */

/*! \addtogroup SEMAPHORE Semaphore
 * @{
 */

/*! @} */

/*! \addtogroup MQUEUE Message Queue (mqueue)
 * @{
 */

/*! @} */

/*! \addtogroup DIRENT Directory Entry (dirent)
 * @{
 */

/*! @} */

/*! \addtogroup ERRNO Error Numbers
 * @{
 *
 * \details
 *
 * Many functions write the \a errno value to indicate which error has occured.  The list below associates
 * \a errno values to their descriptions.  Alternatively, \a perror() can be used to show the description
 * of the error on the stderr descriptor (normally the same descriptor as the stdout).  For example:
 * \code
 * int fd;
 * fd = open("/noexist", O_RDONLY);
 * if ( fd < 0 ){
 * 	perror("Could not open /noexist");
 * }
 * \endcode
 *
 * The above code outputs "Could not open /noexist: No such file or directory\n" on the stderr while \a errno is
 * assigned a value of 2.
 *
 * - EPERM 1:  Not super-user (Not owner)
 * - ENOENT 2:  No such file or directory
 * - ESRCH 3:  No such process
 * - EINTR 4:  Interrupted system call
 * - EIO 5:  I/O error
 * - ENXIO 6:  No such device or address
 * - E2BIG 7:  Arg list too long
 * - ENOEXEC 8:  Exec format error
 * - EBADF 9:  Bad file number
 * - ECHILD 10:  No children
 * - EAGAIN 11:  Resource temporarily unavailable
 * - ENOMEM 12:  Not enough core
 * - EACCES 13:  Permission denied
 * - EFAULT 14:  Bad address
 * - ENOTBLK 15:  Block device required
 * - EBUSY 16:  Mount device busy
 * - EEXIST 17:  File exists
 * - EXDEV 18:  Cross-device link
 * - ENODEV 19:  No such device
 * - ENOTDIR 20:  Not a directory
 * - EISDIR 21:  Is a directory
 * - EINVAL 22:  Invalid argument
 * - ENFILE 23:  Too many open files in system
 * - EMFILE 24:  Too many open files
 * - ENOTTY 25:  Not a character device
 * - ETXTBSY 26:  Text file busy
 * - EFBIG 27:  File too large
 * - ENOSPC 28:  No space left on device
 * - ESPIPE 29:  Illegal seek
 * - EROFS 30:  Read only file system
 * - EMLINK 31:  Too many links
 * - EPIPE 32:  Broken pipe
 * - EDOM 33:  Math arg out of domain of func
 * - ERANGE 34:  Math result not representable
 * - ENOMSG 35:  No message of desired type
 * - EIDRM 36:  Identifier removed
 * - ECHRNG 37:  Channel number out of range
 * - EL2NSYNC 38:  Level 2 not synchronized
 * - EL3HLT 39:  Level 3 halted
 * - EL3RST 40:  Level 3 reset
 * - ELNRNG 41:  Link number out of range
 * - EUNATCH 42:  Protocol driver not attached
 * - ENOCSI 43:  No CSI structure available
 * - EL2HLT 44:  Level 2 halted
 * - EDEADLK 45:  Deadlock condition
 * - ENOLCK 46:  No record locks available
 * - EBADE 50:  Invalid exchange
 * - EBADR 51:  Invalid request descriptor
 * - EXFULL 52:  Exchange full
 * - ENOANO 53:  No anode
 * - EBADRQC 54:  Invalid request code
 * - EBADSLT 55:  Invalid slot
 * - EDEADLOCK 56:  File locking deadlock error
 * - EBFONT 57:  Bad font file fmt
 * - ENOSTR 60:  Device not a stream
 * - ENODATA 61:  No data (for no delay io)
 * - ETIME 62:  Timer expired
 * - ENOSR 63:  Out of streams resources
 * - ENONET 64:  Machine is not on the network
 * - ENOPKG 65:  Package not installed
 * - EREMOTE 66:  The object is remote
 * - ENOLINK 67:  The link has been severed
 * - EADV 68:  Advertise error
 * - ESRMNT 69:  Srmount error
 * - ECOMM 70:  Communication error on send
 * - EPROTO 71:  Protocol error
 * - EMULTIHOP 74:  Multihop attempted
 * - ELBIN 75:  Inode is remote (not really error)
 * - EDOTDOT 76:  Cross mount point (not really error)
 * - EBADMSG 77:  Trying to read unreadable message
 * - EFTYPE 79:  Inappropriate file type or format
 * - ENOTUNIQ 80:  Given log. name not unique
 * - EBADFD 81:  f.d. invalid for this operation
 * - EREMCHG 82:  Remote address changed
 * - ELIBACC 83:  Can't access a needed shared lib
 * - ELIBBAD 84:  Accessing a corrupted shared lib
 * - ELIBSCN 85:  .lib section in a.out corrupted
 * - ELIBMAX 86:  Attempting to link in too many libs
 * - ELIBEXEC 87:  Attempting to exec a shared library
 * - ENOSYS 88:  Function not implemented
 * - ENMFILE 89      :  No more files
 * - ENOTEMPTY 90:  Directory not empty
 * - ENAMETOOLONG 91:  File or path name too long
 * - ELOOP 92:  Too many symbolic links
 * - EOPNOTSUPP 95:  Operation not supported on transport endpoint
 * - EPFNOSUPPORT 96 :  Protocol family not supported
 * - ECONNRESET 104  :  Connection reset by peer
 * - ENOBUFS 105:  No buffer space available
 * - EAFNOSUPPORT 106 :  Address family not supported by protocol family
 * - EPROTOTYPE 107:  Protocol wrong type for socket
 * - ENOTSOCK 108:  Socket operation on non-socket
 * - ENOPROTOOPT 109:  Protocol not available
 * - ESHUTDOWN 110:  Can't send after socket shutdown
 * - ECONNREFUSED 111:  Connection refused
 * - EADDRINUSE 112:  Address already in use
 * - ECONNABORTED 113:  Connection aborted
 * - ENETUNREACH 114:  Network is unreachable
 * - ENETDOWN 115:  Network interface is not configured
 * - ETIMEDOUT 116:  Connection timed out
 * - EHOSTDOWN 117:  Host is down
 * - EHOSTUNREACH 118:  Host is unreachable
 * - EINPROGRESS 119:  Connection already in progress
 * - EALREADY 120:  Socket already connected
 * - EDESTADDRREQ 121:  Destination address required
 * - EMSGSIZE 122:  Message too long
 * - EPROTONOSUPPORT 123:  Unknown protocol
 * - ESOCKTNOSUPPORT 124:  Socket type not supported
 * - EADDRNOTAVAIL 125:  Address not available
 * - ENETRESET 126
 * - EISCONN 127:  Socket is already connected
 * - ENOTCONN 128:  Socket is not connected
 * - ETOOMANYREFS 129
 * - EPROCLIM 130
 * - EUSERS 131
 * - EDQUOT 132
 * - ESTALE 133
 * - ENOTSUP 134:  Not supported
 * - ENOMEDIUM 135   :  No medium (in tape drive)
 * - ENOSHARE 136    :  No such host or network path
 * - ECASECLASH 137  :  Filename exists with different case
 * - EILSEQ 138
 * - EOVERFLOW 139:  Value too large for defined data type
 * - ECANCELED 140:  Operation canceled
 * - ENOTRECOVERABLE 141:  State not recoverable
 * - EOWNERDEAD 142:  Previous owner died
 * - ESTRPIPE 143:  Streams pipe error
 * - EWOULDBLOCK EAGAIN:  Operation would block
 */

/*! @} */

/*! @} */

#ifndef CAOS_H_
#define CAOS_H_

//Standard libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "hwpl.h"
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details This function puts the controller
 * in hibernate mode.  All CPU and peripheral activity is stopped.
 * The RTC continues running and will wake up the controller
 * after \a seconds has elapsed.  When the controller wakes up, it resumes
 * execution in the same state as before \a hibernate() was called.
 *
 * Hibernate corresponds to \a CORE_SLEEP_DEEPSLEEP.  It can be woken up
 * before \a seconds elapsed if other interrupts are enabled such
 * as pin change interrupts. See the device specific documentation
 * for wakeup events for \a CORE_SLEEP_DEEPSLEEP.
 *
 * @param seconds Number of seconds to hibernate (zero to hibernate indefinitely)
 *
 */
int hibernate(int seconds);

 /*! \details This function turns the controller off. The RTC
  * stays on and wakes up the controller after \a seconds has elapsed. When
  * the controller wakes up from power down, it resets.
  *
  * @param seconds Number of seconds to powerdown (zero to powedown indefinitely)
  *
  *
  */
void powerdown(int seconds);


/*! \details This function is weakly bound to code
 * that initializes each filesystem and starts any processes
 * that are designed to auto-run.  It can be completely replaced so
 * that users can customize the startup sequence.
 */
void * initial_thread(void * arg) HWPL_WEAK;

/*! \details Format the filesystem that is mounted at \a path.
 * \return Zero on success
 *
 */
int mkfs(const char * path);

void htoa(char * dest, int num);
char htoc(int nibble);


/*! \details This structure is for executing a function as a result of an interrupt
 * in non-privileged mode.
 */
typedef struct {
	int tid;
	int si_signo;
	int si_sigcode;
	int sig_value;
	int keep;
} signal_callback_t;

/*! \details This function can be set as the callback for hwpl_action_t.
 * In this case, the context is pointing to a signal_callback_t.  When
 * the event happens, it will send a signal to the specified task.
 *
 * If keep is non-zero, the signal will be sent each time the interrupt happens.
 * Otherwise, the signal is just sent one time.
 *
 * In the example below, the thread below will receive a SIGUSR1 the next time
 * the external interrupt goes low.  The run_on_sigusr1() will execute when the
 * signal is received.
 *
 * \code
 * #include <pthread.h>
 * #include <signal.h>
 * #include <applib/Eint.hpp>
 *
 * void run_on_sigusr1(int a){
 * 	printf("got sigusr1\n");
 * }
 *
 * Eint intr(0);
 * intr.init();
 *
 * signal(SIGUSR1, (_sig_func_ptr)run_on_sigusr1);
 *
 * signal_callback_t sig; //this must be valid when the interrupt happens
 * sig.tid = pthread_self();
 * sig.signo = SIGUSR1;
 * sig.keep = 0;
 * intr.setaction(0, EINT_ACTION_EVENT_FALLING, signal_callback, &sig);
 * \endcode
 *
 */
int signal_callback(void * context, const void * data);


#ifdef __cplusplus
}
#endif

#endif /*  CAOS_H_ */

/*! @} */
