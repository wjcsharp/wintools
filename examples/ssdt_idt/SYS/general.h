#define NT_DEVICE_NAME      L"\\Device\\Lookupi"
#define DOS_DEVICE_NAME     L"\\DosDevices\\Lookup"

#define MAXIMUM_FILENAME_LENGTH 256

typedef unsigned long DWORD, *PDWORD;
typedef unsigned short WORD, *PWORD;
typedef unsigned char BYTE, *PBYTE;

typedef struct _SYSTEM_MODULE
    {
    DWORD dwReserved01;
    DWORD dw04;
    PVOID pAddress;
    DWORD dwSize;
    DWORD dwFlags;
	WORD  wId;
    WORD  wRank;
    WORD  w18;
    WORD  wNameOffset;
    BYTE  bName [MAXIMUM_FILENAME_LENGTH];
}SYSTEM_MODULE, * PSYSTEM_MODULE;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	DWORD         dCount;
	SYSTEM_MODULE pModule [1];
}SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

typedef struct ServiceTableEntry 
{
	DWORD *TableBase;
	DWORD *CounterTableBase; 
	DWORD NumberOfServices;
	UCHAR *ParamTableBase;
}t_ServiceTableEntry;

typedef struct ServiceDescriptorTable
{
	t_ServiceTableEntry ServiceTable[4];
}t_ServiceDescriptorTable;

extern t_ServiceDescriptorTable *KeServiceDescriptorTable;

typedef enum _SYSTEMINFOCLASS
{
    SystemBasicInformation,
    SystemProcessorInformation,
    SystemPerformanceInformation,
    SystemTimeInformation,
    SystemPathInformation,
    SystemProcessInformation,
    SystemCallInformation,
    SystemConfigurationInformation,
    SystemProcessorCounters,
    SystemGlobalFlag,
    SystemInfo10,
    SystemModuleInformation,
    SystemLockInformation,
    SystemInfo13,
    SystemPagedPoolInformation,
    SystemNonPagedPoolInformation,
    SystemHandleInformation,
    SystemObjectInformation,
    SystemPageFileInformation,
    SystemInstemulInformation,
    SystemInfo20,
    SystemCacheInformation,
    SystemPoolTagInformation,
    SystemInfo23,
    SystemDpcInformation,
    SystemInfo25,
    SystemLoadDriver,
    SystemUnloadDriver,
    SystemTimeAdjustmentInformation,
    SystemInfo29,
    SystemInfo30,
    SystemInfo31,
    SystemCrashDumpInformation,
    SystemInfo33,
    SystemCrashDumpStateInformation,
    SystemDebuggerInformation,
    SystemThreadSwitchInformation,
    SystemRegistryQuotaInformation,
    SystemAddDriver,
    SystemPrioritySeparationInformation,
    SystemInfo40,
    SystemInfo41,
    SystemInfo42,
    SystemInfo43,
    SystemTimeZoneInformation,
    SystemLookasideInformation,
    MaxSystemInfoClass
}SYSTEMINFOCLASS, *PSYSTEMINFOCLASS;

NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation (
	SYSTEMINFOCLASS sic,
	PVOID           pData,
	DWORD           dSize,
	PDWORD          pdSize);

NTSTATUS DriverEntry(IN OUT PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
NTSTATUS DrvClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DrvCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DrvDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
void     DrvUnloadDriver(IN PDRIVER_OBJECT DriverObject);
char	 *GetModuleName(DWORD hBase);
DWORD	 GetIntTable();
DWORD	 GetIntAddress(DWORD dwIdt, int iIntNumber);