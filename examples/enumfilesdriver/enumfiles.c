NTSTATUS EnumFiles(const char*szFileDirectoryName)//prototype;not used
{
         
    NTSTATUS NtStatus       = 0XC0000004;
    BOOLEAN bIsStarted      = TRUE,
                              bRemainingFiles = TRUE;

    FILE_BOTH_DIR_INFORMATION * pBuffer;
    unsigned long dwSize;

    HANDLE hObjectHandle;
    OBJECT_ATTRIBUTES ObjAttributes;
    UNICODE_STRING uAllocatedBufferName=RTL_CONSTANT_STRING(L"\\??\\C:\\");
    IO_STATUS_BLOCK IoStatusBlock;
    UCHAR*uObjectName;
    
    KIRQL kIrql = KeGetCurrentIrql();
    dwSize = (sizeof(FILE_BOTH_DIR_INFORMATION)*2);

    if (kIrql == PASSIVE_LEVEL)
    {
        DbgPrint("Directory Object Name Initialized : %ls\r\n",uAllocatedBufferName.Buffer);
        
        InitializeObjectAttributes(&ObjAttributes,\
                                   &uAllocatedBufferName,\
                                   OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE,\
                                   NULL,NULL);

        NtStatus = ZwCreateFile(&hObjectHandle,\
                                (GENERIC_READ|SYNCHRONIZE),\
                                &ObjAttributes, &IoStatusBlock,\
                                0,\
                                FILE_ATTRIBUTE_NORMAL,\
                                FILE_SHARE_READ,\
                                FILE_OPEN,\
                                FILE_SYNCHRONOUS_IO_NONALERT,\
                                NULL,0);

        if (!NT_SUCCESS(NtStatus) && IoStatusBlock.Information!=FILE_OPENED)
        {
            DbgPrint("Unable to Initialize ZwCreateFile\r\nNTSTATUS == 0X%08X\r\n",NtStatus);                      
            return NtStatus;
        }

        pBuffer = MemAlloc(dwSize,'JEWS');
        if (pBuffer != NULL)
        {
            while (TRUE) /* This will loop until all files are written to buffer,
                            of course,we can get all the filenames in one call if our 
                            buffer was large enough ( if we knew how many files were in the directory )
                         */ 
            {
                NtStatus = ZwQueryDirectoryFile(hObjectHandle,\
                                                NULL,//no events
                                                NULL,//no routine callbacks
                                                NULL,//no context parameter to be passed
                                                &IoStatusBlock,
/*W*/                                           pBuffer,
                                                dwSize,
                                                FileBothDirectoryInformation,
                                                FALSE,//returna all entries found
                                                NULL,//find all files
                                                bIsStarted//TRUE if first call
                                               );/*9*/ 
                switch (NtStatus)
                {
                case STATUS_BUFFER_OVERFLOW:
                    MemFree(pBuffer,'JEWS');
                    dwSize*=4;
                    pBuffer = MemAlloc(dwSize,'JEWS');
                    if (pBuffer == NULL)
                    {
#ifdef DEBUG
                        DbgPrint("No memory\r\n");
#endif
                        ZwClose(hObjectHandle);
                        return NtStatus;
                    }
                case STATUS_NO_MORE_FILES:
                    CleanupRoutine(hObjectHandle,pBuffer,'JEWS');
                    return STATUS_SUCCESS;
                case STATUS_SUCCESS:
                {
                    if (bIsStarted)
                        bIsStarted = FALSE;
                    while (1)
                    {
                        uObjectName = MemAlloc(pBuffer->FileNameLength * sizeof(WCHAR), 'KIKE');
                        
                        if (uObjectName!=NULL)
                        {
                            RtlCopyMemory(uObjectName, pBuffer->FileName, pBuffer->FileNameLength);
                            DbgPrint("File name is: %S\n", uObjectName);
                            if(IsFileDirectory(pBuffer->FileAttributes))
                              DbgPrint("              File allocation buffer\r\n");
                              
                            MemFree(uObjectName, 'KIKE');
                        }
                        if (pBuffer->NextEntryOffset == 0)
                            break;
                        pBuffer += pBuffer->NextEntryOffset;
                    }
                }
                break;
                default:
                    CleanupRoutine(hObjectHandle,pBuffer,'JEWS');
                    return NtStatus;
                }
            }
        }CleanupRoutine(hObjectHandle,pBuffer,'JEWS');
    }
    return NtStatus;
}