#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
//refactor
#define h_addr h_addr_list[0]

void error(const char *msg)
{
	perror(msg);
	exit(0);
} // Error function used for reporting issues

void sendfile(char *plaintextfile, int socketFD)
{
	FILE *plaintext = fopen(plaintextfile, "r");
	int length = -5;
	char buffer[70000];
	int charsWritten = -5;

	if (plaintext == NULL)
	{
		// error
		perror("ERROR OPENING FILE\n");
	}
	else
	{
		//get length of file read by pointing file descriptor at end
		fseek(plaintext, 0, SEEK_END);
		length = ftell(plaintext);
		// length--;
		// reset file descriptor to beginning to read file
		fseek(plaintext, 0, SEEK_SET);
		if (buffer)
		{
			fread(buffer, 1, length, plaintext);
		}
		fclose(plaintext);
		buffer[length - 1] = '@';	//replace '/n' with '@'
		// buffer[length] = '@';
	}

	// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	// printf("chars written: %d", charsWritten);
	if (charsWritten < 0)
		error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer))
		printf("CLIENT: WARNING: Not all data written to socket!\n");
	//	fclose(plaintext);
}

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent *serverHostInfo;
	char buffer[70000] = {'\0'};
	int length;
	char *plaintextfile = argv[1];
	char *key = argv[2];
	char authenticate[] = "*";

	if (argc < 4)
	{
		fprintf(stderr, "USAGE: %s plaintext key port\n", argv[0]);
		exit(0);
	} // Check usage & args

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
		error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// Get input message from user
	// printf("CLIENT: Enter text to send to the server, and then hit enter: ");
	// memset(buffer, '\0', sizeof(buffer));	 // Clear out the buffer array
	// fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	// buffer[strcspn(buffer, "\n")] = '\0';	 // Remove the trailing \n that fgets adds
	// open file with same name passed in argv[1]

	// authenticate with server, make sure connected to opt_enc_d
	// send a '*' to opt_enc_d
	charsWritten = send(socketFD, authenticate, 1, 0); // Write to the server
	if (charsWritten < 1)
	{
		error("Error sending authentication to server\n");
	}
	else
	{
		memset(buffer, '\0', length); // Clear out the buffer again for reuse
		charsRead = recv(socketFD, buffer, 1, 0);
		if (charsRead < 1)
		{
			error("Error receiving authentication from server\n");
		}
		else
		{
			if (buffer[0] == authenticate[0])
			{
			}
			else
			{
				fprintf(stderr, "Can't authenticate server, port %d", portNumber);
				exit(2);
			}
		}
	}

	sendfile(plaintextfile, socketFD);

	sendfile(key, socketFD);

	// Get return message from server
	memset(buffer, '\0', length); // Clear out the buffer again for reuse

	char *endMessagePtr = NULL;
	char cyphertext[70000] = {'\0'};

	// while ((endMessagePtr = strstr(cyphertext, "@")) == NULL)
	// {
	// 	memset(buffer, '\0', 70000);
	// 	charsRead = recv(socketFD, buffer, 70000, 0); // Read the client's message from the socket
	// 	strncat(cyphertext, buffer, charsRead);
	// 	// printf("chars read plaintxt: %d", charsRead);
	// 	if (charsRead < 1)
	// 	{
	// 		if (charsRead == 0)
	// 		{
	// 			error("charsRead: 0\n");
	// 		}
	// 		else
	// 		{
	// 			error("poopERROR reading from socket");
	// 		}
	// 	}
	// }


	// replace '@' at end of cyphertext received with '\0'
	// int position = endMessagePtr - cyphertext;
	// cyphertext[position] = '\0';

	// charsRead = recv(socketFD, buffer, length - 2, 0); // Read data from the socket, leaving \0 at end
	// if (charsRead < 0)
	// 	error("CLIENT: ERROR reading from socket");

	//printf("%s\n", cyphertext);

	close(socketFD); // Close the socket
	// free(buffer);
	// fclose(plaintext);
	return 0;
}
