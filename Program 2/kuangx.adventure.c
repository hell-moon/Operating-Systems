#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

struct rooms
{
	char *name;
	int outboundConnections[6];
	int numConnections;
	char *roomtype;
};

pthread_mutex_t mutex;

const char *hardCodedRoomNames[] = {"TRAIN", "LOT", "GRAVES", "MET", "HOLE", "BUTT", "FACE", "EVIL", "ILL", "DYNA"};
char *roomTypesList[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};

// function declarations
char *newestDirectory();
void createRooms(struct rooms *[], char *);
void playGame(struct rooms *[]);
int my_strncmp(char *, char *, size_t);
void writeTime();
void printTime();


int main()
{
	int idx;
	// initialize mutex
	struct rooms *roomArr[7];
	for (idx = 0; idx < 7; idx++)
	{
		roomArr[idx] = malloc(sizeof(struct rooms));
		roomArr[idx]->name = calloc(8, sizeof(char));
	}
	char *dir = newestDirectory();
	printf("newest directory is: %s\n", dir);
	createRooms(roomArr, dir);
	// char* name = calloc(8, sizeof(char));
	// strcpy(name, roomArr[0].name);
	// printf("Name is: %s", name);
	// free(name);
	
	playGame(roomArr);
	// free allocated memory
	for (idx = 0; idx < 7; idx++)
	{
		free(roomArr[idx]->name);
		free(roomArr[idx]);
	}
	// unlock and stop threading before program exits
	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
	return 0;
}

// parse info from files opened in newestDirName to create rooms structs with names, connections, and room types, store in passed struct rooms roomsArr[]
void createRooms(struct rooms *roomArr[], char *newestDirName)
{
	// loop iterators
	int idx, jdx = 0;

	// variables for I/O files and directories
	FILE *myFile;
	DIR *fileDir = opendir(newestDirName);

	struct dirent *dirEntry;
	char fileName[50];
	char currentline[50]; //holds the current line of text

	// counters
	int roomArrIterator = 0;
	int numLines = 0; // counter for number of lines in file

	// variables to store string tokens used to compare and create rooms
	char trash1[20], trash2[20]; //stores trash
	char cName[20] = {'\0'};	 //stores connection names
	char rType[20] = {'\0'};	 //stores roomtype
	char rName[20] = {'\0'};	 //stores room name

	// initialize numConnections
	for (idx = 0; idx < 7; idx++)
	{
		roomArr[idx]->numConnections = 0;
	}

	// keep reading in directory entries until it returns NULL (finished)
	while ((dirEntry = readdir(fileDir)) != NULL)
	{
		// reset line counter
		numLines = 0;

		// each directory has . and .. for current and previous directories.  IGNORE
		if (!strcmp(dirEntry->d_name, "..") || !strcmp(dirEntry->d_name, "."))
		{
			continue; // IGNORE
		}

		// store path in fileName
		sprintf(fileName, "./%s/%s", newestDirName, dirEntry->d_name);
		// use fopen to use stdio library, open as read
		myFile = fopen(fileName, "r");

		// count number of lines to figure out how many connections(# connections = total lines - 2)
		while (fgets(trash1, 50, myFile))
		{
			numLines++;
		}
		// file descriptor pointer is at end of file. Close and reopen to reset file descriptor pointer
		fclose(myFile);

		myFile = fopen(fileName, "r"); //File descriptor pointer at beginning of file, open for read
		fgets(currentline, 50, myFile);

		sscanf(currentline, "%s %s %s", trash1, trash2, rName); // parse and store name token
		// store name
		for (idx = 0; idx < 10; idx++)
		{
			if (strcmp(rName, hardCodedRoomNames[idx]) == 0)
			{
				strcpy(roomArr[roomArrIterator]->name, hardCodedRoomNames[idx]);
			}
		}

		// File descriptor pointer starting at 2nd Line and stop @ last line
		for (idx = 0; (idx < numLines - 2); idx++)
		{
			fgets(currentline, 50, myFile);
			sscanf(currentline, "%s %s %s", trash1, trash2, cName); // parse and store connection name token

			// set connection to index of hardcodedRoomList
			for (jdx = 0; jdx < 10; jdx++)
			{
				if (strcmp(cName, hardCodedRoomNames[jdx]) == 0)
				{
					roomArr[roomArrIterator]->outboundConnections[idx] = jdx;
				}
			}
			//inc number of connections
			roomArr[roomArrIterator]->numConnections++;
		}

		// File descriptor pointer now at last line
		fgets(currentline, 50, myFile);
		sscanf(currentline, "%s %s %s", trash1, trash2, rType); // parse and store room type token

		// set room type
		for (idx = 0; idx < 3; idx++)
		{
			if (strcmp(rType, roomTypesList[idx]) == 0)
			{
				roomArr[roomArrIterator]->roomtype = roomTypesList[idx];
			}
		}
		// good practice to close
		fclose(myFile);
		roomArrIterator++;
	}
	// done going through all files in directory
	closedir(fileDir);
	free(newestDirName);
}

char *newestDirectory()
{
	// set variable to hold newest time
	int newestDirTime = 0;
	// set current path variable to loop
	// const char *current = ".";
	DIR *checkDir = opendir(".");

	// set variable to hold newest directory path name
	char *recent = calloc(25, sizeof(char));

	// keep reading directories until readdir() returns NULL
	// readdir() returns next entry in directory as a dirent* struct pointer
	// store that pointer in a dirent struct for analyzing
	struct dirent *dirEntry;
	// store stats info in stat struct*
	struct stat entryAttributes;
	while ((dirEntry = readdir(checkDir)) != NULL)
	{
		stat(dirEntry->d_name, &entryAttributes);
		// if the directory entry is newest so far:
		if ((int)entryAttributes.st_mtime > newestDirTime)
		{ // cast as int to compare
			newestDirTime = (int)entryAttributes.st_mtime;
			// clear recent variable and set it to dirEntry's name
			memset(recent, '\0', sizeof(recent));
			strcpy(recent, dirEntry->d_name);
		}
	}
	// good practice to close
	closedir(checkDir);
	return recent;
}



void playGame(struct rooms *roomArr[])
{
	// variables
	int idx, jdx, kdx = 0;
	char *currentRoom = calloc(8, sizeof(char));
	int currentRoomIdx = -1; // index of current room in roomArr
	int history[50] = {-1};
	int steps = 0;
	char *userInput = calloc(8, sizeof(char));
	int endRoomIdx = -1; // index of end room in roomArr
	int gameOverFlag = -1;
	char timestr[256];
	pthread_t timeThread1;
	// lock a mutex
	pthread_mutex_lock(&mutex);
	// create thread1
	pthread_create(&timeThread1, NULL, writeTime, NULL);

	// start with starting room, set index of start room corresponding to roomArr
	for (idx = 0; idx < 7; idx++)
	{
		if (strcmp(roomArr[idx]->roomtype, "START_ROOM") == 0)
		{
			currentRoomIdx = idx;
			strcpy(currentRoom, roomArr[idx]->name);
		}
	}
	// find end room's index corresponding to roomArr
	for (idx = 0; idx < 7; idx++)
	{
		if (strcmp(roomArr[idx]->roomtype, "END_ROOM") == 0)
		{
			endRoomIdx = idx;
		}
	}

	// infinite loop, break when end room is reached, also set gameover flag
	while (gameOverFlag == -1)
	{

		// INTERFACE
		// Print current room
		printf("\nCurrent Location: %s", roomArr[currentRoomIdx]->name);
		// Print possible connections:
		printf("\nPOSSIBLE CONNECTIONS: "); // loop through outboundConnections but not the last
		for (idx = 0; idx < (roomArr[currentRoomIdx]->numConnections - 1); idx++)
		{
			printf("%s, ", hardCodedRoomNames[roomArr[currentRoomIdx]->outboundConnections[idx]]);
		}
		printf("%s.", hardCodedRoomNames[roomArr[currentRoomIdx]->outboundConnections[roomArr[currentRoomIdx]->numConnections - 1]]);
		// Print where to
		printf("\nWHERE TO? >");

		// get user input and check if it matches any of the outbound connections of the currentRoom
		scanf("%s", userInput);		
		int matchFlag = -1;
		if (strcmp(userInput, "q") == 0)
		{
			break;
		}
		else if (strcmp(userInput, "time") == 0)
		{
			// unlock mutex
			pthread_mutex_unlock(&mutex);
			pthread_join(timeThread1, NULL);
			printTime();
			// lock mutex again
			pthread_mutex_lock(&mutex);
			matchFlag++;
		}
		// printf("\n");
		// match outboundConnections?
		for (idx = 0; idx < roomArr[currentRoomIdx]->numConnections; idx++)
		{
			if (strcmp(userInput, hardCodedRoomNames[roomArr[currentRoomIdx]->outboundConnections[idx]]) == 0)
			{
				matchFlag++;
				// set currentRoom to userInput
				strcpy(currentRoom, userInput);
				//
				// set current room index corresponding to roomArr[]
				for (jdx = 0; jdx < 7; jdx++)
				{
					if (currentRoom[0] == roomArr[jdx]->name[0])
					{
						matchFlag++;
						currentRoomIdx = jdx;
						break;
					}
				}

				// add index of roomArr[] corresponding to new room to history and increment steps
				history[steps] = currentRoomIdx;
				steps++;
			}
		}

		if (matchFlag == -1)
		{
			// find index of next room
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}

		// check to see if moved to end room
		if (currentRoomIdx == endRoomIdx)
		{
			gameOverFlag++;
			break;
		}
	}

	// check gameover flag; if set, print path and victory message
	if (gameOverFlag != -1)

	{
		printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);

		for (kdx = 0; kdx < steps; kdx++)
		{
			printf("%s\n", roomArr[history[kdx]]->name);
		}
	}
	free(currentRoom);
	free(userInput);
	pthread_join(timeThread1, NULL);
};
// https://linux.die.net/man/3/strftime 
void writeTime(){
	char timeStr[256];
	time_t t = time(NULL);	
	struct tm *temp = localtime(&t); 
	// get time using strftime, formatted 
	memset(timeStr, '\0', sizeof(timeStr));
	strftime(timeStr, sizeof(timeStr), "%I:%M%P, %A, %B %e, %Y", temp);
	
	// store time in currentTime.txt 
	FILE* timeFile = fopen("currentTime.txt", "w");	// open for writing
	fprintf(timeFile, "%s", timeStr);
	fclose(timeFile);
}

void printTime(){

	char *timestr = NULL;
	size_t bufferSize = 0;
	
	//open file for reading
	FILE *timeFile = fopen("currentTime.txt", "r");
	if (timeFile == NULL) {
		printf("ERROR OPENING \"currentTime.txt\"\n");
	}

	getline(&timestr, &bufferSize, timeFile);
	printf("\n%s\n", timestr);
	
	// free and close memory/files
	free(timestr);
	fclose(timeFile); 
}
