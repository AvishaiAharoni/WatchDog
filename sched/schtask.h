#ifndef SCHTASK_H

#define SCHTASK_H

#include "uid.h"

typedef struct task task_t;

/* to create a new task.
	the function gets a function to do at the time,
	time (in seconds) to know when the task needs to run,
	and a parameter that needed to the func.
	returns a pointer to the task (if succeed), or a NULL pointer if failure */
task_t *SCHTaskCreate(int (*func)(void *param), size_t due_time, void *param);

/* to destroy a task, by freeing the memory that was allocated */
void SCHTaskDestroy(task_t *task);

/* to get the uid of the task */
uid_type SCHTaskGetUid(const task_t *task);

/* to get the run time of a function */
time_t SCHTaskGetNextCall(const task_t *task);

/* to check if a task is before another by comparing their run time.
	(curr < new)  => return 1 */
int SCHTaskIsBefore(const void *curr_task1, const void *new_task2, void *param);

/* to check if a task uid matches a given uid.
	returns 1 if match, else - 0 */
int SCHTaskIsMatch(const void *uid, void *task);

/* to update the next run time of the function */
void SCHTaskUpdateNextCall(task_t *task);

/* to run a function.
	at exit: returns -1 for failure, 0 - success, 1 - success and rerun */
int SCHTaskRun(task_t *task);


#endif /* SCHTASK_H */
