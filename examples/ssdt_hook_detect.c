//SSDT Hook Detector
//By xWeasel - April 2007
//Finds SSDT hooks by looping through the system service descriptor table
//if syscall address is outside of the kernel's address space, then it's a hooked function

#include <ntddk.h>
#include <ntifs.h>

typedef struct _MODULE_INFO {
	ULONG dwReserved[2];
	ULONG dwBase;
	ULONG dwSize;
	ULONG dwFlags;
	USHORT wIndex;
	USHORT wRank;
	USHORT wLoadCount;
	USHORT NameOffset;
	char cPath[256];
} MODULE_INFO, *PMODULE_INFO, **PPMODULE_INFO;

typedef struct _MODULE_LIST {
	ULONG dwModules;
	MODULE_INFO Modules[1];
} MODULE_LIST, *PMODULE_LIST, **PPMODULE_LIST;

VOID EnumSSDTEntries(ULONG ulKernelBase, ULONG ulKernelEnd);
PMODULE_LIST GetModuleList();

//Driver entry point
NTSTATUS DriverEntry(IN PDRIVER_OBJECT theDriverObject, IN PUNICODE_STRING theRegistryPath){
	NTSTATUS ntStatus;
	ULONG ulKernelBase, ulKernelEnd;
	PMODULE_LIST pModules = NULL;
	PMODULE_INFO pKernelInfo;

	pModules = GetModuleList();
	if(pModules != NULL){
		pKernelInfo = &pModules->Modules[0];
		ulKernelBase = pKernelInfo->dwBase;
		ulKernelEnd = pKernelInfo->dwBase + pKernelInfo->dwSize;
		EnumSSDTEntries(ulKernelBase, ulKernelEnd);
		ExFreePoolWithTag(pModules, 'mlst');
	}else{
		DbgPrint("Failed to get module list! Exiting...\n");
	}

	return STATUS_SUCCESS;
}

//Loop through SSDT entries and check if any system calls aren't within ntoskrnl's range (base and base+size)
VOID EnumSSDTEntries(ULONG ulKernelBase, ULONG ulKernelEnd){
	ULONG ulServiceIndex;
	DbgPrint("Number of services in SSDT: %u\n", (ULONG)KeServiceDescriptorTable->TableSize);
	for(ulServiceIndex = 0; ulServiceIndex < KeServiceDescriptorTable->TableSize; ulServiceIndex++){
		DbgPrint("Syscall #%d is located at %08lX.\n", ulServiceIndex, KeServiceDescriptorTable->ServiceTable[ulServiceIndex]);
		if((ULONG)KeServiceDescriptorTable->ServiceTable[ulServiceIndex] < ulKernelBase || (ULONG)KeServiceDescriptorTable->ServiceTable[ulServiceIndex] > ulKernelEnd){
			DbgPrint("  Syscall #%d at %08lX is hooked!!\n", ulServiceIndex, KeServiceDescriptorTable->ServiceTable[ulServiceIndex]);
		}
	}
}

//List loaded modules (we only need the first one, which is the kernel)
PMODULE_LIST GetModuleList(){
	NTSTATUS NtStatus;
	ULONG ulNeededSize;
	PULONG pulModuleList;

	ZwQuerySystemInformation(SystemModuleInformation, &ulNeededSize, 0, &ulNeededSize);
	pulModuleList = ExAllocatePoolWithTag(PagedPool, ulNeededSize, 'mlst');
	NtStatus = ZwQuerySystemInformation(SystemModuleInformation, pulModuleList, ulNeededSize, 0);

	if(!NT_SUCCESS(NtStatus)){
		DbgPrint("ZwQuerySystemInformation failed! ulNeededSize = %ul, NtStatus = %u.\n", ulNeededSize, NtStatus);
	}

	return (PMODULE_LIST) pulModuleList;
}
