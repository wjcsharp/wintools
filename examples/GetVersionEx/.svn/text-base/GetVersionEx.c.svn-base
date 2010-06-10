#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#define BUFSIZE 256

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

BOOL GetOSDisplayString( LPTSTR pszOS)
{
   OSVERSIONINFOEX osvi;
   SYSTEM_INFO si;
   PGNSI pGNSI;
   PGPI pGPI;
   BOOL bOsVersionInfoEx;
   DWORD dwType;
   TCHAR buf[80];

   ZeroMemory(&si, sizeof(SYSTEM_INFO));
   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
      return 1;

   // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

   pGNSI = (PGNSI) GetProcAddress(
      GetModuleHandle(TEXT("kernel32.dll")), 
      "GetNativeSystemInfo");
   if(NULL != pGNSI)
      pGNSI(&si);
   else GetSystemInfo(&si);

   if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && 
        osvi.dwMajorVersion > 4 )
   {
      StringCchCopy(pszOS, BUFSIZE, TEXT("Microsoft "));

      // Test for the specific product.

      if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
      {
         if( osvi.wProductType == VER_NT_WORKSTATION )
         {   
			StringCchCat(pszOS, BUFSIZE, TEXT("Windows Vista "));
		 }
         else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 " ));

         pGPI = (PGPI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");

         pGPI( 6, 0, 0, 0, &dwType);

         switch( dwType )
         {
            case PRODUCT_ULTIMATE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Ultimate Edition" ));
               break;
            case PRODUCT_HOME_PREMIUM:
               StringCchCat(pszOS, BUFSIZE, TEXT("Home Premium Edition" ));
               break;
            case PRODUCT_HOME_BASIC:
               StringCchCat(pszOS, BUFSIZE, TEXT("Home Basic Edition" ));
               break;
            case PRODUCT_ENTERPRISE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition" ));
               break;
            case PRODUCT_BUSINESS:
               StringCchCat(pszOS, BUFSIZE, TEXT("Business Edition" ));
               break;
            case PRODUCT_STARTER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Starter Edition" ));
               break;
            case PRODUCT_CLUSTER_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Cluster Server Edition" ));
               break;
            case PRODUCT_DATACENTER_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition" ));
               break;
            case PRODUCT_DATACENTER_SERVER_CORE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition (core installation)" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER_CORE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition (core installation)" ));
               break;
            case PRODUCT_ENTERPRISE_SERVER_IA64:
               StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition for Itanium-based Systems" ));
               break;
            case PRODUCT_SMALLBUSINESS_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server" ));
               break;
            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
               StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server Premium Edition" ));
               break;
            case PRODUCT_STANDARD_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition" ));
               break;
            case PRODUCT_STANDARD_SERVER_CORE:
               StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition (core installation)" ));
               break;
            case PRODUCT_WEB_SERVER:
               StringCchCat(pszOS, BUFSIZE, TEXT("Web Server Edition" ));
               break;
         }
         if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
            StringCchCat(pszOS, BUFSIZE, TEXT( ", 64-bit" ));
         else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
            StringCchCat(pszOS, BUFSIZE, TEXT(", 32-bit"));
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
      {
         if( GetSystemMetrics(SM_SERVERR2) )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Server 2003 R2, "));
         else if ( osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Storage Server 2003"));
         else if( osvi.wProductType == VER_NT_WORKSTATION &&
                  si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
         {
            StringCchCat(pszOS, BUFSIZE, TEXT( "Windows XP Professional x64 Edition"));
         }
         else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2003, "));

         // Test for the server type.
         if ( osvi.wProductType != VER_NT_WORKSTATION )
         {
            if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
            {
                if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition for Itanium-based Systems" ));
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition for Itanium-based Systems" ));
            }

            else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
            {
                if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter x64 Edition" ));
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise x64 Edition" ));
                else StringCchCat(pszOS, BUFSIZE, TEXT( "Standard x64 Edition" ));
            }

            else
            {
                if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Compute Cluster Edition" ));
                else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition" ));
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition" ));
                else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
                   StringCchCat(pszOS, BUFSIZE, TEXT( "Web Edition" ));
                else StringCchCat(pszOS, BUFSIZE, TEXT( "Standard Edition" ));
            }
         }
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
      {
         StringCchCat(pszOS, BUFSIZE, TEXT("Windows XP "));
         if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Home Edition" ));
         else StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" ));
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
      {
         StringCchCat(pszOS, BUFSIZE, TEXT("Windows 2000 "));

         if ( osvi.wProductType == VER_NT_WORKSTATION )
         {
            StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" ));
         }
         else 
         {
            if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
               StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Server" ));
            else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
               StringCchCat(pszOS, BUFSIZE, TEXT( "Advanced Server" ));
            else StringCchCat(pszOS, BUFSIZE, TEXT( "Server" ));
         }
      }

       // Include service pack (if any) and build number.

      if( _tcslen(osvi.szCSDVersion) > 0 )
      {
          StringCchCat(pszOS, BUFSIZE, TEXT(" ") );
          StringCchCat(pszOS, BUFSIZE, osvi.szCSDVersion);
      }

      

      StringCchPrintf( buf, 80, TEXT(" (build %d)"), osvi.dwBuildNumber);
      StringCchCat(pszOS, BUFSIZE, buf);

      return TRUE; 
   }

   else
   {  
      printf( "This sample does not support this version of Windows.\n");
      return FALSE;
   }
}

int __cdecl _tmain()
{
    TCHAR szOS[BUFSIZE];

    if( GetOSDisplayString( szOS ) )
        _tprintf( TEXT("\n%s\n"), szOS );
}