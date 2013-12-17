/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <pwd.h>
#include <errno.h>

/*! \addtogroup PWD Password
 * @{
 *
 * \ingroup POSIX
 *
 * \details This interface is not supported in this version.
 *
 */

/*! \file */


/*! \details Not implemented (stub only).
 * \return NULL
 */
struct passwd *getpwuid (uid_t uid){
	return NULL;
}

/*! \details Not implemented (stub only).
 * \return NULL
 */
struct passwd *getpwnam (const char * name){
	return NULL;
}

/*! \details Not implemented (stub only).
 * \return 0
 */
int getpwnam_r(const char * name, struct passwd * pwd, char * buffer, size_t bufsize, struct passwd **result){
	return 0;
}

/*! \details Not implemented (stub only).
 * \return 0
 */
int getpwuid_r (uid_t uid, struct passwd * pwd, char * buffer, size_t bufsize, struct passwd **result){
	return 0;
}

/*! \details Not implemented (stub only).
 * \return NULL
 */
struct passwd *getpwent (void){
	return NULL;
}

/*! \details Not implemented (stub only).
 */
void setpwent (void){

}

/*! \details Not implemented (stub only).
 */
void endpwent (void){

}

/*! @} */
