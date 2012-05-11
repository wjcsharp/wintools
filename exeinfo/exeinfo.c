/**
 * Exeinfo v1.0
 * @file
 *
 * 05-25-2008:	JPH	- Created.
 *
 * @author Jacob Hammack
 */
 
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "version.lib")

 
//DWORD GetFileVersionInfoSize(LPCTSTR lptstrFilename, LPDWORD lpdwHandle);
 

BOOL ExeVersionInfo(const TCHAR *filename)
{
	DWORD VersionSize;
	LPTSTR VersionInfo;
	UINT VersionMajor;
	
	VersionSize = GetFileVersionInfoSize((LPTSTR)filename, NULL);
	
	if(!VersionSize)
	{
		printf("Failed to get size\n");	
		
		printf("Error #: %d\n", GetLastError());
		
		return FALSE;
	}

	VersionInfo = GlobalAlloc(GPTR, VersionSize);
	
	if(VersionInfo)
	{
		DWORD BufferSize;
		VS_FIXEDFILEINFO *FixedFileInfo = 0;
		BOOL ReturnValue;
		WORD *LanguageInfo;
		UINT Language;
		TCHAR VersionStringName[128];
		PVOID Pointer;
		UINT BufSize;

		BufferSize = 0;
		BufSize = 0;
		
		GetFileVersionInfo((LPTSTR) filename, NULL, VersionSize, VersionInfo);
		
		if(VerQueryValue(VersionInfo, TEXT("\\"), (LPVOID *) &FixedFileInfo, (UINT *) BufferSize))
		{
			VersionMajor = HIWORD(FixedFileInfo->dwFileVersionMS);
		}
        //Get the Company Name.
        //First, to get string information, we need to get
        //language information.
        VerQueryValue(VersionInfo, _T("\\VarFileInfo\\Translation"), (LPVOID*)&LanguageInfo, &Language);
        //Prepare the label -- default lang is bytes 0 & 1
        //of langInfo
        wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("CompanyName"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
			_tprintf(TEXT("Company Name: %s\n"), (LPTSTR) Pointer);
    	}

        wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("FileVersion"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
			_tprintf(TEXT("File Version: %s\n"), (LPTSTR) Pointer);
    	}

        wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("Comments"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
			_tprintf(TEXT("Comments: %s\n"), (LPTSTR) Pointer);
    	}

        wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("InternalName"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
			_tprintf(TEXT("Internal Name: %s\n"), (LPTSTR) Pointer);
    	}

        wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("ProductName"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
        	//companyName.assign((LPTSTR)lpt);    //*must* save this
			_tprintf(TEXT("Product Name: %s\n"), (LPTSTR) Pointer);
    	}

		wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("LegalCopyright"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
        	//companyName.assign((LPTSTR)lpt);    //*must* save this
			_tprintf(TEXT("Legal Copyright: %s\n"), (LPTSTR) Pointer);
    	}
		
		wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("ProductVersion"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
        	//companyName.assign((LPTSTR)lpt);    //*must* save this
			_tprintf(TEXT("Product Version: %s\n"), (LPTSTR) Pointer);
    	}
		
		wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("FileDescription"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
        	//companyName.assign((LPTSTR)lpt);    //*must* save this
			_tprintf(TEXT("File Description: %s\n"), (LPTSTR) Pointer);
    	}
		
		wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("PrivateBuild"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
        	//companyName.assign((LPTSTR)lpt);    //*must* save this
			_tprintf(TEXT("Private Build: %s\n"), (LPTSTR) Pointer);
    	}
		
		wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("LegalTrademarks"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
        	//companyName.assign((LPTSTR)lpt);    //*must* save this
			_tprintf(TEXT("Legal Trademarks: %s\n"), (LPTSTR) Pointer);
    	}

		wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("OriginalFilename"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
        	//companyName.assign((LPTSTR)lpt);    //*must* save this
			_tprintf(TEXT("Original Filename: %s\n"), (LPTSTR) Pointer);
    	}
		wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("SpecialBuild"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
        	//companyName.assign((LPTSTR)lpt);    //*must* save this
			_tprintf(TEXT("Special Build: %s\n"), (LPTSTR) Pointer);
    	}
		wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("Language"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
        	//companyName.assign((LPTSTR)lpt);    //*must* save this
			_tprintf(TEXT("Language: %s\n"), (LPTSTR) Pointer);
    	}
		wsprintf(VersionStringName, _T("\\StringFileInfo\\%04x%04x\\%s"), LanguageInfo[0], LanguageInfo[1], _T("ProductVersion"));
        //Get the string from the resource data
        if (VerQueryValue(VersionInfo, VersionStringName, &Pointer, BufSize))
        {    
        	//companyName.assign((LPTSTR)lpt);    //*must* save this
			_tprintf(TEXT("Product Version: %s\n"), (LPTSTR) Pointer);
    	}
	}

	GlobalFree((HGLOBAL) VersionInfo);
	
	return TRUE;
}


/**
 * Main entry point for Exeinfo.
 *
 * @author Jacob Hammack
 */
int _tmain(int argc, TCHAR *argv[])
{
	printf("Exeinfo v1.0\nJacob Hammack\nhttp://www.hammackj.com\n\n");
	
	if(argc != 2)
	{
		_tprintf(TEXT("usage: exeinfo exefilename\n"));
		exit(1);	
	}
	
	ExeVersionInfo(argv[1]);
	
	return 0;		
}
