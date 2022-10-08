#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
// YIJian 1155157207 CSCI3150 Assignment 1
// This is my own work except the code provided by the instructor.

typedef struct command{
	char **argv;
	int argc;
}command;

void spawn_command(int in, int out, command *cmd){
	pid_t child_pid;

	if((child_pid = fork()) < 0){
		perror("fork() error \n");
	}
	
	if (child_pid == 0 ){ // child process

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

	int input_fd = 0;
	int output_fd = 1;

	int head = 0;
	int tail = 0;

	while(head < argc - 1)
	{
		while(tail < argc -1 && (args + tail)[0][0] != '|')
			tail++;
		command *cmd = (command *)malloc(sizeof(command));
		cmd->argc = tail - head;
		cmd->argv = (char **)malloc(sizeof(char *) * (cmd->argc + 1));
		for(int i = 0; i < cmd->argc; i++)
			cmd->argv[i] = args[head + i];
		cmd->argv[cmd->argc] = NULL;

		if(tail < argc - 1) // not the last command
		{
			pipe(fd);
			output_fd = fd[1];
			spawn_command(input_fd, output_fd, cmd);

			if(input_fd != 0)
				close(input_fd);
			if(output_fd != 1)
				close(output_fd);
			input_fd = fd[0];// the output of the previous command is the input of the next command
		}
		else if(tail == argc - 1){ // last command
			output_fd = 1;//STDOUT_FILENO;
			spawn_command(input_fd, output_fd, cmd);
		}

		free(cmd);
		pid_t status = 0;
		while(wait(&status) != -1);// wait for all the child processes to finish
		
		return 0;
	}


	
}