Extending the C shell from the assignment as a part of the operating systems and networks course

# Linux Bash Shell

## Working:

- Using priciples of a Linux Bash Shell in the Linux Operating system,
the User-defined interactive shell program can create and manage new processes.
The shell is able to create a process out of a system program like emacs, vi,
or any user-defined executable

## Features :

- cd :<br> The cd command is used to change the current working directory, navigate to root directory or show current directory path<br>
Usage :<br>
<Name@UBUNTU:\~>cd .. : Displays absolute path of current directory from root<br>
<Name@UBUNTU:\~>cd ../ : Navigates to the parent directory<br>
<Name@UBUNTU:\~>cd abc/xyz : Navigates to directory root/abc/xyz<br>
- ls :<br> ls command is used to view all files and directories. Flags -a and -l are also implemented.<br>
Usage :
Any of the following versions of the command shall work :<br>
● ls -a <br>
● ls -l<br>
● ls .<br>
● ls ..<br>
● ls ~<br>
● ls -a -l<br>
● ls -la / ls -al<br>
● ls <Directory/File_name><br>
● ls -<flags> <Directory/File_name><br>
- System Command like emacs and vi can also be executed on the shell in the background and Foreground<br>
- Foreground Processes :<br>
Executing any system program like "vi" command in the foreground implies
that my shell will wait for this process to complete and regain control when this process
exits<br>
- Background Processes :<br>
Any command that begins with "&" is considered a background command. This means that my shell will create
that process instead of waiting for it to exit. It will continue to accept user commands. Whenever a new background is available,
PID of the newly formed background process is printed on my shell after it has started.<br>
Usage:<br>
<Name@UBUNTU:\~> gedit &<br>
456<br>
<Name@UBUNTU:\~> ls -l -a<br>
.<br>
.<br>
Execute other commands<br>
- pinfo Command :<br>
This prints the process-related info of your shell program<br>
Usage :<br>
<Name@UBUNTU:\~>pinfo<br>
pid -- 231<br>
Process Status -- {R/S/S+/Z}<br>
memory -- 67854 {Virtual Memory}<br>
Executable Path -- ~/a.out<br>
- Input Output Redirection :<br>
Using the symbols <, > and >>, the output of commands, usually written to stdout, can be redirected to another file, or the input taken from a file other than stdin. Both
input and output redirection can be used simultaneously.<br>
Usage :<br>
<tux@linux:~> sort < file1.txt > lines_sorted.txt : Takes input from file1.txt and output stored at lines_sorted.txt<br>
- Command Pipelines :<br>
A pipe, identified by |, redirects the output of the command on the left as input to the command on the right. One or more commands can be piped as the following
examples show.<br>
Usage :<br>
<tux@linux:\~> cat sample2.txt | head -7 | tail -5<br>
<tux@linux:\~> cat < in.txt | wc -l > lines.txt<br>
- Jobs :<br>
This command prints a list of all currently running background processes spawned by the shell in alphabetical order of the command name, along with
their job number (a sequential number assigned by your shell), process ID and their state, which can either be running or stopped. There may be flags specified
as well. If the flag specified is -r, then only the running processes is printed else if the flag is -s then the stopped processes are printed.<br>
    
    Usage :<br>
    <tux@linux:\~> jobs<br>
    [1] Running emacs assign1.txt [221]<br>
    [3] Stopped vim [3211]<br>
    <br>
    <tux@linux:\~> jobs -r<br>
    [1] Running emacs assign1.txt [221]<br>
    <br>
    <tux@linux:\~> jobs -s<br>
    [3] Stopped vim [3211]<br>
    
- sig :<br>
Takes the job number (assigned by your shell) of a running job and sends the signal corresponding to signal number to that process.<br>
Usage:<br>
<tux@linux:~> sig 2 9<br>
sends SIGKILL (signal number 9) to the process causing it to terminate<br>
- fg and bg commands :<br>
fg Brings the running or stopped background job corresponding to job number to the foreground, and changes its state to running
bg Changes the state of a stopped background job to running (in the background).<br>
    
    Usage :<br>
    <tux@linux:\~> fg 4<br>
    brings [4] gedit to the foreground<br>
    <br>
    <tux@linux:\~> bg 3<br>
    Changes the state of [3] vim to running<br>
    
- Replay Command :<br>
executes a particular command in fixed time interval for a certain period<br>
Usage :<br>
<Name@UBUNTU:\~> replay -command echo "hi" -interval 3 -period 6<br>
executes echo "hi" command after every 3 seconds until 6 seconds are elapsed<br>
- Signal Handling of Cntrl Z (pushes current foreground process to background), Cntrl-C (interrupts any currently running foreground process)
and Cntrl-D (logs out of shell) is implemented<br>
- pwd : full pathname of the current working directory<br>
Usage :<br>
<Name@UBUNTU:~> pwd<br>
- echo : echo is a command that outputs the strings that are passed to it as arguments<br>
Usage :<br>
<Name@UBUNTU:~>echo abcdef<br>
abcdef<br>

Files

1)main.c

->It contains the main,change_display,checkbgproc,pinfo,execute functions
->Main function runs a while loop unless exit is encountered, It reads commands uses tokenize function 
  from token.c to tokenize the commands and for each tokenized command execute function is called with
  tokenized command array as argument.
->Execute function tokenizes the input string with space and uses if else statements to check the command and implement it
->pinfo gives process information for a given pid through /proc/pid/ directory
->checkbgproc checks if backgrounnd process is running or not and gives respective output if it is exited
->change_display changes the prompt for every directory change

2)ls.c

->Based on the flags -l -a -la -al ls is implemented
->A while loop is used to read the directory files with struct stat and give respective outputs
->For -l localtime function is used retrieve file information

3)token.c

->Using strtok with ; as delimitter commands are tokenized

4)process.c

->Used to create foreground and background processes
->fork is used to create a new process
->For a background process waiting is not required but foreground process waiting is required
->execvp is used

5)history.c

->history is stored and when history command is used history is displayed

You can run the shell from make file and then ./shell

---------------------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------------------EXTENSION OF README----------------------------------------------------------

->   files ls.c,token.c,history.c remain the same

->   process.c is changed so that it returns pid for a background process which is used to store information regarding background processes in the main.c file 
     and a new parameter is passed which stores pid for a foreground process which will later be used to implement fg specification
  
->   New specifications are introduced through main.c

->   Changes in main.c:

-    A new function void cmds_tokenize(char* line) is introduced to handle I/O redirections. It takes the tokens seperated by ';' and checks for '>' , '<' , '>>' and 
     opens appropriate input and output files using dup and dup2 and then pass the space seperated commands as arguments to execute function

-    Using strchr I check for pipes then, tokenize with '|' delimitter and for each token a pipe is created then, the process is forked, 
     
     [in the child process if a file is previously opened it is made as the new file descriptor in place of stdin file descriptor, 
     and stdout file descriptor is replaced file descriptor of write end of the pipe if next argument exists, 
     then close the read end of the pipe, execute the command]
     
     [after child returns close file descriptor of write end of the pipe and store file descriptor of the read end as previously opened file]
     
     Continue this process to execute all the commands
    
-    Execute function is changed to void execute(char* args[100],char* cmd,int len) to directly take space seperated commands as arguments and appropriate changes have been made 
     to include new specifications
     
-    For jobs, all the stored background process names along with their pids are entered into struct which stores background process names along with their pids and job numbers. Then this array of 
     structs is sorted using qsort according to the alphabetical order of their names and jobnumbers are assigned.

-    For sig, respective pid for a job number is retrieved from struct of arrays used for jobs and using kill respective signal is sent to that process

-    For fg, respective pid for a job number is retrieved from struct of arrays used for jobs and using kill,the signal SIGCONT and with waitpid the process is sent to the foreground in running state

-    For bg, respective pid for a job number is retrieved from struct of arrays used for jobs and using kill and the signal SIGCONT, the state of a stopped background job is changed 
     to running (in the background)
     
-    To handle ctrl C, signal call is used with signal number as SIGINT, using the kill call signal 9 (SIGKILL) currently running foreground process is interrupted

-    To handle ctrl Z, signal call is used with signal number as SIGINT, using the kill call signal (SIGTSTP) currently running foreground job is sent into the background and it is stopped

-    To handle ctrl D, while taking input ctrl D is identified and shell is logged out

-    Replay is implemented using sleep, the commands given are executed for respective period and time interval

-----------------------------------------------------------------------------------------------------------------------------------------------

