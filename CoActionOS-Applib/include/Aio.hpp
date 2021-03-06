/*! \file */


#ifndef AIO_HPP_
#define AIO_HPP_

#include <errno.h>
#include <aio.h>

/*! \brief Asynchronous IO Class */
/*! \details The Asynchronous IO class is used for performing asynchronous operations on
 * hardware devices.  When calling synchronous IO, the read/write function is called and then
 * returns when the operation is complete.  With Asynchronous IO, the read/write function returns
 * immediately and the Aio class can be used to see when the operation completes.
 *
 * On CoActionOS, synchronous operations will always cause the running thread to yield the processor.
 * This can be avoided by using the Aio class.  Consider the following example:
 *
 * \code
 * #include <applib/Uart.hpp>
 * #include <applib/Aio.hpp>
 * #include <applib/StringTypes.hpp>
 * #include <applib/Timer.hpp
 *
 * StringMedium buf = "Hello World!\n";
 * Uart uart(0); //we will read/write data to UART port 0
 * Aio aio(buf.data(), buf.size()); //aio uses buf as it's data
 *
 * uart.init(2400); //init the UART with baudrate 2400
 *
 *  //First a synchronous write
 * uart.write(buf.c_str(), buf.size());  //this will return when the bytes are written
 *  //While bytes are writing, the processor will be used on another thread
 *
 *  //Now the AIO version
 * uart.write(aio);
 *  //This thread keeps the processor -- so we can do something else in parallel
 *
 * while( aio.done() == false ){
 *   Timer::wait_msec(5); //wait for the opeation to complete
 * }
 *
 * \endcode
 *
 * You can read <a href="/projects/coactionos/wiki/CoActionOS_Tutorial--Async_IO">this tutorial</a>. for another example.
 *
 *
 */
class Aio {
public:

#ifdef __HWPL_ONLY__
	Aio(volatile void * buf, int nbytes, int offset = 0){
		aio_var.buf = (void*)buf;
		aio_var.nbyte = nbytes;
		aio_var.loc = offset;
	}


	/*! \brief Return the buffer pointer */
	volatile void * buf(void){ return aio_var.buf; }
	/*! \brief Set the buffer pointer */
	void setbuf(volatile void * buf){ aio_var.buf = (void*)buf; }
	/*! \brief Return the number of bytes to transfer */
	int nbytes(void){ return aio_var.nbyte; }
	/*! \brief Set the number of bytes to transfer */
	void setnbytes(int nbytes){ aio_var.nbyte = nbytes; }
	/*! \brief Return the offset (or channel for Dac, Adc, Pwm, etc) */
	int offset(void){ return aio_var.loc; }
	/*! \brief Set the offset (or channcel for Dac, Adc, Pwm, etc) */
	void setoffset(int offset){ aio_var.loc = offset; }
	/*! \brief Return the return value of the operation */
	int ret(void){ return aio_var.nbyte; }
	/*! \brief Return the error number of the operation */
	int error(void){ return aio_var.nbyte; }
	/*! \brief Check to see if operation is complete */
	bool done(void){
		if(  aio_var.flags & BUSY_FLAG ){
			return false;
		} else {
			return true;
		}
	}
#else
	/*! \brief Declare an Aio object. */
	Aio(volatile void * buf, int nbytes, int offset = 0){
		aio_var.aio_buf = buf;
		aio_var.aio_nbytes = nbytes;
		aio_var.aio_offset = offset;
		aio_var.aio_sigevent.sigev_notify = SIGEV_NONE;
	}

	/*! \brief Block until all transfers in list have completed or timeout is reached */
	static int suspend(struct aiocb * const list[], int nent, int timeout_usec = 0){
		struct timespec timeout;
		timeout.tv_sec = timeout_usec / 1000000;
		timeout.tv_nsec = (timeout_usec % 1000000) * 1000;
		if( timeout_usec == 0 ){
			return aio_suspend(list, nent, 0);
		} else {
			return aio_suspend(list, nent, &timeout);
		}
	}

	/*! \brief Block until the operation completes or timeout is reached */
	int suspend(int timeout_usec = 0){
		struct aiocb * const list[1] = { &aio_var };
		return suspend(list, 1, timeout_usec);
	}

	/*! \brief Return the buffer pointer */
	volatile void * buf(void) const { return aio_var.aio_buf; }
	/*! \brief Set the buffer pointer */
	void setbuf(volatile void * buf){ aio_var.aio_buf = buf; }
	/*! \brief Return the number of bytes to transfer */
	int nbytes(void) const { return aio_var.aio_nbytes; }
	/*! \brief Set the number of bytes to transfer */
	void setnbytes(int nbytes){ aio_var.aio_nbytes = nbytes; }
	/*! \brief Return the offset (or channel for Dac, Adc, Pwm, etc) */
	int offset(void) const { return aio_var.aio_offset; }
	/*! \brief Set the offset (or channcel for Dac, Adc, Pwm, etc) */
	void setoffset(int offset){ aio_var.aio_offset = offset; }
	/*! \brief Return the return value of the operation */
	int ret(void){ return aio_return(&aio_var); }
	/*! \brief Return the error number of the operation */
	int error(void){ return aio_error(&aio_var); }
	/*! \brief Check to see if operation is complete */
	bool done(void){
		if( aio_error(&aio_var) == EINPROGRESS ){
			return false;
		} else {
			return true;
		}
	}

	/*! \brief Send a signal when the operation completes */
	/*! \details This method will cause the thread to receive a signal when the operation
	 * completes.
	 *
	 * @param number The signal number (ie SIGTERM) use -1 to prevent a signal from being sent
	 * @param value The signal value (passed as an argument to the handler if using siginfo)
	 * @return Zero on success
	 */
	void signal(int number, int value){
		if( number >= 0 ){
			aio_var.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
			aio_var.aio_sigevent.sigev_signo = number;
			aio_var.aio_sigevent.sigev_value.sival_int = value;
		} else {
			aio_var.aio_sigevent.sigev_notify = SIGEV_NONE;
		}
	}

#endif

	/*! \brief Check to see if operation is still in progress */
	bool inprogress(void){ return !done(); }


private:
	friend class Periph;
#ifdef __HWPL_ONLY__
	friend class Usb;
	device_transfer_t aio_var;
	enum {
		BUSY_FLAG = (1<<16)
	};
#else
	struct aiocb aio_var;
#endif
};


#endif /* AIO_HPP_ */
