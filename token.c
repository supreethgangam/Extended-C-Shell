#include"token.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>

char home[100];

int tokenize(char* cmd,char** tokens)
{
	char* token = (char*)malloc(1024);
	int len = 0;
	token = strtok(cmd,";");
	

	while(token != NULL && strcmp(token,""))
	{
		tokens[len++] = token;
		token = strtok(NULL,";");	
	}

	free(token);
	return len;
}