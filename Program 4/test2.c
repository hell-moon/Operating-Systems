#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "helperFunctions.c"

int main(int argc, char *argv[])
{
	// int messageLength;
	// char *message = NULL;
	// readFileToBuffer(argv[1], &messageLength, &message);
	
	// sendMessageToSocket(&messageLength, message, socketFD);

	// // printf("%d", messageLength);
	// free(message);

	int length = 69333;
	int tmp = htonl(length);
	int size = sizeof(tmp);
	tmp = ntohl(tmp);
}
