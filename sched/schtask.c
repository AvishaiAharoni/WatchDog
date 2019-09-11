#include <stdlib.h> /* malloc, free */
#include <assert.h> /* assert */
#include <unistd.h> /* getpid */
#include <sys/time.h> /* timeval */

#include "schtask.h"

struct task
{
    uid_type uid;
    int (*func)(void *);
    void *param;
    size_t due_time;
    time_t next_run;
};

/*****************************************************************************/

/* to create a new task.
	the function gets a function to do at the time,
	time (in seconds) to know when the task needs to run,
	and a parameter that needed to the func.
	returns a pointer to the task (if succeed), or a NULL pointer if failure */
task_t *SCHTaskCreate(int (*func)(void *param), size_t due_time, void *param)
{
	task_t *new_task = NULL;
	
	/* checking parameters */
	assert(NULL != func);
	
	new_task = (task_t *)malloc(sizeof(task_t));
	if (NULL == new_task)
	{
		return (NULL);
	}
	
	/* to initialize the fields of the struct */
	new_task->uid = UIDCreate();
	new_task->func = func;
	new_task->param = param;
	new_task->due_time = due_time;
	new_task->next_run = (time_t)(due_time) + new_task->uid.time.tv_sec;
	
	return (new_task);
}

/*****************************************************************************/

/* to destroy a task, by freeing the memory that was allocated */
void SCHTaskDestroy(task_t *task)
{
	/* checking parameters */
	assert(NULL != task);

	free(task); task = NULL;
}

/*****************************************************************************/

/* to get the uid of the task */
uid_type SCHTaskGetUid(const task_t *task)
{
	/* checking parameters */
	assert(NULL != task);

	return (task->uid);
}

/*****************************************************************************/

/* to get the run time of a function */
time_t SCHTaskGetNextCall(const task_t *task)
{
	/* checking parameters */
	assert(NULL != task);

	return (task->next_run);
}

/*****************************************************************************/

/* to check if a task is before another by comparing their run time.
	(curr < new)  => return 1 */
int SCHTaskIsBefore(const void *curr_task1, const void *new_task2, void *param)
{
	/* checking parameters */
	assert((NULL != curr_task1) && (NULL != new_task2));
	
	return (SCHTaskGetNextCall((task_t *)curr_task1) < 
										SCHTaskGetNextCall((task_t *)new_task2));
}

/*****************************************************************************/

/* to check if a task uid matches a given uid.
	returns 1 if match, else - 0 */
int SCHTaskIsMatch(const void *task, void *uid)
{
	/* checking parameters */
	assert(NULL != task);
	
	return (UIDIsSame(SCHTaskGetUid((task_t *)task), *(uid_type *)uid));
}

/*****************************************************************************/

/* to update the next run time of the function */
void SCHTaskUpdateNextCall(task_t *task)
{
	/* checking parameters */
	assert(NULL != task);

	task->next_run += task->due_time;
}

/*****************************************************************************/

/* to run a function.
	at exit: returns -1 for failure, 0 - success, 1 - success and rerun */
int SCHTaskRun(task_t *task)
{
	/* checking parameters */
	assert(NULL != task);

	return (task->func(task->param));
}
