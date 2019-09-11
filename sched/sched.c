#include <stdlib.h> /* malloc, free, size_t */
#include <assert.h> /* assert */
#include <time.h>

#include "pqueue.h"
#include "schtask.h"
#include "sched.h"

struct sched
{
	pqueue_t *pq;
    int to_exit;
};

/**************************************************************************/

/* to create a new scheduler.
	the function also creates a pqueue to store all the tasks.
	returns a pointer to the new scheduler 
	(and if the malloc failled - returns NULL) */
sched_t *SCHCreate(void)
{
	sched_t *new_sched = (sched_t *)malloc(sizeof(sched_t));
	if (NULL == new_sched)
	{
		return (NULL);
	}
	
	/* to create a task */
	new_sched->pq = PQCreate(&SCHTaskIsBefore, NULL);
	if (NULL == new_sched->pq)
	{
		free(new_sched); new_sched = NULL;
		
		return (NULL);
	}
	
	new_sched->to_exit = 0;
	
	return (new_sched);
}

/**************************************************************************/

/* to free the memory of the scheduler */
void SCHDestroy(sched_t *sched)
{
	/* checking parameters */
	assert(NULL != sched);
	
	SCHClearAll(sched);
	PQDestroy(sched->pq);
	free(sched); sched = NULL;
}

/**************************************************************************/

/* to check if the scheduler is empty.
	returns 1 - if empty, else - 0 */
int SCHIsEmpty(const sched_t *sched)
{
	/* checking parameters */
	assert(NULL != sched);

	return (PQIsEmpty(sched->pq));
}

/**************************************************************************/

/* to check how many tasks are in the scheduler */
size_t SCHSize(const sched_t *sched)
{
	/* checking parameters */
	assert(NULL != sched);
	
	return (PQSize(sched->pq));
}

/**************************************************************************/

/* to remove a specific task from the scheduler.
  returns 0 for success, and 1 if there is no task to remove */
int SCHRemove(sched_t *sched, uid_type uid)
{
	task_t *erased = NULL;
	
	/* checking parameters */
	assert(NULL != sched);

	erased = PQErase(sched->pq, &SCHTaskIsMatch, &uid);
	
	if (NULL == erased)
	{
		return (1);
	}
	
	SCHTaskDestroy(erased);
	
	return (0);	
}

/**************************************************************************/

/* to clear all the tasks of the scheduler */
void SCHClearAll(sched_t *sched)
{
	/* checking parameters */
	assert(NULL != sched);

	while (0 == PQIsEmpty(sched->pq))
	{
		SCHTaskDestroy(PQPeek(sched->pq));
		PQDequeue(sched->pq);
	}
}

/**************************************************************************/

/* to stop the scheduler from running */
void SCHStop(sched_t *sched)
{
	/* checking parameters */
	assert(NULL != sched);

	sched->to_exit = 1;
}

/**************************************************************************/

/* to create a new task and add it to the scheduler.
	returns the new uid of the task */
uid_type SCHAdd(sched_t *sched, int (*func)(void *arg), void *arg, size_t due_time)
{
	task_t *new_task = NULL;
	uid_type error_uid = {0};

	/* checking parameters */
	assert((NULL != sched) && (NULL != func));

	new_task = SCHTaskCreate(func, due_time, arg);
	
	/* if SCHTaskCreate failed */
	if (NULL == new_task)
	{
		error_uid = UIDCreate();
		error_uid.counter = 0;
		
		return (error_uid);
	}
	
	/* to insert the task to the scheduler, 
	   and check if PQEnqueue failed */
	if (1 == PQEnqueue(sched->pq, new_task))
	{
		SCHTaskDestroy(new_task); new_task = NULL;
		
		error_uid = UIDCreate();
		error_uid.counter = 0;
		
		return (error_uid);	
	}
	
	return (SCHTaskGetUid(new_task));
}

/**************************************************************************/

/* to run a task at run time from the scheduler.
	if all the tasks done - returns 0.
	if the scheduler stopped - returns 1.
	for any problem - return 2 */
int SCHRun(sched_t *sched)
{
	int res_run = 0;
	int flag = 0;
	
	/* checking parameters */
	assert(NULL != sched);
	
	while ((1 != sched->to_exit) && (0 == SCHIsEmpty(sched)))
	{
		time_t current = 0;
		void *data = PQPeek(sched->pq);
		
		/* how much time to sleep */
		double dif = difftime(SCHTaskGetNextCall(data),time(&current));
		sleep((0 < dif) ? dif : 0);
		
		res_run = SCHTaskRun(data);
		
		/* if the task needs to run again */
		if (1 == res_run)
		{
			SCHTaskUpdateNextCall(data);
			PQDequeue(sched->pq);
			
			/* if the enqueue failed */
			if (1 == PQEnqueue(sched->pq, data))
			{
				SCHTaskDestroy(data); data = NULL;
				flag = 2;
			}
			
			continue;
		}
		
		/* if the task finished with error */
		if (-1 == res_run)
		{
			flag = 2;
		}
		
		SCHTaskDestroy(data);
		PQDequeue(sched->pq);
	}

	/* if the user run the stop function */
	if ((1 == sched->to_exit) && (2 != flag))
	{
		SCHClearAll(sched);
		
		flag = 1;
	}
	
	sched->to_exit = 0;
	
	return (flag);
}
