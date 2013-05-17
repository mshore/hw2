#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "tokenizer.h"
#include <sys/wait.h>
/*
 * File: Shell.c 
 * Authors: Marc Leef, Matt Shore
 * Date: 5/27/13
 * Implementation of the fully featured shell for project 2. 
 */
 
 
void parseArgs(char* a[], int args) {
		int k;
		int fence1, fence2 = 0;
		int in, out = 0;
		char *inp;
		char *outp; 
		char *pleft[50] = {};
		char *pright[50] = {}; 
		int leftSize, rightSize = 0;
		inp = 0;
		outp = 0;    
		int p = 0;   
		for(k = 0; k < args + 1; k++) {
			if(a[k] != NULL) {
                
				if(*a[k] == '>') {
					out = 1;
					if(a[k+1] != NULL) {
						if(!outp) {
						outp = a[k+1];
						fence1 = k;
						}
						else {
							perror("Incorrect Use of Redirection");
							exit(0);
						}
					}
                	
				}
				
				if(*a[k] == '<') {
					in = 1;
					if(a[k+1] != NULL) {
						if(!inp) {
							inp = a[k+1];
							fence2 = k;
						}
						
						else {
							perror("Incorrect Use of Redirection");
							exit(0);
						}
					}
				}
				
				if(*a[k] == '|') {
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
					p = 1;
				}
				
			}
			
			else {
				break;
			}
		}
		
		if(!inp && outp && !p) {
			for(fence1 = fence1; fence1 < args; fence1++) {
				a[fence1] = 0;
			}
		}
		
		if(inp && !outp && !p) {
			a[fence2] = a[fence2+1];
			a[fence2+1] = 0;
		}
		
		if(inp && outp && !p) {
			if(fence2 < fence1) {
				for(fence1 = fence1; fence1 < args; fence1++) {
					a[fence1] = 0;
				}
				a[fence2] = a[fence2+1];
				a[fence2+1] = 0;
			}
			
			else {
				a[fence1] = a[fence2 + 1];
				a[fence2 + 1] = 0;
				for(fence1 = fence1+1; fence1 < fence2 + 1; fence1++) {
					a[fence1] = 0;
				}
			
			}
			
		}

/*		
			if (in)
			{
				int fd0 = open(inp, O_WRONLY | O_CREAT | O_TRUNC, 0);
				dup2(fd0, STDIN_FILENO);
				close(fd0);
				in = 0;
			}
*/
			
					
			if (out)
			{
				int fd1 = creat(outp, 0644);
				if(fd1 < 0) {
					perror("");
				}
				dup2(fd1, STDOUT_FILENO);
				close(fd1);
				out = 0;
			}

		
			if(p) {
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

char *my_strcpy(char* dest, char* source)
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




/*
 * Execution of shell, loops continually, takes input, and starts/times new processes
 */
int main(int argc, char *argv[], char *envp[]) {
	TOKENIZER *tokenizer;
	int process;
	int argCount = 0;
	int pid;
	char *input;
	char *tok; 
	char *args[100] = {};
	setvbuf (stdout, NULL, _IONBF, 0);
	
	
	
	while(1) {
		int j = write(1, "shredder# ", 10);
		if(j < 0) {
			perror("");
		}
		
		input = malloc(1024);
		int i = read(0, input, 1024);
		if(i < 0) {
			perror("");
		}
		
		input[i-1] = '\0';
		tokenizer = init_tokenizer(input);
		
		while( (tok = get_next_token( tokenizer )) != NULL ) {
			char* temp;
			temp = my_strcpy(temp, tok);
			args[argCount] = temp;
			free( tok );    /* free the token now that we're done with it */
			argCount = argCount + 1;
		}
		
		args[argCount] = (char *)0;		
		
		free_tokenizer( tokenizer ); /* free memory */
		
		pid = fork();
		if(!pid) {
			parseArgs(args, argCount);
			execvp(args[0],args);
		}
		
		else {
			wait(&process);
		}
		int m;
		for(m = 0; m < argCount; m++)
		{
			args[m] = 0;
		}
		argCount = 0;
		free(input);
		
	}
	
	
	
	
	
	
}



