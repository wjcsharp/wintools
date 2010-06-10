//
// Process Memory Range Scanner v1.0 - by Napalm @ NetCore2K
//
#include <windows.h>
#include <stdio.h>


// Below are helper functions
BOOL DoRtlAdjustPrivilege()
{
	#define SE_DEBUG_PRIVILEGE	20L
	#define AdjustCurrentProcess	0
	BOOL bPrev = FALSE;
	LONG (WINAPI *RtlAdjustPrivilege)(DWORD, BOOL, INT, PBOOL);
	*(FARPROC *)&RtlAdjustPrivilege = GetProcAddress(GetModuleHandle("ntdll.dll"), "RtlAdjustPrivilege");
	if(!RtlAdjustPrivilege) return FALSE;
	RtlAdjustPrivilege(SE_DEBUG_PRIVILEGE, TRUE, AdjustCurrentProcess, &bPrev);
	return TRUE;
}

typedef BOOL (CALLBACK *LPENUMADDRESSES)(LPBYTE lpAddress, DWORD dwSize, DWORD dwState, DWORD dwType, DWORD dwProtect);
BOOL EnumProcessAddresses(HANDLE hProcess, LPENUMADDRESSES lpCallback)
{
	MEMORY_BASIC_INFORMATION mbi;
	SYSTEM_INFO msi;
	ZeroMemory(&mbi, sizeof(mbi));
	GetSystemInfo(&msi);
	for(LPBYTE lpAddress = (LPBYTE)msi.lpMinimumApplicationAddress;
		lpAddress <= (LPBYTE)msi.lpMaximumApplicationAddress;
		lpAddress += mbi.RegionSize){
		if(VirtualQueryEx(hProcess, lpAddress, &mbi, sizeof(mbi))){
			if(lpCallback && !lpCallback((LPBYTE)mbi.BaseAddress, mbi.RegionSize,
				mbi.State, mbi.Type, mbi.Protect)) return FALSE;
		} else break;
	}
	return TRUE;
}

// Below is actual code --
BOOL CALLBACK DoSomethingForAddress(LPBYTE lpAddress, DWORD dwSize, DWORD dwState, DWORD dwType, DWORD dwProtect)
{
	printf("0x%08X - 0x%08X (0x%08X) : ", lpAddress, (lpAddress + dwSize), dwSize);
	if(dwState   & MEM_COMMIT)		printf("COMMITTED ");
	if(dwState   & MEM_FREE)		printf("FREE ");
	if(dwState   & MEM_RESERVE)		printf("RESERVED ");
	if(dwType	& MEM_IMAGE)		printf("IMAGE ");
	if(dwType	& MEM_MAPPED)		printf("MAPPED ");
	if(dwType	& MEM_PRIVATE)		printf("PRIVATE ");
	if(dwProtect & PAGE_EXECUTE)		printf("EXECUTE ");
	if(dwProtect & PAGE_EXECUTE_READ)	printf("EXECUTE_READ ");
	if(dwProtect & PAGE_EXECUTE_READWRITE)	printf("EXECUTE_READWRITE ");
	if(dwProtect & PAGE_EXECUTE_WRITECOPY)	printf("EXECUTE_WRITECOPY ");
	if(dwProtect & PAGE_NOACCESS)		printf("NOACCESS ");
	if(dwProtect & PAGE_READONLY)		printf("READONLY ");
	if(dwProtect & PAGE_READWRITE)		printf("READWRITE ");
	if(dwProtect & PAGE_WRITECOPY)		printf("WRITECOPY ");
	if(dwProtect & PAGE_GUARD)		printf("GUARD ");
	if(dwProtect & PAGE_NOCACHE)		printf("NOCACHE ");
	if(dwProtect & PAGE_WRITECOMBINE)	printf("WRITECOMBINE ");
	printf("\n");
	return TRUE;
}

int main(int argc, char *argv[])
{
	if(!DoRtlAdjustPrivilege()) return 1;
	if(argc < 2) return 1;
	DWORD dwPID = atoi(argv[1]);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
	if(hProcess == NULL) return 1;
	EnumProcessAddresses(hProcess, DoSomethingForAddress);
	CloseHandle(hProcess);
	return 0;
}