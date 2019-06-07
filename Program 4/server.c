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

void error(const char *msg)
{
	perror(msg);
	exit(1);
} // Error function used for reporting issues

/*
	Convert a character, c, to alphabet index: A = 0, B = 1, ... space = 26
*/
int charToInt(char c)
{
	char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	int idx = 0;
	int retval = -5;
	while (alphabet[idx] != '\0')
	{
		if (alphabet[idx] == c)
		{
			retval = idx;
			break;
		}
		idx++;
	}
	return retval;
}

/*
	Convert alphabet index, n,  to uppercase character: 0 = A, 1 = B, ... 26 = space
*/
char intToChar(int n)
{
	char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	return alphabet[n];
}

/*
	For each character in message, perform modular addition with respective character in key.  
	Replace that element in message with the encoded character, msg will hold cyphertext
*/
void encodeMsg(char *msg, char *key)
{
	int idx, jdx = 0;
	int charInt;
	int keyInt;
	int length = strlen(msg);

	for (idx = 0; idx < length; idx++)
	{
		if (msg[idx] == '@')
		{
			break;
		}
		charInt = charToInt(msg[idx]);
		keyInt = charToInt(key[idx]);
		msg[idx] = ((charInt + keyInt) % 27);
		msg[idx] = intToChar(msg[idx]);
	}
}

void sendMsg(char *message, int socketFD)
{
	// FILE *plaintext = fopen(plaintextfile, "r");
	int length = strlen(message);
	int charsWritten = -5;

	int sendCounter = 0;
	while (sendCounter < length)
	{
		// Send message to server
		charsWritten = send(socketFD, message, length, 0); // Write to the server
		// printf("chars written: %d", charsWritten);
		if (charsWritten < 0)
			error("CLIENT: ERROR writing to socket");
		if (charsWritten < length - 1)
			printf("CLIENT: WARNING: Not all data written to socket!\n");
		sendCounter += charsWritten;

		int checkSend = -5; // Bytes remaining in send buffer
		do
		{
			ioctl(socketFD, TIOCOUTQ, &checkSend); // Check the send buffer for this socket
												   //printf("checkSend: %d\n", checkSend);  // Out of curiosity, check how many remaining bytes there are:
		} while (checkSend > 0);				   // Loop forever until send buffer for this socket is empty

		if (checkSend < 0) // Check if we actually stopped the loop because of an error
			error("ioctl error");
	}
}
int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[70000] = {'\0'};
	char authenticate[] = "*";
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

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress);

	// Get the size of the address for the client that will connect
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0)
		error("ERROR on accept");

	// clear buffer before read
	bzero(buffer, 70000);
	// authenticate with client, make sure it is opt_enc
	if((charsRead = recv(establishedConnectionFD, buffer, 1, 0)) < 1){
		error("Error receiving authentication from client\n");
	}else{
		if(buffer[0] == authenticate[0]){
			int charsWritten = send(establishedConnectionFD, authenticate, 1, 0);
			if(charsWritten < 1){
				error("Error sending authentication to client\n");
			}
		}
	}

	// Get the message from the client and display it
	char completeMessage[70000] = {'\0'};
	char key[70000] = {'\0'};

	char *endMessagePtr = NULL;
	while ((endMessagePtr = strstr(completeMessage, "@")) == NULL)
	{
		memset(buffer, '\0', 70000);
		charsRead = recv(establishedConnectionFD, buffer, 70000, 0); // Read the client's message from the socket
		strncat(completeMessage, buffer, charsRead);
		// printf("chars read plaintxt: %d", charsRead);
		if (charsRead < 1)
			error("ERROR reading from socket");
	}

	int position = endMessagePtr - completeMessage;
	completeMessage[position] = '@';
	completeMessage[position + 1] = '\0';

	// Get key from client
	endMessagePtr = NULL;
	while ((endMessagePtr = strstr(key, "@")) == NULL)
	{
		memset(buffer, '\0', 70000);
		charsRead = recv(establishedConnectionFD, buffer, 70000, 0); // Read the client's message from the socket
		strncat(key, buffer, charsRead);
		// printf("chars read key: %d", charsRead);
		if (charsRead < 0)
			error("ERROR reading from socket");
	}

	position = endMessagePtr - key;
	key[position] = '\0';

	encodeMsg(completeMessage, key);

	sendMsg(completeMessage, establishedConnectionFD);

	printf("%s", completeMessage);

	// Send a Success message back to the client
	// charsRead = send(establishedConnectionFD, completeMessage, 39, 0); // Send success back
	// if (charsRead < 0)
	// 	error("ERROR writing to socket");

	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	close(listenSocketFD);			// Close the listening socket
	// free(buffer);
	return 0;
}
