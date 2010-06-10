#define CleanupRoutine(JEWS,DID,WTC) ZwClose(JEWS);/*very appropriate*/\
                                     MemFree(DID,WTC);
void*MemAlloc(size_t size,const unsigned long tag)
{
    void*MemBlock=NULL;
    if (KeGetCurrentIrql() <= APC_LEVEL && tag !=0)
    {
        MemBlock=ExAllocatePoolWithTag(PagedPool,size,tag);
        if(MemBlock!=NULL)
            RtlZeroMemory(MemBlock,size);
    }
    return MemBlock;
}
void MemFree(void*Buffer,const unsigned long tag)
{
    if (KeGetCurrentIrql() <=APC_LEVEL && tag!=0 && Buffer!=NULL)
    {
        try
        {
            ExFreePoolWithTag(Buffer,tag);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            DbgPrint("Invalid Memory\r\n");
        }
    }
}
BOOLEAN IsFileDirectory(unsigned long dwAttributes)
{
        return (BOOLEAN)((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

typedef struct _FILE_BOTH_DIR_INFORMATION {
  ULONG  NextEntryOffset;
  ULONG  FileIndex;
  LARGE_INTEGER  CreationTime;
  LARGE_INTEGER  LastAccessTime;
  LARGE_INTEGER  LastWriteTime;
  LARGE_INTEGER  ChangeTime;
  LARGE_INTEGER  EndOfFile;
  LARGE_INTEGER  AllocationSize;
  ULONG  FileAttributes;
  ULONG  FileNameLength;
  ULONG  EaSize;
  CCHAR  ShortNameLength;
  WCHAR  ShortName[12];
  WCHAR  FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;
NTSTATUS ZwQueryDirectoryFile(
    IN  HANDLE  FileHandle,
    IN OPTIONAL HANDLE  Event,
    IN OPTIONAL PIO_APC_ROUTINE  ApcRoutine,
    IN OPTIONAL PVOID  ApcContext,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    __out_bcount(Length) PVOID  FileInformation,
    IN  ULONG  Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass,
    IN  BOOLEAN  ReturnSingleEntry,
    IN OPTIONAL PUNICODE_STRING  FileName,
    IN  BOOLEAN  RestartScan
    );
typedef struct _FILE_DIRECTORY_INFORMATION { // Information Class 1
ULONG NextEntryOffset;
ULONG Unknown;
LARGE_INTEGER CreationTime;
LARGE_INTEGER LastAccessTime;
LARGE_INTEGER LastWriteTime;
LARGE_INTEGER ChangeTime;
LARGE_INTEGER EndOfFile;
LARGE_INTEGER AllocationSize;
ULONG FileAttributes;
ULONG FileNameLength;
WCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;//disregard this