// ALL_OPEN_TCP_PORTS.cpp : Defines the entry point for the console application.
//

#include <winsock2.h>
#include <ws2tcpip.h>
#include "stdafx.h"
#include "iostream"
using namespace std;
#include "Iphlpapi.h"



int _tmain(int argc, _TCHAR* argv[])
{

	struct in_addr IpAddr;
	char buf[MAX_PATH];

	PMIB_TCPTABLE2 table (new MIB_TCPTABLE2[1000]);
	if(table == NULL)
		cout<<"error allocating memory";

	unsigned long int data = 1000*sizeof(MIB_TCPTABLE2);
	if(GetTcpTable2(table, &data, true)==ERROR_INSUFFICIENT_BUFFER)
		cout<<"rekt";

	for(unsigned int i = 0 ; i<table->dwNumEntries; i++)
	{
		
		cout<<"process id: "<<table->table[i].dwOwningPid<<endl;

		IpAddr.S_un.S_addr = (u_long) table->table[i].dwLocalAddr;
	    strcpy_s(buf, sizeof (buf), inet_ntoa(IpAddr));       
		cout<<"local address: "<<buf<<endl;
		cout<<"local port: "<<table->table[i].dwLocalPort<<endl;

		IpAddr.S_un.S_addr = (u_long) table->table[i].dwRemoteAddr;		
	    strcpy_s(buf, sizeof (buf), inet_ntoa(IpAddr));
		cout<<"remote address:"<<buf<<endl;
		cout<<"remote port:"<<table->table[i].dwRemotePort<<endl;
		
		switch(table->table[i].dwState)
			{

            case MIB_TCP_STATE_CLOSED:
                printf("CLOSED\n");
                break;
            case MIB_TCP_STATE_LISTEN:
                printf("LISTEN\n");
                break;
            case MIB_TCP_STATE_SYN_SENT:
                printf("SYN-SENT\n");
                break;
            case MIB_TCP_STATE_SYN_RCVD:
                printf("SYN-RECEIVED\n");
                break;
            case MIB_TCP_STATE_ESTAB:
                printf("ESTABLISHED\n");
                break;
            case MIB_TCP_STATE_FIN_WAIT1:
                printf("FIN-WAIT-1\n");
                break;
            case MIB_TCP_STATE_FIN_WAIT2:
                printf("FIN-WAIT-2 \n");
                break;
            case MIB_TCP_STATE_CLOSE_WAIT:
                printf("CLOSE-WAIT\n");
                break;
            case MIB_TCP_STATE_CLOSING:
                printf("CLOSING\n");
                break;
            case MIB_TCP_STATE_LAST_ACK:
                printf("LAST-ACK\n");
                break;
            case MIB_TCP_STATE_TIME_WAIT:
                printf("TIME-WAIT\n");
                break;
            case MIB_TCP_STATE_DELETE_TCB:
                printf("DELETE-TCB\n");
                break;
            default:
                wprintf(L"UNKNOWN dwState value: %d\n", table->table[i].dwState);
                break;
	}
		
		cout<<endl<<endl;
	}
	

	//Getting Adapter ip address from current active adapter

	unsigned long int pSize = 10;
	int rData = ERROR_BUFFER_OVERFLOW;
	char *p = 0;
	
	while(rData==ERROR_BUFFER_OVERFLOW)
	{

		p = new char [pSize];
		if(p == NULL)
		{
			cout<<"error allocating memory\n";
			return 1;
		}
		
		rData = GetAdaptersInfo((PIP_ADAPTER_INFO)p, &pSize);
		if(rData==ERROR_BUFFER_OVERFLOW)
		{	
			delete [] p;
		}

		
	}


	switch(rData)
	{
	case ERROR_INVALID_DATA:
		cout<<"Invalid adapter information was retrieved.";
		return 1;
		break;
	case ERROR_NO_DATA:
		cout<<"No adapter information exists for the local computer.";
		return 1;
		break;
	case ERROR_NOT_SUPPORTED:
		cout<<"The GetAdaptersInfo function is not supported by the operating system running on the local computer.";
		return 1;
		break;
	case NO_ERROR:
		cout<<"ok\n";
		break;
	default:
		cout<<"problems have risen:/"<<endl;
		break;
	}




	PIP_ADAPTER_INFO pAdapter = (PIP_ADAPTER_INFO)p;

	//printf("pAdter: %p\n Ipaddresslist: %p\n", pAdapter, pAdapter->IpAddressList);
	
	printf("Adapter Desc: %s\n", pAdapter->Description);
	printf("adapter name: %s\n", pAdapter->AdapterName); 
	printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);
	cout<<"DhcpEnabled: "<<pAdapter->DhcpEnabled<<endl;
	cout<<"IPADDRESS: "<<pAdapter->IpAddressList.IpAddress.String<<endl;
	cout<<"subnet Mask: "<<pAdapter->IpAddressList.IpMask.String<<endl;
	PIP_ADDR_STRING deltaV = pAdapter->IpAddressList.Next;
	while(deltaV)
	{
		cout<<"2nd IP ADDRESS: "<<deltaV->IpAddress.String<<endl;
		deltaV = deltaV->Next;
	}

	tm newtime;
	errno_t error = _localtime32_s(&newtime, (__time32_t*) &pAdapter->LeaseObtained);
	char buffer[32];
	error = asctime_s(buffer, 32, &newtime);
	printf("LeaseObtained: %s\r\n", buffer);


	//GETUDPTABLE

	char *UDPTABLE = new char[1000];
	if(UDPTABLE==NULL)
		cout<<"error allocating memory\n";
	
	ULONG uData = 1000;
	GetUdpTable ((PMIB_UDPTABLE)UDPTABLE, &uData, true);
	PMIB_UDPTABLE UTABLE =  ( PMIB_UDPTABLE) UDPTABLE;

	for (unsigned long int i = 0; i<UTABLE->dwNumEntries; i++)
	{

		IpAddr.S_un.S_addr = (u_long) UTABLE->table[i].dwLocalAddr;
	    strcpy_s(buf, sizeof (buf), inet_ntoa(IpAddr));       
		cout<<"local address: "<<buf<<endl;
		cout<<"local port: "<<UTABLE->table[i].dwLocalPort<<endl;
	}
	 

	//GetExtendedUdpTable function with process ID

	char buf3[1000];
	unsigned long int udData = 1000;

	GetExtendedUdpTable((PMIB_UDPROW_OWNER_PID)buf3, &udData, true, AF_INET, UDP_TABLE_OWNER_PID, 0);

	PMIB_UDPTABLE_OWNER_PID  pUDP = (PMIB_UDPTABLE_OWNER_PID) buf3;
	cout<<"\n\n\n\n\n\n";
	int ctr = 0;

	for(unsigned long int i = 0 ; i <pUDP->dwNumEntries; i++)
		{
			IpAddr.S_un.S_addr = (u_long) pUDP->table[i].dwLocalAddr;
			strcpy_s(buf, sizeof (buf), inet_ntoa(IpAddr));       
			cout<<"local address: "<<buf<<endl;
			cout<<"local port: "<<pUDP->table[i].dwLocalPort<<endl;
			cout<<"process id: "<<pUDP->table[i].dwOwningPid<<endl;
			ctr++;
		}



	cout<<"CTR: "<<ctr<<endl;



	 


	delete [] table;
	delete [] p;
	return 0;
}

