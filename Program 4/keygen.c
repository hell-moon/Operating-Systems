#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		perror("Incorrect number of arguments\n");
		exit(1);
	}
	int keyLength = atoi(argv[1]);

	if (keyLength <= 0)
	{
		perror("Error with keyLength passed in\n");
		exit(1);
	}

	srand(time(NULL));

	char availableChars[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};

	int idx;
	char *returnString;
	if((returnString = calloc(keyLength + 1, sizeof(char))) == NULL){
		perror("Error with memory allocation\n");
	};
	for(idx = 0; idx < keyLength; idx++){
		int num = (rand()%27);
		returnString[idx] = availableChars[num];
	}
	returnString[keyLength] = '\n';

	printf("%s", returnString);

	free(returnString);
	return 0;
}

