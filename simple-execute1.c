#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int shell_execute(char ** args, int argc)
{	
	int child_pid1, child_pid2, child_pid3;
	int counter = 0;
	int fd[2];
	int position[10] = {0};

	if ( strcmp(args[0], "EXIT") == 0)
		return -1; 

	for (int i = 0; i < argc-1; i++)
	{
		if (strcmp(args[i], "|") == 0){
			position[counter] = i;
			args[i] = NULL;
			counter++;
		}

	}
		
	if (pipe(fd) == -1){
		printf("pipe() error \n");
		return 1;
	}
	if (counter == 0){
		if( (child_pid1 = fork()) < 0 ){
			printf("fork() error \n");
		}else if (child_pid1 == 0 ){
			if ( execvp(args[0], args) < 0){ 
				printf("execvp() error \n");
				exit(-1);
			}
		}
		close(fd[0]); close(fd[1]);
		waitpid(child_pid1, NULL, 0);
	}
	else if (counter == 1){
		if( (child_pid1 = fork()) < 0 ){
			printf("fork() error \n");
		}else if (child_pid1 == 0 ){
			close(1);
			dup(fd[1]); close(fd[0]); close(fd[1]);
			if ( execvp(args[0], args) < 0){ 
				printf("execvp() error \n");
				exit(-1);
			}
		}
		if ( (child_pid2 = fork()) < 0 ){
			printf("fork() error \n");
		}else if (child_pid2 == 0){
			close(0);
			dup(fd[0]); close(fd[0]); close(fd[1]);
			if ( execvp(args[position[0]+1], args+(position[0]+1)) < 0){ 
				printf("execvp() error \n");
				exit(-1);
			}
		}
		close(fd[0]); close(fd[1]);
		waitpid(child_pid1, NULL, 0);
		waitpid(child_pid2, NULL, 0);
	}
	else if (counter == 2){
		int fd2[2];
		if (pipe(fd2) == -1){
			printf("pipe() error \n");
			return 1;
		}
		if( (child_pid1 = fork()) < 0 ){
			printf("fork() error \n");
		}else if (child_pid1 == 0 ){
			close(1);
			dup(fd[1]); close(fd[0]); close(fd[1]); close(fd2[0]); close(fd2[1]);
			if ( execvp(args[0], args) < 0){ 
				printf("execvp() error \n");
				exit(-1);
			}
		}
		
		if ( (child_pid2 = fork()) < 0 ){
			printf("fork() error \n");
		}else if (child_pid2 == 0){
			close(0); dup(fd[0]); 
			close(1); dup(fd2[1]);
			close(fd[0]); close(fd[1]);
			close(fd2[0]); close(fd2[1]);
			if ( execvp(args[position[0]+1], args+(position[0]+1)) < 0){ 
				printf("execvp() error \n");
				exit(-1);
			}
		}
		
		if ( (child_pid3 = fork()) < 0 ){
			printf("fork() error \n");
		}else if (child_pid3 == 0){
			close(0); dup(fd2[0]); 
			close(fd[0]); close(fd[1]);
			close(fd2[0]); close(fd2[1]);
			if ( execvp(args[position[1]+1], args+(position[1]+1)) < 0){ 
				printf("execvp() error \n");
				exit(-1);
			}
		}
		close(fd[0]); close(fd[1]);
		close(fd2[0]); close(fd2[1]);
		waitpid(child_pid1, NULL, 0);
		waitpid(child_pid2, NULL, 0);
		waitpid(child_pid3, NULL, 0);
	}
	return 0;
}
