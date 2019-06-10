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
#include "helperFunctions.c"

int main(int argc, char *argv[])
{
	int listenSocketFD, childSocketFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	int optvalue = 1;
	pid_t spawnpid;

	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2)
	{
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		exit(1);
	} // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]);									 // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET;							 // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber);					 // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY;					 // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0)
		error("ERROR opening socket");

	setsockopt(listenSocketFD, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(int));

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
	{ // Connect socket to port
		error("ERROR on binding");
		exit(1);
	}

	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	while (1)
	{
		// Accept a connection, blocking if one is not available until one connects
		// Get the size of the address for the client that will connect
		sizeOfClientInfo = sizeof(clientAddress);
		childSocketFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (childSocketFD < 0)
		{
			error("ERROR on accept");
			exit(1);
		}
		if (((spawnpid = fork()) < 0)) //new child process
		{
			error("ERROR with fork");
			exit(1);
		}
		else if (spawnpid == 0) //if process is the child
		{
			// char buffer[150000] = {'\0'};
			char *keytext = NULL;
			char *plaintext = NULL;
			int msgLength = 0;
			int idx, jdx = 0;
			char authChar = '!';

			// TODO Authenticate here
			authenticateServer(authChar, childSocketFD);
			// TODO Read in plaintext and key from client
			receiveMessageFromSocket(&plaintext, childSocketFD);
			receiveMessageFromSocket(&keytext, childSocketFD);
			// printf("%s\n%s", plaintext, keytext);
			// TODO Encrypt plaintext to cyphertext
			encodeMsg(plaintext, keytext);
			msgLength = strlen(plaintext);
			// printf("%d", msgLength);

			// TODO Send cyphertext to client
			sendMessageToSocket(msgLength, plaintext, childSocketFD);
			exit(1);
		}
		close(childSocketFD); // Close the existing socket which is connected to the client
	}
	close(listenSocketFD); // Close the listening socket
	return 0;
}
