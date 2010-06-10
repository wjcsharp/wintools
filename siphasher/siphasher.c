/**
 * SIP Hasher v1.0
 * @file
 *
 * 06-26-2008:	JPH	- Created.
 *
 * @author Jacob Hammack
 */
 
#include <windows.h>
#include <stdio.h>

typedef BOOL (WINAPI *CryptCATAdminCalcHashFromFileHandleFunc) (HANDLE, DWORD*, BYTE*, DWORD);

/**
 * Main entry point for DriveList.
 *
 * @author Jacob Hammack
 */
int main(int argc, char *argv[])
{
	CryptCATAdminCalcHashFromFileHandleFunc _CryptCATAdminCalcHashFromFileHandleFunc;
	HINSTANCE LibraryInstance;
	
	printf("SIP Hasher v1.0\nJacob Hammack\nhttp://www.hammackj.com\n\n");

	if(argc != 2) {
	
		printf("");
	}

	LibraryInstance = LoadLibrary("Wintrust.dll");
	
	if(LibraryInstance) {
		_CryptCATAdminCalcHashFromFileHandleFunc = (CryptCATAdminCalcHashFromFileHandleFunc) GetProcAddress(LibraryInstance, "CryptCATAdminCalcHashFromFileHandle");
		
		if(_CryptCATAdminCalcHashFromFileHandleFunc) {
			HANDLE FileToBeProcessed;
			BYTE rgSipHash[20];				
			DWORD cbSipHash = 0;
			char strHash[1024];
			int i;
			int j;
			
			cbSipHash = sizeof( rgSipHash );
			
			FileToBeProcessed = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			
			if(FileToBeProcessed == NULL) {
				printf("Cannot Open File\n");
				
				return;
			}
			
            (_CryptCATAdminCalcHashFromFileHandleFunc) (FileToBeProcessed, &cbSipHash, rgSipHash, 0); 

			for(i = 0; i < cbSipHash; i++ ) 
			{
				sprintf( strHash + i * 2, "%02.0x", rgSipHash[i] );
			}

			for(j = 0; j < cbSipHash * 2; j++ ) 
			{
				if( ' ' == strHash[j] ) 
				{
					strHash[j] = '0';	
				}
			}

			printf("%s\n", strHash);
		}
		else {
			printf("Failed to get Instance of Function.\n");
		}
		
		FreeLibrary(LibraryInstance);
	}
	else {
		printf("Failed to load Wintrust.dll\n");
		
	}
	
	return 0;		
}
