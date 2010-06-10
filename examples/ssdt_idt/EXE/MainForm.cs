using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Lookup
{
    public partial class MainForm : Form
    {        
        private KiServiceTable kiServiceTable;
        private InterruptTable interruptTable;

        //constructor
        public MainForm()
        {                        
            InitializeComponent();
        }

        //fill the listview with the information from kiServiceTable
        private void FillServiceTableList()
        {
            int hookCount = 0;  //count hooked services

            for (int i = 0; i < kiServiceTable.NumberOfServices; i++)
            {
                //our current entry in the list
                KiServiceTableEntry entry = kiServiceTable.Get(i);
                
                //new item in list field
                ListViewItem item = new ListViewItem(String.Format("{0:000}", i), 0);

                //add function name
                item.SubItems.Add(entry.Name);

                //convert address into hex string
                string str = "0x" + Convert.ToString(entry.Address, 16);                                
                item.SubItems.Add(str);

                //add module name
                item.SubItems.Add(entry.Module.ToLower());

                if (!((entry.Module == "ntoskrnl.exe") || (entry.Module == "ntkrnlmp.exe") || (entry.Module == "ntkrnlpa.exe") || (entry.Module == "ntkrpamp.exe")))
                {
                    //hooked
                    item.BackColor = System.Drawing.Color.Salmon;
                    hookCount++;
                }


                //add item into listview
                listViewKiService.Items.Add(item);
            }

            //change status text
            statusText.Text = hookCount.ToString() + " service calls are redirected from ntoskrnl.exe";
        }

        //fill the listview with the information from the interruptTable
        private void FillIntTableList()
        {
            for (int i = 0; i < interruptTable.Count; i++)
            {
                //our current entry in the list
                InterruptTableEntry entry = interruptTable.Get(i);

                //new item in list field
                ListViewItem item = new ListViewItem(String.Format("{0:000}", i), 0);

                //convert address into hex string
                string str = "0x" + Convert.ToString(entry.Address, 16);
                item.SubItems.Add(str);

                //add module name
                item.SubItems.Add(entry.Module.ToLower());

                //add item into listview
                listViewIntTable.Items.Add(item);
            }
        }

        //on load
        private void MainForm_Load(object sender, EventArgs e)
        {
            Driver LookupDriver = new Driver();

            //load driver as a service
            LookupDriver.Load();

            //open a device for the service
            if (!LookupDriver.Open("\\\\.\\Lookup"))
            {
                statusText.Text = "couldn't open lookup device";
                LookupDriver.Unload();
                return;
            }
            
            //create the kiServiceTable and get it from ring0
            kiServiceTable = LookupDriver.GetKiServiceTable();

            //reslove every service number into its name from ntdll.dll
            LookupDriver.GetKiServiceNames(kiServiceTable);

            //fill the table into the list field
            FillServiceTableList();

            //same for the interrupt table
            interruptTable = LookupDriver.GetInterruptTable();
            FillIntTableList();

            //close device handle
            LookupDriver.Close();
            //unload driver, we don't need it anymore
            LookupDriver.Unload();
        }

        //menu -> exit
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        //menu -> about
        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutForm about = new AboutForm();
            about.Show();
        }
    }
}