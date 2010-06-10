#include <ntddk.h>
#include <string.h>
#include <stdio.h>
#include "lookup.h"
#include "general.h"

//called by windows to initialize the driver
NTSTATUS DriverEntry(IN OUT PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    NTSTATUS        ntStatus;
    UNICODE_STRING  ntUnicodeString;
  	UNICODE_STRING  ntWin32NameString;
    PDEVICE_OBJECT  deviceObject = NULL;

    RtlInitUnicodeString( &ntUnicodeString, NT_DEVICE_NAME );

	//create "\\.\Lookup" device
    ntStatus = IoCreateDevice( DriverObject, 0, &ntUnicodeString, 
        FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &deviceObject);

    if (!NT_SUCCESS(ntStatus))
        return ntStatus;
   
	//initialize the driver object with the driver functions
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DrvCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DrvClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DrvDeviceControl;
    DriverObject->DriverUnload = DrvUnloadDriver;

    RtlInitUnicodeString( &ntWin32NameString, DOS_DEVICE_NAME );

    //create symbolic link for our device
    ntStatus = IoCreateSymbolicLink( &ntWin32NameString, &ntUnicodeString );

    if(!NT_SUCCESS(ntStatus))
        IoDeleteDevice(deviceObject);

    return ntStatus;
}

//called when a new device is opened
NTSTATUS DrvCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    return STATUS_SUCCESS;
}

//called when a device is closed
NTSTATUS DrvClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

//called when driver is unloaded
void DrvUnloadDriver(IN PDRIVER_OBJECT DriverObject)
{
    PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;
    UNICODE_STRING uniWin32NameString;

    RtlInitUnicodeString(&uniWin32NameString, DOS_DEVICE_NAME);

    IoDeleteSymbolicLink(&uniWin32NameString);

    if (deviceObject != NULL)
    {
        IoDeleteDevice(deviceObject);
	}
}

//called when an io request is send
NTSTATUS DrvDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	//local variabels used later
    PIO_STACK_LOCATION irpSp;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    DWORD inBufLength;
    DWORD outBufLength;
	DWORD addr;
	DWORD hNtoskrnl;
	DWORD dwIdt;
	DWORD i;
	char *inBuf, *outBuf;	
	char *name;
	char *ptr;

    irpSp = IoGetCurrentIrpStackLocation( Irp );
    inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;
    outBufLength = irpSp->Parameters.DeviceIoControl.OutputBufferLength;
	inBuf = Irp->AssociatedIrp.SystemBuffer;
	outBuf = Irp->AssociatedIrp.SystemBuffer;

	//Control Code switch
    switch ( irpSp->Parameters.DeviceIoControl.IoControlCode )
    {
		case IOCTL_GET_MODULE_NAME:			//get a module name for a address
		{
			addr = ((DWORD *)inBuf)[0];		//address from the input buffer
			name = GetModuleName(addr);		//call getmodulename

			if(name != 0)					//module name found?
				memcpy(outBuf, name, strlen(name));
			else
				memcpy(outBuf, "unkown\0", 7);

			Irp->IoStatus.Information = outBufLength;
			ntStatus = STATUS_SUCCESS;
			break;
		}

		case IOCTL_GET_INT_TABLE:			//get complete interrupt descriptor table
		{
			dwIdt = GetIntTable();			//base address from the idt

			((DWORD *)outBuf)[0] = 255;		//hardcoded 255 entries :p will fix it later
			outBuf += 4;
			for(i = 0;i < 255;i++)			//copy each entry into the output buffer
				((DWORD *)outBuf)[i] = GetIntAddress(dwIdt, i);
			  	
			Irp->IoStatus.Information = outBufLength;
			ntStatus = STATUS_SUCCESS;       
			break;
		}

		case IOCTL_GET_SERVICE_TABLE:		//get complete service table
		{
			//number of services avaible in the current os
			((DWORD *)outBuf)[0] = KeServiceDescriptorTable->ServiceTable[0].NumberOfServices;

			//first dword in the output buffer = number of services
			ptr = (char *)KeServiceDescriptorTable->ServiceTable[0].TableBase;
			outBuf += 4;

			//copy each entry into the output buffer
			for(i = 0;i < KeServiceDescriptorTable->ServiceTable[0].NumberOfServices;i++)			
				((DWORD *)outBuf)[i] = ((DWORD *)ptr)[i];
			  	
			Irp->IoStatus.Information = outBufLength;
			ntStatus = STATUS_SUCCESS;
			break;
		}

		default:							//unkown io control code?
		{
			ntStatus = STATUS_INVALID_DEVICE_REQUEST;
			break;
		}
	}

 	Irp->IoStatus.Status = ntStatus;
	IoCompleteRequest( Irp, IO_NO_INCREMENT );
	return ntStatus;
}

//tries to find the module name for a kernel space address
char *GetModuleName(DWORD dwAddr)
{	
	PSYSTEM_MODULE_INFORMATION pMod;
	DWORD *ptr;
	DWORD dwModBase;
	DWORD dwModSize;
	DWORD i;
	char *pName = 0;		//name later returned
	
	ZwQuerySystemInformation( SystemModuleInformation, &i, 0, &i );
	ptr = (DWORD *)ExAllocatePool(NonPagedPool, 4 * i);

	ZwQuerySystemInformation( SystemModuleInformation, ptr, i * sizeof(*ptr), 0);
	pMod = (PSYSTEM_MODULE_INFORMATION)ptr;

	for (i = 0; i<*ptr; i++)
	{
		//base address from the module
		dwModBase =(DWORD)pMod->pModule[i].pAddress;
		//size from the module
		dwModSize = (DWORD)pMod->pModule[i].dwSize;

		//address between start and end?
		if (dwModBase <= dwAddr && dwAddr <= dwModBase + dwModSize)
		{
			//when yes module found
			pName = (char*)pMod->pModule[i].bName + pMod->pModule[i].wNameOffset;
			break;
		}
	}

	ExFreePool(ptr);
	return pName;
}

//Retrieve IDT Address
DWORD GetIntTable()
{
	DWORD dwIdt = 0;
	_asm
	{
		push ebx
		push ebx
		sidt [esp]
		mov edx, [esp+2]
		pop ebx
		pop ebx
		mov dwIdt, edx	
	}
	return dwIdt;
}

//Retrieve specific entry in the idt table
DWORD GetIntAddress(DWORD dwIdt, int iInt)
{
	DWORD dwRet;
	__asm
	{
		mov eax,iInt
		mov edx,dwIdt
		lea edx,[edx+eax*8]
		mov bx,[edx+6]
		shl ebx,10h
		mov bx,[edx]	
		mov dwRet, ebx
	}
	return dwRet;
}