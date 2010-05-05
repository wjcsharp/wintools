/**
 * Drivelist v1.0.1
 * @file
 *
 * 05-25-2008:	JPH	- Created.
 * 08-22-2008:	JPH - Added case 0, 1 and 6 to the switch of types.
 * 07-21-2009:	JPH	- Added imports via comments, to ease mingw compiles
 *
 * @author Jacob Hammack
 */
 
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "kernel32.lib")

/**
 * List's and prints all mounted drives and their drive type.
 *
 * @author Jacob Hammack
 */
void ListMountedDrives(void)
{
	char Buffer[MAX_PATH];
	char *DriveLetter;
	int DriveType;
	GetLogicalDriveStrings(MAX_PATH, Buffer);
	DriveLetter = Buffer;
	
	while (*DriveLetter) 
	{
		if(*DriveLetter == 0)
		{
			break;
		}
		
		DriveType = GetDriveType(DriveLetter);
		
		switch(DriveType)
		{
			case 0:
				printf("%s\tUnknown Type\n", DriveLetter);
			break;
				
			case 1:
				printf("%s\tInvalid Root Path\n", DriveLetter);
			break;
			
			case 2:
				printf("%s\tRemoveable Drive\n", DriveLetter);
			break;
		
			case 3:
				printf("%s\tFixed Drive\n", DriveLetter);
			break;
			
			case 4:
				printf("%s\tNetwork Drive\n", DriveLetter);	
			break;
			
			case 5:
				printf("%s\tCD-ROM Drive\n", DriveLetter);
			break;	
			
			case 6:
				printf("%s\tRam Drive\n", DriveLetter);
			break;
		}

		DriveLetter = &DriveLetter[strlen(DriveLetter) + 1];
	}
}

/**
 * Main entry point for the DriveList Application.
 *
 * @author Jacob Hammack
 */
int main(int argc, char *argv[])
{
	printf("Drivelist v1.0.2\nJacob Hammack\nhttp://www.hammackj.com\n\n");
	
	ListMountedDrives();
	
	return 0;		
}
