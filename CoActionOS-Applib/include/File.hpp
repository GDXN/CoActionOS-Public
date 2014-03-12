/*! \file */

#ifndef FILE_HPP_
#define FILE_HPP_

#include <unistd.h>


#include "Phy.hpp"
#include "String.hpp"

#ifdef fileno
#undef fileno
#endif

/*! \brief File Class
 * \details This class is used to file access.  It uses the POSIX functions open(), read(), write(), close(), etc.  You
 * can always call these functions directly or use the standard C library to access files (fopen(), fread(), fwrite()--
 * these use more memory than this class or the POSIX functions).
 *
 * Here is an example of using this class:
 *
 * \code
 * #include <applib/File.hpp>
 * #include <applib/StringTypes.hpp>
 *
 * File f;
 * StringMedium str;
 *
 *  //create a new file and write a string to it
 * f.create("/home/myfile.txt");
 * str = "Hello New File!\n";
 * f.write(str.c_str(), str.size());
 * f.close();
 *
 *  //Now open the file we just closed
 * f.open("/home/myfile.txt");
 * str = "";
 * f.read(str.data(), str.capacity());
 * f.close();
 *
 *  //This is what was read from the file
 * printf("The String is %s\n", str.c_str());
 *
 * File::remove("/home/myfile.txt"); //delete the file
 *
 * \endcode
 *
 */
class File : public Phy {
public:
	File();

	/*! \brief Delete a file */
	static int remove(const char * name);

	/*! \brief Open an existing file */
	int open(const char * name, int access = RDWR, int perms = 0666);

	/*! \brief Create a new file */
	int create(const char * name, bool overwrite = true, int perms = 0666);

	/*! \brief Read the file */
	int read(void * buf, int nbyte);

	/*! \brief Read the file from the specified location */
	int read(int loc, void * buf, int nbyte);

	/*! \brief Write the file */
	int write(const void * buf, int nbyte);

	/*! \brief Write the file at the specified location */
	int write(int loc, const void * buf, int nbyte);

	/*! \brief See below for details */
	/*! \details List of options for \a whence argument of seek() */
	enum {
		SET = SEEK_SET,
		CURRENT = SEEK_CUR,
		END = SEEK_END

	};

	/*! \brief Seek to the specified location in the file */
	int seek(int loc, int whence = SEEK_SET);

	/*! \brief Return the file size */
	int size(void);

	/*! \brief Close the file */
	int close(void);

	/*! \brief Return the file descriptor */
	int fileno();

	/*! \brief Read up to n-1 bytes in to \a s until end-of-file or \a term is reached.  */
	char * gets(char * s, int n, char term = '\n');

	/*! \brief Read a line in to the string */
	char * gets(String * s, char term = '\n'){ return gets(s->data(), s->capacity(), term); }

	/*! \brief Read a line in to the string */
	char * gets(String & s, char term = '\n'){ return gets(s.data(), s.capacity(), term); }

#ifndef __HWPL_ONLY__
	/*! \brief Return the file descriptor for the peripheral */
	using Phy::ioctl;
	int ioctl(int req, void * arg);
#endif


private:
	int fd;

	enum {
		GETS_BUFFER_SIZE = 128
	};

};

#endif /* FILE_HPP_ */
