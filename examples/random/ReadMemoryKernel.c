汗...下面这点代码没什么技术含量,也很简单..但感觉还是比较有用的..可以利用这个和用户进行通信..和饶过通常的SSDT HOOK啊 .只实现了Raed..不过Write也是一样的道理...    
http://hi.baidu.com/sysnap/ 


VOID Unload(PDRIVER_OBJECT DriverObject) 
{ 
   DbgPrint("Unload Callled\n"); 
} 

ULONG MyReadMemory(IN PVOID BaseAddress,IN SIZE_T BufferSize,IN HANDLE pid) 
{ 
     PEPROCESS EProcess; 
     KAPC_STATE ApcState; 
     PVOID readbuffer; 
     NTSTATUS status; 

     status = PsLookupProcessByProcessId((HANDLE)pid,&EProcess); 
      if(!NT_SUCCESS(status)) 
      { 
           DbgPrint("failed to get the EPROCESS!!\n"); 
           return 0; 
       } 
       
       
      readbuffer = ExAllocatePoolWithTag (NonPagedPool, BufferSize, 'Sys'); 
      if(readbuffer==NULL) 
       { 
          DbgPrint("failed to alloc memory!\n"); 
          return 0; 
       } 
      
      *(ULONG*)readbuffer=(ULONG)0x1; 

      KeStackAttachProcess (EProcess, &ApcState); 

       __try 
        { 
             ProbeForRead ((CONST PVOID)BaseAddress, BufferSize, sizeof(CHAR)); 
             RtlCopyMemory (readbuffer, BaseAddress, BufferSize); 
             KeUnstackDetachProcess (&ApcState); 
         
        } __except(EXCEPTION_EXECUTE_HANDLER) 
        { 
             KeUnstackDetachProcess (&ApcState); 
        } 
         
             DbgPrint("%x\n",*(ULONG*)readbuffer); 
             ExFreePool (readbuffer); 
             return 1; 
         
} 


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING str) 
{ 
     ULONG ret = MyReadMemory((PVOID)0x7c944000,0x8,(HANDLE)904); 
     if(ret==0) 
     DbgPrint("read memory failed!!\n"); 
      
     DriverObject->DriverUnload = Unload; 
     return STATUS_SUCCESS; 
}