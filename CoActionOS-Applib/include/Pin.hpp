/*! \file */

#ifndef PIN_HPP_
#define PIN_HPP_

#include "Pio.hpp"

/*! \brief Pin class
 * \details This class controls the operation of a single pin.  It works similarly
 * to the Pio class, but the mask is built-in.
 *
 * Here is an example of using the Pin class:
 *
 * \code
 * #include <applib/Pin.hpp>
 *
 * Pin pin(1,0); //control pin P1.0
 *  //Or if working with the pinmask Pin  pin(1, 1<<0, true);
 *
 * pin.init(Pin::OUTPUT); //initialize as an output
 *
 *  //These three all set the pin high
 * pin = true;
 * pin.set();
 * pin.on();
 *
 *  //These turn the pin off
 * pin = false;
 * pin.clr();
 * pin.off();
 *
 *  //now convert to an input
 * pin.setattr(Pin::INPUT | Pin::PULLUP); //or use Pin::FLOAT, Pin::PULLDOWN, etc
 *  //or to init as an input use pin.init(Pin::INPUT);
 *
 *  //now check to see if the value is high or low
 *  if( pin.value() == true ){
 *  	//pin is high
 *  } else {
 *  	//pin is low
 *  }
 *
 *  pin.close(); //close the associated file descriptor (pin keeps its IO properties and state)
 *
 * \endcode
 *
 */
class Pin : public Pio {
public:
	/*! \brief Initialize the object with a port/pin combination. */
	Pin(port_t port, uint32_t pin, bool ismask = false) : Pio(port){
		if( ismask ){
			pinmask_ = pin;
		} else {
			pinmask_ = (1<<pin);
		}
	}

	using Pio::setattr;

	/*! \brief Initialize the pin with the specified mode */
	inline int init(int mode){
		return Pio::init(pinmask_, mode);
	}

	/*! \brief Set the pin attributes */
	inline int setattr(int mode){
		return Pio::setattr(pinmask_, mode);
	}

	/*! \brief Set the pin as an input */
	inline int setinput(int mode = 0){
		return Pio::setattr(pinmask_, mode | PIO_MODE_INPUT);
	}

	/*! \brief Set the pin as an output */
	inline int setoutput(int mode = 0){
		return Pio::setattr(pinmask_, mode | PIO_MODE_OUTPUT);
	}

	/*! \brief Assign a boolean to the pin (true is high, false is low) */
	Pin& operator = (bool on){
		if( on ){
			setmask(pinmask_);
		} else {
			clrmask(pinmask_);
		}
		return *this;
	}

	/*! \brief Get the value of the pin (true is high, false is low) */
	inline bool get(){ return (Pio::get() & pinmask_) != 0; }

	/*! \brief Get the value of the pin (true is high, false is low) */
	inline bool value(){ return (Pio::get() & pinmask_) != 0; }

	/*! \brief Check to see if pin is set (high) */
	inline bool isset(){ return get(); }
	/*! \brief Check to see if pin is clear (low) */
	inline bool isclr(){ return !get(); }

	/*! \brief Set the pin high */
	inline int set(void){ return setmask(pinmask_); }
	/*! \brief clr the pin low */
	inline int clr(void){ return clrmask(pinmask_); }

	/*! \brief Set the pin high */
	inline int on(void){ return setmask(pinmask_); }
	/*! \brief clr the pin low */
	inline int off(void){ return clrmask(pinmask_); }

	pio_sample_t pinmask() const { return pinmask_; }

private:
	pio_sample_t pinmask_;
};

#endif /* PIN_HPP_ */
