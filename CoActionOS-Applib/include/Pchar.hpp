/*! \file */

#ifndef APPLIB_PCHAR_HPP_
#define APPLIB_PCHAR_HPP_

#include "Periph.hpp"

/*! \brief Peripheral Character Class
 * \details This class is an abstract class for peripheral character devices.
 */
class Pchar : public Periph {
public:
	Pchar(port_t port, int numports, int * fd = NULL, bool * isinitialized = NULL);

	int getline(const char * dest, int nbyte);

};

#endif /* APPLIB_PCHAR_HPP_ */
