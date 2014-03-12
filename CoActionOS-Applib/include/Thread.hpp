/*! \file */

#ifndef THREAD_HPP_
#define THREAD_HPP_

#include <pthread.h>

/*! \brief Thread Class
 * \details This class creates and manages new threads.  This classes using POSIX thread calls to create
 * new threads.
 *
 * For an example of using this class please see <a href="/projects/coactionos/wiki/CoActionOS_Tutorial--Thread">this
 * tutorial</a>.
 *
 *
 */
class Thread {
public:

	/*! \brief Construct a new thread object */
	/*! \details This constructs a new thread object
	 *
	 * @param stack_size  The stack size of the new thread (default is 4096)
	 * @param detached Whether to create as a detached thread.  If this value is false,
	 * another thread must use join() in order for the thread to terminate correctly.
	 */
	Thread(int stack_size = 4096, bool detached = true);

	/*! \brief Scheduling policies */
	enum {
		OTHER /*! Default scheduling algorithm (SCHED_OTHER), the thread uses a round robin algorithm with a 0 priority value.  */ = SCHED_OTHER,
		ROUND_ROBIN /*! Spend up to the total round robin time on a task before switching, variable priority allowed */ = SCHED_RR,
		FIFO /*! Stay in the thread until the thread yields (usleep(), read()/write(), other Thread::yield()), variable priority allowed */ = SCHED_FIFO
	};

	/*! \brief Sets the stacksize (no effect after create) */
	int setstacksize(int size);

	/*! \brief Gets the stacksize */
	int stacksize(void);

	/*! \brief Sets the thread priority */
	int setpriority(int prio, int policy = ROUND_ROBIN);

	/*! \brief Gets the thread priority */
	int priority(void);

	/*! \brief Get the thread policy */
	int policy(void);

	/*! \brief Gets the ID of the thread */
	int id(void);

	/*! \brief Start the thread */
	int create(void * (*func)(void*), void * args = NULL, int prio = 0, int policy = OTHER);

	/*! \brief Check if the thread is running */
	bool isrunning(void);

	/*! \brief Wait for the thread to complete (joins thread if it is not detached) */
	int wait(void**ret, int interval = 1000);

	/*! \brief Yield the processor to another thread */
	static void yield(void);


private:
	pthread_attr_t pthread_attr;
	pthread_t id_var;
};

#endif /* THREAD_HPP_ */
