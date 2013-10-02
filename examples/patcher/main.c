#include "globals.h"

int main()
{
	DWORD dwAddress = 0x7CE;

	DWORD bPatch   [] = { 0x7C, 0x16 },
		  bOriginal[] = { 0x75, 0x16 };

	strcpy(szFile, "crackme.exe");

	printf("Trying to patch file [ %s ]...\nCreate backup? y/n", szFile);

	switch(_getch())
	{
		case 'Y':
        case 'y':
		{
			sprintf(szBackupFile, "%s.bak", szFile);
			CopyFile(szFile, szBackupFile, FALSE);

			printf("\nFile backed up as: %s!\n\nStart patching...", szBackupFile);
		}
		break;
              default: printf("\n\nNo backup!\nStart patching..."); break;
    }

	if(!patchFile(dwAddress, bOriginal, bPatch, sizeof(bOriginal)))
		printf("\n   Error!");
	else
		printf("\n   Done!");

	_getch();
	return 0;
}