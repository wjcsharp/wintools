#include <iostream>
#include <Windows.h>

using namespace std;

typedef struct _CLIENT_ID
{
	 PVOID UniqueProcess;
	 PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

EXTERN_C LONG WINAPI RtlCreateUserThread(HANDLE,
PSECURITY_DESCRIPTOR,
BOOLEAN,ULONG,
PULONG,PULONG,
PVOID,PVOID,
PHANDLE,PCLIENT_ID);
EXTERN_C LONG WINAPI NtResumeThread(HANDLE ThreadHandle,PULONG SuspendCount);

int main(){
HANDLE hThread;
CLIENT_ID cid;
DWORD PID,exts;
PVOID para;
char dll[60],func[60];
HANDLE hToken;
LUID luid;
LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid);
TOKEN_PRIVILEGES tp;
tp.Privileges[0].Luid=luid;
tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
tp.PrivilegeCount=1;
OpenProcessToken(GetCurrentProcess(),TOKEN_ALL_ACCESS,&hToken);
AdjustTokenPrivileges(hToken,false,&tp,sizeof(tp),NULL,NULL);
cout <<"==========Remote Thread Creator==========\n\n";
cout <<"This program create remote thread within another process using\nRtlCreateUserThread function. Instead of using\n";
cout <<"GetProcAddress function to get the function address, it import the function\n";
cout <<"from ntdll.dll directly.\n\n";
cout <<"Enter PID:";
cin >>PID;
cout <<"Enter DLL name:";
cin >>dll;
cout <<"Enter function name:";
cin >>func;
cout <<"Enter parameter:";
cin >>para;
HMODULE hModule=GetModuleHandle(dll);
HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,false,PID);
RtlCreateUserThread(hProcess,NULL,true,0,0,0,(PVOID)GetProcAddress(hModule,func),para,&hThread,&cid);
cout <<"Thread created in suspended state. Press enter to resume it.\n\n";
system("pause");
NtResumeThread(hThread,NULL);
WaitForSingleObject(hThread,INFINITE);
GetExitCodeThread(hThread,&exts);
cout <<"\n\nThread terminated with status code: "<<exts;
CloseHandle(hThread);
CloseHandle(hProcess);
return 0;
}