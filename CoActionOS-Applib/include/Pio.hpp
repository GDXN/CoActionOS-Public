/*! \file */

#ifndef APPLIB_PIO_HPP_
#define APPLIB_PIO_HPP_

#include <dev/pio.h>
#include "Pchar.hpp"


/*! \brief Pin Input/Output Class
 * \details This class controls pin input/output.
 *
 * Example:
 * \code
 *	//This allocates a PIO object but does not affect HW state
 * Pio pio(2); //use pio port 2
 *
 * 	//This opens the port and sets the attributes -- same as pio.open(); pio.setattr(1<<10, PIO_MODE_OUTPUT);
 * pio.init(1<<10, PIO_MODE_OUTPUT);
 *
 * 	//Bits can be manipulated using clrmask() and setmask()
 * pio.clrmask(1<<10); //clear bit 10 of port 2
 * pio.setmask(1<<10); //set bit 10
 *
 *	//the pin can be reconfigured as an input
 * pio.setattr(1<<10, PIO_MODE_INPUT | PIO_MODE_PULLDOWN); //set as an input with internal pulldown
 *
 * 	//get() is used to read the port
 * if( pio.get() & (1<<10) ){
 *	//bit 10 is set
 * }
 *
 * 	//This closes the port for all instances (ports are closed automatically when program exits)
 * pio.close();
 *
 * \endcode
 */
class Pio : public Periph {
public:
	Pio(port_t port);
	static int getports(){ return HWPL_PIO_PORTS; }
	int open(int flags = 0);
	/*! \brief Get the attributes for the port */
	int getattr(pio_attr_t * attr);
	/*! \brief Set the attributes for the port */
	int setattr(const pio_attr_t * attr);
	/*! \brief Set the action for the port */
	int setaction(const pio_action_t * action);
	/*! \brief Set the specified pin mask */
	int setmask(unsigned int mask);
	/*! \brief Clear the specified mask */
	int clrmask(unsigned int mask);
	/*! \brief Get the value of the port */
	unsigned int get(void);
	/*! \brief Get the value of the port */
	inline unsigned int value(void){ return get(); }

	/*! \brief Set the value of the port */
	int set(unsigned int value);


	/*! \brief Set the attributes for the port */
	int setattr(pio_sample_t mask, int mode){
		pio_attr_t attr;
		attr.mask = mask;
		attr.mode = mode;
		return setattr(&attr);
	}

	/*! \brief Initialize the port.
	 * \details This method opens the port
	 * then sets the attributes using the specified mask and mode.
	 * \code
	 * Pio p(0);
	 * p.init((1<<10)|(1<<5), PIO_MODE_OUTPUT)); //pins 0.5 and 0.10 are outputs
	 * \endcode
	 */
	int init(pio_sample_t mask, int mode){
		if( open() <  0 ){
			return -1;
		}
		return setattr(mask, mode);
	}

	/*! \brief Initialize a single pin as an output
	 * \details This method initializes a single
	 * pin to the specified output mode.
	 *
	 * \code
	 *  //configure as an output
	 * Pio p(0);
	 * p.open();
	 * p.setoutput(1<<10);
	 * \endcode
	 * Or:
	 * \code
	 *   //configure as open drain output
	 * Pio p(0);
	 * p.open();
	 * p.setoutput(1<<10, PIO_MODE_OPENDRAIN);
	 * \endcode
	 *
	 * \sa setattr(), init()
	 *
	 */
	int setoutput(pio_sample_t mask, int mode = 0){	return setattr(mask, mode | PIO_MODE_OUTPUT); }

	/*! \brief Initialize a single pin as an input
	 * \details This method initializes a single
	 * pin to the specified mode.  This uses setattr() to
	 * configure the pin as at output.
	 *
	 * \code
	 *  //configure as an input
	 * Pio p(0);
	 * p.open();
	 * p.setinput(1<<10);
	 * \endcode
	 * Or:
	 * \code
	 *   //configure as open input with pullup enabled
	 * Pio p(0);
	 * p.open();
	 * p.setinput(1<<10, PIO_MODE_PULLUP);
	 * \endcode
	 *
	 * \sa setattr(), init()
	 *
	 */
	int setinput(pio_sample_t mask, int mode = 0){ 	return setattr(mask, mode | PIO_MODE_INPUT); }

	enum {
		INPUT /*! Input mode */ = PIO_MODE_INPUT,
		OUTPUT /*! Output mode */ = PIO_MODE_OUTPUT,
		FLOAT /*! Input mode floating (| with INPUT) */ = PIO_MODE_FLOAT,
		PULLUP /*! Input mode pullup (| with INPUT) */ = PIO_MODE_PULLUP,
		PULLDOWN /*! Input mode pulldown (| with INPUT) */ = PIO_MODE_PULLDOWN,
		OPENDRAIN /*! Output mode open drain (| with OUTPUT) */ = PIO_MODE_OPENDRAIN,
		REPEATER /*! Input mode repeater (| with INPUT) */ = PIO_MODE_REPEATER,
		HYSTERESIS /*! Use hysteresis (| with INPUT) */ = PIO_MODE_HYSTERESIS
	};

#ifdef __HWPL_ONLY__
	int close();
#endif

private:
#ifndef __HWPL_ONLY__
	static int fd[HWPL_PIO_PORTS];
	static bool isinitialized;
#endif
};

#endif /* APPLIB_PIO_HPP_ */
