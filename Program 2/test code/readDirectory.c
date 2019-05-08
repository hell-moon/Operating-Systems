#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void main()
{
	int newestDirTime = -1; // The modified timestamp of the newest subdir we've examined
	char targetDirPrefix[32] = "brewsteb.rooms."; // The prefix we're looking for for all subdirs
	char newestDirName[256]; // Holds the name of the newest directory that contains the prefix
	memset(newestDirName, '\0', sizeof(newestDirName));

	DIR* dirToCheck; // Holds the directory we're starting in
	struct dirent *fileInDir; // Holds the current subdir of the starting dir we're looking at
	struct stat dirAttributes; // Holds information we've gained about subdir
	
	dirToCheck = opendir("."); // Open up the directory this program was run in

	if (dirToCheck > 0) // Make sure the current directory could be opened
	{ 
		while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in the directory
		{
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has the prefix
			{
				printf("Found the prefex: %s\n", fileInDir->d_name);
		                stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry
				
				if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
				{
					newestDirTime = (int)dirAttributes.st_mtime;
					memset(newestDirName, '\0', sizeof(newestDirName));
					strcpy(newestDirName, fileInDir->d_name);
					printf("Newer subdir: %s, new time: %d\n", fileInDir->d_name, newestDirTime);
				}
			}
		} 
	}   		

	closedir(dirToCheck); // Close the directory we opened

	printf("Newest entry found is: %s\n", newestDirName);
}
