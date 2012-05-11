/**
 * Binaudit v0.0.1
 * @file
 *
 * 02-20-2012:	JPH	- Created.
 *
 * @author Jacob Hammack <jacob.hammack@hammackj.com>
 */
 
#include <windows.h>
#include <stdio.h>

const TCHAR VERSION[] = TEXT("0.0.1");
const TCHAR APPLICATION_NAME[] = TEXT("binaudit");

#pragma comment(lib, "kernel32.lib")

bool is_pe(char *filename)
{
	return false;
}

/**
 * Prints out the first 10 bytes of a file which contains the magic number.
 *
 * @param FileToBeProcessed File handle that will be used.
 *
 * @author Jacob Hammack Tasc Inc.
 *
 * @todo update documentation for this function
 */
static void GetMagicNumber(HANDLE FileToBeProcessed, char *magic_number)
{
	DWORD BytesRead;
	char Buffer[MAX_MAGIC_NUMBER] = {0};

	BytesRead = 0;

	SetFilePointer(FileToBeProcessed, (long) NULL, (long) NULL, FILE_BEGIN);

	if(ReadFile(FileToBeProcessed, Buffer, MAX_MAGIC_NUMBER, &BytesRead, NULL) == FALSE)
	{
		return;
	}

	if(BytesRead == 0)
	{
		return;
	}

	memcpy_s(magic_number, MAX_MAGIC_NUMBER, Buffer, MAX_MAGIC_NUMBER);

	return;	
}


void usage(void)
{
	_ftprintf(stderr, TEXT("%s v%s\nJacob Hammack\nhttp://www.hammackj.com\n\n"), APPLICATION_NAME, VERSION);
	_ftprintf(stderr, TEXT("Usage: %s [options]\n"), APPLICATION_NAME);

}


/**
 * Main entry point for the binaudit Application.
 *
 * @author Jacob Hammack
 */
int _tmain(int argc, TCHAR *argv[])
{

	
	return 0;		
}

/*
LPVOID base;
      DWORD header;

      PIMAGE_DOS_HEADER image_dos_header;
      IMAGE_NT_HEADERS image_nt_header;

      HANDLE hFile = CreateFile("C:\\yourexehere.exe", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                                      0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
      HANDLE hMap = CreateFileMapping(hFile, 0, PAGE_READONLY | SEC_COMMIT, 0, 0, 0);
      base = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

      CloseHandle(hMap);
      CloseHandle(hFile);

      image_dos_header = (PIMAGE_DOS_HEADER)base;
      
              if (image_dos_header->e_magic != IMAGE_DOS_SIGNATURE)
      {
              printf("invalid DOS stub!\n");
              getchar();
              UnmapViewOfFile(base);
              return 0; 
      }

      header = (DWORD)base + image_dos_header->e_lfanew;
      memcpy(&image_nt_header, (void*)header, sizeof(IMAGE_NT_HEADERS));
      
      //
      // Your code goes here :P 
      // play with image_nt_header :)
      //

      UnmapViewOfFile(base);
      return 0;
*/
