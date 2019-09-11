#ifndef UID_H

#define UID_H

#include <sys/types.h> /* pid_t */
#include <sys/time.h> /* timeval */

typedef struct uid
{
    pid_t pid;
    size_t counter;
    struct timeval time;
} uid_type;


/* to create a uid struct,
	and init the pid of the uid, the time of create, and the counter of the uid.
	the counter starts from 1.
	returns the uid struct */
uid_type UIDCreate(void);

/* to check if a uid is bad.
	returns 1 if it's bad, 0 - if okay */
int UIDIsSame(uid_type uid1, uid_type uid2);

/* to check if two uids are the same.
	returns 1 if they are same, else - 0 */
int UIDIsBad(uid_type uid);

#endif /* UID_H */
