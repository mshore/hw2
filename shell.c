#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "tokenizer.h"

/*
* File: Shell.c 
* Author: Marc Leef 
* Date: 4/25/13
* Execution of the custom shell for assignment one. User specifies a timeout variable,
* and any process that runs longer than this specified timeout is killed by the snarky
* shell. 
*/

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
   				
                int k;
                int in, out = 0;             
                for(k = 0; k < argCount + 1; k++) {
                	if(args[k] != NULL) {
                
                		if(*args[k] == '>') {
                			in = 1;
                	
                		}
                	
                		if(*args[k] == '<') {
                			out = 1;
                		}	
                	}
                	
                	else {
                		break;
                	}
                }
                
                
                	
                
                
                
   				
   				free_tokenizer( tokenizer ); /* free memory */
                
                pid = fork();
                if(!pid) {
                
                	if (in)	{
        				int fd0 = open(input, O_RDONLY, 0);
        				dup2(fd0, STDIN_FILENO);
        				close(fd0);
    				}

    				if (out)
    				{
        				int fd1 = creat(output, 0644);
        				dup2(fd1, STDOUT_FILENO);
        				close(fd1);
   					}
                
                        execvp(args[0],args);
                }

                else {
                        wait(&process);
                }
                argCount = 0;
                free(input);

        }






}



