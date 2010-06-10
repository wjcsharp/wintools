/**
 * Installed Application Fetcher
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
#include <Msi.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "msi.lib")

int _tmain(int argc, TCHAR *argv[])
{	
	DWORD i = 0;
	UINT result = 0;
	HMODULE mod;

	_tprintf(TEXT("Enumerate Installed Applications v1.0.0\nJacob Hammack\nhttp://www.hammackj.com\n\n"));

	/* Lets Check to see if we can find the Msi.dll so this will work.*/
	if((mod = LoadLibrary(TEXT("Msi.dll"))) == NULL)
	{
		_tprintf(TEXT("[!] Unable to load MSI.lib, Please install Microsoft Installer, Returned error %d\n"), GetLastError());

		return -1;
	}

	do
	{
		TCHAR ApplicationGUID[40];
		ZeroMemory(ApplicationGUID, 40); 

		result = MsiEnumProducts(i, ApplicationGUID);

		if(result == ERROR_SUCCESS)
		{
			TCHAR ApplicationName[256];
			DWORD Size = (DWORD) 256;
			UINT res = 0;
			ZeroMemory(ApplicationName, 2);

			res = MsiGetProductInfo(ApplicationGUID, INSTALLPROPERTY_PRODUCTNAME, ApplicationName, &Size);

			if(res != ERROR_SUCCESS)
			{
				_tprintf(TEXT("[!] Unable to Get Advanced Product Info, Res = %d, Returned error %d\n"), res,GetLastError());
			}
			else
			{
				_tprintf(TEXT("[*] %s\n"), ApplicationName);
			}			
		}

		i++;
	} while(result != ERROR_NO_MORE_ITEMS);

	FreeLibrary(mod);

	return 0;
}