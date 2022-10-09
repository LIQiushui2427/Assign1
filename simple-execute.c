#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
// YIJian 1155157207 CSCI3150 Assignment 1
// I am in a group with Juluan SHI
// Discussed with some friends about the assignment

typedef struct command{
	char **argv;
	int argc;
}command;

void spawn_command(int in, int out, struct command *cmd){
	pid_t child_pid;

	if((child_pid = fork()) < 0){
		perror("fork() error \n");
	}
	
	if (child_pid == 0 ){

		if (in != STDIN_FILENO){
			// should be replaced with dup2; same for the following
			close(STDIN_FILENO);
			dup(in);
			close(in);
		}

		if (out != STDOUT_FILENO){
			close(STDOUT_FILENO);
			dup(out);
			close(out);
		}

		if (execvp(cmd->argv[0], cmd->argv) < 0){
			perror("execvp() error");
			exit(1);
		}

	}
}

int shell_execute(char ** args, int argc)
{
	if(strcmp(args[0], "EXIT") == 0)
		return -1;
	
	int fd[2];

	int input_fd = STDIN_FILENO;
	int output_fd = STDOUT_FILENO;

	int head = 0;// the head of the command
	int tail = 0;// the tail of the command

	while(head < argc - 1)
	{
		while(tail < argc -1 && strcmp(args[tail], "|") != 0)
			tail++;
		command *cmd = (command *)malloc(sizeof(command));
		cmd->argc = tail - head;
		cmd->argv = malloc(sizeof(char *) * (cmd->argc + 1));
		for(int i = 0; i < cmd->argc; i++)
			cmd->argv[i] = args[head + i];
		cmd->argv[cmd->argc] = NULL;

		if(tail == argc - 1) //last command
		{
			output_fd = 1;//STDOUT_FILENO;
			spawn_command(input_fd, output_fd, cmd);
		}
		else if(tail < argc - 1){ //not the last command
			pipe(fd);
			output_fd = fd[1];
			spawn_command(input_fd, output_fd, cmd);

			if(input_fd != 0)//close the input file descriptor
				close(input_fd);
			if(output_fd != 1)//close the output file descriptor
				close(output_fd);
			input_fd = fd[0];// the output of the previous command is the input of the next command
		}
		
		free(cmd);

		if (tail >= argc - 1) break;//last command
		if ((args + tail)[0][0] == '|') head = tail = tail + 1;//skip the '|'
			else break;
	}

		pid_t wpid;
		int status = 0;
		while ((wpid = wait(&status)) > 0); // wait for all the child processes 
		
		return 0;
	}


	