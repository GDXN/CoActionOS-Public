/*! \file */

#ifndef APPLIB_SPI_HPP_
#define APPLIB_SPI_HPP_

#include <dev/spi.h>
#include "Pchar.hpp"

/*! \brief SPI Class
 * \details This class gives access to a SPI port.
 */
class Spi : public Pchar {
public:
	Spi(port_t port);
	static int getports(){ return HWPL_SPI_PORTS; }
	int open(int flags = 0);
	/*! \brief get the SPI attributes */
	int getattr(spi_attr_t * attr);
	/*! \brief set the SPI attributes */
	int setattr(const spi_attr_t * attr);
	/*! \brief swap a byte on the SPI bus */
	int swap(int byte);
	/*! \brief set the pointer for duplex operations */
	int setduplex(void * buf);

#ifdef __HWPL_ONLY__
	int read(void * buf, int nbyte);
	int write(const void * buf, int nbyte);
	int close(void);
#endif

	/*! \brief Set SPI attributes using values specified */
	int setattr(int bitrate = 2000000,
			int pin_assign = 0,
			uint8_t mode = SPI_ATTR_MODE0,
			uint8_t master = SPI_ATTR_MASTER,
			uint8_t format = SPI_ATTR_FORMAT_SPI,
			uint8_t width = 8){
		spi_attr_t attr;
		attr.bitrate = bitrate;
		attr.mode = mode;
		attr.pin_assign = pin_assign;
		attr.format = format;
		attr.width = width;
		attr.master = master;
		return setattr(&attr);
	}

	/*! \brief initialize the SPI port */
	int init(int bitrate = 2000000,
			int pin_assign = 0,
			uint8_t mode = SPI_ATTR_MODE0,
			uint8_t master = SPI_ATTR_MASTER,
			uint8_t format = SPI_ATTR_FORMAT_SPI,
			uint8_t width = 8){
		if( open() < 0 ){
			return -1;
		}
		return setattr(bitrate, pin_assign, mode, master, format, width);
	}



private:
#ifndef __HWPL_ONLY__
	static int fd[HWPL_SPI_PORTS];
	static bool isinitialized;
#endif
};

#endif /* APPLIB_SPI_HPP_ */
