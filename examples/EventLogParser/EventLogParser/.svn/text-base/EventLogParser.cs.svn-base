using System;
using System.Collections.Generic;
using System.Data;
using System.Text;
using System.IO;
using System.Runtime;
using System.Runtime.InteropServices;
using System.Collections;
using System.Diagnostics;

namespace EventLogParser
{
    // Show the progress 
    public delegate void ProgressHandler(int val, int max);
    // Send any message to UI
    public delegate void MessageHandler(string msg);
    // Parsed a new event.
    public delegate void NewEventFoundHandler(object[] items);

    class EventLogParser
    {
        //Constants for api call
        const int NO_ERROR = 0;
        const int ERROR_INSUFFICIENT_BUFFER = 122;

        public event ProgressHandler OnProgress;
        public event MessageHandler OnAction;
        public event NewEventFoundHandler OnFoundRecord;

        uint offset;

        enum SID_NAME_USE
        {
            SidTypeUser = 1,
            SidTypeGroup,
            SidTypeDomain,
            SidTypeAlias,
            SidTypeWellKnownGroup,
            SidTypeDeletedAccount,
            SidTypeInvalid,
            SidTypeUnknown,
            SidTypeComputer
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public unsafe struct EventLogHeader
        {
            public uint HeaderLength; // length 
            public uint Signature; // signature
            public uint Unknown1;
            public uint Unknown2;
            public uint Unknown3;
            public uint FooterOffset;
            public uint NextIndex;
            public uint FileLength; // always wont give correct value
            public uint Unknown6;
            public uint Unknown7;
            public uint Unknown8;
            public uint EndHeaderLength;

            public EventLogHeader(byte[] data)
            {
                fixed (byte* pData = data)
                {
                    this = *(EventLogHeader*)pData;
                }
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public unsafe struct EventLogFooter
        {
            public uint FooterLength; // length 
            public uint Unknown0; // 0x11111111
            public uint Unknown1; // 0x22222222
            public uint Unknown2; // 0x33333333
            public uint Unknown3; // 0x44444444
            public uint Unknown4;
            public uint FooterOffset;
            public uint NextIndex;
            public uint Unknown7;
            public uint EndFooterLength;

            public EventLogFooter(byte[] data)
            {
                fixed (byte* pData = data)
                {
                    this = *(EventLogFooter*)pData;
                }
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public unsafe struct EventLogRecord
        {
            public uint Length;
            public uint Reserved;
            public uint RecordNumber;
            public uint TimeGenerated;
            public uint TimeWritten;
            public uint EventID;

            public ushort EventType;
            public ushort NumStrings;
            public ushort EventCategory;
            public ushort ReservedFlags;
            public uint ClosingRecordNumber;
            public uint StringOffset;
            public uint UserSidLength;
            public uint UserSidOffset;
            public uint DataLength;
            public uint DataOffset;

            public EventLogRecord(byte[] data)
            {
                fixed (byte* pData = data)
                {
                    this = *(EventLogRecord*)pData;
                }
            }

        }

        [StructLayout(LayoutKind.Sequential)]
        private struct SystemTime
        {
            public short year;
            public short month;
            public short dayOfWeek;
            public short day;
            public short hour;
            public short minute;
            public short second;
            public short milliseconds;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        private struct TimeZoneInformation
        {
            public int bias;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string standardName;
            public SystemTime standardDate;
            public int standardBias;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string daylightName;
            public SystemTime daylightDate;
            public int daylightBias;
        }

        // Class for holding an event log entry
        public class EventLogEntry
        {
            public EventLogRecord rec;
            public string SourceName;
            public string Computername;
            public string UserSid;
            public string Strings;
            public byte[] Data;
        }

        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static extern bool LookupAccountSid(string lpSystemName,[MarshalAs(UnmanagedType.LPArray)] byte[] Sid,System.Text.StringBuilder lpName,ref uint cchName,System.Text.StringBuilder ReferencedDomainName,ref uint cchReferencedDomainName,out SID_NAME_USE peUse);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        private static extern int GetTimeZoneInformation(out TimeZoneInformation lpTimeZoneInformation);

        // Ctor
        public EventLogParser()
        {
            TimeZoneInformation tzi;
            GetTimeZoneInformation(out tzi);
            offset = (uint)(tzi.bias * 60) - (uint)(tzi.daylightBias * 60);
        }

        // Parse the file
        public unsafe void Parse(string filename)
        {
            try
            {
                // Open the file
                using (FileStream fs = new FileStream(filename, FileMode.Open))
                {
                    // Use BinaryReader to read the file
                    using (BinaryReader br = new BinaryReader(fs))
                    {
                        //Read the header of the file
                        byte[] header = new byte[sizeof(EventLogHeader)];
                        br.Read(header, 0, header.Length);
                        EventLogHeader _h = new EventLogHeader(header);
                        // Validate the file
                        if (!Validate(_h))
                        {
                            this.OnAction("Invalid file format.");
                            return;
                        }
                        //
                        int totalEvents = (int)(_h.NextIndex - 1);
                        this.OnAction(String.Format("Found {0} events", totalEvents));
                        // Read the items
                        EventLogEntry e;
                        int cnt = 0;
                        uint offset = _h.FooterOffset;
                        while (true)
                        {
                            byte[] buff = ReadEntry(br, ref offset);
                            e = ReadEntry(buff);
                            cnt++;
                            DateTime dt = GetTime(e.rec.TimeGenerated);
                            this.OnFoundRecord(
                                new object[] { 
                                    Enum.GetName(typeof(EventLogEntryType),e.rec.EventType),
                                    dt.ToShortDateString(),
                                    dt.ToShortTimeString(),
                                    e.SourceName,
                                    e.Strings,
                                    e.rec.EventCategory,
                                    e.rec.EventID,
                                    e.UserSid, 
                                    e.Computername});
                            if (cnt % 200 == 0) this.OnProgress(cnt, totalEvents);
                            if (offset == 48)
                                break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                this.OnAction(String.Format("Error Occured! {0}", ex.Message));
            }
            return;
        }

        // Read an event log record as byte[] from the file.
        private byte[] ReadEntry(BinaryReader br, ref uint endPoint)
        {
            br.BaseStream.Seek(endPoint - 4, SeekOrigin.Begin);
            uint length = br.ReadUInt32();
            endPoint -= length;
            br.BaseStream.Seek(endPoint, SeekOrigin.Begin);
            byte[] buff = new byte[length];
            br.Read(buff, 0, buff.Length);
            return buff;
        }

        // Parse the byte[] as an event log record
        private unsafe EventLogEntry ReadEntry(byte[] buff)
        {
            EventLogEntry entry;
            try
            {
                fixed (byte* ptr = buff)
                {
                    entry = new EventLogEntry();
                    entry.rec = new EventLogRecord(buff);
                    // Read SourceName
                    uint start = (uint)sizeof(EventLogRecord);
                    // Get the Source Name
                    entry.SourceName = ReadString(ptr, ref start);
                    // Get the Computer Name
                    entry.Computername = ReadString(ptr, ref start);
                    // Get the User Name
                    byte[] uname = new byte[entry.rec.UserSidLength];
                    Copy(ptr, ref start, uname, uname.Length);
                    entry.UserSid = GetUserInfo(uname);
                    // read the strings
                    entry.Strings = ReadString(ptr, ref start, (int)(entry.rec.DataOffset - entry.rec.StringOffset) / 2);
                    // read the data
                    entry.Data = new byte[(int)entry.rec.DataLength];
                    Copy(ptr, ref start, entry.Data, entry.Data.Length);
                    //
                }
            }
            catch (Exception)
            {
                entry = null;
            }
            return entry;
        }

        // Check whether the evt file is a valid one.
        private bool Validate(EventLogHeader header)
        {
            if (!(header.HeaderLength == 0x00000030 &&
                    header.Signature == 0x654C664c &&
                    header.Unknown1 == 0x00000001 &&
                    header.Unknown2 == 0x00000001))
                return false;
            return true;
        }

        // Read string from the byte[]
        private unsafe string ReadString(byte* ptr, ref uint start)
        {
            StringBuilder result = new StringBuilder();
            char temp;
            ptr += start;
            while (true)
            {
                temp = (char)*((ushort*)ptr);
                ptr += 2;
                start += 2;
                if (temp == '\0')
                    break;
                result.Append(temp);
            }
            return result.ToString();
        }

        // Read the Description according to the length specified.
        private unsafe string ReadString(byte* ptr, ref uint start, int count)
        {
            StringBuilder result = new StringBuilder(count);
            char temp;
            ptr += start;
            for (; count > 0; count--)
            {
                temp = (char)*((ushort*)ptr);
                ptr += 2;
                start += 2;
                result.Append(temp);
            }
            return result.ToString();
        }

        // Get the user name from SID
        private string GetUserInfo(byte[] buff)
        {
            StringBuilder name = new StringBuilder();
            uint cchName = (uint)name.Capacity;
            StringBuilder referencedDomainName = new StringBuilder();
            uint cchReferencedDomainName = (uint)referencedDomainName.Capacity;
            SID_NAME_USE sidUse;

            int err = NO_ERROR;
            if (!LookupAccountSid(null, buff, name, ref cchName, referencedDomainName, ref cchReferencedDomainName, out sidUse))
            {
                err = System.Runtime.InteropServices.Marshal.GetLastWin32Error();
                if (err == ERROR_INSUFFICIENT_BUFFER)
                {
                    name.EnsureCapacity((int)cchName);
                    referencedDomainName.EnsureCapacity((int)cchReferencedDomainName);
                    err = NO_ERROR;
                    if (!LookupAccountSid(null, buff, name, ref cchName, referencedDomainName, ref cchReferencedDomainName, out sidUse))
                        err = System.Runtime.InteropServices.Marshal.GetLastWin32Error();
                }
            }
            if (err == 0)
                return String.Format(@"{0}\{1}", referencedDomainName.ToString(), name.ToString());
            else
                return @"N\A";
        }

        // copy the byte[]
        private unsafe void Copy(byte* pSrc, ref uint srcIndex, byte[] dst, int count)
        {
            if (count == 0)
                return;
            fixed (byte* pDst = dst)
            {
                byte* ps = pSrc;
                ps += srcIndex;
                byte* pd = pDst;

                // Loop over the count in blocks of 4 bytes, copying an
                // integer (4 bytes) at a time:
                for (int n = 0; n < count / 4; n++)
                {
                    *((int*)pd) = *((int*)ps);
                    pd += 4;
                    ps += 4;
                }

                // Complete the copy by moving any bytes that weren't
                // moved in blocks of 4:
                for (int n = 0; n < count % 4; n++)
                {
                    *pd = *ps;
                    pd++;
                    ps++;
                }

                srcIndex += (uint)count;
            }
        }

        // Convert to seconds to date time format
        private DateTime GetTime(uint time)
        {
            DateTime output = new DateTime(1970, 1, 1, 0, 0, 0);
            time = time - offset;
            output = output.AddSeconds(time);
            return output;
        }
    }
}
