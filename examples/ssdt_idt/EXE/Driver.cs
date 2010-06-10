using System;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Lookup
{
    class Driver
    {
        //api imports we need
        [DllImport("kernel32", SetLastError = true)]
        static extern unsafe int DeviceIoControl(
              IntPtr Device,                      // handle to the device 
              uint IoControlCode,                 // Control code for the operation
              byte* InBuffer,                     // Pointer to the input buffer 
              int InBufferSize,                   // Size of the input buffer
              byte* OutBuffer,                    // Pointer to the output buffer 
              int OutBufferSize,                  // Size of the output buffer
              ref int BytesReturned,              // size stored in the output buffer
              int Overlapped                      // Pointer to an OVERLAPPED structure
            );
        [DllImport("kernel32", SetLastError = true)]
        static extern unsafe IntPtr CreateFile(
              string FileName,                    // file name
              uint DesiredAccess,                 // access mode
              uint ShareMode,                     // share mode
              uint SecurityAttributes,            // Security Attributes
              uint CreationDisposition,           // how to create
              uint FlagsAndAttributes,            // file attributes
              int hTemplateFile                   // handle to template file
              );
        [DllImport("kernel32", SetLastError = true)]
        static extern unsafe bool CloseHandle(
              IntPtr hObject                      // handle to object to close
              );
        [DllImport("kernel32")]
        static extern unsafe int GetModuleHandle(
              string ModuleName                   // name of the module 
              );
        [DllImport("kernel32")]
        static extern unsafe int GetProcAddress(
              int Module,                         // Handle to the DLL module 
              string ProcName                     // the function or variable name
              );
        [DllImport("imagehlp")]
        static extern unsafe int ImageDirectoryEntryToData(
              int Base,                           // Base address of the image
              bool MappedAsImage,                 // file mapped by the system as an image
              ushort DirectoryEntry,              // Index number of the desired directory 
              ref int Size                        // Pointer to a variable that receives the size 
              );

        //driver device name        
        private const string drvServiceName = "Lookup";
        //driver handle
        private IntPtr drvHandle;
        ServiceInstaller service = new ServiceInstaller();

        //load driver
        public bool Load()
        {
            string svcPath;
            string svcName;
            string svcDispName;

            svcPath = Directory.GetCurrentDirectory();
            svcPath = svcPath + "\\lookup.sys";
            svcDispName = drvServiceName;
            svcName = drvServiceName;
            
            return service.InstallService(svcPath, svcName, svcDispName);
        }

        //unload driver
        public bool Unload()
        {
            return service.UnInstallService(drvServiceName);
        }

        public void Close()
        {
            CloseHandle(drvHandle);
        }

        //opens the driver with the CreateFile API
        public bool Open(string name)
        {
            //constants for CreateFileA, in c++ defined in WinNT.h
            const uint GENERIC_READ          = 0x80000000;
            const uint GENERIC_WRITE         = 0x40000000;
            const uint CREATE_ALWAYS         = 2;
            const uint FILE_ATTRIBUTE_NORMAL = 0x00000080;
            const int  INVALID_HANDLE        = -1;

            // open the existing file for reading          
            drvHandle = CreateFile(
                  name,
                  GENERIC_WRITE | GENERIC_READ,
                  0,
                  0,
                  CREATE_ALWAYS,
                  FILE_ATTRIBUTE_NORMAL,
                  0);

            //error?
            if (drvHandle.ToInt32() == INVALID_HANDLE)
            {
                MessageBox.Show("can't open \"" + name + "\" device");
                return false;
            }
            
            return true;
        }

        //get data from the driver
        private unsafe int Interact(
            uint IoControlCode,         //control code for the driver
            byte[] inBuffer,            //input buffer, send to the driver
            byte[] outBuffer)           //output buffer, receive from the driver
        {
            int result = 0;
            int returned = 0;

            //call DeviceIoControl
            fixed (byte* pIn = inBuffer)        //unmanaged data
            {
                fixed (byte* pOut = outBuffer)  //unmanaged data
                {
                    result = DeviceIoControl(   //call DeviceIoControl API
                        drvHandle,
                        IoControlCode, 
                        pIn,
                        inBuffer.Length,
                        pOut,
                        outBuffer.Length,
                        ref returned,
                        0);
                }
            }

            return result;
        }

        //CTL_CODE, in C++ it is a #define
        private uint CTL_CODE(uint DeviceType, uint Function, uint Method, uint Access)
        {
            return ((DeviceType << 16) | (Access << 14) | (Function << 2) | Method);
        }

        //constants defined in c++ mostly in winnt.h
        private const uint GET_MODULE_NAME = 0x900;  
        private const uint GET_SERVICE_TABLE = 0x902;
        private const uint GET_INT_TABLE = 0x903;
        private const uint FILE_ANY_ACCESS = 0;
        private const uint IOCTL_TYPE = 40000;
        private const uint METHOD_BUFFERED = 0;

        //our IOCTL codes
        private uint IOCTL_GET_MODULE_NAME()
        {
            return (CTL_CODE(IOCTL_TYPE, GET_MODULE_NAME, METHOD_BUFFERED, FILE_ANY_ACCESS));
        }
        private uint IOCTL_GET_SERVICE_TABLE()
        {
            return (CTL_CODE(IOCTL_TYPE, GET_SERVICE_TABLE, METHOD_BUFFERED, FILE_ANY_ACCESS));
        }
        private uint IOCTL_GET_INT_TABLE()
        {
            return (CTL_CODE(IOCTL_TYPE, GET_INT_TABLE, METHOD_BUFFERED, FILE_ANY_ACCESS));
        }

        //calls the driver and recieve complete idt
        public unsafe InterruptTable GetInterruptTable()
        {
            //new idt, we will return this object later
            InterruptTable interruptTable = new InterruptTable();
            int Count;
            
            try
            {
                //our in and out buffer
                byte[] inBuffer = new byte[0];
                byte[] outBuffer = new byte[256 * 4];

                //call driver and recieve data
                Interact(IOCTL_GET_INT_TABLE(), inBuffer, outBuffer);

                Count = 0;
                fixed (byte* pOut = outBuffer)        //unmanaged data
                {
                    //first dword is cnt
                    Count = ((int*)pOut)[0];

                    //afterwards is the idt
                    for (int cnt = 0; cnt < Count; cnt++)
                    {
                        //get address
                        uint address = ((uint*)pOut)[cnt+1];

                        //create new entry
                        InterruptTableEntry entry = new InterruptTableEntry(address);
                        //reslove module name for that address
                        entry.Module = GetModuleName(address);
                        //and add into our table for later display
                        interruptTable.Add(entry);
                    }
                }
            }
            catch (Exception e)
            {
                string msg = e.StackTrace + "\n\n" +
                             e.Message + "\n\n" +
                             e.HelpLink + "\n please contact the developer";

                MessageBox.Show(msg);
            }

            return interruptTable;
        }

        //opens a driver device and gets the complete KiServiceTable
        public unsafe KiServiceTable GetKiServiceTable()
        {
            //the service table we will fill out and return later
            KiServiceTable kiServiceTable = new KiServiceTable();
            int NumberOfServices;

            //open driver
            try
            {
                //our in and out buffer
                byte[] inBuffer = new byte[0];
                byte[] outBuffer = new byte[0x1000]; //should be big enough 

                //
                Interact(IOCTL_GET_SERVICE_TABLE(), inBuffer, outBuffer);

                //didn't found a byte[4] to int convertion method ...
                //First dword is NumberOfServices
                NumberOfServices = 0;
                fixed (byte* pOut = outBuffer)        //unmanaged data
                {
                    //first dword is cnt for number of services in this os
                    NumberOfServices = ((int*)pOut)[0];

                    //and then the complete service table ...
                    for (int cnt = 0; cnt < NumberOfServices; cnt++)
                    {
                        //get address
                        uint address = ((uint*)pOut)[cnt + 1];

                        //create new entry
                        KiServiceTableEntry entry = new KiServiceTableEntry(address);
                        //resolve module name for that address
                        entry.Module = GetModuleName(address);
                        kiServiceTable.Add(entry);
                    }
                }
            }
            catch (Exception e)
            {
                string msg = e.StackTrace + "\n\n" +
                             e.Message + "\n\n" +
                             e.HelpLink + "\n please contact the developer";

                MessageBox.Show(msg);
            }

            return kiServiceTable;
        }
        
        //tries to resolve the module name for an address
        public unsafe string GetModuleName(uint addr)
        {
            string module = "";    

            //open driver
            try
            {
                byte[] inBuffer = new byte[100];
                byte[] outBuffer = new byte[100];     

                //our in and out buffer
                fixed (byte* pIn = inBuffer)
                {
                    ((uint*)pIn)[0] = addr;
                }

                //get module name
                Interact(IOCTL_GET_MODULE_NAME(), inBuffer, outBuffer);               

                //copy module name into a string object
                foreach (char b in outBuffer)
                {
                    if (b == 0)
                        break;
                    module = module + b;
                }
            }
            catch (Exception e)
            {
                string msg = e.StackTrace + "\n\n" +
                             e.Message + "\n\n" +
                             e.HelpLink + "\n please contact the developer";

                MessageBox.Show(msg);
            }

            return module;
        }

        //searchs in the ntdll.dll export list for each service
        public unsafe void GetKiServiceNames(KiServiceTable kiServiceTable)
        {
            //base address from ntdll.dll
            //ntdll.dll is loaded in every process in nt automatically with kernel32.dll
            int ntdllBase = GetModuleHandle("ntdll.dll");
            if (ntdllBase == 0) //should never happen
                return; 

            int size = 0;
            //we need the address from the export table
            //export table is the first directory entry in the  pe header
            int export = ImageDirectoryEntryToData(ntdllBase, true, 0, ref size);

            if (export == 0)    //should never happen
                return;

            //+0x18 Number of names            
            byte* ptr = (byte* )(export + 0x18);
            int numberOfNames = ((int *)ptr)[0];
            //+0x20 Address of names
            ptr = (byte*)(export + 0x20);
            int addressOfNames = ((int*)ptr)[0];
            
            //translate rva into va in memory
            //it is unsafe to do this way, need to correct it with the pe header
            addressOfNames += ntdllBase;

            for (int i = 0; i < numberOfNames; i++)
            {
                //get address from the name
                ptr = (byte*)(addressOfNames + i*4);
                int address = ((int*)ptr)[0];
                address += ntdllBase;

                //copy string 
                string name = "";
                for (int i2 = 0; ((byte*)address)[i2] != 0; i2++)
                {
                    name = name + (char)(((byte*)address)[i2]);
                }
                
                //get address from the method
                //getprocaddress helps us here
                address = GetProcAddress(ntdllBase, name);

                //check if it is service call
                //service calls always starts with
                //mov eax, code 

                ptr = (byte*)(address);

                //mov eax has opocde 0xb8
                if (((byte*)ptr)[0] != 0xb8 || ((byte*)ptr)[5] != 0xba)
                    continue;

                ptr++;
                //reslove service code
                int serviceCode = ((int*)ptr)[0];

                KiServiceTableEntry entry =
                    kiServiceTable.Get(serviceCode);

                if (entry.Name.Length != 0)     //already inserted
                    continue;
                entry.Name = entry.Name + name;              
            }
        }
    }
}
