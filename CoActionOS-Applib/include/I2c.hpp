/*! \file */

#ifndef APPLIB_I2C_HPP_
#define APPLIB_I2C_HPP_

#include <dev/i2c.h>
#include "Pblock.hpp"

/*! \brief I2C Peripheral Class
 * \details This class implements I2C device peripherals.
 */
class I2c : public Pblock {
public:
	/*! \brief Initialize the instance with \a port */
	I2c(port_t port);
	static int getports(){ return HWPL_I2C_PORTS; }
	int open(int flags = 0);

	/*! \brief Get the I2C attributes */
	int getattr(i2c_attr_t * attr);
	/*! \brief Set the I2C attributes */
	int setattr(const i2c_attr_t * attr);
	/*! \brief Setup an I2C transaction */
	int setup(const i2c_reqattr_t * req);

	/*! \brief Get the last error */
	int geterr(void);

#ifdef __HWPL_ONLY__
	using Pblock::read;
	using Pblock::write;
	int read(void * buf, int nbyte);
	int write(const void * buf, int nbyte);
	int close(void);
#endif


	/*! \brief Setup an I2C transaction using the slave addr and type */
	int setup(uint16_t slave_addr, i2c_transfer_t type = I2C_TRANSFER_NORMAL){
		i2c_reqattr_t req;
		req.slave_addr = slave_addr;
		req.transfer = type;
		return setup(&req);
	}

	/*! \brief Set attributes using specified bitrate and pin assignment. */
	int setattr(uint32_t bitrate = 100000, uint8_t pin_assign = 0){
		i2c_attr_t attr;
		attr.bitrate = bitrate;
		attr.pin_assign = pin_assign;
		return setattr(&attr);
	}

	/*! \brief Initialize the I2C
	 * \details This method initializes the I2C port.  It opens the port
	 * and sets the attributes as specified.
	 * \code
	 * I2c i2c(0);  //Use port 0
	 * i2c.init(400000); //400KHz bitrate
	 * \endcode
	 *
	 */
	int init(int bitrate = 100000, int pin_assign = 0){
		if( open() < 0 ){
			return -1;
		}
		return setattr(bitrate, pin_assign);
	}
private:
#ifndef __HWPL_ONLY__
	static int fd[HWPL_I2C_PORTS];
	static bool isinitialized;
#endif
};

/*! \brief IIC Class (alternate class for I2c)
 *
 */
class Iic : public I2c {
public:
	Iic(port_t port) : I2c(port){}
};

#endif /* APPLIB_I2C_HPP_ */
