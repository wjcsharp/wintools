#include <windows.h>
#include <stdio.h>


BOOL SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    ) 
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if ( !LookupPrivilegeValue( 
			NULL,            // lookup privilege on local system
			lpszPrivilege,   // privilege to lookup 
			&luid ) )        // receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError() ); 
		return FALSE; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if ( !AdjustTokenPrivileges(
		   hToken, 
		   FALSE, 
		   &tp, 
		   sizeof(TOKEN_PRIVILEGES), 
		   (PTOKEN_PRIVILEGES) NULL, 
		   (PDWORD) NULL) )
	{ 
		  printf("AdjustTokenPrivileges error: %u\n", GetLastError() ); 
		  return FALSE; 
	} 

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		  printf("The token does not have the specified privilege. \n");
		  return FALSE;
	} 

	return TRUE;
}


void main()
{

	HANDLE ProcessToken;
	HKEY key;
	LRESULT result;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &ProcessToken)) {

		SetPrivilege(ProcessToken, SE_BACKUP_NAME, TRUE);
		result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &key);
		result = RegSaveKey(key, L"c:\\saved.reg", NULL);
	}
}