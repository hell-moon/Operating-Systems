// #include <sys/types.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <sys/wait.h>

// int main()
// {
// 	// char* execArgs[] = {"ls", "-a", NULL, NULL, NULL, NULL};
// 	// execvp(execArgs[0], execArgs);
// 	char* doubledolla = "$$";
// 	printf("the size of '$$' is: %ld", sizeof(doubledolla));
// 	return 0;
// }

// char *replace_str(char *str, char *orig, char *rep)
// {
//   static char buffer[4096];
//   char *p;

//   if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
//     return str;

//   strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
//   buffer[p-str] = '\0';

//   sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

//   return buffer;
// }

// int main(void)
// {
// 	int thisPid = getpid();
// 	char pidstr[16];
// 	sprintf(pidstr, "%d", thisPid);
//   puts(replace_str("$$boiibitch", "$$", pidstr));

//   return 0;
// }

// #include <signal.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <sys/wait.h>
// void catchSIGINT(int signo)
// {
// 	char *message = "caught SIGINT, sleeping for 5 seconds\n";
// 	write(STDOUT_FILENO, message, 38);
// 	raise(SIGUSR2);
// 	sleep(5);
// }
// void catchSIGUSR2(int signo)
// {
// 	char *message = "caught SIGUSR2, exiting\n";
// 	write(STDOUT_FILENO, message, 25);
// 	exit(0);
// }
// // struct sigaction
// // {
// // 	void (*sa_handler)(int);
// // 	void (*sa_sigaction)(int, siginfo_t *, void *);
// // 	sigset_t sa_mask;
// // 	int sa_flags;
// // 	void (*sa_restorer)(void);
// // };
// int main()
// {

// 	struct sigaction SIGINT_action = {0}, SIGUSR2_action = {0}, ignore_action = {0};

// 	return 0;
// }

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
// void catchSIGINT(int signo)
// {
// 	char *message = "Caught SIGINT, sleeping for 5 seconds\n";
// 	write(STDOUT_FILENO, message, 38);
// 	raise(SIGUSR2);
// 	sleep(5);
// }
// void catchSIGUSR2(int signo)
// {
// 	char *message = "Caught SIGUSR2, exiting!\n";
// 	write(STDOUT_FILENO, message, 25);
// 	exit(0);
// }

// int main()
// {
// 	// declare 3 sigaction structs
// 	struct sigaction SIGINT_action = {0}, SIGUSR2_action = {0}, ignore_action = {0};

// 	SIGINT_action.sa_handler = catchSIGINT;	// SIGINT_action struct sets its handler to point to catchSIGINT function
// 	sigfillset(&SIGINT_action.sa_mask);		// block all signals
// 	SIGINT_action.sa_flags = 0;

// 	SIGUSR2_action.sa_handler = catchSIGUSR2;
// 	sigfillset(&SIGUSR2_action.sa_mask);	// block all signals
// 	SIGUSR2_action.sa_flags = 0;

// 	ignore_action.sa_handler = SIG_IGN;

// 	sigaction(SIGINT, &SIGINT_action, NULL);	// associating SIGINT signal with SIGINT_action struct that points to catchSIGINT function
// 	sigaction(SIGUSR2, &SIGUSR2_action, NULL);	// associating SIGUSR2 signal with SIGUSR2_action struct that points to catchSIGUSR2 function
// 	sigaction(SIGTERM, &ignore_action, NULL);	// SIGTERM, SIGHUP, SIGQUIT are set to be ignored
// 	sigaction(SIGHUP, &ignore_action, NULL);
// 	sigaction(SIGQUIT, &ignore_action, NULL);

// 	printf("SIGTERM, SIGHUP, and SIGQUIT are disabled.\n");
// 	printf("Send a SIGUSR2 signal to kill this program.\n");
// 	printf("Send a SIGINT signal to sleep 5 seconds, then kill this program.\n");
// 	while (1)
// 		pause();
// }

void catchSIGCHILD(int signo, siginfo_t *info, void* ignr)
{
// this function handles a SIGCHILD signal.  
// when a child process terminates, this function gets called
    printf("Signal originates from process %d\n",info->si_pid);

}

int main()
{
	int spawnpid;
	int waitstatus;

	struct sigaction SIGCHLD_action = {0};

	SIGCHLD_action.sa_sigaction = catchSIGCHILD;
	sigaddset(&SIGCHLD_action.sa_mask, SIGCHLD);	// blocks CHLD signal
	SIGCHLD_action.sa_flags = 1;

	sigaction(SIGCHLD, &SIGCHLD_action, NULL);

	spawnpid = fork();

	switch (spawnpid)
	{
	case 0:
		// child process
		printf("child process\n");
		execlp("ls", "ls", "-a", NULL);
		(exit);
		break;
	case -1:
		// error
		printf("error\n");
		exit(5);
		break;
	default:
		// parent process
		printf("parent process, PID: %d\n", getpid());
		waitpid(spawnpid, &waitstatus, 0);
		break;
	}
}
