// #include <sys/types.h>
// #include <signal.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// void catchSIGINT(int signo)
// {
// 	char *message = "CAUGHT SIGINT, sleeping for 5 seconds\n";
// 	write(STDOUT_FILENO, message, 38);
// 	raise(SIGUSR2);
// 	sleep(5);
// }
// void catchSIGUSR2(int signo)
// {
// 	char *message = "CAUGHT SIGUSR2, exiting\n";
// 	write(STDOUT_FILENO, message, 24);
// 	exit(0);
// }

// int main(int argc, char *argv[])
// {

// 	// pid_t spawnPid = -5;
// 	// int childExitStatus = -5;

// 	// spawnPid = fork();
// 	// if (spawnPid == -1)
// 	// {
// 	// 	// error
// 	// 	perror("fucked shit up\n");
// 	// 	exit(1);
// 	// }
// 	// else if (spawnPid == 0)
// 	// {
// 	// 	printf("this is da child. fork returned: %d and pid is: %d\n", spawnPid, (int)getpid());
// 	// 	exit(6);
// 	// }

// 	// printf("this is da parent. da pid is: %d \n", getpid());
// 	// waitpid(spawnPid, &childExitMethod, 0);
// 	// printf("this is da parent.  my child is dying now\n");

// 	// // check how child terminated
// 	// if (WIFEXITED(childExitMethod) != 0){
// 	// 	// exited normally
// 	// 	int exitStatus = WEXITSTATUS(childExitMethod);
// 	// 	printf("child terminated normally, exit status is: %d\n", (int)childExitMethod);
// 	// } else if(WIFSIGNALED(childExitMethod != 0)){
// 	// 	// terminated via signal
// 	// 	int termSignal = WTERMSIG(childExitMethod);
// 	// 	printf("child terminated from signal, signal is: %d\n", (int)childExitMethod);
// 	// }
// 	// exit(0);

// 	// 	switch(spawnPid){
// 	// 		case -1: {
// 	// 			perror("oopsy, sumfing went wrong\n");
// 	// 			exit(1);
// 	// 			break;
// 	// 		}
// 	// 		case 0: {
// 	// 			// printf("Child PID: %d, going to sleep for 1 second\n", getpid());
// 	// 			// sleep(1);
// 	// 			// printf("Child PID: %d, converting into \'ls -a\'\n", getpid());
// 	// 			// execlp("ls", "ls", "-la", NULL);
// 	// 			// // if execlp succeeds, then no statements after that call will run
// 	// 			// // if reach here, then something went wrong
// 	// 			// perror("Child: exec failed, oopsy\n");
// 	// 			// exit(2);

// 	// 			printf("CHILD TERMINATING\n");
// 	// 			break;
// 	// 		}
// 	// 		default: {
// 	// 			// printf("Parent PID: %d, going to sleep for 2 seconds\n", getpid());
// 	// 			// sleep(2);
// 	// 			// printf("Parent PID: %d, waiting for child PID: %d to terminate\n", getpid(), spawnPid);
// 	// 			// pid_t actualPid = waitpid(spawnPid, &childExitStatus, 0);
// 	// 			// // if reach here, then child has terminated and can check exit status
// 	// 			// printf("WIFEXITED value: %d\n", WIFEXITED(childExitStatus));
// 	// 			// if(WIFEXITED(childExitStatus) != 0){
// 	// 			// 	// if wait exit status is 0, then fail. So we check if not 0, to pass
// 	// 			// 	printf("Child: %d terminated normally\n", actualPid);
// 	// 			// 	int exitStatus = WEXITSTATUS(childExitStatus);
// 	// 			// 	printf("Child exit status is: %d\n", exitStatus);
// 	// 			// }
// 	// 			// printf("Parent %d: Child(%d) terminated, Exiting\n", getpid(), actualPid);
// 	// 			// exit(0);

// 	// 			printf("PARENT: making a child a zombie for ten seconds\n");
// 	// 			printf("PARENT: sleeping\n");
// 	// 			fflush(stdout);
// 	// 			sleep(10);
// 	// 			waitpid(spawnPid, &childExitStatus, 0);
// 	// 			break;
// 	// 		}

// 	// 	// }

// 	// 	// setenv("MYVAR", "SOME Text here big boii, 420BlazeIT", 1);
// 	// 	// printf("MYVAR: %s", getenv("MYVAR"));

// 	// 	// char array[1000];
// 	// 	// strcpy(array, getenv(argv[1]));
// 	// 	// strcat(array, " POOPYBUTTHOLE\n");
// 	// 	// setenv(argv[1], array, 1);
// 	// 	// printf("Variable %s has value: %s\n", argv[1], getenv(argv[1]));
// 	// }printf("This line will be executed by both child and parent\n");
// 	// 		exit(0);

// 	printf("Eat my ass bigboii\n");

// 	while(1);
// }

// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <signal.h>
// #include <unistd.h>

// void catchSIGINT(int signo)
// {
// 	char* message = "SIGINT. Use CTRL-Z to Stop.\n";
// 	write(STDOUT_FILENO, message, 28);
// }

// void main()
// {
// 	struct sigaction SIGINT_action = {0};
// 	SIGINT_action.sa_handler = catchSIGINT;
// 	sigfillset(&SIGINT_action.sa_mask);
// 	//SIGINT_action.sa_flags = SA_RESTART;
// 	sigaction(SIGINT, &SIGINT_action, NULL);

// 	int numCharsEntered = -5; // How many chars we entered
// 	int currChar = -5; // Tracks where we are when we print out every char
// 	size_t bufferSize = 0; // Holds how large the allocated buffer is
// 	char* lineEntered = NULL; // Points to a buffer allocated by getline() that holds our entered string + \n + \0
	
// 	while(1)
// 	{
// 		// Get input from the user
// 		while(1)	
// 		{
// 			printf("Enter in a line of text (CTRL-C to exit):");
// 			numCharsEntered = getline(&lineEntered, &bufferSize, stdin); // Get a line from the user
// 			if (numCharsEntered == -1)
// 				clearerr(stdin);
// 			else
// 				break; // Exit the loop - we've got input
// 		}

// 		printf("Allocated %zu bytes for the %d chars you entered.\n", bufferSize, numCharsEntered);
// 		printf("Here is the raw entered line: \"%s\"\n", lineEntered);
// 		lineEntered[strcspn(lineEntered, "\n")] = '\0'; // Remove the trailing \n that getline adds
// 		printf("Here is the cleaned line: \"%s\"\n", lineEntered);

// 		free(lineEntered); // Free the memory allocated by getline() or else memory leak
// 		lineEntered = NULL;
// 	}
// }
