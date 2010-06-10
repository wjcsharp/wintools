
#include <windows.h>


/**
 * Process Enumeration Functions API 
 *
 * Methods for enumerating Windows Processes
 *
 * 1. CreateToolhelp32Snapshot() Method
 * 2. PSapi Method
 * 3. NtQuerySystemInformation() Method
 * 4. 
 */

int enumerate_processes_toolhelp_api(void);

int enumerate_processes_psapi_api(void);

int enumerate_processes_ntquery_api(void);

int display_processes_xml(void);

int display_processes_screen(void);


/**
 * Windows system helper functions
 *
 *
 */

int os_major_version_api(void);

int os_minor_version_api(void);

BOOL SetTokenPrivileges(TCHAR *priv, BOOL enable);


/**
 * Set the passed Privileges to the current processes token.
 *
 * @return FALSE on fail TRUE on success 
 * 
 * @author Jacob Hammack
 */
BOOL SetTokenPrivileges(TCHAR *PrivToRequest, BOOL enable)
{
	HANDLE TokenHandle;
	LUID UniqueID;
	TOKEN_PRIVILEGES TokenPrivileges;

	if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle))
	{
		if (!LookupPrivilegeValue(NULL, PrivToRequest, &UniqueID))
		{
			CloseHandle(TokenHandle);
		}

		TokenPrivileges.PrivilegeCount = 1;
		TokenPrivileges.Privileges[0].Luid = UniqueID;
		
		if(enable == TRUE)
		{
            TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        }
        else
        {
            TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
        }
			
		if (!AdjustTokenPrivileges(TokenHandle, FALSE, &TokenPrivileges, sizeof(TokenPrivileges), NULL, NULL))
		{
			CloseHandle(TokenHandle);
			
			return FALSE;
		}

		CloseHandle(TokenHandle);
		
		return TRUE;
			
	}
	else
	{
		return FALSE;
	}
}
