#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

typedef enum
{
	false,
	true
} bool;

// function declarations for int array randomizer
// from https://www.geeksforgeeks.org/shuffle-a-given-array-using-fisher-yates-shuffle-algorithm/
void swap(int *, int *);
void printArray(int[], int);
void randomize(int[], int);

struct rooms
{
	char *name;
	struct rooms *outboundConnections[6];
	int numConnections;
	char *roomtype;
};

bool IsGraphFull(struct rooms *[]);
void AddRandomConnection(struct rooms *[]);
struct rooms *GetRandomRoom();
bool CanAddConnectionFrom(struct rooms *);
bool ConnectionAlreadyExists(struct rooms *, struct rooms *);
void ConnectRoom(struct rooms *, struct rooms *);
bool IsSameRoom(struct rooms *, struct rooms *);
void makerooms(struct rooms*[]);


int main()
{
	// indexes for looping 
	int idx = 0;
	int jdx = 0;
	// make new directory according to assignment
	char dirName[25];
	memset(dirName, '\0', sizeof(dirName));
	sprintf(dirName, "kuangx.rooms.%d", getpid());
	mkdir(dirName, 0700); 	// set full permissions for owner
	// declare all rooms as structs
	struct rooms startRoom;
	struct rooms endRoom;
	struct rooms midroom1;
	struct rooms midroom2;
	struct rooms midroom3;
	struct rooms midroom4;
	struct rooms midroom5;

	// organize rooms in an array
	struct rooms *roomArr[7];
	roomArr[0] = &startRoom;
	roomArr[1] = &endRoom;
	roomArr[2] = &midroom1;
	roomArr[3] = &midroom2;
	roomArr[4] = &midroom3;
	roomArr[5] = &midroom4;
	roomArr[6] = &midroom5;

	// list of room names to choose from
	const char *roomList[10] = {"TRAIN", "LOT", "GRAVES", "MET", "HOLE", "BUTT", "FACE", "EVIL", "ILL", "DYNA"};
	// determine random order of room names
	int random10[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int n = sizeof(random10) / sizeof(random10[0]);
	randomize(random10, n);
	// random10[] is now randomized, first 7 items will be used as indexes of roomList to get random rooms

	// assign room names and types
	for (idx = 0; idx < 7; idx++)
	{
		// allocate memory for the name member to point to
		roomArr[idx]->name = calloc(8, sizeof(char));
		// copy randomized names to rooms in the order assigned into roomArr[]
		strcpy(roomArr[idx]->name, roomList[random10[idx]]);
		// allocate memory for room type member to point to
		roomArr[idx]->roomtype = calloc(11, sizeof(char));
		// set number of connections of each room to 0
		roomArr[idx]->numConnections = 0;
		// set room types to rooms in the order assigned into roomArr[]
		if (idx == 0)
		{
			strcpy(roomArr[0]->roomtype, "START_ROOM");
		}
		else if (idx == 1)
		{
			strcpy(roomArr[1]->roomtype, "END_ROOM");
		}
		else
		{
			strcpy(roomArr[idx]->roomtype, "MID_ROOM");
		}
	}
	// all rooms have names and types, need to randomly assign connections now

	// create all connections
	makerooms(roomArr);

	// using room information, make new files in directory created above
	chdir(dirName);		// open new directory from above
	// loop through all rooms and for each room, open a new file stream and append name, list of connections, and room type
	for(idx = 0; idx < 7; idx++){
		// use fopen() instead of open() to use stdio library
		FILE* openFile = fopen(roomArr[idx]->name, "a");	// variable openFile points to file named the room name
		// append room name to file
		fprintf(openFile, "ROOM NAME: %s\n", roomArr[idx]->name);
		// loop through all connections and print each connection name in a list
		for(jdx = 0; jdx < roomArr[idx]->numConnections; jdx++){	// print all connection names except last one for formatting
			fprintf(openFile, "CONNECTION %d: %s\n",(jdx+1) ,roomArr[idx]->outboundConnections[jdx]->name);
		}
		// print room type
		fprintf(openFile, "ROOM TYPE: %s", roomArr[idx]->roomtype);
	}

	// free allocated memory
	for (idx = 0; idx < 7; idx++)
	{
		// free memory for the name member to point to
		free(roomArr[idx]->name);
		// free memory for room type member to point to
		free(roomArr[idx]->roomtype);
	}	
	
	return 0;
}

// C Program to shuffle a given array, sourced from https://www.geeksforgeeks.org/shuffle-a-given-array-using-fisher-yates-shuffle-algorithm/
// A utility function to swap to integers
void swap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

// A utility function to print an array
void printArray(int arr[], int n)
{int i;
	for ( i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");
}

// A function to generate a random permutation of arr[]
void randomize(int arr[], int n)
{
	// Use a different seed value so that we don't get same
	// result each time we run this program
	srand(time(NULL));

	// Start from the last element and swap one by one. We don't
	// need to run for the first element that's why i > 0
	int i;
	for (i = n - 1; i > 0; i--)
	{
		// Pick a random index from 0 to i
		int j = rand() % (i + 1);

		// Swap arr[i] with the element at random index
		swap(&arr[i], &arr[j]);
	}
}
bool IsGraphFull(struct rooms *X[])
{
	bool isFull = false;
	int counter = 0;
	int idx;
	for (idx = 0; idx < 7; idx++)
	{
		if (X[idx]->numConnections <= 6 && X[idx]->numConnections >= 3)
		{
			counter++;
		}
	}
	if(counter == 7){
		isFull = true;
	}
	return isFull;
}

void AddRandomConnection(struct rooms *X[])
{
	struct rooms *A;
	struct rooms *B;

	while (true)
	{
		A = GetRandomRoom(X);

		if (CanAddConnectionFrom(A) == true)
		{
			break;
		}
	}

	do
	{
		B = GetRandomRoom(X);
	} while (CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

	ConnectRoom(A, B);
}

struct rooms *GetRandomRoom(struct rooms *X[])
{
	int random7[7] = {0, 1, 2, 3, 4, 5, 6};
	int n = sizeof(random7) / sizeof(random7[0]);
	randomize(random7, n); // random7 is now randomized

	// get a random pointer to room from roomArr
	return X[random7[0]];
}

bool CanAddConnectionFrom(struct rooms *X)
{
	// check to see if numConnections is less than 6
	if (X->numConnections < 6)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ConnectionAlreadyExists(struct rooms *X, struct rooms *Y)
{
	int idx;
	bool exists = false;
	for (idx = 0; idx < X->numConnections; idx++)
	{
		// check if any of the connections in X point to Y
		if (X->outboundConnections[idx] == Y)
		{
			exists = true;
		}
	}
	return exists;
}

void ConnectRoom(struct rooms *X, struct rooms *Y)
{
	// connects room x to y and y to x
	X->outboundConnections[X->numConnections] = Y;
	X->numConnections++;
	Y->outboundConnections[Y->numConnections] = X;
	Y->numConnections++;
}

bool IsSameRoom(struct rooms *X, struct rooms *Y)
{
	if (X == Y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void makerooms(struct rooms *X[])
{
	while (IsGraphFull(X) == false)
	{
		AddRandomConnection(X);
	}
}
