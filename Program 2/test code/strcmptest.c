#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int my_strncmp(char *str1, char *str2, size_t max)
{
    size_t n = 0;
    while (n < max)
    {
         if ((str1[n] != str2[n]))
             return 0;	//fail
         n++;
    }
    return 1;
}
struct rooms
{
	char *name;
	int outboundConnections[6];
	int numConnections;
	char *roomtype;
};



// char* hardCodedRoomNames[10] = {"TRAIN", "LOT", "GRAVES", "TRAX", "HOLE", "BUTT", "FACE", "EVIL", "DOGOOD", "DYNA"};

const char* hardCodedRoomNames[] = {"TRAIN", "LOT", "GRAVES", "TRAX", "HOLE", "BUTT", "FACE", "EVIL", "DOGOOD", "DYNA"};
char *roomTypesList[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};



int main(){
	// variables
	int idx, jdx = 0;
	char* currentRoom = calloc(8, sizeof(char));
	int currentRoomIdx = -1; // index of current room in roomArr
	int history[50] = {-1};
	int steps = 0;
	char* userInput = calloc(8, sizeof(char));
	int endRoomIdx = -1; // index of end room in roomArr
	int gameOverFlag = -1;

	struct rooms* roomArr[7];
	for(idx = 0; idx < 7; idx++){
		roomArr[idx] = malloc(sizeof(struct rooms));
		roomArr[idx]->name = calloc(8, sizeof(char));
	}


	strcpy(roomArr[0]->name, hardCodedRoomNames[0]);	//train
	scanf("%s", userInput);								//type train

	if(my_strncmp(userInput, roomArr[0]->name, 3) == 1){
		printf("Same rooms");
	}else{
				printf("NOT Same rooms");
	}
	
	return 0;
}
