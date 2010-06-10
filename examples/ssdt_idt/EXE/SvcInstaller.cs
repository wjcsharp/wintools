/***********************************************************************
 * 
 * class ServiceInstaller 
 * http://www.c-sharpcorner.com/UploadFile/sachin.nigam/InstallingWinServiceProgrammatically11262005061332AM/
 *        InstallingWinServiceProgrammatically.aspx
 * 
 * really simple wrappper class for the Service APIs
 * notice: need to write an own class
 * 
 ***********************************************************************/

using System;
using System.Runtime.InteropServices;
namespace Lookup
{
/// <summary>
/// Summary description for ServiceInstaller.
/// </summary>
class ServiceInstaller
{
    #region DLLImport
    [DllImport("advapi32.dll")]
    public static extern IntPtr OpenSCManager(string lpMachineName,string lpSCDB, int scParameter);
    [DllImport("Advapi32.dll")]
    public static extern IntPtr CreateService(IntPtr SC_HANDLE,string lpSvcName,string lpDisplayName,
    int dwDesiredAccess,int dwServiceType,int dwStartType,int dwErrorControl,string lpPathName,
    string lpLoadOrderGroup,int lpdwTagId,string lpDependencies,string lpServiceStartName,string lpPassword);
    [DllImport("advapi32.dll")]
    public static extern void CloseServiceHandle(IntPtr SCHANDLE);
    [DllImport("advapi32.dll")]
    public static extern int StartService(IntPtr SVHANDLE,int dwNumServiceArgs,string lpServiceArgVectors);
    [DllImport("advapi32.dll",SetLastError=true)]
    public static extern IntPtr OpenService(IntPtr SCHANDLE,string lpSvcName,int dwNumServiceArgs);
    [DllImport("advapi32.dll")]
    public static extern int DeleteService(IntPtr SVHANDLE);
    [DllImport("advapi32.dll")]
    public static extern int ControlService(IntPtr SVHANDLE,int dwControl,string lpServiceStatus);
    [DllImport("kernel32.dll")]
    public static extern int GetLastError();
    #endregion DLLImport

    /// <summary>
    /// This method installs and runs the service in the service control manager.
    /// </summary>
    /// <param name="svcPath">The complete path of the service.</param>
    /// <param name="svcName">Name of the service.</param>
    /// <param name="svcDispName">Display name of the service.</param>
    /// <returns>True if the process went thro successfully. False if there was anyerror.</returns>
    public bool InstallService(string svcPath, string svcName, string svcDispName)
    {
        #region Constants declaration.                
        int SC_MANAGER_CONNECT            = 0x0001;
        int SC_MANAGER_CREATE_SERVICE     = 0x0002;
        int SC_MANAGER_ENUMERATE_SERVICE  = 0x0004;
        int SC_MANAGER_LOCK               = 0x0008;
        int SC_MANAGER_QUERY_LOCK_STATUS  = 0x0010;
        int SC_MANAGER_MODIFY_BOOT_CONFIG = 0x0020;
        int SC_MANAGER_ALL_ACCESS         = (SC_MANAGER_CONNECT            |
                                             SC_MANAGER_CREATE_SERVICE     |
                                             SC_MANAGER_ENUMERATE_SERVICE  |
                                             SC_MANAGER_LOCK               |
                                             SC_MANAGER_QUERY_LOCK_STATUS  |
                                             SC_MANAGER_MODIFY_BOOT_CONFIG);


        int SERVICE_KERNEL_DRIVER = 0x00000001;        
        int SERVICE_DEMAND_START = 0x00000003;
        int SERVICE_ERROR_NORMAL = 0x00000001;
        int STANDARD_RIGHTS_REQUIRED = 0xF0000;
        int SERVICE_QUERY_CONFIG = 0x0001;
        int SERVICE_CHANGE_CONFIG = 0x0002;
        int SERVICE_QUERY_STATUS = 0x0004;
        int SERVICE_ENUMERATE_DEPENDENTS = 0x0008;
        int SERVICE_START =0x0010;
        int SERVICE_STOP =0x0020;
        int SERVICE_PAUSE_CONTINUE =0x0040;
        int SERVICE_INTERROGATE =0x0080;
        int SERVICE_USER_DEFINED_CONTROL =0x0100;
        int SERVICE_ALL_ACCESS = (STANDARD_RIGHTS_REQUIRED | SERVICE_QUERY_CONFIG |
            SERVICE_CHANGE_CONFIG | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS |
            SERVICE_START | SERVICE_STOP | SERVICE_PAUSE_CONTINUE | SERVICE_INTERROGATE |
            SERVICE_USER_DEFINED_CONTROL);        
        #endregion Constants declaration.

        try
        {
            IntPtr sc_handle = OpenSCManager(null, null, SC_MANAGER_ALL_ACCESS);
            if (sc_handle.ToInt32() != 0)
            {
                IntPtr sv_handle = CreateService(
                    sc_handle,
                    svcName,
                    svcDispName,
                    SERVICE_ALL_ACCESS,
                    SERVICE_KERNEL_DRIVER,
                    SERVICE_DEMAND_START,
                    SERVICE_ERROR_NORMAL,
                    svcPath,
                    null,
                    0,
                    null,
                    null,
                    null
                    );

                if(sv_handle.ToInt32() == 0)
                {
                    int ERROR_SERVICE_EXISTS = 1073;
                    int error = GetLastError();
                    if (error != ERROR_SERVICE_EXISTS)
                    {
                        CloseServiceHandle(sc_handle);
                        return false;
                    }
                    else
                    {
                        sv_handle = OpenService(sc_handle, svcName, SERVICE_ALL_ACCESS);
                    }
                }

                //now trying to start the service
                int i = StartService(sv_handle,0,null);
                // If the value i is zero, then there was an error starting the service.
                // note: error may arise if the service is already running or some other problem.
                if(i==0)
                    return false;

                CloseServiceHandle(sv_handle);
                CloseServiceHandle(sc_handle);
                return true;                
            }
            else                
                return false;
        }
        catch(Exception e)
        {
            throw e;
        }
    }

    /// <summary>
    /// This method uninstalls the service from the service conrol manager.
    /// </summary>
    /// <param name="svcName">Name of the service to uninstall.</param>
    public bool UnInstallService(string svcName)
    {
        #region Constants declaration.
        int SC_MANAGER_CONNECT = 0x0001;
        int SC_MANAGER_CREATE_SERVICE = 0x0002;
        int SC_MANAGER_ENUMERATE_SERVICE = 0x0004;
        int SC_MANAGER_LOCK = 0x0008;
        int SC_MANAGER_QUERY_LOCK_STATUS = 0x0010;
        int SC_MANAGER_MODIFY_BOOT_CONFIG = 0x0020;
        int SC_MANAGER_ALL_ACCESS = (SC_MANAGER_CONNECT |
                                             SC_MANAGER_CREATE_SERVICE |
                                             SC_MANAGER_ENUMERATE_SERVICE |
                                             SC_MANAGER_LOCK |
                                             SC_MANAGER_QUERY_LOCK_STATUS |
                                             SC_MANAGER_MODIFY_BOOT_CONFIG);
        int STANDARD_RIGHTS_REQUIRED = 0xF0000;
        int SERVICE_QUERY_CONFIG = 0x0001;
        int SERVICE_CHANGE_CONFIG = 0x0002;
        int SERVICE_QUERY_STATUS = 0x0004;
        int SERVICE_ENUMERATE_DEPENDENTS = 0x0008;
        int SERVICE_START = 0x0010;
        int SERVICE_STOP = 0x0020;
        int SERVICE_PAUSE_CONTINUE = 0x0040;
        int SERVICE_INTERROGATE = 0x0080;
        int SERVICE_USER_DEFINED_CONTROL = 0x0100;
        int SERVICE_ALL_ACCESS = (STANDARD_RIGHTS_REQUIRED | SERVICE_QUERY_CONFIG |
            SERVICE_CHANGE_CONFIG | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS |
            SERVICE_START | SERVICE_STOP | SERVICE_PAUSE_CONTINUE | SERVICE_INTERROGATE |
            SERVICE_USER_DEFINED_CONTROL);
        int SERVICE_CONTROL_STOP = 0x00000001;
        #endregion Constants declaration.
        IntPtr sc_hndl = OpenSCManager(null, null, SC_MANAGER_ALL_ACCESS);
        if(sc_hndl.ToInt32() !=0)
        {
            IntPtr svc_hndl = OpenService(sc_hndl, svcName, SERVICE_ALL_ACCESS);
            
            if(svc_hndl.ToInt32() !=0)
            {
                string svStatus = "";
                ControlService(svc_hndl, SERVICE_CONTROL_STOP, svStatus);                
                int i = DeleteService(svc_hndl);
                if (i != 0)
                {
                    CloseServiceHandle(sc_hndl);
                    return true;
                }
                else
                {
                    int error = GetLastError();
                    CloseServiceHandle(sc_hndl);
                    return false;
                }
            }
            else
                return false;
        }
        else
            return false;
    }
}
}