#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
       #define _GNU_SOURCE

// char** getInputParsed();


// int main(int argc, char* argv[]){
// 	// // getline test
// 	// // read input from stdin, then parse and print each word on new line
// 	// char* buffer = NULL;
// 	// size_t buffer_size = 0;
// 	// ssize_t numCharRead = 0;
// 	// // store tokens in this string array
// 	// char** parsedString = malloc(512*sizeof(char));

// 	// printf("> ");
// 	// if((numCharRead = getline(&buffer, &buffer_size, stdin)) == -1){
// 	// 	perror("error with getline");
// 	// }
// 	// //remove trailing /n from the input
// 	// buffer[numCharRead - 1] = '\0';
// 	// printf("the line read in was: \"%s\".\n", buffer);

// 	// // use strtok to parse string 
// 	// char* token = strtok(buffer, " ");

// 	// int i = 0;
// 	// int counter = 0;
// 	// while(token != NULL){
// 	// 	parsedString[counter] = token;
// 	// 	counter++;
// 	// 	token = strtok(NULL, " ");
// 	// }
// 	// parsedString[counter] = NULL;
// 	// for(i = 0; i < counter; i++){
// 	// 	printf("%s\n", parsedString[i]);
// 	// 	// free(parsedString[i]);
// 	// }
// 	// free(parsedString);
// 	// free(token);
// 	// free(buffer);
// 	char** parsedString = 	getInputParsed();

// 	int idx = 0;
// 	while (parsedString[idx] != NULL){
// 		printf("%s\n", parsedString[idx]);
// 		idx++;
// 	}

// 	return 0;

// }

// This function gets input from stdin, parses the string, and then returns an array of pointers with the separated arguments
char** getInputParsed(){
		// getline test
	// read input from stdin, then parse and print each word on new line
	char* buffer = NULL;
	size_t buffer_size = 0;
	ssize_t numCharRead = 0;
	// store tokens in this string array
	char** parsedString = malloc(512*sizeof(char));

	printf("> ");
	if((numCharRead = getline(&buffer, &buffer_size, stdin)) == -1){
		perror("error with getline");
	}
	//remove trailing /n from the input
	buffer[numCharRead - 1] = '\0';
	printf("the line read in was: \"%s\".\n", buffer);

	// use strtok to parse string 
	char* token = strtok(buffer, " ");

	int i = 0;
	int counter = 0;
	while(token != NULL){
		parsedString[counter] = token;
		counter++;
		token = strtok(NULL, " ");
	}
	parsedString[counter] = NULL;
	return parsedString;
}




// int main(int argc, char* argv[]){
// 	// getline test
// 	// read input from stdin, then parse and print each word on new line
// 	char* buffer = NULL;
// 	size_t buffer_size = 0;
// 	ssize_t numCharRead = 0;
// 	// store tokens in this string array
// 	char** parsedString = malloc(512*sizeof(char));

// 	printf("> ");
// 	if((numCharRead = getline(&buffer, &buffer_size, stdin)) == -1){
// 		perror("error with getline");
// 	}
// 	//remove training /n from the input
// 	buffer[numCharRead - 1] = '\0';
// 	printf("the line read in was: \"%s\".\n", buffer);

// 	// use strtok to parse string 
// 	char* token = strtok(buffer, " ");

// 	int i = 0;
// 	int counter = 0;
// 	while(token != NULL){
// 		parsedString[counter] = token;
// 		counter++;
// 		token = strtok(NULL, " ");
// 	}
// 	parsedString[counter] = NULL;
// 	for(i = 0; i < counter; i++){
// 		printf("%s\n", parsedString[i]);
// 	}


// 	return 0;
// }
