/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


/*! \addtogroup GRP
 * @{
 *
 * \details This interface is not implemented in this version.
 *
 *
 */

/*! \file */


#include <grp.h>

static struct group default_group = {
		.gr_name = "users",
		.gr_passwd = NULL,
		.gr_gid = 0,
		.gr_mem = NULL
};

/*! \details Not implemented (stub only).
 *
 * \return default group
 */
struct group *getgrgid (gid_t gid){
	return &default_group;
}

/*! \details Not implemented (stub only).
 *
 * \return default group
 */
struct group *getgrnam (const char * name){
	return &default_group;
}

/*! \details Not implemented (stub only).
 *
 * \return 0
 */
int getgrnam_r(const char * name, struct group * grp,
			char * buffer, size_t bufsize, struct group **result){
	return 0;
}

/*! \details Not implemented (stub only).
 *
 * \return 0
 */
int getgrgid_r (gid_t gid, struct group * grp,
			char * buffer, size_t bufsize, struct group ** result){
	return 0;
}

/*! \details Not implemented (stub only).
 *
 * \return NULL
 */
struct group *getgrent (void){
	return NULL;
}

/*! \details Not implemented (stub only).
 *
 */
void setgrent (void){

}

/*! \details Not implemented (stub only).
 *
 */
void endgrent (void){

}

/*! @} */

