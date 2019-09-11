#define _GNU_SOURCE			/* kill */

#include <assert.h>			/* assert */
#include <unistd.h>			/* access */
#include <signal.h>			/* signal handler */
#include <semaphore.h>		/* semaphore */
#include <fcntl.h>          /* For O_* constants - named semaphore */
#include <sys/stat.h>       /* For mode constants - named semaphore */
#include <errno.h>			/* errno */
#include <string.h>    		/* memset */
#include <pthread.h>		/* pthread */
#include <stdlib.h>			/* setenv, getenv */

#include "watchdog.h"		/* watchdog */
#include "sched/uid.h"		/* uid */
#include "sched/sched.h"	/* scheduler */

/******************************************************************************/

#define CHECK_INTERVAL 3
#define SEND_INTERVAL 1
#define TRY_TO_CLOSE_PROCESS 5
#define WATCHDOG_FILE_PATH "./wd.out"

/******************************************************************************/

typedef int boolean;

enum { STOP = 0, RERUN = 1 };

struct wd 
{
	sched_t *sched;
	pid_t partner;
	int is_parent_process;
	char **argv;
};

typedef enum
{
	FROM_WD,
	FROM_APP
}e_start_from_t;

/******************************************************************************/

static volatile sig_atomic_t g_sig_counter = 0;
static volatile sig_atomic_t g_to_finish = 0;
static const char *shared_sem_name = "wd_sem_lock";
static sem_t *sem_lock = NULL;
static wd_t wd = { 0 };

/******************************************************************************/
/* 			                SIG Handler Functions                             */  
/******************************************************************************/

static void SigHandlerUSR1(int sig)
{
	++g_sig_counter;
}

/******************************************************************************/
static void SigHandlerUSR2(int sig)
{
	g_to_finish = 1;
}

/******************************************************************************/

static void SigHandlerINT(int sig)
{
	;
}

/******************************************************************************/

static void SetSignalHandler(void)
{
	struct sigaction handle;
	memset(&handle, 0, sizeof(handle));
	
	handle.sa_handler = &SigHandlerUSR1;
	sigaction(SIGUSR1, &handle, NULL);
	
	handle.sa_handler = &SigHandlerUSR2;
	sigaction(SIGUSR2, &handle, NULL);
	
	handle.sa_handler = &SigHandlerINT;
	sigaction(SIGINT, &handle, NULL);
}	

/******************************************************************************/
/* 			                Help Functions                                    */  
/******************************************************************************/

static boolean IsFirstWDLoad()
{
	/* return error if the semaphore exist */
	sem_lock = sem_open(shared_sem_name, O_CREAT | O_EXCL, 0644, 0);
	
	return (EEXIST != errno);
}

/******************************************************************************/

static boolean IsFileExist(const char *file_path)
{
	return (0 == access(file_path, F_OK));
}

/******************************************************************************/

static e_start_from_t StartFrom(void)
{
	return ((0 == strcmp(getenv("IS_WD"), "1")) ? FROM_WD : FROM_APP);
}

/******************************************************************************/

static void CleanAll(wd_t *wd)
{
	assert(wd);
	
	SCHDestroy(wd->sched); wd->sched = NULL;
	
	sem_unlink(shared_sem_name);
}

/******************************************************************************/

static int TaskSend(void *args)
{
	wd_t *wd = NULL;
	
	assert(args);
	
	wd = (wd_t *)args;
	
	if (0 == g_to_finish)
	{
		kill(wd->partner, SIGUSR1);
	}
	else
	{
		SCHStop(wd->sched);
		
		return (STOP);
	}
	
	return (RERUN);
}

/******************************************************************************/

static void KillTheChildProcess(pid_t pid)
{
	if (0 != pid)
	{
		kill(pid, SIGKILL);
	}
}

/******************************************************************************/

static int TaskCheck(void *args)
{
	pid_t child_pid = 0;
	wd_t *wd = NULL;
	
	assert(args);
	
	wd = (wd_t *)args;
	
	/* there is no partner because it's the first time, or it was terminated */
	if (((0 == g_sig_counter) || (0 == wd->partner)) && (0 == g_to_finish))
	{
		/* to be sure that the child doesn't exist */
		KillTheChildProcess(wd->partner);

		child_pid = fork();
		if (0 == child_pid)
		{
			
			if (FROM_WD == StartFrom())
			{
				setenv("IS_WD", "0", 1);
				execvp(wd->argv[0], wd->argv);
			}
			else
			{
				setenv("IS_WD", "1", 1);
				execvp(WATCHDOG_FILE_PATH, wd->argv);
			}
		}
		else /* from the parent */
		{
			wd->partner = child_pid;
			wd->is_parent_process = 1;
			
			sem_wait(sem_lock);
		}
	}
	
	g_sig_counter = 0;
	
	return (RERUN);
}

/******************************************************************************/

static e_error_t InitWD(wd_t *wd, char **argv)
{
	assert(argv);
	assert(wd);
	
	wd->sched = SCHCreate();
	if (NULL == wd->sched)
	{
		return (ERROR_SCHED);
	}
	
	wd->partner = getppid();
	wd->is_parent_process = 0;
	wd->argv = argv;

	return (SUCCESS);	
}

/******************************************************************************/

static e_error_t SetAttr(pthread_attr_t attr)
{
	e_error_t was_error = SUCCESS;
	
	if (FROM_APP == StartFrom())
	{
		if (0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
		{
			was_error = ERROR_THREAD;
		}
	}
	else
	{
		if (0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE))
		{	
			was_error = ERROR_THREAD;
		}
	}

	return (was_error);
}

/******************************************************************************/

static e_error_t LoadSched(const wd_t *wd)
{
	uid_type result_send = { 0 };
	uid_type result_check = { 0 };
	
	assert(wd);

	result_send = SCHAdd(wd->sched, &TaskSend, (void *)wd, SEND_INTERVAL);
	result_check = SCHAdd(wd->sched, &TaskCheck, (void *)wd, CHECK_INTERVAL);
	
	if ((1 == UIDIsBad(result_send)) || (1 == UIDIsBad(result_check)))
	{
		return (ERROR_TASK);
	}
	
	if (0 == wd->is_parent_process)
	{
		sem_post(sem_lock);
	}

	return (SUCCESS);
}

/******************************************************************************/

void *SignalPingPong(void *args)
{
	wd_t *wd = NULL;
	
	assert(args);
	
	wd = (wd_t *)args;
	
	if (SUCCESS == LoadSched(wd))
	{
		SCHRun(wd->sched);
	}
	
	CleanAll(wd);
	
	return (NULL);
}

/******************************************************************************/

static e_error_t RunThread(wd_t *wd)
{
	pthread_t thread = 0;
	pthread_attr_t attr;
	e_error_t was_error = SUCCESS;
	
	assert(wd);
	
	if (0 != pthread_attr_init(&attr))
	{
		return (ERROR_THREAD);
	}
	
	was_error = SetAttr(attr);
	
	if ((SUCCESS == was_error) &&
		(0 != pthread_create(&thread, &attr, &SignalPingPong, (void *)wd)))
	{
		was_error = ERROR_THREAD;
	}

	if ((SUCCESS == was_error) && (FROM_WD == StartFrom()))
	{
		if (0 != pthread_join(thread, NULL))
		{
			was_error = ERROR_THREAD;
		}
	}
	
	pthread_attr_destroy(&attr);
	
	return (was_error);
}

/******************************************************************************/

int StartWD(char **argv)
{
	assert(argv);
	
	/* check if the WD exist */
	if (0 == IsFileExist(WATCHDOG_FILE_PATH))
	{
		return (ERROR_WD_DONT_EXIST);
	}

	/* init the struct */
	if (ERROR_SCHED == InitWD(&wd, argv))
	{
		return (ERROR_WD_INIT);
	}
	
	/* set signals */
	SetSignalHandler();
	
	if (1 == IsFirstWDLoad())
	{
		/* to add a new environment var and write there that it's not WD */
		setenv("IS_WD", "0", 1);
		wd.partner = 0;
		wd.is_parent_process = 1;
	}
	else
	{
		/* joining to the sen */
		sem_lock = sem_open(shared_sem_name, 0);	
	}
		
	/* set thread */
	if (SUCCESS != RunThread(&wd))
	{
		CleanAll(&wd);
		
		return (ERROR_THREAD);
	}
	
	return (SUCCESS);
}

/******************************************************************************/

void StopWD(void)
{
	size_t times_to_try = 0;
	
	/* changes the flag to start clean process */
	g_to_finish = 1;
	
	/* trying to send SIGUSR to the child to sign him to start clean the process,
	   try num of time to check if t */
	for (times_to_try = 0; times_to_try < TRY_TO_CLOSE_PROCESS; ++times_to_try)
	{
		kill(wd.partner, SIGUSR2);
	}
}
