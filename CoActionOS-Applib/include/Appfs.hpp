/*! \file */

#ifndef APPFS_HPP_
#define APPFS_HPP_

#include <link_protocol.h>

/*! \brief Class for accessing Application file system (Flash/RAM) */
/*! \details This class provides an interface for installing programs in
 * the Flash/RAM as well as for saving data to the flash.
 *
 * The following is a basic example of creating a data file in flash and then reading from it.
 *
 * \code
 * #include <applib/Appfs.hpp>
 * char my_user_data[16];
 *  //populate my_user_data as you please here
 * Appfs::create("my_data", my_user_data, 16); //creates /app/flash/my_data as read only data
 * \endcode
 *
 * Now reading from the file.
 *
 * \code
 * #include <applib/File.hpp>
 * #include <applib/Appfs.hpp>
 * char my_user_data[16];
 * File f;
 * f.open("/app/flash/my_data", File::RDONLY); //only the file read only
 * f.seek( Appfs::offset() ); //seek to the start of user data
 * f.read(my_user_data, 16); //read 16 bytes of user data
 * f.close();  //free resources
 * \endcode
 *
 */
class Appfs {
public:
	Appfs();

	/*! \brief Create a data file in the flash memory */
	/*! \details This method creates a file in flash memory consisting
	 * of the data specified
	 * @param name The name of the data file (no path info)
	 * @param buf A pointer to the data to be saved
	 * @param nbyte The number of bytes to save
	 * @param mount The mount path (default is /app)
	 * @return Zero on success or -1 with errno set accordingly
	 *
	 */
	static int create(const char * name, const void * buf, int nbyte, const char * mount = "/app", link_phy_t h = LINK_PHY_OPEN_ERROR);

	/*! \brief Returns the offset of user data in the file */
	/*! \details Appfs file contain a header that is only applicable to
	 * executable files.  This gives the offset of where the user data
	 * starts with files created using Appfs::create()
	 * @return The offset in bytes
	 *
	 *
	 */
	static int offset(){ return sizeof(link_appfs_file_t); }

	/*! \brief Returns the page size for writing data */
	static int pagesize(){ return APPFS_PAGE_SIZE; }
};

#endif /* APPFS_HPP_ */
