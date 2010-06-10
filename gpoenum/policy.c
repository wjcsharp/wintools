#include <windows.h>
#include <tchar.h>

#include <Userenv.h> /*Windows 2000*/

#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Advapi32.lib")

void SetPrivilege(DWORD privilege);

int _tmain(int argc, TCHAR *argv[])
{	
	PGROUP_POLICY_OBJECT *pGPOList;
	PHANDLE *TokenHandle;
	GUID guid = { 0x1A6364EB, 0x776B4120, 0xADE1B63A, 0x406A76B5 };

	SetPrivilege(SE_DEBUG_NAME);
	SetPrivilege(SE_IMPERSONATE_NAME);
	SetPrivilege(SE_TCB_NAME);

	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &TokenHandle))
	{
		_tprintf(TEXT("[!] Unable to open the current process and steal the token, Returned error %d\n"), GetLastError());
	}

	/*if(!GetGPOList(TokenHandle, NULL, NULL, NULL, 0, &pGPOList))
	{
		_tprintf(TEXT("[!] Unable get the GPO policy, Returned error %d\n"), GetLastError());
	}
	*/

	if(!GetAppliedGPOList(GPO_LIST_FLAG_MACHINE, null, null, guid, pGPOList))
	{
		_tprintf(TEXT("[!] Unable get the Applied GPO policy, Returned error %d\n"), GetLastError());
	}


	FreeGPOList(pGPOList);

	return 0;
}

/**
 * Elevate to DEBUG Privileges
 *
 * Attempts elevate to system debug privileges, if this fails or suceeds doesn't really matter
 * it just provides us a little bit more access to the system.
 *
 * @author Jacob Hammack NGC/TASC
 */
void SetPrivilege(DWORD privilege)
{
	HANDLE hToken;
	LUID debug;
	TOKEN_PRIVILEGES tp;

	if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		if (!LookupPrivilegeValue(NULL, privilege, &debug))
		{
			CloseHandle(hToken);
		}

		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = debug;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
		{
			CloseHandle(hToken);
		}

		_tprintf(TEXT("[*] Successfully set privilege.\n"));

		CloseHandle(hToken);
			
	}
	else
	{
		_tprintf(TEXT("[!] Unable to set privilege.\n"));
	}
}