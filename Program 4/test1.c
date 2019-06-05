#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(int argc, char *argv[])
{

	FILE *f = fopen("plaintext4", "r");
	int counter = 0;
	int innercounter = 0;
	char buffer[70000];
	bzero(buffer, 70000);
	int fd = open("filedescript", O_TRUNC | S_IRUSR | S_IWUSR | O_RDWR);
	if(fd < 0){
		perror("error\n");
		exit(1);
	}
	int length;
	int numbytes;
	while ((length = fread(buffer, sizeof(char), 70000, f)) > 0)
	{
		if((numbytes = write(fd, buffer, length)) < 0){
			break;
		}
		bzero(buffer, 70000);
		counter++;
	};
	// printf("%s", buffer);
	return 0;
}










