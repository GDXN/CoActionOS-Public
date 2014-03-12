/*! \file */
#ifndef APPLIB_PBLOCK_HPP_
#define APPLIB_PBLOCK_HPP_

#include "Periph.hpp"

/*! \brief Peripheral Block Class
 * \details This is an abstract class for implementing block devices.
 */
class Pblock : public Periph {
public:
	Pblock(port_t port, int numports, int * fd = NULL, bool * isinitialized = NULL);

	using Periph::read;
	using Periph::write;
	/*! \brief Reads the block device at the specified location. */
	virtual int read(int loc, void * buf, int nbyte);
	/*! \brief Writes to the block device at the specified location. */
	virtual int write(int loc, const void * buf, int nbyte);

protected:
#ifdef __HWPL_ONLY__
	int loc;
#endif
};

#endif /* APPLIB_PBLOCK_HPP_ */
