/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


/*! \addtogroup SIGNAL
 *
 * @{
 *
 * \ingroup POSIX
 */

/*! \file */

#include <signal.h>

/*! \details This function sets \a signo in \a set.
 * \return 0
 */
int sigaddset(sigset_t * set, const int signo){
	*set |= (1<<signo);
	return 0;
}

/*! \details This function clears \a signo in \a set.
 * \return 0
 */
int sigdelset(sigset_t * set, const int signo){
	*set &= ~(1<<signo);
	return 0;
}

/*! \details This function checks to see if \a signo is a member of \a set.
 * \return non-zero if \a signo is a member of \a set
 */
int sigismember(const sigset_t * set, int signo){
	sigset_t test_set;
	test_set = (1<<signo);
	return ( (*set & test_set) == test_set );
}

/*! \details This function sets all signal in \a set.
 * \return 0
 */
int sigfillset(sigset_t * set){
	*set = -1;
	return 0;
}

/*! \details This function clears all signal in \a set.
 * \return 0
 */
int sigemptyset(sigset_t * set){
	*set = 0;
	return 0;
}

/*! @} */
