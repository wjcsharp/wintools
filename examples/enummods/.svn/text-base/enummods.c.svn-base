#include <windows.h> 
#include <tlhelp32.h> 
#include <stdio.h> 
 
//  Forward declarations: 
BOOL ListProcessModules( DWORD dwPID ); 
void printError( TCHAR* msg ); 
 
void main( )
{
  ListProcessModules(GetCurrentProcessId() );
}

BOOL ListProcessModules( DWORD dwPID ) 
{ 
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE; 
  MODULEENTRY32 me32; 
 
//  Take a snapshot of all modules in the specified process. 
  hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID ); 
  if( hModuleSnap == INVALID_HANDLE_VALUE ) 
  { 
    printError( "CreateToolhelp32Snapshot (of modules)" ); 
    return( FALSE ); 
  } 
 
//  Set the size of the structure before using it. 
  me32.dwSize = sizeof( MODULEENTRY32 ); 
 
//  Retrieve information about the first module, 
//  and exit if unsuccessful 
  if( !Module32First( hModuleSnap, &me32 ) ) 
  { 
    printError( "Module32First" ); // Show cause of failure 
    CloseHandle( hModuleSnap );    // Must clean up the
                                   //   snapshot object! 
    return( FALSE ); 
  } 
 
//  Now walk the module list of the process, 
//  and display information about each module 
  do 
  { 
    printf( "\n\n     MODULE NAME:     %s",
      me32.szModule ); 
    printf( "\n     executable     = %s",
      me32.szExePath ); 
    printf( "\n     process ID     = 0x%08X",
      me32.th32ProcessID ); 
    printf( "\n     ref count (g)  =     0x%04X",
      me32.GlblcntUsage ); 
    printf( "\n     ref count (p)  =     0x%04X",
      me32.ProccntUsage ); 
    printf( "\n     base address   = 0x%08X",
      (DWORD) me32.modBaseAddr ); 
    printf( "\n     base size      = %d",
      me32.modBaseSize ); 
 
  } while( Module32Next( hModuleSnap, &me32 ) ); 
 
//  Do not forget to clean up the snapshot object. 
  CloseHandle( hModuleSnap ); 
  return( TRUE ); 
} 
 
 
void printError( TCHAR* msg )
{
  DWORD eNum;
  TCHAR sysMsg[256];
  TCHAR* p;

  eNum = GetLastError( );
  FormatMessage(
         FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, eNum,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default lang.
         sysMsg, 256, NULL );

  // Trim the end of the line and terminate it with a null
  p = sysMsg;
  while( ( *p > 31 ) || ( *p == 9 ) )
    ++p;
  do { *p-- = 0; } while( ( p >= sysMsg ) &&
                          ( ( *p == '.' ) || ( *p < 33 ) ) );

  // Display the message
  printf( "\n  WARNING: %s failed with error %d (%s)",
    msg, eNum, sysMsg );
}
