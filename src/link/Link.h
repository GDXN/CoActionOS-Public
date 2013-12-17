/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*!
 * \addtogroup LINKCPP CPP Wrappers
 * @{
 *
 * \ingroup USBLINK
 *
 */

/*! \file
 * \brief USB Link Protocol Header File
 *
 */

#ifndef LINK_H
#define LINK_H

#include <string>
#include <vector>
#include "hwpl.h"
#include "link_protocol.h"

using namespace std;

/*! \brief Class to access CoActionOS from a desktop C++ application
 * \details This class is used to access devices
 * running CoActionOS from a desktop C++ application.
 *
 */
class Link {
public:
    Link();
    ~Link();

    /*! \details This function creates a list of serial numbers of
     * all the devices that are attached to the host.
     */
    static vector<string> listDevices(int max = 64);

    /*! \details This gets the error message if an
     * operation fails.
     */
    string errorMessage(void){ return errMsg; }

    /*! \details This gets the current progress of an operation.  This allows
     * multi-threaded applications to update a progress bar while copying files.
     */
    int getProgress(void);

    /*! \details This gets the maximum progress value of the current operation.
     *
     */
    int getProgessMax(void);

    /*! \details This gets the status of the current operation in progress.
     *
     */
    string getStatusMessage(void);

    /*! \details This connects to the specified CoActionOS device.  After calling this,
     * other applications will not have access to the device.
     *
     */
    int init(string sn /*! The serial number or an empty string to ignore */);
    int reinit(void){ return init(lastsn); }

    /*! \details This disconnects from the device.  After calling this,
     * other applications can access the device.
     */
    int exit(void);

    /*! \details This checks to see if the device is connected.
     * \return true if connected
     */
    bool connected(void);

    //These are all the file transfer options over CoActionOS Link

    //Operations on the device
    /*! \details This creates a directory on the target device.
     *
     */
    int mkdir(string directory /*! The directory name */,
    		link_mode_t mode /*! The access permissions */);

    /*! \details This removes a directory on the target device.
     *
     */
    int rmdir(string directory /*! The directory name (must be empty) */); //Directory must be empty

    /*! \details This deletes a file on the target device.
     *
     */
    int unlink(string filename /*! The filename to delete */);

    /*! \details This creates a symbolic link on the device.
     * \note CoActionOS does not currently support symbolic links.  This function will
     * always return an error.
     *
     * \return Zero on sucess.
     */
    int symlink(string oldPath /*! The existing path */,
    		string newPath /*! The path to the new link */);

    /*! \details This function opens a directory such that it's contents can be
     * read with readdir().
     */
    int opendir(string directory /*! The directory to open */);

    /*! \details This function reads an entry from an open directory.
     *
     * \return The name of the next entry in the directory.
     */
    int readdir_r(int dirp /*! The directory to read (returned from opendir()) */,
    		struct link_dirent * entry /*! A pointer to the destination memory */,
    		struct link_dirent ** result /*! Assigned to \a entry on success or NULL on failure */);

    /*! \details This closes an open directory.
     *
     */
    int closedir(int dirp /*! The directory to close */);

    /*! \details This copies a file either from the device to the
     * host or from the host to the device depending on the value of \a toDevice.
     *
     * \return Zero on success
     */
    int cp(string src /*! The path to the source file */,
    		string dest /*! The path to the destination file */,
    		link_mode_t mode /*! The access permissions if copying to the device */,
    		bool toDevice = true /*! When true, copy is from host to device */,
    		bool (*update)(int, int) = 0);

    /*! \details This formats the filesystem on the device.
     *
     * \return Zero on success
     */
    int format(string path); //Format the drive

    /*! \details This runs an application on the device.
     *
     * \return Zero on success
     */
    int runApp(string path);


    /*! \details This opens a file (or device such as /dev/adc0) on the target device.
     *
     * \return The file descriptor on success or -1 on failure
     */
    int open(string file /*! The name of the file to open */,
    		int flags /*! The access flags such as LINK_O_RDWR */,
    		link_mode_t mode = 0 /*! The access permissions when creating a new file */);

    /*! \details This reads an open file descriptor.
     *
     * \return Number of bytes read or less than zero on failure
     */
    int read(int fd, void * buf, int nbyte);

    /*! \details This write an open file descriptor.
     *
     * \return Number of bytes written or less than zero on failure
     */
    int write(int fd, const void * buf, int nbyte);

    /*! \details This checks to see if the target is in bootloader mode.
     * \return Non zero if bootloader mode is active.
     */
    bool isBootloader(void);


    /*! \details This reads the flash memory of the device.
     * \return Number of bytes read.
     */
    int readFlash(int addr, void * buf, int nbyte);

    /*! \details This writes the flash memory of the device.
     * \return Number of bytes read.
     */
    int writeFlash(int addr, const void * buf, int nbyte);

    /*! \details This performs IO ctl on an open file descriptor.  \a fd
     * must refer to a device rather than a regular file.  \a ctl must
     * be unionized with a link_bulk_t, for example:
     * \code
     * union {
     * 	adc_attr_t attr;
     * 	link_bulk_t buf;
     * } adc;
     * adc.attr.freq = 200000;
     * adc.attr.enabled_channels = 0x0F;
     * adc.attr.pin_assign = 0;
     * dev.ioctl(fd, I_SETATTR, &adc);
     * \endcode
     *
     * \return Number of bytes read or less than zero on failure
     */
    int ioctl(int fd, int request, void * ctl = NULL);

    /*! \details This function performs an lseek \a fd.
     *
     * \return Zero on success
     */
    int lseek(int fd, int offset, int whence);

    /*! \details This function reads the file statistics on
     * the specified target device file.
     *
     * \return Zero on success with \a st populated or less than zero on failure
     */
    int stat(string path /*! The path to the target device file */,
    		struct link_stat * st /*! A pointer to the destination structure */);
    int close(int fd);

    /*! \details This function opens the stdio on the device.
     * This is associated with /dev/link-stdio.
     */
    int openStdio(void);

    /*! \details This function reads the stdout from the device.
     * This is associated with /dev/link-stdio.
     */
    int readStdout(void * buf /*! the destination pointer */,
    		int nbyte /*! max bytes to read */,
    		volatile bool * abort /*! if set to true, operation will be aborted */);

    /*! \details This function writes the stdin on the device.
     * This is associated with /dev/link-stdio.
     */
    int writeStdin(const void * buf /*! the data to write */, int nbyte /*! the number of bytes to write */);

    /*! \details This function opens the stdio on the device.
     * This is associated with /dev/link-stdio.
     */
    int closeStdio(void);

    /*! \details This function kills the process \a pid.
     *
     * \return Zero on success or less than zero on error
     */
    int killPid(int pid, int signo);

    /*! \details This function resets the device (connection will be terminated)
     *
     * \return Zero on success or less than zero on error
     */
    int reset(void);

    /*! \details This function resets the device and invokes the DFU bootloader.
     * The connection to the device is terminated with this call.
     *
     * \return Zero on success or less than zero on error
     */
    int resetBootloader(void);

    /*! \details This function reads the time from
     * the device.
     *
     * \return Zero on success
     *
     */
    int getTime(struct tm * gt);

    /*! \details This function sets the time on the device.
     * \return Zero on success
     */
    int setTime(struct tm * gt);


    /*! \details This gets the address of the security word on the device.
     * \return Zero on success.
     */
    int getSecurityAddr(uint32_t * addr);

    /*! \details This renames a file.
     * \return Zero on success
     */
    int rename(string old_path, string new_path);

    /*! \details This changes the ownership of a file.
     * \return Zero on success.
     */
    int chown(string path, int owner, int group);

    /*! \details This changes the mode of a file.
     * \return Zero on success.
     */
    int chmod(string path, int mode);

    /*! \details This method checks to see if a process called \a name is running.
     * \return The pid of the running process or -1 if no processes match the name
     */
    int isExecuting(string name);

    int updateOS(string path, bool verify, bool (*update)(int,int) = 0);

    link_phy_t handle();

    void setProgress(int p){ progress = p; }
    void setProgressMax(int p){ progressMax = p; }

private:
    int checkError(int err);

    string lastsn;
    string errMsg;
    string statusMessage;
    int stdoutFd;
    int stdinFd;
    volatile int progress;
    volatile int progressMax;
    int lockDevice(void);
    int unlockDevice(void);
    volatile int lock;
    void resetProgress(void);
    bool isBoot;
    link_phy_t phyHandle;
};

#endif // LINK_H

/*! @} */
