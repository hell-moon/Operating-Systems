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

int main()
{
	int foregroundExitStatus = 0; // default to 0, set exit value if exited normally, or set termination signal
	int backgroundExitStatus = 0; // default to 0, set background exit value for printing
	int howExited = 0;			  // default to 0, set to 0 if exited normally, 1 if terminated
	int backgroundPIDS[100];
	int pidCounter = 0;
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
				printf("the current working directory is: %s\n", getcwd(dirName, 0));
				fflush(stdout);

				// if no path argument passed, then change directory to path in HOME environment variable
				if (parsedString[1] == NULL)
				{
					chdir(getenv("HOME"));
					printf("the current working directory is: %s\n", getcwd(dirName, 0));
					fflush(stdout);
				}
				else
				{
					chdir(parsedString[1]);
					printf("the current working directory is: %s\n", getcwd(dirName, 0));
					fflush(stdout);
				}
				printf("cd was entered\n");
				fflush(stdout); // test print
			}
			else if (strcmp(parsedString[0], "exit") == 0)
			{
				printf("exit was entered\n");
				fflush(stdout); // test print
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
						spawnpid = waitpid(spawnpid, &childExitMethod, WNOHANG);
						if (spawnpid == -1)
						{
							printf("waitpid() error\n");
							exit(1);
						}
						break;
					}
					// ***********************END OF PARENT PROCESS********************************************
				}
			}
			// free(userInput);
			fflush(stdout);
			free(parsedString);
		}
	}

	return 0;
}

// This function gets input from stdin, parses the string, and then returns an array of pointers with the separated arguments
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
		{						 // else if reached & at end of command
			*backgroundFlag = 1; // child will check flag to see if process will be run in background
			break;				 // & as a token will always be at end of command list, break from loop
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

	if (!(p = strstr(str, orig))) // Is 'orig' even in 'str'?
		return str;

	strncpy(buffer, str, p - str); // Copy characters from 'str' start to 'orig' st$
	buffer[p - str] = '\0';

	sprintf(buffer + (p - str), "%s%s", rep, p + strlen(orig));

	return buffer;
}
