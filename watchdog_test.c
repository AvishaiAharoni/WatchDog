/******************************************************************************/
/* 						      OS - Watch-Dog				                  */ 
/******************************************************************************/
#define _GNU_SOURCE			/* usleep */

#include <stdio.h>			/*printf*/
#include <stdlib.h> 		/* atoi */
#include <unistd.h>
#include <string.h>			
#include "watchdog.h"		/* watchdog header */

int main(int argc, char *argv[]) 
{
	size_t counter = 0;
	char input[4] = {0};
	
	strcpy(input, argv[2]);
	
	printf("run agin %d\n", getpid());
	
	StartWD(argv);
	while (counter < 50)
	{
		printf("%s %lu\n", input, counter);
		fflush(stdout);
		usleep(1000000);
		counter += atoi(argv[1]);
	}
	printf("%lu\n", counter);
	StopWD();
	counter = 0;
	printf("now can stop\n");
	while (counter < 30)
	{
		sleep(1);
		counter += atoi(argv[1]);
	}
	printf("stop %lu\n", counter);
	
	return(0);
}
