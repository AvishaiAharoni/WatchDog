#include <sys/types.h> /* pid_t */
#include <unistd.h> /* getpid */
#include <sys/time.h> /* timeval */

#include "uid.h"

/*****************************************************************************/

/* to create a uid struct,
	and init the pid of the uid, the time of create, and the counter of the uid.
	the counter starts from 1.
	returns the uid struct */
uid_type UIDCreate(void)
{
	static size_t count = 1;
	uid_type new_uid = {0};
	
	/* to initialize the fields in the struct */
	new_uid.pid = getpid();
	gettimeofday(&new_uid.time, NULL);
	new_uid.counter = count;
	
	++count;
	
	return (new_uid);
}

/*****************************************************************************/

/* to check if a uid is bad.
	returns 1 if it's bad, 0 - if okay */
int UIDIsBad(uid_type uid)
{
	return (0 == uid.counter);
}

/*****************************************************************************/

/* to check if two uids are the same.
	returns 1 if they are same, else - 0 */
int UIDIsSame(uid_type uid1, uid_type uid2)
{
	return ((uid1.pid == uid2.pid) &&
		 (uid1.time.tv_sec == uid2.time.tv_sec) && 
		  (uid1.counter == uid2.counter));
}
