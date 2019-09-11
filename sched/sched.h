#ifndef SCHED_H_
#define SCHED_H_


typedef struct sched sched_t;

/********************************Functions*************************************/

/* to create a new scheduler.
	the function also creates a pqueue to store all the tasks.
	returns a pointer to the new scheduler 
	(and if the malloc failled - returns NULL) */
sched_t *SCHCreate(void);

/* to free the memory of the scheduler */
void SCHDestroy (sched_t *sched);

/* to remove a specific task from the scheduler.
  returns 0 for success, and 1 if there is no task to remove */
int SCHRemove(sched_t *sched, uid_type uid);

/* to create a new task and add it to the scheduler.
	returns the new uid of the task */
uid_type SCHAdd(sched_t *sched, int (*func) (void *arg), void *arg, size_t due_time);

/* to clear all the tasks of the scheduler */
void SCHClearAll(sched_t *sched);

/* to run a task at run time from the scheduler.
	if all the tasks done - returns 0.
	if the scheduler stopped - returns 1.
	for any problem - return 2 */
int SCHRun(sched_t *sched);

/* to stop the scheduler from running */
void SCHStop(sched_t *sched);

/* to check how many tasks are in the scheduler */
size_t SCHSize(const sched_t *sched);

/* to check if the scheduler is empty.
	returns 1 - if empty, else - 0 */
int SCHIsEmpty(const sched_t *sched);


#endif /* SCHED_H_ */
