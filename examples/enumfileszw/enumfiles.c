/*
OH well, looks like I'm replying to myself. I've thrashed out the sample
below and it seemd to work OK. I know there are only 5 or 6 of the
target files in the directory so I know the buffer size of 0x1000 is
plenty big enough. If you have more target files you may want to call
ZwQueryDirectoryFile in a loop, with RestartScan set to TRUE for the
first call and FALSE for subsequent, terminating when the Offset field
of the FILE_DIRECTORY_INFORMATION struct is zero.
*/

int GetFrdFile(void)
{
	HANDLE	FileHandle;
	NTSTATUS status;
	OBJECT_ATTRIBUTES	ourAttributes;
	IO_STATUS_BLOCK	ourStatBlock;
	PVOID		FileInformation;
	BOOLEAN	RestartScan = TRUE;
	UNICODE_STRING		sysDir;
	UNICODE_STRING		fileSpec;
	ULONG		myTag = 0xAABBCCDD;
	WCHAR		fname[30];
	BYTE		*addr;
	PFILE_DIRECTORY_INFORMATION	pDir;
	int		offset;

	RtlInitUnicodeString(&sysDir, L"\\Systemroot\\System32\\");
	RtlInitUnicodeString(&fileSpec, L"*.frd");

	InitializeObjectAttributes(&ourAttributes, &sysDir, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE | OBJ_OPENIF, NULL, NULL);

	status = ZwCreateFile(	&FileHandle, 0x21, &ourAttributes, &ourStatBlock, NULL, 0, 3, 1, 0x21, 0, 0);

	if(!NT_SUCCESS(status)) 
	{
		#if DBG
			DbgPrint("GetFrdFile : ZwCreateFile failed\n");
		#endif
		
		return 10;
	}

	FileInformation = ExAllocatePoolWithTag(NonPagedPool, 0x1000, myTag);

	if(FileInformation == NULL) 
	{
		#if DBG
			DbgPrint("GetFrdFile : ExAllocatePoolWithTag failed\n");
		#endif
		
		return 10;
	}

	// will return a FILE_DIRECTORY_INFORMATION struct for each file
	status = ZwQueryDirectoryFile(FileHandle, NULL, NULL, NULL, &ourStatBlock, FileInformation, 0x1000, FileDirectoryInformation, 0, &fileSpec, RestartScan);

	if(!NT_SUCCESS(status)) 
	{
		#if DBG
			DbgPrint("GetFrdFile : ZwQueryDirectoryFile failed\n");
		#endif
		
		return 10;
	}
	else
	{
		#if DBG
			DbgPrint("GetFrdFile : ZwQueryDirectoryFile got %x bytes\n", ourStatBlock.Information);
		#endif
		addr = (BYTE *)FileInformation;

		do
		{
			pDir = (PFILE_DIRECTORY_INFORMATION)addr;
			memset(fname, 0x00, 2*30);
			wcscpy(fname, pDir->FileName);
			fname[10] = 0x0000;
			#if DBG
				DbgPrint("GetFrdFile : ZwQueryDirectoryFile found matching file :%ws\n", fname);
			#endif
			offset = pDir->NextEntryOffset;
			addr += offset;
		} while(offset != 0);

	}

	ExFreePoolWithTag(FileInformation, myTag);

	return 0;
}