#include <stdio.h>
#include <windows.h>
#include <conio.h>
#define WTS_CURRENT_SERVER_HANDLE  ((HANDLE)NULL)


int main(void)
{
	char	szProcess[MAX_PATH];
	wchar_t   *wPtr;
	DWORD	dwAddress = 0,
			pCount    = 0,
			dwPid     = 0,
			dwOffset  = 0,
			dwIndex  = 12;

	BOOL(WINAPI *WinStationGetAllProcess)(HANDLE,DWORD,DWORD *,DWORD *);

	*(FARPROC*)&WinStationGetAllProcess = GetProcAddress(LoadLibrary("winsta.dll"),"WinStationGetAllProcesses");
	WinStationGetAllProcess(WTS_CURRENT_SERVER_HANDLE,0x0,&pCount,&dwAddress);

	for(DWORD dwCount = 0;dwCount < pCount;dwCount++) 
	{
		dwOffset = *(DWORD*)(dwAddress + dwIndex);
		dwPid    =  dwOffset;
		dwPid    = *(DWORD*)(dwPid + 0x3C + sizeof(DWORD) + sizeof(DWORD));
		dwOffset = *(DWORD*)(dwOffset + 0x3C);

		WideCharToMultiByte(CP_ACP,0,
							(wchar_t*)dwOffset,
							-1,
							szProcess,
							sizeof(szProcess),
							NULL,NULL);

		printf("hey man its %s and PID = %d\n",szProcess,dwPid);
		_getch();
		dwIndex+= 12;


	}
	_getch();
    return 0;
}
