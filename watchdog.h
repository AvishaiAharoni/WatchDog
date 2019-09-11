/******************************************************************************/
/* 						      OS - Watch-Dog				                  */ 
/******************************************************************************/

#ifndef WD_H_							
#define WD_H_					

typedef struct wd wd_t;

typedef enum
{
	SUCCESS,
	ERROR_ALLOC,
	ERROR_SCHED,
	ERROR_TASK,
	ERROR_THREAD,
	ERROR_WD_DONT_EXIST,
	ERROR_WD_INIT	
}e_error_t;

/****************************************************************************/
/*	Function Name - StartWD		              		    				    */
/*	Parameter:																*/
/*		argv.        						         		 		        */
/*	Return Value:															*/
/*		enum e_error_t.  									                */ 
/*	Description:															*/
/*		the function runing a watchdog to protect on a process,				*/
/*		if the process is down, the watchdog re-start it.					*/
/****************************************************************************/
int StartWD(char **argv);

/****************************************************************************/
/*	Function Name - StopWD		              		    				    */
/*	Parameter:																*/
/*		nothing.       						         		 		        */
/*	Return Value:															*/
/*		nothing.		  									                */ 
/*	Description:															*/
/*		the function stop the watchdog and destroy is resources.			*/
/****************************************************************************/
void StopWD(void);


#endif	/* WD_H */	
