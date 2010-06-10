// driver loader by _FIL73R_

#include <windows.h>
#include <ntdef.h>
#include <stdio.h>

typedef FARPROC ( WINAPI *NtLoadDriver ) ( IN PUNICODE_STRING DriverServiceName );
NtLoadDriver fNtLoadDriver;
typedef VOID ( WINAPI *RtlInitUnicodeString ) ( PUNICODE_STRING DestinationString,
                                                PCWSTR SourceString );
RtlInitUnicodeString fRtlInitUnicodeString;


bool loadsysfile(char *theDriverName)
{
char aPath[1024];
char aCurrentDirectory[515];
SC_HANDLE sh = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
if(!sh)
{
  printf("\nCouldn't open SCM");
  return false;
}
GetCurrentDirectory( 512, aCurrentDirectory);
_snprintf(aPath,1022,"%s\\%s",aCurrentDirectory,theDriverName);

printf("\nloading %s\n", aPath);
SC_HANDLE rh = CreateService(sh,
theDriverName,
theDriverName,
SERVICE_ALL_ACCESS,
SERVICE_KERNEL_DRIVER,
SERVICE_DEMAND_START,
SERVICE_ERROR_NORMAL,
aPath,
NULL,
NULL,
NULL,
NULL,
NULL);

if(!rh)
{
  if (GetLastError() == ERROR_SERVICE_EXISTS) // service exists
  {
   rh = OpenService(sh,theDriverName,SERVICE_ALL_ACCESS);
   if(!rh)
   {
    CloseServiceHandle(sh);
    printf("\nService already exists");
    return false;
   }
  }
  else
  {
   CloseServiceHandle(sh);
   return false;
  }
}

// start the drivers
if(rh)
{
  if(0 == StartService(rh, 0, NULL))
  {
   if(ERROR_SERVICE_ALREADY_RUNNING == GetLastError())
   {
    // no real problem
   }
   else
   {
    CloseServiceHandle(sh);
    CloseServiceHandle(rh);
    return false;
   }
  }
  CloseServiceHandle(sh);
  CloseServiceHandle(rh);
}
return true;
}

WCHAR *ToWideChar( char *szStr, int szStrLen )
{
int nMax = szStrLen*2;
WCHAR *wc;
      wc = (WCHAR*)malloc( nMax );
      memset( wc, 0, nMax );
      
      for( int x = 0; x<=szStrLen; x++ )
           wc[x] = szStr[x];

return wc;
}

void RegLoadDriver( char *szDriverName )
{
char szRegPath[MAX_PATH] = { 0 };
HINSTANCE hNt;
UNICODE_STRING us;

    hNt = LoadLibraryA( "ntdll.dll" );
    if( hNt ) {
        
        fNtLoadDriver = (NtLoadDriver) GetProcAddress( hNt, "NtLoadDriver" );
        fRtlInitUnicodeString = (RtlInitUnicodeString) GetProcAddress( hNt, "RtlInitUnicodeString" );
        if( fRtlInitUnicodeString && fNtLoadDriver ) {
            
            sprintf( szRegPath, "%s%s","\\registry\\machine\\SYSTEM\\CurrentControlSet\\Services\\", szDriverName );
            fRtlInitUnicodeString( &us, ToWideChar( szRegPath, strlen(szRegPath) ) );
            fNtLoadDriver( &us );

        }
        FreeLibrary( hNt );
    }
}

bool RegMakeDriverEntry( char *szDriverName, char *szDriverPath )
{
DWORD dwType = 1, // driver
      dwStart = 3; // force start
HKEY hMain;
char szMain[1024] = {0},
     szImgPath[1024] = {0};
sprintf( szMain, "%s%s","SYSTEM\\CurrentControlSet\\Services\\", szDriverName );
sprintf( szImgPath, "%s%s", "\\??\\", szDriverPath );
     
     if( RegCreateKeyA( HKEY_LOCAL_MACHINE, szMain, &hMain ) == EXIT_SUCCESS ) {
         RegSetValueExA( hMain, 
                         "DisplayName", 
                         0, 
                         REG_SZ, 
                         (LPBYTE)szDriverName, 
                         (DWORD)strlen(szDriverName) );
         RegSetValueExA( hMain, 
                         "ImagePath", 
                         0, 
                         REG_EXPAND_SZ, // cos it's a unicode string
                         (LPBYTE)szImgPath, 
                         (DWORD)strlen(szImgPath) );
         RegSetValueExA( hMain, 
                         "Type",
                         0,
                         REG_DWORD,
                         (LPBYTE)&dwType,
                         (DWORD)sizeof(dwType) );
         RegSetValueExA( hMain, 
                         "Start",
                         0,
                         REG_DWORD,
                         (LPBYTE)&dwStart,
                         (DWORD)sizeof(dwStart) );
                         //         Reg
     } else return FALSE;
return TRUE;
}

int main(int argv, char *argv[])
{
    //if( RegMakeDriverEntry( "DriverEntry", "X:\\windows\\system32\\drivers\\YourDriverName.sys")) RegLoadDriver( "DriverEntry" );
	
	loadsysdriver(argv[1]);
	
	return 0;
}