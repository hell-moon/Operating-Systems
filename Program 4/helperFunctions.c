#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
	perror(msg);
	exit(0);
} // Error function used for reporting issues

//newline is removed at end, length is -1 of plaintext
void readFileToBuffer(char *fileName, int *fileLength, char **buffer)
{
	// TODO: open file
	FILE *fileptr = fopen(fileName, "r");
	if (fileptr == NULL)
	{
		error("Error opening file");
		exit(1);
	}
	// TODO: get and store length of the file
	fseek(fileptr, 0, SEEK_END);
	*fileLength = (ftell(fileptr));
	fseek(fileptr, 0, SEEK_SET);
	// TODO: store contents of file in buffer
	*buffer = calloc(*fileLength, sizeof(char));
	fread(*buffer, 1, (*fileLength), fileptr);
	buffer[0][((*fileLength) - 1)] = '\0';
}

void sendMessageToSocket(int length, char *message, int socketFD)
{
	// first send length, then send the message
	int tmp = htonl(length);

	int numSent = write(socketFD, &tmp, sizeof(tmp));
	// error check
	if (numSent == 0)
	{
		error("sendMessageToSocket: 0 bytes sent");
	}
	else if (numSent < 0)
	{
		error("sendMessageToSocket: error sending");
	}
	else
	{
		// send message
		numSent = write(socketFD, message, length);
		// printf("%d", numSent);
	}
}

void receiveMessageFromSocket(char **destination, int socketFD)
{
	// receive # of bytes to read
	int tmp, bytesToRead = 0;

	int numRead = read(socketFD, &tmp, sizeof(tmp));
	// printf("server read # bytes: %d", numRead);
	if (numRead == 0)
	{
		error("receiveMessageFromSocket: 0 bytes read");
	}
	else if (numRead < 0)
	{
		error("receiveMessageFromSocket: error reading");
	}

	bytesToRead = ntohl(tmp);
	// printf("%d", bytesToRead);
	*destination = calloc(bytesToRead, sizeof(char));

	char buffer[80000] = {'\0'};

	while (1)
	{
		if (bytesToRead == 0)
		{
			break;
		}
		bzero(buffer, 80000);
		numRead = read(socketFD, buffer, bytesToRead);

		strncat(destination[0], buffer, numRead);

		bytesToRead = (bytesToRead - numRead);
	}
	//fprintf(stderr, "Length is %ld\n", strlen(buffer));

	//printf("%s", buffer);
}

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
		charInt = charToInt(msg[idx]);
		keyInt = charToInt(key[idx]);
		msg[idx] = ((charInt + keyInt) % 27);
		msg[idx] = intToChar(msg[idx]);
	}
}

void decodeMsg(char *msg, char *key)
{
	int idx, jdx = 0;
	int charInt;
	int keyInt;
	int length = strlen(msg);

	for (idx = 0; idx < length; idx++)
	{
		charInt = charToInt(msg[idx]);
		keyInt = charToInt(key[idx]);
		msg[idx] = ((charInt - keyInt) % 27);
		msg[idx] = intToChar(msg[idx]);
	}
}

void authenticateServer(char c, int socketFD)
{
	// TODO: receive a char c from client, 1 byte
	char authBuffer[1] = {'\0'};
	char authChar[1];
	authChar[0] = c;

	int numRead = read(socketFD, authBuffer, 1);
	if (numRead == 0)
	{
		error("SERVER: 0 bytes read from authenticator");
	}
	else if (numRead < 0)
	{
		error("SERVER: error reading from authenticator");
	}
	// TODO: check if authenticator byte recieved is correct
	if (authChar[0] == c)
	{
		// TODO: if correctly received authenticator char, send it back to client
		int numSent = write(socketFD, authChar, 1);
	}
	else
	{
		// TODO: wrong authenticator received, send something else back
		char l[1] = "*";
		int numSent = write(socketFD, l, 1);
	}
}

int authenticateClient(char c, int socketFD)
{
	char authBuffer[1] = {'\0'};
	char authChar[1];
	authChar[0] = c;

	// TODO: send authenticator character to server
	int numSent = write(socketFD, authChar, 1);

	// TODO: check if authenticator character received is correct
	int numRead = read(socketFD, authBuffer, 1);
	if (authChar[0] != c)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
