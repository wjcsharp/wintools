/**
 * MD5Search v1.0
 * @file
 *
 * 07-02-2008:	JPH	- Created.
 *
 * @author Jacob Hammack
 */
 
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>

#include "md5.h"

static void EnumerateFiles();
static void RecurseFileSystem(TCHAR *StartingPath);
static void CheckHashes(TCHAR *filename);

TCHAR HashToFind[32];

/**
 * Main entry point for MD5Search.
 *
 * @author Jacob Hammack
 *
 */
int _tmain(int argc, TCHAR *argv[])
{
	printf("MD5Search v1.0\nCopyright 2008 Jacob Hammack\nhttp://www.hammackj.com\n\n");

	if(argc != 2) {
	
		printf("Usage: md5search hash\n");
		
		return 1;
	}
	
	_tcscpy(HashToFind, argv[1]);
	printf("[*] Searching....\n");
	
	EnumerateFiles();
	

	printf("[!] Search Complete\n");

	return 0;		
}

/**
 * Function to enumerate all drives on the system and then recurse it.
 *
 * @see RecurseFileSystem()
 *
 * @author Jacob Hammack
 */
static void EnumerateFiles()
{
	TCHAR DriveLetter[MAX_PATH];
	TCHAR *pch;
	GetLogicalDriveStrings(MAX_PATH, DriveLetter);
	pch = DriveLetter;

	while (*pch) 
	{
		if(*pch == 0)
		{
			break;
		}

		if(GetDriveType(pch) == DRIVE_FIXED)
		{
			TCHAR tmp[MAX_PATH];
			_tcsncpy_s(tmp, _tcslen(pch), pch, lstrlen(pch) -1);
			RecurseFileSystem(tmp);
		}

		pch = &pch[lstrlen(pch) + 1];
	}	
}

/**
 * Processes each file on the remote systemRecursive Function for listing all files on the system.
 *
 * @param path Path to search.
 *
 * @author Jacob Hammack
 */
static void RecurseFileSystem(TCHAR *StartingPath)
{
	HANDLE CurrentFileHandle;
	WIN32_FIND_DATA FileInformation;
	TCHAR CurrentFileName[MAX_PATH];
	TCHAR m_szFolderInitialPath[MAX_PATH];
	TCHAR wildCard[MAX_PATH] = TEXT("\\*.*");

	_tcscpy_s(CurrentFileName, MAX_PATH, StartingPath);
	_tcscpy_s(m_szFolderInitialPath, MAX_PATH, StartingPath);
	_tcsncat_s(m_szFolderInitialPath, MAX_PATH, wildCard, MAX_PATH);

	CurrentFileHandle = FindFirstFile(m_szFolderInitialPath, &FileInformation);

	if(CurrentFileHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if((_tcscmp( FileInformation.cFileName, TEXT(".") ) != 0) && (_tcscmp( FileInformation.cFileName, TEXT("..") ) != 0))
			{
				_tcscpy_s(CurrentFileName, MAX_PATH, StartingPath);
				_tcsncat_s(CurrentFileName, MAX_PATH, TEXT("\\"), MAX_PATH);
				_tcsncat_s(CurrentFileName, MAX_PATH, FileInformation.cFileName, MAX_PATH);

				if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					RecurseFileSystem(CurrentFileName);
				}
				else
				{
					CheckHashes(CurrentFileName);
				}
			}
		}
		while(FindNextFile(CurrentFileHandle, &FileInformation) == TRUE);

		FindClose(CurrentFileHandle);
	}
}

/*
 *
 *
 */
static void CheckHashes(TCHAR *filename)
{
	FILE *FileToHash;
	MD5_CTX MDContext;
	int bytes;
	int i;
	unsigned char data[1024];
	unsigned char hash[32];
	unsigned char tmphash[10];
	errno_t err;
	
	err = _tfopen_s(&FileToHash, filename, TEXT("rb"));
	
	if((err != 0) || (FileToHash == NULL))
	{
		return;
	}
	
	MD5Init(&MDContext);
	
	while((bytes = fread(data, 1, 1024, FileToHash)) != 0)
	{
		MD5Update(&MDContext, data, bytes);
	}
	
	fclose(FileToHash);
	MD5Final(&MDContext);
	
	sprintf(hash, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
	MDContext.digest[0],  MDContext.digest[1],  MDContext.digest[2],  MDContext.digest[3],  
	MDContext.digest[4],  MDContext.digest[5],  MDContext.digest[6],  MDContext.digest[7],  
	MDContext.digest[8],  MDContext.digest[9],  MDContext.digest[10],  MDContext.digest[11], 
	MDContext.digest[12],  MDContext.digest[13],  MDContext.digest[14],  MDContext.digest[15]);
	
	
	if(strcmp(hash, HashToFind) == 0)
	{
		_tprintf("[$] %s\n", filename);
	}
	
}
