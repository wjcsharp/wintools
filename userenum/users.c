/**
 * User Enumerator
 * @file
 * @version 1.0
 *
 * @author Jacob Hammack
 *
 * 09-16-2009:	JPH	- Created initial program.
 *
 */

#include <windows.h>
#include <tchar.h>
#include <lm.h>

#pragma comment(lib, "netapi32.lib")

int _tmain(int argc, TCHAR *argv[])
{
	DWORD ReturnedEntryCount = 0;
	DWORD Index = 0;
	DWORD Result = 0;
	LPVOID SortedBuffer = NULL;
	DWORD i;

	_tprintf(TEXT("Enumerate Users v1.0.0\nJacob Hammack\nhttp://www.hammackj.com\n\n"));

	do
	{
		Result = NetQueryDisplayInformation(NULL, 1, Index, 1000, MAX_PREFERRED_LENGTH, &ReturnedEntryCount, &SortedBuffer);

		if((Result == ERROR_SUCCESS) || (Result == ERROR_MORE_DATA))
		{
			PNET_DISPLAY_USER CurrentUser = (PNET_DISPLAY_USER) SortedBuffer;

			for(i = 0; i < ReturnedEntryCount; ++i, ++CurrentUser)
			{
				_tprintf(TEXT("Name: %s, Comment: %s, User ID: %u\n"), CurrentUser->usri1_name, CurrentUser->usri1_comment, CurrentUser->usri1_user_id);
				_tprintf(TEXT("Attributes:\n"));

				if(CurrentUser->usri1_flags & UF_ACCOUNTDISABLE)
				{
					_tprintf(TEXT("\tAccount is Disabled\n"));
				}

				if(CurrentUser->usri1_flags & UF_HOMEDIR_REQUIRED)
				{
					_tprintf(TEXT("\tAccount Needs a Home directory\n"));
				}

				if(CurrentUser->usri1_flags & UF_LOCKOUT)
				{
					_tprintf(TEXT("\tAccount Locked out\n"));
				}

				if(CurrentUser->usri1_flags & UF_PASSWD_NOTREQD)
				{
					_tprintf(TEXT("\tAccount does not require a password\n"));
				}

				if(CurrentUser->usri1_flags & UF_PASSWD_CANT_CHANGE)
				{
					_tprintf(TEXT("\tAccount cannot change the password\n"));
				}

				if(CurrentUser->usri1_flags & UF_DONT_EXPIRE_PASSWD)
				{
					_tprintf(TEXT("\tAccount Password never expires\n"));
				}

				if(CurrentUser->usri1_flags & UF_SMARTCARD_REQUIRED)
				{
					_tprintf(TEXT("\tAccount requires Smartcard\n"));
				}

				_tprintf(TEXT("\n"));
			}

			Index += i;

			NetApiBufferFree(SortedBuffer);
		}
		else
		{
			_tprintf(TEXT("Error: %u"), Result);
		}
	} while (Result == ERROR_MORE_DATA);

	return 0;
}