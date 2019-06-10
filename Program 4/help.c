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

int main(int argc, char* argv[]){
		int plainLength = -5;
	int keyLength = -5;
	int optvalue = 1;
	char *plaintext = NULL;
	char *keytext = NULL;
	char *cyphertext = NULL;
	char authChar = '!';
	int failed = 0;
	int idx = 0;

	readFileToBuffer(argv[1], &plainLength, &plaintext);
	readFileToBuffer(argv[2], &keyLength, &keytext);
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
			failed = 1;
			if(failed == 1){
		printf("it failed, index: %d", idx);
	}
			break;
		}
	}

	

	return 0;


}
