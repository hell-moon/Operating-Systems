#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include "helperFunctions.c"
#define h_addr h_addr_list[0]

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		fprintf(stderr, "USAGE: %s plaintext key port\n", argv[0]);
		exit(0);
	} // Check usage & args

	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent *serverHostInfo;
	// char buffer[150000] = {'\0'};
	// char message[150000] = {'\0'};

	int plainLength = -5;
	int keyLength = -5;
	int optvalue = 1;
	char *plaintext = NULL;
	char *keytext = NULL;
	char *cyphertext = NULL;
	char authChar = '!';
	int idx = 0;

	readFileToBuffer(argv[1], &plainLength, &plaintext);
	readFileToBuffer(argv[2], &keyLength, &keytext);
	// printf("%d %d", plainLength, keyLength);

	// TODO Verify key is at least as large as plaintext
	if (keyLength < plainLength)
	{
		error("Key is too short");
		exit(1);
	}
	// TODO check key for invalid characters
	for (idx = 0; idx < (keyLength - 1); idx++)
	{
		if (isspace(keytext[idx]) || isupper(keytext[idx]))
		{
		}
		else
		{
			error("Key is invalid");
			break;
			exit(1);
		}
	}
	// Set up the server address struct
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]);									 // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET;							 // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber);					 // Store the port number
	serverHostInfo = gethostbyname("localhost");				 // Convert the machine name into a special form of address

	if (serverHostInfo == NULL)
	{
		fprintf(stderr, "CLIENT: ERROR, no such host\n");
		exit(0);
	}
	memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0)
	{
		error("CLIENT: ERROR opening socket");
		exit(1);
	}

	setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(int));

	// Connect to server
	if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
	{
		error("CLIENT: ERROR connecting");
		exit(1);
	}

	// TODO Authenticate w/ server
	if (authenticateClient == 0)
	{
		fprintf(stderr, "Can't authenticate server, port %d", portNumber);
		exit(2);
	}

	// TODO Send plaintext and key to server
	sendMessageToSocket(plainLength, plaintext, socketFD);
	sendMessageToSocket(keyLength, keytext, socketFD);
	// TODO Receive cyphertext from server

	receiveMessageFromSocket(&cyphertext, socketFD);
	printf("%s\n", cyphertext);

	close(socketFD); // Close the socket
	return 0;
}
