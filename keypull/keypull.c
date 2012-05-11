/**
 * Keypull v1.1
 * @file
 *
 * 09-30-2008:	JPH - Created.
 * 10-12-2010:  JPH - Removed a bunch of code that wasn't needed, not sure why
 *  it was there.
 *
 * @author Jacob Hammack
 */
 
/**
 * Includes 
 *
 */ 
#include <windows.h>
#include <tchar.h>

/**
 * Forces the compiler to link these libraries
 *
 */
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "Advapi32.lib")

int cstrtot_s(char* string, unsigned int length, TCHAR* output);
int tstrtoc_s(TCHAR* string, unsigned int length, char* output);
void GetCDKey(TCHAR *RegKey);

/**
 *
 */
void DecodeKey(BYTE *IncomingKey, TCHAR *ResultKey);

/**
 * _tmain is a the main for a unicode or ansi program depending on how it is compiled
 *
 * @param argc
 * @param argv
 *
 * @author Jacob Hammack
 */
int _tmain(int argc, TCHAR *argv[])
{
    _tprintf(TEXT("Keypull v1.0\nCopyright 2008 Jacob Hammack\nhttp://www.hammackj.com\n\n"));
 
    {
        TCHAR *Keys[] = 
        { 
            TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),										 /* Windows 2003 */
            TEXT("SOFTWARE\\Microsoft\\Office\\12.0\\Registration\\{90120000-0011-0000-0000-0000000FF1CE}"), /* Office 2007 Plus */
            TEXT("SOFTWARE\\Microsoft\\Office\\12.0\\Registration\\{90120000-0021-0000-0000-0000000FF1CE}"), /* Visio 2007 */
            TEXT("SOFTWARE\\Microsoft\\Office\\12.0\\Registration\\{90120000-0045-0000-0000-0000000FF1CE}"), /* Expression Web 2 */
            TEXT("SOFTWARE\\Microsoft\\Office\\12.0\\Registration\\{90120000-0051-0000-0000-0000000FF1CE}"), /* Project 2007 */
						TEXT("SOFTWARE\\Microsoft\\Office\\11.0\\Registration\\{90110409-6000-11D3-8CFE-0150048383C9}"), /* Not sure */
            NULL
        };
        
        TCHAR **Key = Keys;

        while(*Key) 
        {
            GetCDKey(*Key);
            Key++;
            
        }
    }
    
    return 0;
}

/**
 * Pulls a cdkey and product name from the registry based on the productid
 *
 * @RegKey Registry key to get ProductName and DigitalProductID
 *
 * @author Jacob Hammack
 */
void GetCDKey(TCHAR *RegKey)
{
    TCHAR Input[30];
    TCHAR Output[30];
    BYTE StringBuffer[1024];
    DWORD Type;
    HKEY key;
    LONG value;
    DWORD StringBufferLength;
    int i;

    value = RegOpenKeyEx(HKEY_LOCAL_MACHINE, RegKey, 0, KEY_READ, &key);

    if(value == ERROR_SUCCESS)
    {
        StringBufferLength = 1024 * sizeof(TCHAR);
        
        value = RegQueryValueEx(key, TEXT("ProductName"), NULL, &Type, StringBuffer, &StringBufferLength);
        
        if(value == ERROR_SUCCESS )
        {               
            _tprintf(TEXT("%s\n"), StringBuffer);
        }
        
        StringBufferLength = 1024 * sizeof(TCHAR);
        
        value = RegQueryValueEx(key, TEXT("DigitalProductID"), NULL, &Type, StringBuffer, &StringBufferLength);
        
        if(value == ERROR_SUCCESS )
        {               
            DecodeKey(StringBuffer, Output);
            _tprintf(TEXT("%s\n\n"), Output);
        }

        RegCloseKey(key);
    }
}

/**
 * Decode's a ProductID into a CDKey
 *
 * @author Jacob Hammack
 */
void DecodeKey(BYTE *IncomingKey, TCHAR *ResultKey)
{
    /*Characters avaliable in the Key */
    char Digits[] = 
    { 
        'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'M', 'P', 'Q', 'R', 
        'T', 'V', 'W', 'X', 'Y', '2', '3', '4', '6', '7', '8', '9' 
    };
    
    TCHAR DecodedChars[30];
    const int DecodedLen = 29;
    
    const int EncodedLen = 15;
    const int StartOffset = 52;
    const int EndOffset = StartOffset + 15;
    int i;
    int j;
    BYTE HexDigitalPID[29];
    int Value;
    int HN;
    BYTE inkey[1024];
    
    memcpy(inkey, IncomingKey, 1024);
    
    for(i = StartOffset; i <= EndOffset; i++)
    {
        HexDigitalPID[i - StartOffset] = (BYTE) inkey[i];
    }
      
    for(i = DecodedLen - 1; i >= 0; i--)
    {
        if((i + 1) % 6 == 0)
        {
            DecodedChars[i] = '-';
        }
        else
        {
            HN = 0;
            
            for(j = EncodedLen - 1; j >= 0; j--)
            {
                Value = (HN << 8) | (BYTE) HexDigitalPID[j];
                HexDigitalPID[j] = (BYTE) (Value / 24);
                HN =  Value % 24;
                DecodedChars[i] = Digits[HN];
            }
        }
    }
    
    DecodedChars[DecodedLen] = '\0';
         
    cstrtot_s(DecodedChars, 30, ResultKey);     
}


/**
* Convert C string to TCHAR string
*
* @param string C string to convert
* @param length length of C string
* @param output Resulting TCHAR string
*
* @author Jacob Hammack
*/
int cstrtot_s(char* string, unsigned int length, TCHAR* output)
{
	unsigned int i;

	if(string != NULL && output != NULL && length > 0) {
		int endCount = 0;

		for(i = 0; i < length; i++)
		{
			if(string[i] != '\0') {
				output[i] = string[i];
			}else
			{
				break;
			}
			endCount++;
		}
		//end our string
		output[endCount] = '\0';

		return 1;
	}else
	{
		//indicate failure
		return 0;
	}
}

/**
* Convert TCHAR string to C string
*
* @param string TCHAR string to convert
* @param length length of TCHAR string
* @param output Resulting C string
*
* @author Jacob Hammack
*/
int tstrtoc_s(TCHAR* string, unsigned int length, char* output)
{
	unsigned int i;

	if(string != NULL && output != NULL && length > 0)
	{
		int endCount = 0;

		for(i = 0; i < length; i++)
		{
			if(string[i] != '\0')
			{
				output[i] = (char)string[i];
			}else
			{
				break;
			}
			endCount++;

		}

		//end our string
		output[endCount] = TEXT('\0');

		return 1;
	}else
	{
		//indicate failure
		return 0;
	}
}
