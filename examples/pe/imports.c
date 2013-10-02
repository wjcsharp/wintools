#include<windows.h>
#include<stdio.h>
int main(int argc,char *argv[])
{
	 if(argc<2)
	 {
	   printf("\nUsage : ImportDirParser.exe TargetExe\n");
	   ExitProcess(0);
	 }
	 HANDLE hFile,hFileMap;
	 DWORD dwImportDirectoryVA,dwSectionCount,dwSection=0,dwRawOffset;
	 LPVOID lpFile;
	 PIMAGE_DOS_HEADER pDosHeader;
	 PIMAGE_NT_HEADERS pNtHeaders;
	 PIMAGE_SECTION_HEADER pSectionHeader;
	 PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor;
	 PIMAGE_THUNK_DATA pThunkData;
	 hFile = CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	 if(hFile==INVALID_HANDLE_VALUE)
		ExitProcess(1);
	 hFileMap = CreateFileMapping(hFile,0,PAGE_READONLY,0,0,0);
	 lpFile = MapViewOfFile(hFileMap,FILE_MAP_READ,0,0,0);
	 pDosHeader = (PIMAGE_DOS_HEADER)lpFile;
	 pNtHeaders = (PIMAGE_NT_HEADERS)((DWORD)lpFile+pDosHeader->e_lfanew);
	 dwSectionCount = pNtHeaders->FileHeader.NumberOfSections;
	 dwImportDirectoryVA = pNtHeaders->OptionalHeader.DataDirectory[1].VirtualAddress;
	 pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pNtHeaders+sizeof(IMAGE_NT_HEADERS));
	 for(;dwSection < dwSectionCount && pSectionHeader->VirtualAddress <= dwImportDirectoryVA;pSectionHeader++,dwSection++);
	 pSectionHeader--;
	 dwRawOffset = (DWORD)lpFile+pSectionHeader->PointerToRawData;
	 pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(dwRawOffset+(dwImportDirectoryVA-pSectionHeader->VirtualAddress));
	 for(;pImportDescriptor->Name!=0;pImportDescriptor++)
	 {
		 printf("\nDLL Name : %s\n\n",dwRawOffset+(pImportDescriptor->Name-pSectionHeader->VirtualAddress));
		 pThunkData = (PIMAGE_THUNK_DATA)(dwRawOffset+(pImportDescriptor->FirstThunk-pSectionHeader->VirtualAddress));
		 for(;pThunkData->u1.AddressOfData != 0;pThunkData++)
		 printf("\tFunction : %s\n",(dwRawOffset+(pThunkData->u1.AddressOfData-pSectionHeader->VirtualAddress+2)));
	 }
	UnmapViewOfFile(lpFile);
	CloseHandle(hFileMap);
	CloseHandle(hFile);
	return 0;
}
