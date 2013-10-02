#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

typedef struct _MAPINFO{
	HANDLE  hFile;
	HANDLE  hFileMapping;
	LPBYTE  lpBuffer;
} MAPINFO, *LPMAPINFO;

int WalkPeIAT(const char *szExePath);
int WalkModuleEAT(const char *szDllPath);
LPMAPINFO LoadFile(LPCTSTR lpszFileName, DWORD dwStubSize);
void UnloadFile(LPMAPINFO lpMapInfo);
void Usage();
DWORD Rva2Offset(DWORD dwRva, PIMAGE_SECTION_HEADER dwSecRva, USHORT uNumOfSecs);

int main(int argc, char **argv)
{
	SetConsoleTitle("- PEWalk, Version 0.2 | illuz1oN 2009");
	printf("\n\n  +++++++++++++++++++++++++++++++++++++++++++++++++");
	printf("\n  +	  PEWalk Version 0.2 - illuz1oN 2009	   +");
	printf("\n  +	Walks IAT / EAT of a dll module / file	 +");
	printf("\n  + Thanks: Rohitab.com, KOrUPt, Irwin & #rohitab +");
	printf("\n  +++++++++++++++++++++++++++++++++++++++++++++++++\n\n");
	if(argc < 3)
		Usage();
	char *g_File = NULL, *g_Ext = NULL;
	bool nDebug = false;
	argv++;
	while(char *ch = *argv++)
	{
		if(!strncmp(ch, "-h", 2))
			Usage();
		if(!strncmp(ch, "-f", 2))
			g_File = *argv++;
		if(!strncmp(ch, "-w", 2))
			g_Ext = *argv++;
	}
	if(g_File != NULL)
	{
		if(!strcmp(g_Ext, "eat"))
		{
			int nFunc = WalkModuleEAT(g_File);
			if(nFunc == 0)
				printf("\n  + (EAT) No Found: Error Occured!\n\n");
			else
				printf("\n  + (EAT) Functions Found: %d\n\n", nFunc);
		}
		else
		{
			int nFunc = WalkPeIAT(g_File);
			if(nFunc == 0)
				printf("\n  + (IAT) No Found: Error Occured!\n\n");
			else
				printf("\n  + (IAT) Functions Found: %d\n\n", nFunc);
		}
	}
	return 0;
}

int WalkPeIAT(const char *szExePath) // Thanks KOrUPt for helping me.
{
	int nFunctions = 0;
	LPMAPINFO lpMapInfo;
	if((lpMapInfo = LoadFile(szExePath, 0)))
	{
		PIMAGE_NT_HEADERS pNtHeaders = ImageNtHeader(lpMapInfo->lpBuffer);
		if(pNtHeaders)
		{
			if(IMAGE_FIRST_SECTION(pNtHeaders) != NULL)
			{
				PIMAGE_DATA_DIRECTORY pDataDir = pNtHeaders->OptionalHeader.DataDirectory + IMAGE_DIRECTORY_ENTRY_IMPORT;
				if(pDataDir->Size)
				{
					PIMAGE_IMPORT_DESCRIPTOR pImpDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE *)lpMapInfo->lpBuffer + Rva2Offset(pDataDir->VirtualAddress, IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections));
					while(pImpDescriptor->Name)
					{
						const char *szModule = (const char *)((BYTE *)lpMapInfo->lpBuffer + Rva2Offset(pImpDescriptor->Name, IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections));						PIMAGE_THUNK_DATA pThunkData = (PIMAGE_THUNK_DATA)((BYTE *)lpMapInfo->lpBuffer + Rva2Offset(pImpDescriptor->FirstThunk, IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections));
						printf("  + Module: %s\n\n", szModule);
						PIMAGE_THUNK_DATA pOrigThunkData = (PIMAGE_THUNK_DATA)((BYTE *)lpMapInfo->lpBuffer + Rva2Offset(pImpDescriptor->OriginalFirstThunk, IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections));
						do
						{
							if(pOrigThunkData->u1.Ordinal & 0x80000000)
								continue;
							PIMAGE_IMPORT_BY_NAME pImportName = (PIMAGE_IMPORT_BY_NAME)((BYTE *)lpMapInfo->lpBuffer + Rva2Offset(pOrigThunkData->u1.AddressOfData, IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections));
							char *szFunc = (char *)pImportName->Name;
							DWORD szFuncAddress = (DWORD)((BYTE)lpMapInfo->lpBuffer + Rva2Offset(pThunkData->u1.Function, IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections));
							printf("  + Function: %s,  Address: 0x%08X\n", szFunc, szFuncAddress);
							pOrigThunkData++;
							pThunkData++;
						} while(pOrigThunkData->u1.Function != 0 && nFunctions++);
						printf("\n");
						pImpDescriptor++;
					}
				}
			}
		}
		UnloadFile(lpMapInfo);
		return nFunctions;
	}
	return 0;
}

int WalkModuleEAT(const char *szDllPath)
{
	int nFunctions = 0;
	LPMAPINFO lpMapInfo;
	if((lpMapInfo = LoadFile(szDllPath, 0)))
	{
		PIMAGE_NT_HEADERS pNtHeaders = ImageNtHeader(lpMapInfo->lpBuffer);
		if(pNtHeaders)
		{
			if(IMAGE_FIRST_SECTION(pNtHeaders) != NULL)
			{
				PIMAGE_DATA_DIRECTORY pDataDir = pNtHeaders->OptionalHeader.DataDirectory + IMAGE_DIRECTORY_ENTRY_EXPORT;
				if(pDataDir->Size)
				{
					PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)((BYTE *)lpMapInfo->lpBuffer + Rva2Offset(pDataDir->VirtualAddress, IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections));
					DWORD *pFuncAddr = (DWORD *)((BYTE *)lpMapInfo->lpBuffer + Rva2Offset(pExportDir->AddressOfFunctions, IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections));
					DWORD *pNameAddr = (DWORD *)((BYTE *)lpMapInfo->lpBuffer + Rva2Offset(pExportDir->AddressOfNames, IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections));
					printf("  + Module: %s\n\n", szDllPath);
					for(DWORD pIndex = 0; pIndex < pExportDir->NumberOfFunctions; pIndex++, nFunctions++)
					{
						const char *pFunc = (const char *)((BYTE *)lpMapInfo->lpBuffer + Rva2Offset(pNameAddr[pIndex], IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections));
						DWORD pAddr = (DWORD)pFuncAddr[pIndex];
						printf("  + Function: %s,  Address: 0x%08X\n", pFunc, pAddr);
					}
				}
			}
		}
		UnloadFile(lpMapInfo);
		return nFunctions;
	}
	return 0;
}

LPMAPINFO LoadFile(LPCTSTR lpszFileName, DWORD dwStubSize) // By Irwin
{
	LPMAPINFO lpMapInfo;
	HANDLE	hFile, hFileMapping;
	LPBYTE	lpBuffer;
	DWORD	 dwSize;
	hFile = CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		dwSize = GetFileSize(hFile, 0);
		if(dwSize != INVALID_FILE_SIZE)
		{
			if(dwStubSize)
				dwSize += dwStubSize + 4;
			hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, dwSize, NULL);
			if(hFileMapping != NULL)
			{
				lpBuffer = (LPBYTE)MapViewOfFile(hFileMapping, SECTION_MAP_READ, 0, 0, dwSize);
				if(lpBuffer != NULL)
				{
					lpMapInfo = (LPMAPINFO)HeapAlloc(GetProcessHeap(), 0, sizeof(MAPINFO));
					if(lpMapInfo != NULL)
					{
						__try
						{
							lpMapInfo->hFile = hFile;
							lpMapInfo->hFileMapping = hFileMapping;
							lpMapInfo->lpBuffer	 = lpBuffer;
							return lpMapInfo;
						} __except (EXCEPTION_EXECUTE_HANDLER)
						{
							HeapFree(GetProcessHeap(), 0, (LPVOID)lpMapInfo);
						}
					}

					UnmapViewOfFile(lpBuffer);
				}

				CloseHandle(hFileMapping);
			}
		}

		CloseHandle(hFile);
	}

	return NULL;
}

void UnloadFile(LPMAPINFO lpMapInfo) // By Irwin
{
	if(lpMapInfo != NULL)
	{
		UnmapViewOfFile(lpMapInfo->lpBuffer);
		CloseHandle(lpMapInfo->hFileMapping);
		CloseHandle(lpMapInfo->hFile);
		HeapFree(GetProcessHeap(), 0, (LPVOID)lpMapInfo);
	}
}

void Usage()
{
	printf("  Usage:");
	printf("\n\n  -f\tFile path, e.g. C:\\PEWalk.exe");
	printf("\n	  \tWill display & log EAT / IAT of file");
	printf("\n\n  -w\tFile extention, eat or iat only\n");
	printf("\n\n  -h\tWill display this, for help :)\n\n");
	ExitProcess(0);
}

DWORD Rva2Offset(DWORD dwRva, PIMAGE_SECTION_HEADER dwSecRva, USHORT uNumOfSecs)
{
	for(USHORT i = 0; i < uNumOfSecs; i++)
	{
		if(dwRva >= dwSecRva->VirtualAddress)
		{
			if(dwRva < dwSecRva->VirtualAddress + dwSecRva->Misc.VirtualSize)
				return (DWORD)(dwRva - dwSecRva->VirtualAddress + dwSecRva->PointerToRawData);
		}
		dwSecRva++;
	}
	return -1;
}
