
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
bool IsGraphFull(struct rooms **);
void AddRandomConnection(struct rooms **);
struct rooms *GetRandomRoom();
bool CanAddConnectionFrom(struct rooms *);
bool ConnectionAlreadyExists(struct rooms *, struct rooms *);
void ConnectRoom(struct rooms *, struct rooms *);
bool IsSameRoom(struct rooms *, struct rooms *);

struct rooms
{
	char *name;
	struct rooms *outboundConnections[6];
	int numConnections;
	char *roomtype;
};

bool IsGraphFull(struct rooms *X[])
{
	int idx;
	for (idx = 0; idx < 7; idx++)
	{
		if (X[idx]->numConnections <= 6 && X[idx]->numConnections >= 3)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
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
		return false
	};
}

void makerooms(struct rooms *X[])
{
	while (IsGraphFull(X) == false)
	{
		AddRandomConnection(X);
	}
}
int main(){
		struct rooms startRoom;
		struct rooms endRoom;
		struct rooms* roomArr[1];
		roomArr[0] = &startRoom;
		roomArr[1] = &endRoom;
		roomArr[0]->numConnections = 3;
		roomArr[0]->outboundConnections[0] = roomArr[1];

		if(ConnectionAlreadyExists(roomArr[0], roomArr[1]) == true){
			printf("true\n");
		}else{
			printf("false\n");
		}

		return 0;

}
