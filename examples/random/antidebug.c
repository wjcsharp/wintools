/**********************防止被进程附加调试*******************************/ 
bool AntiAttach() 
{ 
     HMODULE ntdll; // ntdll handle 
     void* pDbgUiRemoteBreakin; // function handle 
     DWORD dwOldProtect; // just for fun 
     DWORD dwCodeSize; // Size of code to copy 
      
     // Get ntdll.dll handle 
     ntdll = GetModuleHandle("ntdll.dll"); 
     if(ntdll) 
     { 
          // Get target function addr 
          pDbgUiRemoteBreakin = GetProcAddress(ntdll, "DbgUiRemoteBreakin"); 
          if(pDbgUiRemoteBreakin) 
          { 
               __asm 
               { 
                    // Get code size 
                    lea eax, __CodeToCopyStart 
                         lea ecx, __CodeToCopyEnd 
                         sub ecx, eax 
                         mov dwCodeSize, ecx 
               } 
               // Make sure that we have write rights ... 
               if(VirtualProtect(pDbgUiRemoteBreakin, dwCodeSize, PAGE_EXECUTE_READWRITE, &dwOldProtect)) 
               { 
                    __asm 
                    { 
                         // Copy code between __CodeToCopyStart and __CodeToCopyEnd 
                         mov edi, pDbgUiRemoteBreakin 
                              lea esi, __CodeToCopyStart 
                              mov ecx, dwCodeSize 
                              rep movsb 
                              // Skip code 
                              jmp __CodeEnd 
                               
__CodeToCopyStart: 
                         lea eax, __CodeToCopyEnd 
                              jmp eax 
__CodeToCopyEnd: 
                    } 
                     
                    // ***CODE*HERE*** 
                    __asm 
                    { 
                         // Clear registers 
                         xor eax, eax 
                              pushfd 
                              mov [esp], eax 
                              popfd 
                              xor ebx, ebx 
                              xor ecx, ecx 
                              xor edx, edx 
                              xor esi, esi 
                              xor edi, edi 
                              xor esp, esp 
                              xor ebp, ebp 
                              // Jump to address 0 
                              jmp eax 
                    } 
                    // *************** 
                     
__CodeEnd:; 
           return true; 
               } 
          } 
     } 
     return false;      
} 


void AntiDebug() 
{ 
     HANDLE handle=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
     PROCESSENTRY32* info=new PROCESSENTRY32; 
info->dwSize=sizeof(PROCESSENTRY32); 
int i=0; 
//     HTREEITEM hItem[100]; 
      
     int MyParentProcessID=0;//自身的父进程ID 
     int ExplorerProcessID=0;//Explorer.exe的进程ID 
     int WinRarProcessID = 0;//winrar进程ID 
     if (Process32First(handle,info)) 
     { 
          while (Process32Next(handle,info)) 
          { 
               CString exe; 
               exe.Format("%s",info->szExeFile); 
                
               CString ProcessID; 
               ProcessID.Format("进程ID：%d",info->th32ProcessID); 
                
               CString ParentID; 
               ParentID.Format("父进程ID：%d",info->th32ParentProcessID); 

               //比较进程名，这里忽略大小写 
               if (stricmp(exe.GetBuffer(exe.GetLength()),"WinRAR.exe") == 0) 
                    WinRarProcessID = info->th32ProcessID; 
               if (stricmp(exe.GetBuffer(exe.GetLength()),"explorer.exe")==0) 
                    ExplorerProcessID=info->th32ProcessID;//获得EXPLORER的进程ID 
               if (stricmp(exe.GetBuffer(exe.GetLength()),"test.exe")==0) 
                    MyParentProcessID=info->th32ParentProcessID;//获得自身父进程ID 
                
               i++; 
          } 
     }      
//     SetDlgItemText(IDC_STATUS,sta);      
     CloseHandle(handle);      
     delete info; 
     //检查自身程序的父进程是否是winrar 
     if(MyParentProcessID != WinRarProcessID) 
     { 
          //检测自身的父进程是否是EXPLORER 
          if (MyParentProcessID!=ExplorerProcessID) 
          { 
               //          AfxMessageBox("检测到调试器"); 
               PrintLog("检测到调试器,结束自身"); 
//               exit(0); 
               //          return ; 
          } 
     } 
//     else 
//          AfxMessageBox("注意：没有检测到调试器..."); 
}