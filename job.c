#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "job.h"


char* jobFinished(job j) {
	
	j.state = 0;
	if(j.bg) {
		j.bg = 0;
	}
	return j.jobArgs;
}

char* jobStarted(job j) {
	j.state = 2;
	j.bg = 1;
	return j.jobArgs;
}



