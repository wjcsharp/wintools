/**
 * System Info v1.0 
 * @file
 *
 * 05-29-2008:	JPH	- Created.
 *
 * @author Jacob Hammack
 */
 
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
 
#define BUFFER_LEN 256
 
static BOOL GetOSInfo(TCHAR OSName[BUFFER_LEN], int OSNameLength, TCHAR OSServicePack[BUFFER_LEN], int OSServicePackLength);
 
int _tmain(int argc, TCHAR *argv[])
{
	TCHAR OSName[BUFFER_LEN];
	TCHAR OSServicePack[BUFFER_LEN];
	
	ZeroMemory(&OSName, sizeof(TCHAR));
	ZeroMemory(&OSServicePack, sizeof(TCHAR));
	
	GetOSInfo(OSName, BUFFER_LEN, OSServicePack, BUFFER_LEN);
	
	_tprintf(TEXT("Operating System: %s %s\n"), OSName, OSServicePack);
	
	free(OSName);
	free(OSServicePack);
	
	return 0;
}

static BOOL GetOSInfo(TCHAR OSName[BUFFER_LEN], int OSNameLength, TCHAR OSServicePack[BUFFER_LEN], int OSServicePackLength)
{
	OSVERSIONINFOEX OSVersionInfo;
	
	ZeroMemory(&OSVersionInfo, sizeof(OSVERSIONINFOEX));
	OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			
	if(!GetVersionEx((OSVERSIONINFO *) &OSVersionInfo))
	{
		return FALSE;
	}	
	
	if(OSVersionInfo.dwMajorVersion == 5)
	{
		if(OSVersionInfo.dwMinorVersion == 0)
		{
			_tcscpy_s(OSName, OSNameLength, TEXT("Windows 2000"));
		}
		else if(OSVersionInfo.dwMinorVersion == 1)
		{
			_tcscpy_s(OSName, OSNameLength, TEXT("Windows XP"));
		}
		else if(OSVersionInfo.dwMinorVersion == 2)
		{
			_tcscpy_s(OSName, OSNameLength, TEXT("Windows 2003"));
		}
	}
	else if(OSVersionInfo.dwMajorVersion == 6)
	{
		
	}
	
	_tcscpy_s(OSServicePack, OSServicePackLength, OSVersionInfo.szCSDVersion);
	
	return TRUE;
}
