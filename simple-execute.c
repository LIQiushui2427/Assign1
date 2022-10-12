#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
// Jian YI 1155157207 CSCI3150 Assignment 1
// I am in a group with 
// Juluan SHI 1155160208
// Yinxi LI 1155160255
// Discussed ideas with some friends about the assignment
// Reference: https://www.geeksforgeeks.org/making-linux-shell-c/

typedef struct command{
	char **argv;//argument vector
	int argc;//number of arguments
}command;

void spawn_command(int in, int out, struct command *cmd){   
	pid_t child_pid;

	if((child_pid = fork()) < 0){//here is the error
		perror("fork() error \n");
	}
	
	if (child_pid == 0 ){
		if (in != STDIN_FILENO){//here we need to check if the input is the same as the standard input
			close(STDIN_FILENO);
			dup(in);//here we need to duplicate the input
			close(in);//here we need to close the input
		}
		if (out != STDOUT_FILENO){//here we need to check if the output is the same as the standard output
			close(STDOUT_FILENO);
			dup(out);
			close(out);
		}
		if (execvp(cmd->argv[0], cmd->argv) < 0){//here we need to check if the command is valid
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
		while(tail < argc -1 && strcmp(args[tail], "|") != 0)//
			tail++;
		command *cmd = (command *)malloc(sizeof(command));
		cmd->argc = tail - head;
		cmd->argv = malloc(sizeof(char *) * (cmd->argc + 1));//here we need to allocate the memory for the command
		for(int i = 0; i < cmd->argc; i++)//here we need to copy the command
			cmd->argv[i] = args[head + i];//here we need to copy the command to the argv
		cmd->argv[cmd->argc] = NULL;

		if(tail == argc - 1) //last command
		{
			output_fd = 1;//STDOUT_FILENO;
			spawn_command(input_fd, output_fd, cmd);
		}
		else if(tail < argc - 1){ //not the last command
			pipe(fd);
			output_fd = fd[1];//将输出重定向到管道的写端
			spawn_command(input_fd, output_fd, cmd);//

			if(input_fd != 0)//close the input file descriptor
				close(input_fd);
			if(output_fd != 1)//
				close(output_fd);
			input_fd = fd[0];// 将输入重定向到管道的读端
		}
		
		free(cmd);

		if (tail >= argc - 1) break;//last command

		if ((strcmp(args[tail], "|") == 0))head = ++tail;//skip the '|'
		else break;
	}

		while(wait(NULL) > 0);//wait for all the child processes to finish
		
		return 0;
	}


	