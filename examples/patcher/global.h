#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>

char szFile[10], szBackupFile[sizeof(szFile) + 4];

BOOL patchFile(DWORD dwOffset, DWORD * dwOriginal[], DWORD * dwPatch[], int nIndex);