//来自 瑞星公司 
//very good , a simple SDT HOOK example .
//Instead of clearing wp-bit in cr0, use MmMapLockedPagesSpecifyCache.


#include <ntddk.h> 
#include <ntimage.h> 
#pragma pack(1) 
typedef struct ServiceDescriptorEntry { 
unsigned int *ServiceTableBase; 
unsigned int *ServiceCounterTableBase; //Used only in checked build 
unsigned int NumberOfServices; 
unsigned char *ParamTableBase; 
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t; 
#pragma pack() 

__declspec(dllimport) ServiceDescriptorTableEntry_t KeServiceDescriptorTable; 
#define SYSTEMSERVICE(_function) KeServiceDescriptorTable.ServiceTableBase[ *(PULONG)((PUCHAR)_function+1)] 
#define SDT SYSTEMSERVICE 
#define KSDT KeServiceDescriptorTable 
#define FILE_DEVICE_UNKNOWN 0x00000022 
#define IOCTL_UNKNOWN_BASE FILE_DEVICE_UNKNOWN 
#define IOCTL_INIT CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 
//补充定义数据及结构 
typedef struct _INITIAL_TEB { 
PVOID StackBase; 
PVOID StackLimit; 
PVOID StackCommit; 
PVOID StackCommitMax; 
PVOID StackReserved; 
} INITIAL_TEB, *PINITIAL_TEB; 
typedef enum _SYSTEM_INFORMATION_CLASS 
{ 
SystemBasicInformation, 
SystemProcessorInformation, 
SystemPerformanceInformation, 
SystemTimeOfDayInformation, 
SystemNotImplemented1, 
SystemProcessesAndThreadsInformation, 
SystemCallCounts, 
SystemConfigurationInformation, 
SystemProcessorTimes, 
SystemGlobalFlag, 
SystemNotImplemented2, 
SystemModuleInformation, 
SystemLockInformation, 
SystemNotImplemented3, 
SystemNotImplemented4, 
SystemNotImplemented5, 
SystemHandleInformation, 
SystemObjectInformation, 
SystemPagefileInformation, 
SystemInstructionEmulationCounts, 
SystemInvalidInfoClass1, 
SystemCacheInformation, 
SystemPoolTagInformation, 
SystemProcessorStatistics, 
SystemDpcInformation, 
SystemNotImplemented6, 
SystemLoadImage, 
SystemUnloadImage, 
SystemTimeAdjustment, 
SystemNotImplemented7, 
SystemNotImplemented8, 
SystemNotImplemented9, 
SystemCrashDumpInformation, 
SystemExceptionInformation, 
SystemCrashDumpStateInformation, 
SystemKernelDebuggerInformation, 
SystemContextSwitchInformation, 
SystemRegistryQuotaInformation, 
SystemLoadAndCallImage, 
SystemPrioritySeparation, 
SystemNotImplemented10, 
SystemNotImplemented11, 
SystemInvalidInfoClass2, 
SystemInvalidInfoClass3, 
SystemTimeZoneInformation, 
SystemLookasideInformation, 
SystemSetTimeSlipEvent, 
SystemCreateSession, 
SystemDeleteSession, 
SystemInvalidInfoClass4, 
SystemRangeStartInformation, 
SystemVerifierInformation, 
SystemAddVerifier, 
SystemSessionProcessesInformation 
} SYSTEM_INFORMATION_CLASS; 
typedef struct _SYSTEM_HANDLE_INFORMATION 
{ 
ULONG ProcessId; 
UCHAR ObjectTypeNumber; 
UCHAR Flags; 
USHORT Handle; 
PVOID Object; 
ACCESS_MASK GrantedAccess; 
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION; 

//私有变量 
typedef struct _DEVICE_EXTENSION 
{ 
PDEVICE_OBJECT DeviceObject; 
PKEVENT Event; 

BOOLEAN bPCreate; 
} DEVICE_EXTENSION, *PDEVICE_EXTENSION; 


// 全局设备对象 
PDEVICE_OBJECT g_pDeviceObject; 

UNICODE_STRING g_RegPath; 

//补充定义函数 
NTKERNELAPI NTSTATUS ObQueryNameString ( 
IN PVOID Object, 
IN OUT PUNICODE_STRING Name, 
IN ULONG MaximumLength, 
OUT PULONG ActualLength 
); 
NTKERNELAPI NTSTATUS ZwSetSecurityObject( 
IN HANDLE Handle, 
IN SECURITY_INFORMATION SecurityInformation, 
IN PSECURITY_DESCRIPTOR SecurityDescriptor 
); 
NTKERNELAPI NTSTATUS ZwTerminateProcess( 
IN HANDLE ProcessHandle OPTIONAL, 
IN NTSTATUS ExitStatus ); 
NTKERNELAPI NTSTATUS ZwOpenProcess( 
OUT PHANDLE ProcessHandle, 
IN ACCESS_MASK AccessMask, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN PCLIENT_ID ClientId ); 
NTKERNELAPI NTSTATUS ZwOpenThread( 
OUT PHANDLE ThreadHandle, 
IN ACCESS_MASK AccessMask, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN PCLIENT_ID ClientId ); 
NTKERNELAPI NTSTATUS ZwLoadDriver( 
IN PUNICODE_STRING DriverServiceName ); 
NTKERNELAPI NTSTATUS ZwSetSystemInformation( 
IN SYSTEM_INFORMATION_CLASS SystemInformationClass, 
IN PVOID SystemInformation, 
IN ULONG SystemInformationLength ); 
NTKERNELAPI NTSTATUS ZwQuerySystemInformation( 
IN SYSTEM_INFORMATION_CLASS SystemInformationClass, 
OUT PVOID SystemInformation, 
IN ULONG SystemInformationLength, 
OUT PULONG ReturnLength OPTIONAL ); 


//函数声明 
NTSTATUS DriverEntry( 
IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath); 
void UnloadDriver(PDRIVER_OBJECT DriverObject); 
NTSTATUS DispatchCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp); 
NTSTATUS DispatchClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp); 
NTSTATUS DispatchIoCtrl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp); 
void StartHook(void); 
void RemoveHook(void); 
NTSTATUS Hook_ZwWriteFile( 
IN HANDLE FileHandle, 
IN HANDLE Event OPTIONAL, 
IN PIO_APC_ROUTINE ApcRoutine OPTIONAL, 
IN PVOID ApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
IN PVOID Buffer, 
IN ULONG Length, 
IN PLARGE_INTEGER ByteOffset OPTIONAL, 
IN PULONG Key OPTIONAL ); 
NTSTATUS Hook_ZwReadFile( 
IN HANDLE FileHandle, 
IN HANDLE Event OPTIONAL, 
IN PIO_APC_ROUTINE ApcRoutine OPTIONAL, 
IN PVOID ApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
OUT PVOID Buffer, 
IN ULONG Length, 
IN PLARGE_INTEGER ByteOffset OPTIONAL, 
IN PULONG Key OPTIONAL ); 
NTSTATUS Hook_ZwSetSystemInformation( 
IN SYSTEM_INFORMATION_CLASS SystemInformationClass, 
IN PVOID SystemInformation, 
IN ULONG SystemInformationLength ); 
NTSTATUS Hook_ZwQuerySystemInformation( 
IN SYSTEM_INFORMATION_CLASS SystemInformationClass, 
OUT PVOID SystemInformation, 
IN ULONG SystemInformationLength, 
OUT PULONG ReturnLength OPTIONAL ); 
NTSTATUS Hook_ZwLoadDriver( 
IN PUNICODE_STRING DriverServiceName ); 
NTSTATUS Hook_ZwSetSecurityObject( 
IN HANDLE ObjectHandle, 
IN SECURITY_INFORMATION SecurityInformationClass, 
IN PSECURITY_DESCRIPTOR DescriptorBuffer); 
NTSTATUS Hook_ZwOpenKey( 
OUT PHANDLE KeyHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes); 
NTSTATUS Hook_ZwCreateKey ( 
OUT PHANDLE KeyHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN ULONG TitleIndex, 
IN PUNICODE_STRING Class OPTIONAL, 
IN ULONG CreateOptions, 
OUT PULONG Disposition OPTIONAL); 
NTSTATUS Hook_ZwSetValueKey( 
IN HANDLE KeyHandle, 
IN PUNICODE_STRING ValueName, 
IN ULONG TitleIndex OPTIONAL, 
IN ULONG Type, 
IN PVOID Data, 
IN ULONG DataSize); 
NTSTATUS Hook_ZwDeleteKey( 
IN HANDLE KeyHandle); 
NTSTATUS Hook_ZwDeleteValueKey( 
IN HANDLE KeyHandle, IN PUNICODE_STRING ValueName); 
NTSTATUS Hook_ZwOpenSection( 
OUT PHANDLE SectionHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes ); 
NTSTATUS Hook_ZwCreateSection( 
OUT PHANDLE SectionHandle, 
IN ULONG DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
IN PLARGE_INTEGER MaximumSize OPTIONAL, 
IN ULONG PageAttributess, 
IN ULONG SectionAttributes, 
IN HANDLE FileHandle OPTIONAL ); 
NTSTATUS Hook_ZwCreateProcess( 
OUT PHANDLE ProcessHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
IN HANDLE ParentProcess, 
IN BOOLEAN InheritObjectTable, 
IN HANDLE SectionHandle OPTIONAL, 
IN HANDLE DebugPort OPTIONAL, 
IN HANDLE ExceptionPort OPTIONAL ); 
NTSTATUS Hook_ZwCreateProcessEx( 
OUT PHANDLE ProcessHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
IN HANDLE ParentProcess, 
IN BOOLEAN InheritObjectTable, 
IN HANDLE SectionHandle OPTIONAL, 
IN HANDLE DebugPort OPTIONAL, 
IN HANDLE ExceptionPort OPTIONAL, 
IN HANDLE UnknownHandle ); 
NTSTATUS Hook_ZwTerminateProcess( 
IN HANDLE ProcessHandle OPTIONAL, 
IN NTSTATUS ExitStatus ); 
NTSTATUS Hook_ZwOpenProcess( 
OUT PHANDLE ProcessHandle, 
IN ACCESS_MASK AccessMask, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN PCLIENT_ID ClientId ); 
NTSTATUS Hook_ZwCreateThread( 
OUT PHANDLE ThreadHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
IN HANDLE ProcessHandle, 
OUT PCLIENT_ID ClientId, 
IN PCONTEXT ThreadContext, 
IN PINITIAL_TEB InitialTeb, 
IN BOOLEAN CreateSuspended ); 
NTSTATUS Hook_ZwTerminateThread( 
IN HANDLE ThreadHandle, 
IN NTSTATUS ExitStatus ); 
NTSTATUS Hook_ZwOpenThread( 
OUT PHANDLE ThreadHandle, 
IN ACCESS_MASK AccessMask, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN PCLIENT_ID ClientId ); 
NTSTATUS Hook_ZwCreateFile( 
OUT PHANDLE FileHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
IN PLARGE_INTEGER AllocationSize OPTIONAL, 
IN ULONG FileAttributes, 
IN ULONG ShareAccess, 
IN ULONG CreateDisposition, 
IN ULONG CreateOptions, 
IN PVOID EaBuffer OPTIONAL, 
IN ULONG EaLength ); 
NTSTATUS Hook_ZwOpenFile( 
OUT PHANDLE FileHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
IN ULONG ShareAccess, 
IN ULONG OpenOptions ); 


NTSTATUS Hook_ZwClose( 
IN HANDLE ObjectHandle ); 
#ifdef ALLOC_PRAGMA 
#pragma alloc_text(INIT, DriverEntry) 
#pragma alloc_text(INIT, StartHook) 
#pragma alloc_text(PAGE, DispatchCreate) 
#pragma alloc_text(PAGE, DispatchClose) 
#pragma alloc_text(PAGE, DispatchIoCtrl) 
#pragma alloc_text(PAGE, RemoveHook) 
#pragma alloc_text(PAGE, UnloadDriver) 
#pragma alloc_text(PAGE, Hook_ZwOpenKey) 
#pragma alloc_text(PAGE, Hook_ZwSetSecurityObject) 
#pragma alloc_text(PAGE, Hook_ZwCreateKey) 
#pragma alloc_text(PAGE, Hook_ZwSetValueKey) 
#pragma alloc_text(PAGE, Hook_ZwDeleteKey) 
#pragma alloc_text(PAGE, Hook_ZwDeleteValueKey) 
#pragma alloc_text(PAGE, Hook_ZwOpenSection) 
#pragma alloc_text(PAGE, Hook_ZwCreateSection) 
#pragma alloc_text(PAGE, Hook_ZwOpenProcess) 
#pragma alloc_text(PAGE, Hook_ZwTerminateProcess) 
#pragma alloc_text(PAGE, Hook_ZwOpenThread) 
#pragma alloc_text(PAGE, Hook_ZwCreateFile) 
#pragma alloc_text(PAGE, Hook_ZwOpenFile) 
#pragma alloc_text(PAGE, Hook_ZwClose) 
#pragma alloc_text(PAGE, Hook_ZwLoadDriver) 
#pragma alloc_text(PAGE, Hook_ZwSetSystemInformation) 
#pragma alloc_text(PAGE, Hook_ZwQuerySystemInformation) 
#pragma alloc_text(PAGE, Hook_ZwReadFile) 
#pragma alloc_text(PAGE, Hook_ZwWriteFile) 
#endif 

//函数原型定义 
typedef NTSTATUS (*ZWLOADDRIVER)( 
IN PUNICODE_STRING DriverServiceName ); 

typedef NTSTATUS (*ZWCREATEFILE)( 
OUT PHANDLE FileHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
IN PLARGE_INTEGER AllocationSize OPTIONAL, 
IN ULONG FileAttributes, 
IN ULONG ShareAccess, 
IN ULONG CreateDisposition, 
IN ULONG CreateOptions, 
IN PVOID EaBuffer OPTIONAL, 
IN ULONG EaLength ); 

typedef NTSTATUS (*ZWOPENFILE)( 
OUT PHANDLE FileHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
IN ULONG ShareAccess, 
IN ULONG OpenOptions ); 

typedef NTSTATUS (*ZWCLOSE)( 
IN HANDLE ObjectHandle ); 

typedef NTSTATUS (*ZWWRITEFILE)( 
IN HANDLE FileHandle, 
IN HANDLE Event OPTIONAL, 
IN PIO_APC_ROUTINE ApcRoutine OPTIONAL, 
IN PVOID ApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
IN PVOID Buffer, 
IN ULONG Length, 
IN PLARGE_INTEGER ByteOffset OPTIONAL, 
IN PULONG Key OPTIONAL ); 

typedef NTSTATUS (*ZWREADFILE)( 
IN HANDLE FileHandle, 
IN HANDLE Event OPTIONAL, 
IN PIO_APC_ROUTINE ApcRoutine OPTIONAL, 
IN PVOID ApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
OUT PVOID Buffer, 
IN ULONG Length, 
IN PLARGE_INTEGER ByteOffset OPTIONAL, 
IN PULONG Key OPTIONAL ); 

typedef NTSTATUS (*ZWCREATEPROCESS)( 
OUT PHANDLE ProcessHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
IN HANDLE ParentProcess, 
IN BOOLEAN InheritObjectTable, 
IN HANDLE SectionHandle OPTIONAL, 
IN HANDLE DebugPort OPTIONAL, 
IN HANDLE ExceptionPort OPTIONAL); 

typedef NTSTATUS (*ZWCREATEPROCESSEX)( 
OUT PHANDLE ProcessHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
IN HANDLE ParentProcess, 
IN BOOLEAN InheritObjectTable, 
IN HANDLE SectionHandle OPTIONAL, 
IN HANDLE DebugPort OPTIONAL, 
IN HANDLE ExceptionPort OPTIONAL, 
IN HANDLE Unknown ); 

typedef NTSTATUS (*ZWOPENPROCESS)( 
OUT PHANDLE ProcessHandle, 
IN ACCESS_MASK AccessMask, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN PCLIENT_ID ClientId ); 

typedef NTSTATUS (*ZWTERMINATEPROCESS)( 
IN HANDLE ProcessHandle OPTIONAL, 
IN NTSTATUS ExitStatus ); 

typedef NTSTATUS (*ZWCREATETHREAD)( 
OUT PHANDLE ThreadHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
IN HANDLE ProcessHandle, 
OUT PCLIENT_ID ClientId, 
IN PCONTEXT ThreadContext, 
IN PINITIAL_TEB InitialTeb, 
IN BOOLEAN CreateSuspended ); 

typedef NTSTATUS (*ZWTERMINATETHREAD)( 
IN HANDLE ThreadHandle, 
IN NTSTATUS ExitStatus ); 

typedef NTSTATUS (*ZWOPENTHREAD)( 
OUT PHANDLE ThreadHandle, 
IN ACCESS_MASK AccessMask, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN PCLIENT_ID ClientId ); 

typedef NTSTATUS (*ZWCREATESECTION)( 
OUT PHANDLE SectionHandle, 
IN ULONG DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
IN PLARGE_INTEGER MaximumSize OPTIONAL, 
IN ULONG PageAttributess, 
IN ULONG SectionAttributes, 
IN HANDLE FileHandle OPTIONAL ); 

typedef NTSTATUS (*ZWOPENSECTION)( 
OUT PHANDLE SectionHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes ); 

//注册表 

typedef NTSTATUS (*ZWCREATEKEY) ( 
OUT PHANDLE KeyHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN ULONG TitleIndex, 
IN PUNICODE_STRING Class OPTIONAL, 
IN ULONG CreateOptions, 
OUT PULONG Disposition OPTIONAL 
); 

typedef NTSTATUS (*ZWOPENKEY) ( 
OUT PHANDLE KeyHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes 
); 

typedef NTSTATUS (*ZWSETVALUEKEY)( 
IN HANDLE KeyHandle, 
IN PUNICODE_STRING ValueName, 
IN ULONG TitleIndex OPTIONAL, 
IN ULONG Type, 
IN PVOID Data, 
IN ULONG DataSize 
); 

typedef NTSTATUS (*ZWSETSECURITYOBJECT)( 
IN HANDLE ObjectHandle, 
IN SECURITY_INFORMATION SecurityInformationClass, 
IN PSECURITY_DESCRIPTOR DescriptorBuffer); 


typedef NTSTATUS (*ZWDELETEKEY)( 
IN HANDLE KeyHandle); 
typedef NTSTATUS (*ZWDELETEVALUEKEY)( 
IN HANDLE KeyHandle, IN PUNICODE_STRING ValueName); 


typedef NTSTATUS (*ZWSETSYSTEMINFORMATION)( 
IN SYSTEM_INFORMATION_CLASS SystemInformationClass, 
IN PVOID SystemInformation, 
IN ULONG SystemInformationLength ); 

typedef NTSTATUS (*ZWQUERYSYSTEMINFORMATION)( 
IN SYSTEM_INFORMATION_CLASS SystemInformationClass, 
OUT PVOID SystemInformation, 
IN ULONG SystemInformationLength, 
OUT PULONG ReturnLength OPTIONAL ); 

// SDT 原函数地址 
static ZWCREATEFILE OldZwCreateFile; 
static ZWOPENFILE OldZwOpenFile; 
static ZWCLOSE OldZwClose; 
static ZWWRITEFILE OldZwWriteFile; 
static ZWREADFILE OldZwReadFile; 
static ZWTERMINATEPROCESS OldZwTerminateProcess; 
static ZWOPENPROCESS OldZwOpenProcess; 
static ZWOPENTHREAD OldZwOpenThread; 
static ZWCREATESECTION OldZwCreateSection; 
static ZWOPENSECTION OldZwOpenSection; 
static ZWCREATEKEY OldZwCreateKey; 
static ZWSETVALUEKEY OldZwSetValueKey; 
static ZWDELETEKEY OldZwDeleteKey; 
static ZWDELETEVALUEKEY OldZwDeleteValueKey; 
static ZWSETSECURITYOBJECT OldZwSetSecurityObject; 
static ZWOPENKEY OldZwOpenKey; 

static ZWLOADDRIVER OldZwLoadDriver; 

static ZWSETSYSTEMINFORMATION OldZwSetSystemInformation; 
static ZWQUERYSYSTEMINFORMATION OldZwQuerySystemInformation; 
//挂接函数执行体 
NTSTATUS Hook_ZwWriteFile( 
IN HANDLE FileHandle, 
IN HANDLE Event OPTIONAL, 
IN PIO_APC_ROUTINE ApcRoutine OPTIONAL, 
IN PVOID ApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
IN PVOID Buffer, 
IN ULONG Length, 
IN PLARGE_INTEGER ByteOffset OPTIONAL, 
IN PULONG Key OPTIONAL ) 
{ 
NTSTATUS rc; 
rc = OldZwWriteFile(FileHandle,Event,ApcRoutine,ApcContext,IoStatusBlock,Buffer,Length,ByteOffset,Key); 
return rc; 
} 
NTSTATUS Hook_ZwReadFile( 
IN HANDLE FileHandle, 
IN HANDLE Event OPTIONAL, 
IN PIO_APC_ROUTINE ApcRoutine OPTIONAL, 
IN PVOID ApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
OUT PVOID Buffer, 
IN ULONG Length, 
IN PLARGE_INTEGER ByteOffset OPTIONAL, 
IN PULONG Key OPTIONAL ) 
{ 
NTSTATUS rc; 
rc = OldZwReadFile(FileHandle,Event,ApcRoutine,ApcContext,IoStatusBlock,Buffer,Length,ByteOffset,Key); 
return rc; 
} 

NTSTATUS Hook_ZwSetSystemInformation( 
IN SYSTEM_INFORMATION_CLASS SystemInformationClass, 
IN PVOID SystemInformation, 
IN ULONG SystemInformationLength ) 
{ 
NTSTATUS rc; 
rc = OldZwSetSystemInformation(SystemInformationClass,SystemInformation,SystemInformationLength); 
return rc; 
} 

NTSTATUS Hook_ZwQuerySystemInformation( 
IN SYSTEM_INFORMATION_CLASS SystemInformationClass, 
OUT PVOID SystemInformation, 
IN ULONG SystemInformationLength, 
OUT PULONG ReturnLength OPTIONAL ) 
{ 
NTSTATUS rc; 
rc = OldZwQuerySystemInformation(SystemInformationClass,SystemInformation,SystemInformationLength,ReturnLength); 
return rc; 
} 

NTSTATUS Hook_ZwLoadDriver( 
IN PUNICODE_STRING DriverServiceName ) 
{ 
NTSTATUS rc; 

rc = OldZwLoadDriver(DriverServiceName); 
return rc; 
} 

NTSTATUS Hook_ZwSetSecurityObject( 
IN HANDLE ObjectHandle, 
IN SECURITY_INFORMATION SecurityInformationClass, 
IN PSECURITY_DESCRIPTOR DescriptorBuffer) 
{ 

NTSTATUS rc; 

rc = OldZwSetSecurityObject(ObjectHandle,SecurityInformationClass,DescriptorBuffer); 

return rc; 
} 


NTSTATUS Hook_ZwOpenKey( 
OUT PHANDLE KeyHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes) 
{ 
NTSTATUS rc; 

rc = OldZwOpenKey(KeyHandle,DesiredAccess,ObjectAttributes); 

return rc; 
} 

NTSTATUS Hook_ZwCreateKey ( 
OUT PHANDLE KeyHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN ULONG TitleIndex, 
IN PUNICODE_STRING Class OPTIONAL, 
IN ULONG CreateOptions, 
OUT PULONG Disposition OPTIONAL 
) 
{ 
NTSTATUS rc; 

rc = OldZwCreateKey(KeyHandle, DesiredAccess, ObjectAttributes, 
TitleIndex, Class, CreateOptions, Disposition); 

return rc; 
} 

NTSTATUS Hook_ZwSetValueKey( 
IN HANDLE KeyHandle, 
IN PUNICODE_STRING ValueName, 
IN ULONG TitleIndex OPTIONAL, 
IN ULONG Type, 
IN PVOID Data, 
IN ULONG DataSize 
) 
{ 
NTSTATUS rc; 


rc = OldZwSetValueKey(KeyHandle,ValueName,TitleIndex,Type,Data,DataSize); 

return rc; 
} 


NTSTATUS Hook_ZwDeleteKey(IN HANDLE KeyHandle) 
{ 
NTSTATUS rc; 

rc = OldZwDeleteKey(KeyHandle); 

return rc; 
} 


NTSTATUS Hook_ZwDeleteValueKey( IN HANDLE KeyHandle, IN PUNICODE_STRING ValueName) 
{ 
NTSTATUS rc; 

rc = OldZwDeleteValueKey(KeyHandle,ValueName); 

return rc; 
} 

NTSTATUS Hook_ZwOpenSection( 
OUT PHANDLE SectionHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes ) 
{ 
NTSTATUS rc; 
// DbgPrint("Hook_ZwOpenSection\n"); 
rc = OldZwOpenSection(SectionHandle,DesiredAccess,ObjectAttributes); 
return rc; 
} 

NTSTATUS Hook_ZwCreateSection( 
OUT PHANDLE SectionHandle, 
IN ULONG DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
IN PLARGE_INTEGER MaximumSize OPTIONAL, 
IN ULONG PageAttributess, 
IN ULONG SectionAttributes, 
IN HANDLE FileHandle OPTIONAL ) 
{ 
NTSTATUS rc; 
// DbgPrint("Hook_ZwCreateSection"); 
return OldZwCreateSection(SectionHandle,DesiredAccess,ObjectAttributes, 
MaximumSize,PageAttributess,SectionAttributes,FileHandle); 
return rc; 
} 


NTSTATUS Hook_ZwTerminateProcess( 
IN HANDLE ProcessHandle OPTIONAL, 
IN NTSTATUS ExitStatus ) 
{ 
NTSTATUS rc; 

rc = OldZwTerminateProcess(ProcessHandle,ExitStatus); 
return rc; 
} 

NTSTATUS Hook_ZwOpenProcess( 
OUT PHANDLE ProcessHandle, 
IN ACCESS_MASK AccessMask, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN PCLIENT_ID ClientId ) 

{ 
NTSTATUS rc; 

rc = OldZwOpenProcess(ProcessHandle,AccessMask,ObjectAttributes,ClientId); 
return rc; 
} 


NTSTATUS Hook_ZwOpenThread( 
OUT PHANDLE ThreadHandle, 
IN ACCESS_MASK AccessMask, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
IN PCLIENT_ID ClientId ) 
{ 
NTSTATUS rc; 

rc = OldZwOpenThread(ThreadHandle,AccessMask,ObjectAttributes,ClientId); 

return rc; 
} 

NTSTATUS Hook_ZwCreateFile( 
OUT PHANDLE FileHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
IN PLARGE_INTEGER AllocationSize OPTIONAL, 
IN ULONG FileAttributes, 
IN ULONG ShareAccess, 
IN ULONG CreateDisposition, 
IN ULONG CreateOptions, 
IN PVOID EaBuffer OPTIONAL, 
IN ULONG EaLength ) 
{ 
NTSTATUS rc; 

rc = OldZwCreateFile(FileHandle,DesiredAccess,ObjectAttributes,IoStatusBlock, 
AllocationSize,FileAttributes,ShareAccess,CreateDisposition, 
CreateOptions,EaBuffer,EaLength); 

return rc; 
} 

NTSTATUS Hook_ZwOpenFile( 
OUT PHANDLE FileHandle, 
IN ACCESS_MASK DesiredAccess, 
IN POBJECT_ATTRIBUTES ObjectAttributes, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
IN ULONG ShareAccess, 
IN ULONG OpenOptions ) 
{ 
NTSTATUS rc; 

rc = OldZwOpenFile(FileHandle,DesiredAccess,ObjectAttributes,IoStatusBlock,ShareAccess, 
OpenOptions); 

return rc; 
} 


NTSTATUS Hook_ZwClose( 
IN HANDLE ObjectHandle ) 
{ 
NTSTATUS rc; 

//在这里执行扫描必须十分注意,否则容易蓝屏 

rc = OldZwClose(ObjectHandle); 
return rc; 
} 
NTSTATUS DriverEntry( 
IN PDRIVER_OBJECT DriverObject, 
IN PUNICODE_STRING RegistryPath 
) 
{ 
NTSTATUS ntStatus; 
UNICODE_STRING uszDriverString; 
UNICODE_STRING uszDeviceString; 
UNICODE_STRING uszEventString; 
PDEVICE_OBJECT pDeviceObject; 
PDEVICE_EXTENSION extension; 
// 初始化设备对象名 
RtlInitUnicodeString(&uszDriverString, L"\\Device\\ITSys"); 
// 创建并初始化对象 
ntStatus = IoCreateDevice( 
DriverObject, 
sizeof(DEVICE_EXTENSION), 
&uszDriverString, 
FILE_DEVICE_UNKNOWN, 
0, 
FALSE, 
&pDeviceObject 
); 
if(ntStatus != STATUS_SUCCESS) 
return ntStatus; 
extension = pDeviceObject->DeviceExtension; 
RtlInitUnicodeString(&uszDeviceString, L"\\DosDevices\\ITSys"); 
// 创建用户可见连接名称 
ntStatus = IoCreateSymbolicLink(&uszDeviceString, &uszDriverString); 
if(ntStatus != STATUS_SUCCESS) 
{ 
// 创建失败，删除对象并返回错误值 
IoDeleteDevice(pDeviceObject); 
return ntStatus; 
} 
// 赋值全局设备对象指针 

// Assign global pointer to the device object for use by the callback functions 
g_pDeviceObject = pDeviceObject; 
// 设置所有可用的DeviceIoControl的处理IRP的函数 

DriverObject->DriverUnload = UnloadDriver; 
DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate; 
DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose; 
DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoCtrl; 

#if DBG 
KdPrint(("RegistryPath : %ws\n",RegistryPath->Buffer)); 
#endif 

//SDT挂接 
StartHook(); 

return ntStatus; 
} 

void StartHook (void) 
{ 
//获取未导出的服务函数索引号 
HANDLE hFile; 
PCHAR pDllFile; 
ULONG ulSize; 
ULONG ulByteReaded; 

__asm 
{ 
push eax 
mov eax, CR0 
and eax, 0FFFEFFFFh 
mov CR0, eax 
pop eax 
} 
//挂接SDT函数 
OldZwCreateFile = (ZWCREATEFILE) InterlockedExchange((PLONG) 
&SDT(ZwCreateFile), 
(LONG)Hook_ZwCreateFile); 
OldZwOpenFile = (ZWOPENFILE) InterlockedExchange((PLONG) 
&SDT(ZwOpenFile), 
(LONG)Hook_ZwOpenFile); 
OldZwClose = (ZWCLOSE) InterlockedExchange((PLONG) 
&SDT(ZwClose), 
(LONG)Hook_ZwClose); 
OldZwReadFile = (ZWREADFILE) InterlockedExchange((PLONG) 
&SDT(ZwReadFile), 
(LONG)Hook_ZwReadFile); 
OldZwWriteFile = (ZWWRITEFILE) InterlockedExchange((PLONG) 
&SDT(ZwWriteFile), 
(LONG)Hook_ZwWriteFile); 

OldZwTerminateProcess = (ZWTERMINATEPROCESS)InterlockedExchange((PLONG) 
&SDT(ZwTerminateProcess), 
(LONG)Hook_ZwTerminateProcess); 

OldZwOpenProcess = (ZWOPENPROCESS)InterlockedExchange((PLONG) 
&SDT(ZwOpenProcess), 
(LONG)Hook_ZwOpenProcess); 

OldZwOpenThread = (ZWOPENTHREAD)InterlockedExchange((PLONG) 
&SDT(ZwOpenThread), 
(LONG)Hook_ZwOpenThread); 
OldZwCreateSection = (ZWCREATESECTION)InterlockedExchange((PLONG) 
&SDT(ZwCreateSection), 
(LONG)Hook_ZwCreateSection); 
OldZwOpenSection = (ZWOPENSECTION)InterlockedExchange((PLONG) 
&SDT(ZwOpenSection), 
(LONG)Hook_ZwOpenSection); 

OldZwOpenKey = (ZWOPENKEY) InterlockedExchange((PLONG) 
&SDT(ZwOpenKey), 
(LONG)Hook_ZwOpenKey); 
OldZwCreateKey = (ZWCREATEKEY) InterlockedExchange((PLONG) 
&SDT(ZwCreateKey), 
(LONG)Hook_ZwCreateKey); 
OldZwSetValueKey = (ZWSETVALUEKEY) InterlockedExchange((PLONG) 
&SDT(ZwSetValueKey), 
(LONG)Hook_ZwSetValueKey); 
OldZwDeleteKey = (ZWDELETEKEY) InterlockedExchange((PLONG) 
&SDT(ZwDeleteKey), 
(LONG)Hook_ZwDeleteKey); 
OldZwDeleteValueKey = (ZWDELETEVALUEKEY) InterlockedExchange((PLONG) 
&SDT(ZwDeleteValueKey), 
(LONG)Hook_ZwDeleteValueKey); 
OldZwSetSecurityObject = (ZWSETSECURITYOBJECT)InterlockedExchange((PLONG) 
&SDT(ZwSetSecurityObject), 
(LONG)Hook_ZwSetSecurityObject); 

OldZwLoadDriver = (ZWLOADDRIVER)InterlockedExchange((PLONG) 
&SDT(ZwLoadDriver), 
(LONG)Hook_ZwLoadDriver); 

OldZwSetSystemInformation = (ZWSETSYSTEMINFORMATION)InterlockedExchange((PLONG) 
&SDT(ZwSetSystemInformation), 
(LONG)Hook_ZwSetSystemInformation); 

OldZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)InterlockedExchange((PLONG) 
&SDT(ZwQuerySystemInformation), 
(LONG)Hook_ZwQuerySystemInformation); 

//关闭 
__asm 
{ 
push eax 
mov eax, CR0 
or eax, NOT 0FFFEFFFFh 
mov CR0, eax 
pop eax 
} 
return ; 
} 

void RemoveHook (void) 
{ 
__asm 
{ 
push eax 
mov eax, CR0 
and eax, 0FFFEFFFFh 
mov CR0, eax 
pop eax 
} 

InterlockedExchange( (PLONG) &SDT(ZwCreateFile) , (LONG) OldZwCreateFile ); 
InterlockedExchange( (PLONG) &SDT(ZwOpenFile) , (LONG) OldZwOpenFile ); 
InterlockedExchange( (PLONG) &SDT(ZwClose) , (LONG) OldZwClose ); 
InterlockedExchange( (PLONG) &SDT(ZwReadFile) , (LONG) OldZwReadFile ); 
InterlockedExchange( (PLONG) &SDT(ZwWriteFile) , (LONG) OldZwWriteFile ); 

InterlockedExchange( (PLONG) &SDT(ZwTerminateProcess) , (LONG) OldZwTerminateProcess ); 
InterlockedExchange( (PLONG) &SDT(ZwOpenProcess) , (LONG) OldZwOpenProcess ); 
InterlockedExchange( (PLONG) &SDT(ZwOpenThread) , (LONG) OldZwOpenThread ); 
InterlockedExchange( (PLONG) &SDT(ZwCreateSection) , (LONG) OldZwCreateSection ); 
InterlockedExchange( (PLONG) &SDT(ZwOpenSection) , (LONG) OldZwOpenSection ); 

InterlockedExchange( (PLONG) &SDT(ZwOpenKey) , (LONG) OldZwOpenKey ); 
InterlockedExchange( (PLONG) &SDT(ZwCreateKey) , (LONG) OldZwCreateKey ); 
InterlockedExchange( (PLONG) &SDT(ZwSetValueKey) , (LONG) OldZwSetValueKey ); 
InterlockedExchange( (PLONG) &SDT(ZwDeleteKey) , (LONG) OldZwDeleteKey ); 
InterlockedExchange( (PLONG) &SDT(ZwDeleteValueKey) , (LONG) OldZwDeleteValueKey ); 
InterlockedExchange( (PLONG) &SDT(ZwSetSecurityObject) , (LONG) OldZwSetSecurityObject ); 

InterlockedExchange( (PLONG) &SDT(ZwLoadDriver) , (LONG) OldZwLoadDriver ); 

InterlockedExchange( (PLONG) &SDT(ZwSetSystemInformation) , (LONG) OldZwSetSystemInformation ); 
InterlockedExchange( (PLONG) &SDT(ZwQuerySystemInformation) , (LONG) OldZwQuerySystemInformation ); 

__asm 
{ 
push eax 
mov eax, CR0 
or eax, NOT 0FFFEFFFFh 
mov CR0, eax 
pop eax 
} 
} 

void UnloadDriver(IN PDRIVER_OBJECT DriverObject) 
{ 
UNICODE_STRING uszDeviceString; 
NTSTATUS ntStatus; 


//移除挂接 
RemoveHook(); 

IoDeleteDevice(DriverObject->DeviceObject); 

RtlInitUnicodeString(&uszDeviceString, L"\\DosDevices\\ITSys"); 
IoDeleteSymbolicLink(&uszDeviceString); 

}