#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
int main(int argc, char **argv)
{
	char buffer[70000] = {'\0'};
	int length = 0;
	int charsRead;
	FILE *plaintext = fopen(argv[1], "r");
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
		// length++;
		// reset file descriptor to beginning to read file
		fseek(plaintext, 0, SEEK_SET);
		// buffer = malloc(length);
		if (buffer)
		{
			 charsRead = fread(buffer, 1, length, plaintext);
		}
		fclose(plaintext);
		buffer[length - 1] = '@';
		buffer[length] = '@';
		printf("%s", buffer);
		// free(buffer);
	}

	// int result;
	// int length = 5562;
	// result = (length/1000);
	// int remainder = length%1000;

	return 0;


}
