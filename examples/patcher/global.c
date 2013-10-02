#include "globals.h"

BOOL patchFile(DWORD dwOffset, DWORD * bOriginal[], DWORD * bPatch[], int nIndex)
{
	int i;
	HANDLE hFile;
	DWORD dwRead, dwError, * dwFoundBytes, dwWritten;
	dwFoundBytes = (DWORD *)malloc(nIndex / sizeof(DWORD));

	printf("\nOffset: 0x%x\n", dwOffset);

	printf("Original byte(s): ");
	for(i = 0; i < nIndex / sizeof(DWORD); i++)
		printf("0x%x ", bOriginal[i]);

	printf("\nNew byte(s)     : ");
	for(i = 0; i < nIndex / sizeof(DWORD); i++)
		printf("0x%x ", bOriginal[i]);

	printf("\n\nOpening file...");
	hFile = CreateFile(szFile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);

    if(hFile == INVALID_HANDLE_VALUE)
	{
		printf("\n\nError while accessing the file!\nAborting...\n");
        CloseHandle(hFile);

        return FALSE;
    }
	printf("\tDone!");

	printf("\nReading original bytes...");
	dwError = SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
	if(dwError == INVALID_SET_FILE_POINTER)
	{
		printf("\n\nError occured!");
		return FALSE;
	}

    if(ReadFile(hFile, dwFoundBytes, (DWORD)nIndex / sizeof(DWORD), &dwRead, NULL))
		printf("\tDone!");
	else 
	{
		printf("\tError reading file!");
		return FALSE;
	}

	if(memcmp(dwFoundBytes, bOriginal, (DWORD)nIndex / sizeof(DWORD)) == 0)
		printf("\n\nEverything's ok!\n");
	else if(memcmp(dwFoundBytes, bPatch, (DWORD)nIndex / sizeof(DWORD)) == 0)
	{
		printf("\n\nFile already patched!");
		return FALSE;
	}

	dwError = SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
	if(dwError == INVALID_SET_FILE_POINTER)
	{
		printf("\n\nError occured!");
		return FALSE;
	}

    if(WriteFile(hFile, bPatch, (DWORD)nIndex / sizeof(DWORD), &dwWritten, NULL))
		printf("\nPatched successfully!\nFile: %s\nNumber of bytes patched: %x/%x", szFile, dwWritten, (DWORD)nIndex / sizeof(DWORD));
    else
		printf("\nCouldn't apply patch!?");
    
	return TRUE;
}