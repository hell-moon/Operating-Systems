#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main()
{
	// create a socket
	int networkSocket;
	networkSocket = socket(AF_INET, SOCK_STREAM, 0);

	// specify an address for the socket
	struct sockaddr_in server_address; 
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(networkSocket, (struct sockaddr *) &server_address, sizeof(server_address));
	if(connection_status == -1){
		printf("There was an error making a connection to remote socket\n\n");
	}

	char server_response[256];
	rcev(networkSocket, &server_response, sizeof(server_response), 0);

	printf("the server sent the dats: %s", server_response);

	close(networkSocket);

	return 0;
}
