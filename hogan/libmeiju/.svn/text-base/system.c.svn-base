#include "libmeiju.h"

/**
 * Returns the hajor version of the operating system.  
 *
 * @return 0 on fail, non zero on success
 * 
 * @author Jacob Hammack 
 */
int os_major_version_api(void)
{
	OSVERSIONINFO ovi;
	
	ovi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	
	if (GetVersionEx (&ovi) && (ovi.dwPlatformId == VER_PLATFORM_WIN32_NT))
	{
		return (int) ovi.dwMajorVersion;
	}
	else
	{
		return 0;
	}
}

/**
 * Returns the minor version of the operating system.  
 *
 * @return -1 on fail, non zero on success
 * 
 * @author Jacob Hammack 
 */
int os_minor_version_api(void)
{
	OSVERSIONINFO ovi;
	
	ovi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	
	if (GetVersionEx (&ovi) && (ovi.dwPlatformId == VER_PLATFORM_WIN32_NT))
	{
		return (int) ovi.dwMinorVersion;
	}
	else
	{
		return -1;
	}
}
