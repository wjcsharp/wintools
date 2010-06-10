/**
 * ADSsearch v1.0
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

static void EnumerateFiles();
static void RecurseFileSystem(TCHAR *StartingPath);
static void CheckAlternateDataStream(TCHAR *filename);

/**
 * Main entry point for MD5Search.
 *
 * @author Jacob Hammack
 *
 */
int _tmain(int argc, TCHAR *argv[])
{
	printf("ADSSearch v1.0\nCopyright 2008 Jacob Hammack\nhttp://www.hammackj.com\n\n");

	/*if(argc != 2) {
	
		printf("Usage: adsearch\n");
		
		return 1;
	}
	
	_tcscpy(HashToFind, argv[1]);*/
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
					CheckAlternateDataStream(CurrentFileName);
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

static void CheckAlternateDataStreamA(TCHAR *filename)
{

}


static void CheckAlternateDataStreamA(TCHAR *filename)
{
	HANDLE FileHandle;
	WIN32_STREAM_ID sid;
	DWORD dwStreamHeaderSize;
	TCHAR wszStreamName[MAX_PATH];
	DWORD dwRead;
	DWORD dw1;
	DWORD dw2;
	BOOL bContinue = TRUE;
	LPVOID lpContext = NULL;
	TCHAR *pwsz;
	
	if((FileHandle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE)
	{
		//_tprintf(TEXT("Error #%d\n"), GetLastError());

		return;	
	}

	do
	{	
		ZeroMemory(&sid, sizeof(WIN32_STREAM_ID));
		ZeroMemory(wszStreamName, MAX_PATH);

		dwStreamHeaderSize = (LPBYTE)&sid.cStreamName - (LPBYTE)&sid+ sid.dwStreamNameSize;

		bContinue = BackupRead(FileHandle, (LPBYTE) &sid, dwStreamHeaderSize, &dwRead, FALSE, FALSE, &lpContext);

		if (!dwRead)
		{
			break;
		}
		if (dwRead != dwStreamHeaderSize)
		{
			break;
		}

		BackupRead(FileHandle,wszStreamName, sid.dwStreamNameSize, &dwRead, FALSE, FALSE, &lpContext);

		if (dwRead != sid.dwStreamNameSize)
		{
			break;
		}

		if(_tcslen(wszStreamName))
		{
			//TCHAR *wp;
			//pwsz = malloc(sizeof(TCHAR) * 260);

			//_tcscpy(pwsz, wszStreamName + sizeof(TCHAR));
			//wp = _tcsstr(pwsz, TEXT(":"));
			
			//pwsz[wp - pwsz] = 0;
			//_tcscpy(wszStreamName, pwsz);
			//delete [] pwsz;

			//free(pwsz);

			_tprintf(TEXT("%s"), filename);
			printf(":%s\n", wszStreamName);
		}

		BackupSeek(FileHandle, sid.Size.LowPart, sid.Size.HighPart, &dw1, &dw2, &lpContext);
	} while(bContinue);

	CloseHandle(FileHandle);
}
