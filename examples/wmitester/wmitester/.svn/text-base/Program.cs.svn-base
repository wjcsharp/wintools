using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Management;
using System.Management.Instrumentation;

namespace wmitester
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            ConnectionOptions options = new ConnectionOptions();

            ManagementScope scope = new ManagementScope("\\\\vejxamcr2dc604\\root\\cimv2", options);

            scope.Connect();

            ObjectQuery query = new ObjectQuery("SELECT * FROM Win32_OperatingSystem");

            ManagementObjectSearcher searcher = new ManagementObjectSearcher(scope, query);

            ManagementObjectCollection queryCollection = searcher.Get();

            foreach (ManagementObject m in queryCollection)
            {
                Console.WriteLine("Computer Name: {0}", m["csname"]);
                Console.WriteLine("Windows Directory: {0}", m["WindowsDirectory"]);
                Console.WriteLine("Operating System: {0}", m["Caption"]);
                Console.WriteLine("Version: {0}", m["Version"]);
                Console.WriteLine("Manufacturer: {0}", m["Manufacturer"]);
            }
        }
    }
}
