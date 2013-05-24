#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "tokenizer.h"
#include "job.h"

#define MAXJOBS 20

/*
* File: Shell.c 
* Authors: Marc Leef, Matt Shore
* Date: 5/27/13
* Implementation of the fully featured shell for project 2. 
*/


job backgroundJobs[MAXJOBS];
job allJobs[MAXJOBS];
job jobHistory[MAXJOBS];

job mostRecentlyStopped;


int bgpCount=0;
int jobCount = 0;
int pCount = 0;
int finishedMessage = 0;
int stoppedMessage = 0;
char* finishedJob;
char* stoppedJob;
pid_t bgPID = 0;
pid_t fgPID = 0;
pid_t mainPID = 0;


void removeAmpersand(char* a) {
int i = 0;
while (a[i] != '\0')
	{
		if(a[i] == '&') {
			a[i] = '\0';
			break;
		}
		i++;
	}

}

int isBGP(char* a[], int args) {
	if(*a[args-1] == '&') {
		a[args-1] = 0;
		return 1;
	}
	return 0;

}
void parseArgs(char* a[], int args) {
		int k;
		int fence1 = 0;
		int fence2 = 0;
		int in = 0;
		int out = 0;
		char *inp;
		char *outp; 
		char *pleft[50] = {};
		char *pright[50] = {}; 
		int leftSize = 0;
		int rightSize = 0;
		int isInp = 0;
		int isOutp = 0;    
		int isPipe = 0;  
		for(k = 0; k < args + 1; k++) {
			if(a[k] != NULL) {

				if(*a[k] == '>') {
					//printf("1 \n"); 
					out = 1;
					if(a[k+1] != NULL) {
						if(!isOutp) {
							outp = a[k+1];
							fence1 = k;
							isOutp = 1;
						}
						else {
							perror("Incorrect Use of Redirection");
							exit(0);
						}
					}
               	
				}
				
				if(*a[k] == '<') {
					//printf("2 \n"); 
					in = 1;
					if(a[k+1] != NULL) {
						if(!isInp) {
							inp = a[k+1];
							fence2 = k;
							isInp = 1;
						}
						
						else {
							perror("Incorrect Use of Redirection");
							exit(0);
						}
					}
				}
				
				if(*a[k] == '|') {
					//printf("3 \n"); 
					int i;
					for(i = 0; i < k; i++) {
						pleft[i] = a[i];
						leftSize++;
					}
					int w = 0;
					for(i = k+1; i < args; i++) {
						pright[w] = a[i];
						rightSize++;
						w++;
					}
					isPipe = 1;
				}
				
			}
			
			else {	
				break;
			}
		}
		
		if(!isInp && isOutp && !isPipe) {
			for(fence1 = fence1; fence1 < args; fence1++) {
				a[fence1] = 0;
			}
		}
		
		else if(isInp && !isOutp && !isPipe) {
			for(fence2 = fence2; fence2 < args; fence2++) {
				a[fence2] = 0;
			}
		}
		
		else if(isInp && isOutp && !isPipe) {
			if(fence2 < fence1) {
				for(fence2 = fence2; fence2 < args; fence2++) {
					a[fence2] = 0;
				}
			}
			
			else {
				for(fence1 = fence1; fence1 < args; fence1++) {
					a[fence1] = 0;
				}
			
			}
			
		}

		
			if (in)
			{
				//printf("5 \n"); 
				int fd0 = open(inp, O_RDONLY);
				if(fd0 < 0) {
					perror("");
				}
				dup2(fd0, STDIN_FILENO);
				close(fd0);
				in = 0;
			}

			
					
			if (out)
			{
				//printf("6 \n"); 
				int fd1 = creat(outp, 0644);
				if(fd1 < 0) {
					perror("");
				}
				dup2(fd1, STDOUT_FILENO);
				close(fd1);
				out = 0;
			}

		
			if(isPipe) {
				//printf("7 \n"); 
				int pipefd[2];
				pipe(pipefd);
				int pid2 = fork();
				
				if (pid2 == 0){
					dup2(pipefd[1], 1);
					close(pipefd[0]);
					parseArgs(pleft, leftSize);
					setpgid (pid2, 0);
					execvp(pleft[0], pleft);	
				}
			
				else {
					dup2(pipefd[0], 0);
					close(pipefd[1]);
					parseArgs(pright, rightSize);
					setpgid (pid2, 0);
					execvp(pright[0], pright);
				}

			}
}


void execute(char* args[], int argCount, char* input) {
	pid_t pid=fork();
	int bgp = isBGP(args, argCount);
	if(!bgp)
	{
		if(pid<0)
		{
			perror("Fork Error");
			exit(0);
		}
		else if (pid==0)
		{
			parseArgs(args, argCount);
			tcsetpgrp (0, pid);
			if(execvp(args[0],args)<0)
			{
				perror("Improper Command");
				exit(0);
			}
		}
		else
		{
			allJobs[jobCount].jobArgs = input;
			allJobs[jobCount].pid=pid;
			allJobs[jobCount].bg=0;
			
			jobHistory[jobCount].jobArgs = input;
			jobHistory[jobCount].pid=pid;
			jobHistory[jobCount].bg=0;
			
			jobCount++;
			fgPID = pid;
			wait(NULL);
		}
	}
	else if (bgp)
	{
		if(pid<0)
		{
			perror("Fork Error");
			exit(0);
		}
		else if (pid==0)
		{
			parseArgs(args, argCount);
			if(execvp(args[0],args)<0)
			{
				perror("Improper Command");
				exit(0);
			}
		}
		else
		{
		
			
			jobHistory[jobCount].jobArgs = input;
			jobHistory[jobCount].pid=pid;
			jobHistory[jobCount].bg=0;
			jobCount++;
			
			
			backgroundJobs[bgpCount].jobArgs = input;
			char* temp = jobStarted(backgroundJobs[bgpCount]);
			removeAmpersand(temp);
			printf("Running: %s\n", temp);
			bgPID = pid;
			backgroundJobs[bgpCount].pid=pid;
			backgroundJobs[bgpCount++].bg=1;
		}
	}
}

void fg() {
	char* temp = jobStarted(mostRecentlyStopped);
	printf("Running: %s\n", temp);
	kill(mostRecentlyStopped.pid, SIGCONT);
}

void bg() {
	char* temp = jobStarted(mostRecentlyStopped);
	printf("Running: %s\n", temp);
	kill(mostRecentlyStopped.pid, SIGCONT);

}

char *stringCopy(char* dest, char* source)
{
	dest = malloc(1024);
	int i = 0;
	while (source[i] != '\0')
	{
		dest[i] = source[i];
		i++;
	}
	dest[i] = '\0';
	return(dest);
}

void SIGTSTP_handler(int signum) {
	int i;
	if(bgPID) {
		for(i=0;i<bgpCount;i++)
		{
			if(backgroundJobs[i].pid==bgPID)
			{	
					mostRecentlyStopped.pid = bgPID;
					char* t = backgroundJobs[i].jobArgs;
					mostRecentlyStopped.jobArgs = stringCopy(mostRecentlyStopped.jobArgs, t);
					
					stoppedMessage = 1;
					finishedMessage = 0;
					char* temp = jobFinished(backgroundJobs[i]);
					stoppedJob = stringCopy(stoppedJob, temp);
					removeAmpersand(stoppedJob);
					kill(bgPID, SIGKILL);
					printf("\nStopped: %s \n", stoppedJob);	
					int j = write(1, "shredder# ", 10);
					if(j < 0) {
						perror("");
					}
					bgPID = 0;
			}
		}
	}
	else if(fgPID){
		for(i=0;i<jobCount;i++)
		{
			if(allJobs[i].pid==fgPID)
			{	
					mostRecentlyStopped.pid = fgPID;
					char* t = allJobs[i].jobArgs;
					mostRecentlyStopped.jobArgs = stringCopy(mostRecentlyStopped.jobArgs, t);
					char* temp = jobFinished(allJobs[i]);
					stoppedJob = stringCopy(stoppedJob, temp);
					removeAmpersand(stoppedJob);
					kill(fgPID, SIGKILL);
					printf("\nStopped: %s \n", stoppedJob);	
					fgPID = 0;
			}
		}
	}
	
	else {
		printf("\nNo processes to stop. \n");	
		int j = write(1, "shredder# ", 10);
		if(j < 0) {
			perror("");
		}
	}
	signal(SIGTSTP, SIGTSTP_handler);
	return;

}

void SIGCHLD_handler(int signum) {
	int pid;
	int i;
	pid = waitpid(WAIT_ANY, NULL, WNOHANG);
	for(i=0;i<bgpCount;i++)
	{
		if(backgroundJobs[i].pid==pid)
		{
			if(!stoppedMessage) {
				finishedMessage = 1;
				char* temp = jobFinished(backgroundJobs[i]);
				finishedJob = stringCopy(finishedJob, temp);
				removeAmpersand(finishedJob);
			}
			stoppedMessage = 0;
		}
	}
	signal(SIGCHLD, SIGCHLD_handler);
	return;
}


/*
* Execution of shell, loops continually, takes input, and starts/times new processes
*/
int main(int argc, char *argv[], char *envp[]) {
	TOKENIZER *tokenizer;
	int argCount = 0;
	char *input;
	char *tok; 
	char *args[100] = {};
	char *bgCalled = "bg";
	char *fgCalled = "fg";
	setvbuf (stdout, NULL, _IONBF, 0);
	signal(SIGCHLD,SIGCHLD_handler);
	signal(SIGTSTP, SIGTSTP_handler);
	
	mainPID = getpid();
	tcsetpgrp (0, mainPID);
	
	
while(1) {
		if(finishedMessage) {
			printf("Finished: %s \n", finishedJob);
			finishedMessage = 0;
		}
		
		int j = write(1, "shredder# ", 10);
		if(j < 0) {
			perror("");
		}
		
		input = malloc(1024);
		int i = read(0, input, 1024);
		if(i < 0) {
			perror("");
		}
		if(!input) {
			continue;
		}
		input[i-1] = '\0';
		tokenizer = init_tokenizer(input);
		
		while( (tok = get_next_token( tokenizer )) != NULL ) {
			char* temp;
			temp = stringCopy(temp, tok);
			args[argCount] = temp;
			free( tok );    /* free the token now that we're done with it */
			argCount = argCount + 1;
		}
		
		args[argCount] = (char *)0;		
		
		free_tokenizer( tokenizer ); /* free memory */
		
		if(*args[0] == *bgCalled) {
			bg();
		}
		
		else if(*args[0] == *fgCalled) {
			fg();
		}
		else {
			execute(args,argCount,input);
		}
		
		int m;
		for(m = 0; m < argCount; m++)
		{
			free(args[m]);
			args[m] = 0;
		}
		argCount = 0;
		free(input);
		
		}
	
		bgpCount = 0;	
		jobCount = 0;
			
		}
		
		
		
	
	