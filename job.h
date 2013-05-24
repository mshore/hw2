#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct job
{
	char *jobArgs;
	int bg;
	int state;
	int pid;
}job;


char* jobFinished(job j);
char* jobStarted(job j);
