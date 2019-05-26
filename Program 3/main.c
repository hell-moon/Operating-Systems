/*
OREGON STATE UNIVERSITY, CS344 OPERATING SYSTEMS, SPRING 2019
Title: Program 3, smallsh
Description: This is the third programming assignment for CS344.  This program is a simple shell with three built-in commands: cd, status, and exit.  
The shell reads in user input and if not one of these built-in commands, it will execute user commands passed into the program.  
Input and output redirection is supported, "< 'filename'" will redirect stdin from that filename, "> 'filename'" will redirect stdout to that filename.
The shell will execute commands by forking a child process from itself and then calling execvp(). 
The shell itself ignores SIGINT signals, but passes it on to foreground children processes.  
Background processes are supported, by ending the command with the option, '&'.
A background mode can be toggled by sending a SIGTSTP signal to the shell.  This will toggle the ability to start a process in background mode(renders & useless).
Author: Xiao Kuang
Date: 5/26/2019
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

// function declarations
char **getInputParsed(char **, char **, int *);
char *replace_str(char *, char *, char *);

// global variables for handling sigtstp
int backgroundMode = 0; // default to 0, 1 disables running in background, handleSIGTSTP will toggle this value each time a SIGTSTP signal sent to shell

void toggleBackgroundMode(int signo)
{
	if (backgroundMode == 0)
	{
		backgroundMode = 1;
		printf("Entering foreground-only mode (& is now ignored)\n:");
		fflush(stdout);
	}
	else
	{
		backgroundMode = 0;
		printf("Exiting foreground-only mode\n:");
		fflush(stdout);
	}
}

int main()
{

	int foregroundExitStatus = 0; // default to 0, set exit value if exited normally, or set termination signal
	int backgroundExitStatus = 0; // default to 0, set background exit value for printing
	int howExited = 0;			  // default to 0, set to 0 if exited normally, 1 if terminated

	// set up signal set to ignore SIGINT
	sigset_t set_SIGINT;
	sigemptyset(&set_SIGINT);
	sigaddset(&set_SIGINT, SIGINT);

	// set up signal set with SIGTSTP
	sigset_t set_SIGTSTP;
	sigemptyset(&set_SIGTSTP);
	sigaddset(&set_SIGTSTP, SIGTSTP);

	// set up to ignore ctrl-c
	struct sigaction act_SIGINT;
	memset(&act_SIGINT, '\0', sizeof(act_SIGINT));
	act_SIGINT.sa_handler = SIG_IGN;
	sigaction(SIGINT, &act_SIGINT, NULL);

	struct sigaction act_SIGTSTP;
	memset(&act_SIGTSTP, '\0', sizeof(act_SIGTSTP));
	act_SIGTSTP.sa_handler = toggleBackgroundMode;
	act_SIGTSTP.sa_flags = SA_RESTART;
	sigaction(SIGTSTP, &act_SIGTSTP, NULL);

	while (1) // infinite loop, will exit by typing 'exit' in prompt
	{

		int idx, jdx, kdx = 0;
		char **parsedString = NULL;
		char *dirName = NULL;
		pid_t spawnpid = -5;
		int childExitMethod = -5;

		int inputFD = -1; // each while loop, these variables get initialized to NULL
		int outputFD = -1;
		char *inputFile = NULL;
		char *outputFile = NULL;
		int backgroundFlag = 0; // default 0, set to 1 to execute as background process

		// print prompt for command:
		printf(": ");
		fflush(stdout);
		parsedString = getInputParsed(&inputFile, &outputFile, &backgroundFlag);
		if (parsedString[0] != NULL) // ignore empty input
		{
			// *******************************START OF BUILT-IN COMMANDS***************************************
			// check if input was a built - in shell command : cd, status, exit int idx = 0;
			if (strcmp(parsedString[0], "cd") == 0)
			{
				// printf("the current working directory is: %s\n", getcwd(dirName, 0));
				// fflush(stdout);

				// if no path argument passed, then change directory to path in HOME environment variable
				if (parsedString[1] == NULL)
				{
					chdir(getenv("HOME"));
					// printf("the current working directory is: %s\n", getcwd(dirName, 0));
					// fflush(stdout);
				}
				else
				{
					chdir(parsedString[1]);
					// printf("the current working directory is: %s\n", getcwd(dirName, 0));
					// fflush(stdout);
				}
				// printf("cd was entered\n");
				// fflush(stdout); // test print
			}
			else if (strcmp(parsedString[0], "exit") == 0)
			{
				// printf("exit was entered\n");
				// fflush(stdout); // test print
				exit(0);
				break;
			}
			else if (strcmp(parsedString[0], "status") == 0)
			{ // this will print the exit value of the most recent foreground process or it's  terminating signal
				// print exit value 0 if status called before a foreground process
				if (howExited == 0)
				{ // exited normally, print exit value
					printf("exit value %d\n", foregroundExitStatus);
					fflush(stdout); // test print
				}
				if (howExited == 1)
				{ // exited by termination, print termination signal
					printf("terminated by signal %d\n", foregroundExitStatus);
					fflush(stdout); // test print
				}
				// printf("exit value %d\n", foregroundExitStatus);fflush(stdout);	// test print
			}
			// *******************************END OF BUILT-IN COMMANDS*****************************************
			else
			{
				// if this point is reached, that means it is not a built-in command. fork a child and execute given command
				spawnpid = fork();
				switch (spawnpid)
				{
				case -1:
					perror("error with spawning child fork");
					exit(10);
					break;
				case 0:
					// ***********************START OF CHILD PROCESS*******************************************
					// redirect input and output, if applicable.
					if (inputFile != NULL)
					{ // inputFile is default NULL, if not NULL then input redirection was specified
						// try to open file with same name as inputFile, if successful then redirect FD = 0 to where FD = inputFile points
						inputFD = open(inputFile, O_RDONLY);
						if (inputFD == -1)
						{
							// error opening file
							printf("Unable to open input file \"%s\"", inputFile);
							fflush(stdout);
							exit(1);
						}
						else
						{
							dup2(inputFD, 0); // redirect stdin to where inputFD points to
						}
					}
					else
					{
						// if inputFile not specified, and background flag is set, then have stdin redirect from /dev/null
						if (backgroundFlag == 1)
						{
							// open /dev/null
							int devNull = open("/dev/null", O_RDONLY);
							if (devNull == -1)
							{
								// error
								printf("Unable to open /dev/null\n");
								fflush(stdout);
								exit(1);
							}
							else
							{
								dup2(devNull, 0);
							}
						}
					}
					if (outputFile != NULL)
					{
						// outputFile is default NULL, if not NULL, then output redirection was specified
						outputFD = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0744); // open file for as truncate; clear file for writing if exists, or create if doesn't exist
						if (outputFD == -1)
						{
							// error opening file
							printf("Unable to open output file \"%s\"", outputFile);
							fflush(stdout);
							exit(1);
						}
						else
						{
							dup2(outputFD, 1); // redirect stdout to where outputFD points to
						}
					}
					else
					{
						// if inputFile not specified, and background flag is set, then have stdin redirect from /dev/null
						if (backgroundFlag == 1)
						{
							// open /dev/null
							int devNull = open("/dev/null", O_WRONLY);
							if (devNull == -1)
							{
								// error
								printf("Unable to open /dev/null\n");
								fflush(stdout);
								exit(1);
							}
							else
							{
								dup2(devNull, 1);
							}
						}
					}
					// if foreground command, allow SIGINT to terminate
					if (backgroundFlag == 0)
					{
						sigprocmask(SIG_UNBLOCK, &set_SIGINT, NULL);
						// block SIGTSTP during foreground
						sigprocmask(SIG_SETMASK, &set_SIGTSTP, NULL);
					}
					// exec here
					if (execvp(parsedString[0], parsedString) == -1)
					{
						// if reached here, then there is an error, set foregroundExitStatus to 1 and print error to screen
						foregroundExitStatus = 1;
						printf("There was an error executing that command, please try again\n");
						fflush(stdout);
						exit(1);
					}
					break;
					// ***********************END OF CHILD PROCESS*********************************************

				default:
					// ***********************START OF PARENT PROCESS******************************************
					// if background mode is 1, disable running in background
					if (backgroundMode == 0)
					{
						// determine if parent will wait for foreground child or NOT WAIT for background child
						if (backgroundFlag == 0) // backgrond flag NOT SET, wait for foreground child
						{
							spawnpid = waitpid(spawnpid, &childExitMethod, 0); // waits for it's child to terminate, store exit status in 2nd arg
							if (spawnpid == -1)
							{
								printf("waitpid() error\n");
								exit(1);
								break;
							}
							// checking exit status of child
							else
							{
								if (WIFEXITED(childExitMethod) != 0)
								{ // child terminated normally, get exit value
									foregroundExitStatus = WEXITSTATUS(childExitMethod);
									howExited = 0;
								}
								else
								{
									// child terminated by signal
									foregroundExitStatus = WTERMSIG(childExitMethod); // get termination signal
									howExited = 1;									  // set status flag
								}
								break;
							}
						}
						// background flag SET, don't wait for background child
						else
						{
							printf("[bg]%d\n", spawnpid);
							break;
						}
					}
					else // background mode == 1, disable running in background regardless of &
					{
						spawnpid = waitpid(spawnpid, &childExitMethod, 0); // waits for it's child to terminate, store exit status in 2nd arg
						if (spawnpid == -1)
						{
							printf("waitpid() error\n");
							exit(1);
							break;
						}
						// checking exit status of child
						else
						{
							if (WIFEXITED(childExitMethod) != 0)
							{ // child terminated normally, get exit value
								foregroundExitStatus = WEXITSTATUS(childExitMethod);
								howExited = 0;
							}
							else
							{
								// child terminated by signal
								foregroundExitStatus = WTERMSIG(childExitMethod); // get termination signal
								howExited = 1;									  // set status flag
							}
							break;
						}
					}

					// ***********************END OF PARENT PROCESS********************************************
				}
			}
			// free(userInput);
			fflush(stdout);
			free(parsedString);
			free(inputFile);
			free(outputFile);

			// check if any process has completed, return immediately with 0 if none have
			spawnpid = waitpid(-1, &childExitMethod, WNOHANG);
			while (spawnpid > 0)
			{
				if (WIFEXITED(childExitMethod) != 0)
				{ // child terminated normally, get exit value
					backgroundExitStatus = WEXITSTATUS(childExitMethod);
					howExited = 0;
				}
				else
				{
					// child terminated by signal
					backgroundExitStatus = WTERMSIG(childExitMethod); // get termination signal
					howExited = 1;
				}

				if (howExited == 0)
				{ // exited normally, print exit value
					printf("[bg]%d donezo, exit value %d\n", spawnpid, backgroundExitStatus);
					fflush(stdout); // test print
				}
				if (howExited == 1)
				{ // exited by termination, print termination signal
					printf("[bg]%d donezo , termintion signal %d\n", spawnpid, backgroundExitStatus);
					fflush(stdout); // test print
				}
				spawnpid = waitpid(-1, &childExitMethod, WNOHANG);
			}
		}
	}

	return 0;
}

/* This function gets input from stdin, parses the string, and then returns an array of pointers with the separated arguments.

*/
char **getInputParsed(char **inputFile, char **outputFile, int *backgroundFlag)
{
	// getline test
	// read input from stdin, then parse and print each word on new line
	char *buffer = NULL;
	size_t buffer_size = 0;
	ssize_t numCharRead = 0;
	// store tokens in this string array
	char **parsedString = malloc(512 * sizeof(char));

	if ((numCharRead = getline(&buffer, &buffer_size, stdin)) == -1) // read in line from user, check if read correctly
	{
		clearerr(stdin);
		perror("error with getline");
	}

	//remove trailing /n from the input
	buffer[numCharRead - 1] = '\0';

	// printf("the line read in was: \"%s\".\n", buffer);
	// fflush(stdout); // test print this line

	// use strtok to parse string
	char *token = strtok(buffer, " ");

	int i = 0;
	int counter = 0;
	while (token != NULL)
	{
		if (strcmp(token, "<") == 0)
		{
			// if input redirection found
			// then the next token is the input file name
			token = strtok(NULL, " ");
			// search token for "$$", if found, replace with process id
			if ((strstr(token, "$$")) != NULL)
			{
				int thisPid = getpid();
				char pidstr[16];
				sprintf(pidstr, "%d", thisPid);
				token = replace_str(token, "$$", pidstr);
			}
			// allocate memory and copy input file name over
			*inputFile = calloc(512, sizeof(char));
			strcpy(*inputFile, token);
			// prepare next token to be examined
			token = strtok(NULL, " ");
		}
		else if (strcmp(token, ">") == 0)
		{ // else if output redirection found
			// then the next token is the output file name
			token = strtok(NULL, " ");
			// search token for "$$", if found, replace with process id
			if ((strstr(token, "$$")) != NULL)
			{
				int thisPid = getpid();
				char pidstr[16];
				sprintf(pidstr, "%d", thisPid);
				token = replace_str(token, "$$", pidstr);
			}
			// allocate memory and copy output file name over
			*outputFile = calloc(512, sizeof(char));
			strcpy(*outputFile, token);
			// prepare next token to be examined
			token = strtok(NULL, " ");
		}
		else if (strcmp(token, "&") == 0)
		{
			// else if reached & at end of command
			if ((token = strtok(NULL, " ")) == NULL)
			{
				*backgroundFlag = 1; // child will check flag to see if process will be run in background
				break;				 // & as a token will always be at end of command list, break from loop
			}
			else
			{
				parsedString[counter] = "&";
				counter++;
			}
		}
		else
		{
			// if reached this point, then has to be command or argument
			// search token for "$$", if found, replace with process id
			if ((strstr(token, "$$")) != NULL)
			{
				int thisPid = getpid();
				char pidstr[16];
				sprintf(pidstr, "%d", thisPid);
				token = replace_str(token, "$$", pidstr);
			}
			// store token in parsed string
			parsedString[counter] = token;
			// if line starts with #, stop parsing and function returns pointer to null
			if (((int)(*parsedString[0])) == '#')
			{
				break;
			}
			counter++;
			token = strtok(NULL, " ");
		}
	}
	// make sure parsed string ends with NULL
	parsedString[counter] = NULL;
	return parsedString;
}

// function to find '$$' in substring and replace with passed in pid in string
// https://www.linuxquestions.org/questions/programming-9/replace-a-substring-with-another-string-in-c-170076/
char *replace_str(char *str, char *orig, char *rep)
{
	static char buffer[4096];
	char *p;

	// Is 'orig' even in 'str'?
	if (!(p = strstr(str, orig))){
		return str;
	}
		
	// Copy characters from 'str' start to 'orig' st$
	strncpy(buffer, str, p - str);
	buffer[p - str] = '\0';

	sprintf(buffer + (p - str), "%s%s", rep, p + strlen(orig));

	return buffer;
}
