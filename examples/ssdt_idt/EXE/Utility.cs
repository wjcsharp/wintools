using System;
using System.Collections;
using System.Text;

namespace Lookup
{
    //complete KiServiceTable
    class KiServiceTable
    {
        private ArrayList kiServiceTable;       //our ServiceTable

        //constructor
        public KiServiceTable()
        {
            kiServiceTable = new ArrayList();   //initialize empty array
        }

        //get numberOfServices in the list
        public int NumberOfServices
        {
            get { return kiServiceTable.Count; }
        }

        //get one entry at index
        public KiServiceTableEntry Get(int index)
        {
            //when index out of range
            if (index > kiServiceTable.Count - 1)
                return null;

            //we need to cast into KiServiceTableEntry
            //but we add only this type, thats why np
            return (KiServiceTableEntry)kiServiceTable[index];
        }

        //add one entry
        public void Add(KiServiceTableEntry kiServiceTableEntry)
        {
            kiServiceTable.Add(kiServiceTableEntry);
        }
    }

    //one entry in the KiServiceTable
    class KiServiceTableEntry
    {        
        private uint address;       //address
        private string name;        //name from the service entry
        private string module;      //module name

        //constructor
        public KiServiceTableEntry(uint address)
        {            
            this.address = address;
            this.name = "";
            this.module = "";
        }

        public uint Address         //property for address
        {
            get { return address; }
        }

        public string Name          //property for name
        {
            get { return name; }
            set { name = value; }
        }

        public string Module        //property for module
        {
            get { return module; }
            set { module = value; }
        }
    }

    //same for interrupt table, see comments above
    class InterruptTable
    {
        private ArrayList interruptTable;

        public InterruptTable()
        {
            interruptTable = new ArrayList();
        }

        public int Count
        {
            get { return interruptTable.Count; }
        }

        public InterruptTableEntry Get(int index)
        {
            if (index > interruptTable.Count - 1)
                return null;

            return (InterruptTableEntry)interruptTable[index];
        }

        public void Add(InterruptTableEntry intTableEntry)
        {
            interruptTable.Add(intTableEntry);
        }

    }

    class InterruptTableEntry
    {
        private uint address;
        private string module;

        public InterruptTableEntry(uint address)
        {
            this.address = address;
            this.module = "";
        }

        public uint Address
        {
            get { return address; }
        }

        public string Module
        {
            get { return module; }
            set { module = value; }
        }
    }
}
