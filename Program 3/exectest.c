#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
	char* execArgs[] = {"ls", "-a", NULL, NULL, NULL, NULL};
	execvp(execArgs[0], execArgs);

	return 0;
}
