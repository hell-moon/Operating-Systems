#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <error.h>

int main(int argc, char *argv[])
{
	FILE *plaintext = fopen("plaintext4", "r");

	char buffer[100000] = {'\0'};

	if(plaintext == NULL){
		perror("error opening file\n");
	}
	fseek(plaintext, 0, SEEK_END);
	int length = ftell(plaintext);
	// length--;
	// reset file descriptor to beginning to read file
	fseek(plaintext, 0, SEEK_SET);
	if (buffer)
	{
		fread(buffer, 1, length, plaintext);
	}
	fclose(plaintext);
	int len = strlen(buffer);

	printf("%s", buffer);

	return 0;
}
