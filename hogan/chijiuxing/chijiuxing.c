#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define MAX_VALUE 1024 * 1024
#define MAX_VALUE_NAME 16383
#define MAX_KEY_NAME 255

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "advapi32.lib")

int query_key(HKEY h_key);
int recursively_enum_registry_key_api(HKEY hive, TCHAR *key);
void printfiletime(FILETIME *time);


int recursively_enum_registry_key_api(HKEY hive, TCHAR *key)
{
	HKEY h_key;

	if(RegOpenKeyEx(hive, key, 0, KEY_READ, &h_key) == ERROR_SUCCESS)
	{
	   query_key(h_key);
	}
	else
	{
		printf("[!] Error shit fucked! - %d\n", GetLastError());
	}

	RegCloseKey(h_key);

	return 0;
}

int query_key(HKEY h_key)
{
	DWORD data_length, name_length, value_length, sub_key_length, index, subkey_index, type, ret;
	DWORD sub_keys_count, max_sub_key, max_name, values_count, max_value, max_value_data;
	DWORD security_descriptor;
	
	TCHAR value[MAX_VALUE_NAME], data[MAX_VALUE], name[MAX_KEY_NAME], sub_key_name[MAX_KEY_NAME];
	
	HKEY key;
	FILETIME last_write_time;
	
	DWORD sub_key_value_count = MAX_VALUE_NAME;
	
	ret = RegQueryInfoKey(h_key, name, &name_length, NULL, &sub_keys_count, &max_sub_key, &max_name, &values_count, &max_value, &max_value_data, &security_descriptor, &last_write_time);

	
	if(sub_keys_count)
	{
		printf("Number of subkeys: %d\n", sub_keys_count);
		
		for(index = 0; index < sub_keys_count; index++)
		{
			name_length = MAX_KEY_NAME;
			
			ret = RegEnumKeyEx(h_key, index, sub_key_name, &sub_key_length, NULL, NULL, NULL, &last_write_time);
			
			if(ret == ERROR_SUCCESS)
			{
				_tprintf(TEXT("(%d) %s\n"), index + 1, sub_key_name);
				printfiletime(&last_write_time);
			}
		}
	}
	
	if(values_count)
	{
		printf("\nNumber of values %d\n", values_count);
		
		for(index = 0, ret = ERROR_SUCCESS; index < values_count; index++)
		{
			sub_key_value_count = MAX_VALUE_NAME;
			sub_key_name[0] = '\0';
			
			ret = RegEnumValue(h_key, index, sub_key_name, &sub_key_value_count, NULL, NULL, data, &data_length);
			
			if(ret == ERROR_SUCCESS)
			{
				_tprintf(TEXT("(%d) %s - %s\n"), index + i, sub_key_name, data);
			}
		}
	}
}

void printfiletime(FILETIME *time)
{
	SYSTEMTIME stUTC, stLocal;


	// Convert the last-write time to local time.
	FileTimeToSystemTime(time, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	// Build a string showing the date and time.
	//dwRet = StringCchPrintf(string, dwSize, TEXT("%02d/%02d/%d  %02d:%02d"), stLocal.wMonth, stLocal.wDay, stLocal.wYear, stLocal.wHour, stLocal.wMinute);
	
	_tprintf("Last Write Time = %02d/%02d/%d  %02d:%02d\n", stLocal.wMonth, stLocal.wDay, stLocal.wYear, stLocal.wHour, stLocal.wMinute);
}

int _tmain (int argc, TCHAR *argv[])
{
	_tprintf(TEXT("Chijiuxing v1.0.0\nJacob Hammack\nhttp://www.hammackj.com\n\n"));
	
	recursively_enum_registry_key_api(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"));
	
	//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server\Wds\rdpwd
	
	return 0;
}




