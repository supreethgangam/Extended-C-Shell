#include<stdio.h>
#include<stdlib.h>
#include<pwd.h>
#include<unistd.h>
#include<string.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include"process.h"


int process(char* argv[],int flag,int* fg_pid)
{
	if(flag)
	{
		pid_t pid;
		pid = fork();
		//printf("11");
		if(pid < 0)
		{
			perror("Cannot create the background process");
			exit(1);
		}
		
		if(pid == 0)
		{
			setpgid(0,0);
			if (execvp(argv[0], argv) == -1) 
			{
				perror("Enter a proper command");
			}
			exit(1);
		}
		
		if(pid > 0)
		{
			printf("%d\n",pid);
			return pid;
		}

	}

	if(!flag)
	{
		int status;
		pid_t pid;
		pid = fork();
		//*child = getpid();
		//printf("10");		
		if(pid < 0)
		{
			perror("Cannot create the process");
			exit(1);
		}
		if(pid == 0)
		{
			if (execvp(argv[0], argv) == -1) 
			{
				perror("Enter a proper command");
				exit(1);
			}
		}
		if(pid > 0)
		{
			*fg_pid = pid;
			waitpid(pid, &status, WUNTRACED);
			if (WIFSTOPPED(status)) 
			{
				return -1;
			}	
		}
	}

	return -1;
}
