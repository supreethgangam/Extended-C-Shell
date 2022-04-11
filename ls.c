#include<stdio.h>
#include<stdlib.h>
#include<pwd.h>
#include<unistd.h>
#include<string.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include"ls.h"
#include<time.h>

int flag_l=0,flag_a=0;

void list(char* directory,int flag)
{
	DIR* dir;
	dir = opendir(directory);
	if(dir == NULL)
	{
		perror("Error: ");
		return;
	}
	struct dirent *entry;
	struct stat mystat;	
	while(entry = readdir(dir))
	{
		if(flag || (strcmp(entry->d_name,".") && strcmp(entry->d_name,"..")))
		{
			stat(entry->d_name, &mystat);    
        	//printf("%ld",mystat.st_size);
     		printf("%s ", entry->d_name);
        }
	}
	closedir(dir);
}

void condition(int flg,char c)
{
	if(flg)
	{
		printf("%c",c);
		return;
	}
	printf("-");
}

void list1(char* directory)
{
	register struct passwd *pw;
	DIR* dir;
	dir = opendir(directory);
	if(dir == NULL)
	{
		perror("Error: ");
		return;
	}
	struct dirent *entry;
	struct stat mystat;
	int blocks=0;	
	while(entry = readdir(dir))
	{
		stat(entry->d_name, &mystat);
		if(!S_ISDIR(mystat.st_mode))
		{
			blocks += mystat.st_blocks;
		}
		else
		{
			blocks += (mystat.st_blocks/2);
		}
	}
	closedir(dir);
	printf("total %d\n",blocks/2);
	dir = opendir(directory);
	//char mode[][7] = {"","S_IWUSR","S_IXUSR","S_IRGRP","S_IWGRP","S_IXGRP","S_IROTH","S_IWOTH","S_IXOTH"} 
	while(entry = readdir(dir))
	{
		if(((!flag_a) && flag_l && strcmp(entry->d_name,".") && strcmp(entry->d_name,"..")) || flag_l && flag_a)
		{
			stat(entry->d_name, &mystat);    
        	condition(S_ISDIR(mystat.st_mode),'d');
        	condition(mystat.st_mode & S_IRUSR,'r');
        	condition(mystat.st_mode & S_IWUSR,'w');
        	condition(mystat.st_mode & S_IXUSR,'x');
        	condition(mystat.st_mode & S_IRGRP,'r');
        	condition(mystat.st_mode & S_IWGRP,'w');
        	condition(mystat.st_mode & S_IXGRP,'x');
        	condition(mystat.st_mode & S_IROTH,'r');
        	condition(mystat.st_mode & S_IWOTH,'w');
        	condition(mystat.st_mode & S_IXOTH,'x');
        	printf(" %ld",mystat.st_nlink);
  			pw = getpwuid(mystat.st_uid);
  			if(pw)
  			{
  				printf(" %6s",pw->pw_name);
  			}
  			pw = getpwuid(mystat.st_gid);
  			if(pw)
  			{
  				printf(" %6s",pw->pw_name);
  			}
  			printf(" %6ld",mystat.st_size);
     		char temp[100];
     		strftime(temp, 100, "%b %d %H:%M", localtime(&(mystat.st_mtime)));
     		printf(" %s",temp);	
     		printf(" %s\n", entry->d_name);
		}
		//printf("\n");
	}
	closedir(dir);
}


void ls_function(int argc,char* argv[])
{
	flag_l=0;
	flag_a=0;

	char cwd[100];
    getcwd(cwd,100);

	for(int i=1;i<argc;i++)
	{
		if(!strcmp(argv[i],"-l"))
		{
			flag_l = 1;
		}
		if(!strcmp(argv[i],"-a"))
		{
			flag_a = 1;
		}
		if(!strcmp(argv[i],"-la") || !strcmp(argv[i],"-al"))
		{
			flag_l = 1;
			flag_a = 1;
		}
	}

	if(!flag_a && !flag_l)
	{
		int fg=0;
		for(int i=1;i<argc;i++)
		{
			fg=1;
			if(argc>2)
			{
				printf("%s:\n",argv[i]);
			}
			char temp[50];
			strcpy(temp,argv[i]);
			temp[strlen(temp)]='\0';
			list(temp,0);
			printf("\n");
		}
		if(!fg)
		{
			list(cwd,0);
			printf("\n");
		}
		return;
	}

	if(flag_a && !flag_l)
	{
		int fg=0;
		for(int i=1;i<argc;i++)
		{
    		if(argv[i][0]!='-')
    		{
    			fg=1;
    			if(argc>3)
				{
					printf("%s:\n",argv[i]);
				}
				char temp[50];
				strcpy(temp,argv[i]);
				temp[strlen(temp)]='\0';
				list(temp,1);
				printf("\n");
    		}

		}
		if(!fg)
		{
			list(cwd,1);
			printf("\n");
		}
		return;
	}


	if(!flag_a && flag_l)
	{
		int fg=0;
		for(int i=1;i<argc;i++)
		{
			if(argv[i][0]!='-')
			{
				fg = 1;
				if(argc>3)
				{
					printf("%s:\n",argv[i]);
				}
				char temp[50];
				strcpy(temp,argv[i]);
				temp[strlen(temp)]='\0';
				list1(temp);
				printf("\n");
			}
		}
		if(!fg)
		{
			list1(cwd);
			printf("\n");
		}
		return;
	}

	if(flag_a && flag_l)
	{
		int fg=0;
		for(int i=1;i<argc;i++)
		{
			if(argv[i][0]!='-')
			{
				fg = 1;
				if(argc>4 || (argc<3 && fg))
				{
					printf("%s:\n",argv[i]);
				}
				char temp[50];
				strcpy(temp,argv[i]);
				temp[strlen(temp)]='\0';
				list1(temp);
				printf("\n");
			}
		}
		if(!fg)
		{
			list1(cwd);
			printf("\n");
		}
		return;
	}
}