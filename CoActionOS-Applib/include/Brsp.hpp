
/*! \file */

#ifndef BRSP_HPP_
#define BRSP_HPP_

#include "Uart.hpp"
#include "StringTypes.hpp"

/*! \brief BRSP Address class */
class BrspAddress : public StringStatic<12> {
public:
	BrspAddress() : StringStatic(){}
};

/*! \brief BRSP class (bluetooth low energy serial protocol) */
/*! \details This class is used to communicate with a BRSP bluetooth
 * low energy module.  It can connect to other BRSP devices creating
 * a wireless serial connection.
 *
 * The following code is an example of using the BRSP class:
 *
 * \code
 * #include <applib/Brsp.hpp>
 * #include <applib/coactionhero.h>
 * #include <applib/StringTypes.hpp>
 *
 * Brsp brsp(1); //BRSP is using UART1
 * StringMedium name;
 * brsp.open(Brsp::NONBLOCK); //open in nonblocking mode
 *  //These are the init settings for using the BLE Device board with the CoAction Hero
 * brsp.setattr(115200, COACTION_EXP_UART_PINASSIGN, UART_PARITY_NONE, UART_ATTR_STOP_BITS_1);
 *
 * name = brsp.name(); //Read the module name
 * printf("Name of BRSP device is %s\n", name.c_str());
 * brsp.close(); //Close the UART when access is not needed
 * \endcode
 *
 */
class Brsp : public Uart{
public:
	/*! \brief Construct a BRSP object */
	/*! \details This constructs a BRSP object.
	 *
	 * @param port The UART port where the device is connected
	 */
	Brsp(Uart::port_t port) : Uart(port){ timeout = 100; };

	/*! \brief The the millisecond timeout for waiting for replies */
	inline void settimeout(int t){ timeout = t; }

	/*! \brief See below for details */
	/*! \details List of BRSP modes */
	typedef enum {
		MODE_COMMAND /*! Command mode (send commands over UART) */,
		MODE_DATA /*! Data mode (send data to connected device) */,
		MODE_REMOTE_COMMAND /*! Remote command mode (send commands to connected device) */
	} mode_t;

	/*! \brief See below for details. */
	/*! \details List of baudrates when setting module baudrate (see setuart()). */
	typedef enum {
		BAUDRATE_9600 /*! 9600 Baud */,
		BAUDRATE_19200 /*! 19200 Baud */,
		BAUDRATE_38400 /*! 38400 Baud */,
		BAUDRATE_57600 /*! 57600 Baud */,
		BAUDRATE_115200 /*! 115200 Baud */,
		BAUDRATE_230400 /*! 230400 Baud */,
		BAUDRATE_460800 /*! 460800 Baud */,
		BAUDRATE_921600 /*! 921600 Baud */
	} baudrate_t;

	/*! \brief See below for details. */
	/*! \details Parity values (see setuart()) */
	typedef enum {
		PARITY_NONE /*! No Parity */,
		PARITY_ODD /*! Odd Parity */,
		PARITY_EVEN /*! Even Parity */
	} parity_t;

	/*! \brief See below for details. */
	/*! \details List of Stop bit settings (see setuart()) */
	typedef enum {
		STOPBITS_ONE /*! One stop bit */,
		STOPBITS_TWO /*! Two stop bits */
	} stopbits_t;

	/*! \brief See below for details. */
	/*! \details List of Flowcontrol bit settings (see setuart()) */
	typedef enum {
		FLOWCONTROL_OFF /*! Flowcontrol off */,
		FLOWCONTROL_ON /*! Flowcontrol on */
	} flowcontrol_t;

	/*! \brief set UART configuration */
	bool setuart(baudrate_t baudrate, parity_t parity, stopbits_t stopbits, flowcontrol_t flowcontrol);

	/*! \brief Change between command, data, and remote command modes */
	int setmode(mode_t mode, int handle = -1);

	/*! \brief Discover new devices */
	int discover(BrspAddress addrs[], int max);

	/*! \brief Connect to the specified address */
	int connect(const BrspAddress & addr, mode_t mode);

	/*! \brief Disconnect \a handle */
	int disconnect(int handle = 0);

	/*! \brief Check if device is paired (returns true if paired) */
	bool paired(void);

	/*! \brief Get the name of the device */
	inline StringSmall name(void){ return property("ATSN?\r"); }

	/*! \brief Get the version of the device */
	inline StringSmall version(void){ return property("ATV?\r"); }

	/*! \brief Get the module type */
	inline StringSmall type(void){ return property("ATMT?\r"); }

	/*! \brief Get the stack type */
	inline StringSmall stacktype(void){ return property("ATST?\r"); }

	/*! \brief Get the bluetooth address */
	inline StringSmall addr(void){ return property("ATA?\r"); }

	/*! \brief Get the RSSI value */
	StringSmall rssi(int handle);

	/*! \brief Listen for an event */
	StringMedium listen(void);

	/*! \brief Check to see if the specified event arrived */
	bool event(StringSmall ev);

	/*! \brief Set the name of the device */
	bool setname(StringSmall name);

	/*! \brief Send a command (parse response with a callback) */
	int sendcommand(const char * command, void (*response)(void *, const void *,int) = 0, void * response_arg = 0);
	/*! \brief Send a command (write response to \a buffer) */
	int sendcommand(const char * command, void * buffer, int max);
	/*! \brief Wait for the \a expected response */
	bool wait(const char * expected);


	/*! \brief Set the duty cycle of the connected status LED (PIO2) */
	int set_connect_duty(uint16_t duty, uint16_t period){ return set_duty('0', duty, period); }
	/*! \brief Set the duty cycle of the disconnected status LED (PIO5) */
	int set_status_duty(uint16_t duty, uint16_t period){ return set_duty('1', duty, period); }

	/*! \brief Flush the serial buffer */
	void flush(void);

	StringSmall property(StringSmall command);
	int setproperty(StringSmall cmd, StringSmall value);

	using Uart::write;
	using Periph::write;

private:
	int timeout;

	int set_duty(char led, uint16_t duty, uint16_t period);
};



#endif /* BRSP_HPP_ */
