#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

// function declarations
char **getInputParsed();

int main()
{
	int exitStatus = 0; // default to 0, set exit value if exited normally, or set termination signal
	int howExited = 0;  // default to 0, set to 0 if exited normally, 1 if terminated

	while (1) // infinite loop, will exit by typing 'exit' in prompt
	{

		int idx, jdx, kdx = 0;
		char **parsedString = NULL;
		char *dirName = NULL;
		pid_t spawnpid = -5;
		int childExitMethod;

		// print prompt for command:
		printf(": ");
		fflush(stdout);
		parsedString = getInputParsed();
		if (parsedString[0] != NULL) // ignore empty input
		{
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
					printf("exit value %d\n", exitStatus);
					fflush(stdout); // test print
				}
				if (howExited == 1)
				{ // exited by termination, print termination signal
					printf("terminated by signal %d\n", exitStatus);
					fflush(stdout); // test print
				}
				// printf("exit value %d\n", exitStatus);fflush(stdout);	// test print
			}
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
					// child process
					// exec here
					execvp(parsedString[0], parsedString);
					// exit here, if exec succeeds, will not reach this line
					exit(11);
					break;
				default:
					waitpid(spawnpid, &childExitMethod, 0); // waits for it's child to terminate, store exit status in 2nd arg
					// checking exit status of child
					if (WIFEXITED(childExitMethod) != 0)
					{ // child terminated normally, get exit value
						exitStatus = WEXITSTATUS(childExitMethod);
						howExited = 0;
					}
					else
					{
						// child terminated by signal
						exitStatus = WTERMSIG(childExitMethod);	// get termination signal
						howExited = 1;							// set status flag
					}
					break;
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
char **getInputParsed()
{
	// getline test
	// read input from stdin, then parse and print each word on new line
	char *buffer = NULL;
	size_t buffer_size = 0;
	ssize_t numCharRead = 0;
	// store tokens in this string array
	char **parsedString = malloc(512 * sizeof(char));

	if ((numCharRead = getline(&buffer, &buffer_size, stdin)) == -1)
	{
		perror("error with getline");
	}

	//remove trailing /n from the input
	buffer[numCharRead - 1] = '\0';

	// printf("the line read in was: \"%s\".\n", buffer);
	fflush(stdout); // test print this line

	// use strtok to parse string
	char *token = strtok(buffer, " ");

	int i = 0;
	int counter = 0;
	while (token != NULL)
	{
		parsedString[counter] = token;
		counter++;
		token = strtok(NULL, " ");
	}
	parsedString[counter] = NULL;

	return parsedString;
}
