#include<stdio.h>
#include<stdlib.h>
#include<pwd.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/wait.h>
#include"token.h"
#include"ls.h"
#include"process.h"
#include"history.h"
#include<ctype.h>

char home[1024];
char display[1024];
char bgcmd[30];
char prev[200];
char* bglist[100];
int pidlist[100];
int pos = 0;
int pos1 = 0;
int shell;
int fg_pid;
int fg_flag;

struct bgproc{
  int bg_pid;
  char bg_name[100];
  int jobnum;
};

struct bgproc bgs[100];

void cmds_tokenize(char* line);
void change_display(char* address)
{
  register struct passwd *pw;
  register uid_t uid;
  char user[200];
  uid = geteuid();
  pw = getpwuid(uid);
  if(pw)
  {
    strcpy(user,pw->pw_name);
  }
  char host[200];
  host[199]='\0';
  gethostname(host,199);
  strcpy(display,"<");
  strcat(display,user);
  strcat(display,"@");
  strcat(display,host);
  strcat(display,":~");
  strcat(display,address);
  strcat(display,">");
}


void checkbgproc()
{
  int status;
  pid_t w;
  w = waitpid(-1, &status, WNOHANG);
  if(w>0)
  {
    int i=0;
    if(i<pos)
    {
      while(pidlist[i] != w && i<pos) 
      {
        i++;
      }
    }
    if(WIFEXITED(status))
    {
      fprintf(stderr,"%s with pid %d exited normally\n",bglist[i],w);
    }
    else
    {
      fprintf(stderr,"%s with pid %d exited abnormally\n",bglist[i],w);
    }
    pidlist[i] = 0;
    free(bglist[i]); 
  }
}


void pinfo(char* argv[],int len)
{
  pid_t pid;  
  char* file = (char*)malloc(100);
  if(len == 1)
  {
    pid = getpid();
    sprintf(file,"/proc/%d/stat",pid);
  }
  else 
  {
    sprintf(file,"/proc/%s/stat",argv[1]); 
  }
  FILE* fp = fopen(file,"r");
  if(!fp)
  {
    perror("Stat File cant be acccessed");
    return;
  }
  char* path = (char*)malloc(1000);
  char st;
  char* word = (char*)malloc(1000);
  int vm;

  char status;
  int pgrp;
  int tpgid;
  int mem;
  fscanf(fp,"%d %*s %c %*s %d %*s %*s %d ",&pid, &status, &pgrp, &tpgid);
  fclose(fp);
  if(len == 1)
  {
    pid = getpid();
    sprintf(file,"/proc/%d/status",pid);
  }
  else 
  {
    sprintf(file,"/proc/%s/status",argv[1]); 
  }
  fp = fopen(file,"r");
  if(!fp)
  {
    perror("Status File cant be acccessed");
    return;
  }
  while (fscanf(fp, " %s", word) == 1) 
  {
        if (strcmp(word, "VmSize:") == 0) 
        {
            fscanf(fp, "%d", &mem);
        }
  }
  if(len == 1)
  {
    pid = getpid();
    sprintf(file,"/proc/%d/exe",pid);
  }
  else 
  {
    sprintf(file,"/proc/%s/exe",argv[1]); 
  }

  char* path1 = (char*)malloc(1000);
  int i = readlink(file,path1,200);
  if(i!=-1)
  {
    path1[i]='\0';
  }
  else
  {
    strcat(path1,path);
  }

  free(file);
  free(word);
  fclose(fp);

  printf("pid -- %d\n",pid);
  printf("Process Status -- %c",status);    
  if(pgrp == tpgid)
  {
    printf("+");
  }
  printf("\n");
  printf("memory -- %d KB\n",mem);
  printf("Executable Path -- %s\n",path1);
    
  free(path);  
  free(path1);
}

int struct_cmp(const void *a, const void *b) 
{ 
    struct bgproc *ia = (struct bgproc *)a;
    struct bgproc *ib = (struct bgproc *)b;
    return strcmp(ia->bg_name, ib->bg_name);
} 

void sort_bgs()
{
  pos1 = 0;
  int jobnum = 0;
  for(int i=0;i<100;i++)
  {
    if(pidlist[i])
    {
      strcpy(bgs[pos1].bg_name,bglist[i]);
      bgs[pos1].bg_pid = pidlist[i];
      bgs[pos1].jobnum = jobnum;
      pos1++;
      jobnum++;
    } 
  }
  qsort(bgs, pos1, sizeof(struct bgproc), struct_cmp); 
  int var = 0;
  for(int i=0;i<pos1;i++)
  {
    bgs[i].jobnum = var;
    var++;
    //printf("%s--%d\n",bgs[i].bg_name,bgs[i].bg_pid);
  } 
}

void jobs(char* args[100],int len)
{
  char* file = (char*)malloc(1000);
  char status;
  sort_bgs();

  for(int i=0;i<pos1;i++)
  {
    
    sprintf(file,"/proc/%d/stat",bgs[i].bg_pid);

    FILE* f = fopen(file,"r");

    if(f)
    {
      fscanf(f,"%*d %*s %c",&status);
      if(len == 1)
      {
        printf("[%d] %s %s [%d]\n",(bgs[i].jobnum)+1, status == 'T'?"Stopped" : "Running",bgs[i].bg_name,bgs[i].bg_pid);
      }
      else
      {
        if(!strcmp(args[1],"-s") && status == 'T')
        {
          printf("[%d] %s %s [%d]\n",(bgs[i].jobnum)+1,"Stopped",bgs[i].bg_name,bgs[i].bg_pid);
        }
        if(!strcmp(args[1],"-r") && status != 'T')
        {
          //printf("1");
          printf("[%d] %s %s [%d]\n",(bgs[i].jobnum)+1,"Running",bgs[i].bg_name,bgs[i].bg_pid);
        }
      }
      fclose(f);
    }

    else
    {
        //perror("error");
    }

  } 
  free(file);
}

void execute(char* args[100],char* cmd,int len)
{
  //checkbgproc();

  char* address = (char*)malloc(100);
  if(len>1)
  {
    strcpy(address,args[1]);
  }
  else
  {
    strcpy(address,"");
  }

  if(!strcmp(cmd,"cd"))
  {
    char cwd3[100];
    char temp[100];
    if(!strcmp(address,"~") || len == 1)
    {
      strcpy(address,home);
    }
    
    getcwd(cwd3,100);
    strcpy(temp,cwd3);

    if(!strcmp(address,"-"))
    {
      strcpy(address,prev);
      printf("%s\n",address);
      chdir(home);
      char cwd2[100];
    }

    strcpy(prev,temp);
    

    if(access(address,F_OK) == 0)
    {
      chdir(address);
      char cwd1[100];
      getcwd(cwd1,100);
      change_display(cwd1);
    }

    else
    {
      perror("Cannot access the directory");
      return;
    }

  }

  else if(!strcmp(cmd,"pwd"))
  {
    char cwd[100];
    getcwd(cwd,100);
    puts(cwd);
  }

  else if (!strcmp(cmd,"echo"))
  {
    char* temp1 = (char*)malloc(10000);
    strcpy(temp1,args[1]);
    for(int i=2;i<len;i++)
    {
      strcat(temp1,args[i]);
    }
    printf("%s\n",temp1);
  }

  else if(!strcmp(cmd,"ls"))
  {
    ls_function(len,args);
  }

  else if(!strcmp(cmd,"pinfo"))
  {
    pinfo(args,len);
  }

  else if(!strcmp(cmd,"repeat"))
  {
    int num = atoi(args[1]);
    char* temp = (char*)malloc(100);
    strcpy(temp,args[2]);
    if(len<3)
    {
      printf("Error: Enter proper repeat command");
      return;
    }
    for(int i=3;i<len;i++)
    {
      strcat(temp," ");
      strcat(temp,args[i]);
    }
    while(num--)
    {
      cmds_tokenize(temp);
    }
  }

  else if(!strcmp(cmd,"history"))
  {
    if(len == 1)
    {
      show_history();
    }
    else if(len == 2)
    {
      int num;
      if(strlen(args[1])==2)
      {
        num = args[1][0]-'0';
        num *= 10;
        num += args[1][1]-'0';
        if(num>20 || num<0)
        {
          printf("Enter proper command");
        }
        show_history1(num);  
      }
      else
      {
        show_history1(args[1][0]-'0');
      }
    }
    else
    {
      printf("Enter proper command");
    }
  }

  else if(!strcmp(cmd,"jobs"))
  {
    jobs(args,len);
  }

  else if(!strcmp(cmd,"sig"))
  {
    jobs(args,len);
    if(len == 1)
    {
      printf("Syntax Error");
      return;
    }
    char* ptr;
    pid_t pid;
    int num=strtol(args[1],&ptr,10);
    num = num - 1;
    if(num>=pos1)
    {
      printf("No job with the given job number exists\n");
      return;
    }
    for(int i=0;i<pos1;i++)
    {
      if(bgs[i].jobnum == num)
      {
        pid = bgs[i].bg_pid;
        break;
      }
    }
    int sgnum = strtol(args[2],&ptr,10);
    kill(pid,sgnum);
  }

  else if(!strcmp(cmd,"fg"))
  {
    jobs(args,len);
    if(len==1)
    {
      printf("Error");
    }
    else
    {
      char* ptr;
      pid_t pid;
      int num=strtol(args[1],&ptr,10);
      num = num - 1;
      if(num>=pos1)
      {
        printf("No job with the given job number exists\n");
        return;
      }
      for(int i=0;i<pos1;i++)
      {
        if(bgs[i].jobnum == num)
        {
          pid = bgs[i].bg_pid;
          break;
        }
      }

      int status;
      setpgid(pid, getpgid(0));

      signal(SIGTTIN, SIG_IGN);
      signal(SIGTTOU, SIG_IGN);

      tcsetpgrp(0, pid);

      if (kill(pid, SIGCONT) < 0) 
      {
        perror("Error");
      }

      waitpid(pid, &status, WUNTRACED);

      tcsetpgrp(0, getpgid(0));

      signal(SIGTTIN, SIG_DFL);
      signal(SIGTTOU, SIG_DFL);

      if (WIFSTOPPED(status)) 
      {
        return;
      }
    }
  }

  else if(!strcmp(cmd,"bg"))
  {
    jobs(args,len);
    if(len==1)
    {
      printf("Error");
    }
    else
    {
      char* ptr;
      pid_t pid;
      int num=strtol(args[1],&ptr,10);
      num = num - 1;
      if(num>=pos1)
      {
        printf("No job with the given job number exists\n");
        return;
      }
      for(int i=0;i<pos1;i++)
      {
        if(bgs[i].jobnum == num)
        {
          pid = bgs[i].bg_pid;
          break;
        }
      }
      if (kill(pid, SIGCONT) < 0) {
      perror("Error");
      }
    }
  }

  else if(!strcmp(cmd,"replay"))
  {
    if(len<7)
    {
      return;
    }
    char* newargs[20];
    int num1,num2;
    int j=0;
    for(int i=0;i<len;i++)
    {
      if(!strcmp(args[i],"-command"))
      {
        if(i+1>=len)
        {
          printf("Error");
          return;
        } 
        i++;
        while((strcmp(args[i],"-interval")||strcmp(args[i],"-interval")) && i<len)
        {
          newargs[j] = (char*)malloc(100);
          strcpy(newargs[j],args[i]);
          j++;
          i++;
        }
      }
      if(!strcmp(args[i],"-interval"))
      {
        if(i+1 < len)
        {
          num1 = atoi(args[i+1]);
        }
        else
        {
          printf("Error");
        }
      }
      if(!strcmp(args[i],"-period"))
      {
        if(i+1 < len)
        {
          num2 = atoi(args[i+1]);
        }
        else
        {
          printf("Error");
        }
      }
    }
    /*printf("%d%d\n",num1,num2);
    for(int i = 0;i<j;i++)
    {
      printf("%s\n",newargs[i]);
    }*/
    for(int i=0;i<num2/num1;i++)
    {
      execute(newargs,newargs[0],j);
      sleep(num1);
    }
  }

  else
  {
    fg_flag = 1;
    if(strcmp(args[len-1],"&"))
    {
      //printf("1")
      args[len] = NULL;
      process(args,0,&fg_pid);
    }
    else
    {
      args[--len] = NULL;
      int pid = process(args,1,&fg_pid);
      if(pid)
      {
        pidlist[pos] = pid;
        bglist[pos] = malloc(strlen(args[0])+1);
        strcpy(bglist[pos],args[0]);
        pos++;
      }
    }
    fg_flag = 0;
  }

  free(address);

}

void cmds_tokenize(char* line)
{
  int len = 0;
  char* token = (char*)malloc(1024);
  char cmd[100];
  char* args[100];
  token = strtok(line," \t\n");
  strcpy(cmd,token);
  int flg_redir = -1;
  int fd = 0,fd1 = 1;
  int chk = 0,chk1 = 0;

  while(token != NULL && strcmp(token,""))
  {
    if(flg_redir > -1)
    {
      if(flg_redir == 0)
      {
        fd = open(token,O_RDONLY);
      }
      else if(flg_redir == 1)
      {
        fd1 = open(token,O_WRONLY | O_CREAT | O_TRUNC,0644);
      }
      else if(flg_redir == 2)
      {
        fd1 = open(token,O_WRONLY | O_CREAT | O_APPEND,0644);
      }
      flg_redir = -1;
      chk1 = 1;
    }

    else if(!strcmp(token,"<")) 
    {
      flg_redir = 0;
      chk = 1;
      chk1 = 0;
    } 

    else if(!strcmp(token,">")) 
    {
      flg_redir = 1;
      chk = 1;
      chk1 = 0; 
    } 

    else if (!strcmp(token,">>"))
    {
      flg_redir = 2;
      chk = 1;
      chk1 = 0;
    } 

    else
    {
      args[len++] = token;
      args[len] = NULL;
    }
 
    token = strtok(NULL, " \t\n");
  }

  if(!chk1 && chk)
  {
    printf("Syntax Error\n");
    return;
  }

  if(fd < 0 || fd1 < 0) 
  {      
    perror("Error");
  }

  if(fd) 
  {
    int stin = dup(0);
    dup2(fd, 0);
  }
  
  if(fd1) 
  {
    int stout = dup(1);
    dup2(fd1, 1);
  }
   
  free(token);
  execute(args,cmd,len);
}


void pipe_handler(char* line)
{
  char* token[1000];
  int n=0;
  token[n] = strtok(line,"|");
  while(token[n] != NULL)
  {
    n++;
    token[n]=strtok(NULL,"|");
  }
 
  /*if(n==1)
  {

  }*/

  int out = dup(1);
  int in = dup(0);
  int prev_open = -1;
  int pd = 0;
  int fildes[2];

  for(int i=0;i<n;i++)
  {

    if(pipe(fildes)<0)
    {
      perror("Could not create pipe.");
      return;
    }

    pd = fork();

    if(pd == 0)
    {
      if (prev_open != -1) 
      {
        dup2(prev_open, 0);
      }

      if(i+1<n)
      {
        dup2(fildes[1],1);
      }

      close(fildes[0]);

      cmds_tokenize(token[i]);

      exit(2);
    }

    else
    {
      close(fildes[1]);
      prev_open = fildes[0];
    }
  }

}


void killbg() 
{
  pid_t pid;
  pid = getpid();
  kill(pid,SIGKILL);
}


void ctrlC()
{
  if(fg_flag==1)
  {
    if(kill(fg_pid,9)==0)
      fg_flag = 0;
    return;
  }
  else
  {
    signal(SIGINT,ctrlC);
    fflush(stdout);
  }
}

void ctrlZ()
{
  if(fg_flag==1)
  {
    if(kill(fg_pid,SIGTSTP)==0)
      //fg_z = 1;
    return;
  }
  else
  {
    signal(SIGSTOP,ctrlZ);
    fflush(stdout);
  }
}


int main()
{
  shell = getpid();
  getcwd(home,1024);
  change_display(home);
  signal(SIGINT,ctrlC);
  signal(SIGCHLD,checkbgproc);
  signal(SIGPIPE,SIG_IGN);
  signal(SIGTSTP,ctrlZ); 
  //fg_z = 0;*/
  fg_pid = 0;
  while(1)
  {
    fg_flag = 0;
    int a = dup(0);
    int b = dup(1);
    printf("%s",display);
    int j;
    char* cmd = (char*)malloc(1024);
    if(fgets(cmd,1024,stdin)==NULL)
    {
      printf("\n");
      break;
    }
    history(cmd);
    int asc = (int)cmd[0];
    if(!strcmp(cmd,"exit\n"))
    {
      killbg();
      break;
    }
    if(asc!=10)
    {
      char* tokens[100];
      int num_tokens;
      num_tokens = tokenize(cmd,tokens);
      int flag_bg = 0;
      for(int i=0;i<num_tokens;i++)
      {
        if(strchr(tokens[i],'|'))
        {
          pipe_handler(tokens[i]);
        }
        else
        {
          cmds_tokenize(tokens[i]);
        }
      }
    }
    dup2(a,0);
    dup2(b,1);
    free(cmd);
  }
  return 0;
}