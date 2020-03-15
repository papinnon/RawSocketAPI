#ifndef RAWSOCKET_CPP
#define RAWSOCKET_CPP
#include "RawSocket.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <string>

#include <net/if.h> // fro struct ifreq
#include <arpa/inet.h> // for htons
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h> // for IEEE802.3 protocols
#include <sys/ioctl.h>
/*************************************************************************************
// Specify string iterface name use argument IfaceName, Specify Protocol(if_ether.h:46)
// use argument Protocol. 
// The returning socket will **only** intercept [Protocol] packets from driver.
*************************************************************************************/

typedef int SOCKET;
typedef struct sockaddr_ll SADDR;
typedef struct ifreq IFREQ;
SADDR gsaddr;
SOCKET GetDGRAMSocket(const char * IfaceName, int Protocol ) 
{/*{{{*/
	SOCKET sockfd;
	SADDR  sa_ll;
	IFREQ  ifr;	
	bzero(&sa_ll, sizeof(SADDR));
	bzero(&ifr, sizeof(IFREQ));
	strncpy(ifr.ifr_name, IfaceName, IFNAMSIZ);	
	if( ( sockfd = socket( PF_PACKET, SOCK_DGRAM, htons(Protocol))) == -1)
	{
		perror("create socket");
		exit(1);
	}
	if( ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1)
	{
		perror("get interface index ");
		exit(1);
	}
	// Prepase sockaddr_ll for binding(see man 7 packet)
	sa_ll.sll_family= AF_PACKET;
	sa_ll.sll_ifindex= ifr.ifr_ifindex;
	sa_ll.sll_protocol= htons(Protocol);
	sa_ll.sll_pkttype = PACKET_HOST;
	if( bind(sockfd, (const sockaddr* )&sa_ll, sizeof(sa_ll))== -1)
	{
		perror("bind to interface");
		exit(1);
	}
	memcpy(&gsaddr, &sa_ll, sizeof(SADDR));
	return sockfd;
}/*}}}*/

SOCKET GetRAWSocket(const char * IfaceName, int Protocol ) 
{/*{{{*/
	SOCKET sockfd;
	SADDR  sa_ll;
	IFREQ  ifr;	
	bzero(&sa_ll, sizeof(SADDR));
	bzero(&ifr, sizeof(IFREQ));
	strncpy(ifr.ifr_name, IfaceName, IFNAMSIZ);	
	if( ( sockfd = socket( AF_PACKET, SOCK_RAW, htons(Protocol))) == -1)
	{
		perror("create socket");
		exit(1);
	}
	if( ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1)
	{
		perror("get interface index ");
		exit(1);
	}
	// Prepase sockaddr_ll for binding(see man 7 packet)
	sa_ll.sll_family= AF_PACKET;
	sa_ll.sll_ifindex= ifr.ifr_ifindex;
	sa_ll.sll_protocol= htons(Protocol);
	sa_ll.sll_pkttype = PACKET_HOST;
	if( bind(sockfd, (const sockaddr *)&sa_ll, sizeof(sa_ll))== -1)
	{
		perror("bind to interface");
		exit(1);
	}
	return sockfd;
}/*}}}*/


/*************************************************************************************
// Send count bytes point to by buffer to MAC address in interface fd is bound to.
// MAC can be std::string const char *
// Recv versa
*************************************************************************************/

using std::string;
typedef unsigned char byte,*pbyte;
static inline void set_mac(pbyte dest, string mac);
int SendToMAC(SOCKET fd, const string MAC, const pbyte buffer, int count )
{/*{{{*/
	int cnt;
	SADDR peer= gsaddr;
	//Set peer MAC
	set_mac(peer.sll_addr, MAC);
	peer.sll_halen=6;
	//Sendto
	if( (cnt= sendto(fd, buffer, count, 0, (const sockaddr *)&peer, sizeof(SADDR)))== -1)
	{
		perror("SendToMac");
		exit(1);
	}
	return cnt;
}/*}}}*/

#include "./Auxiliary/hexdump.h"
#include "./Auxiliary/binhandle.h"
#include <linux/filter.h>
struct sock_filter prog_pat []={
	{ 0x20, 0, 0, 0x00000008 },
	{ 0x15, 0, 3, 0x0d594058 },
	{ 0x28, 0, 0, 0x00000006 },
	{ 0x15, 0, 1, 0x0000d815 },
	{ 0x6, 0, 0, 0x00040000 },
	{ 0x6, 0, 0, 0x00000000 },
};
int RecvFromMac(SOCKET fd,  string MAC, pbyte  buffer, int count)
{/*{{{*/
	int cnt;
	int sz;
	int pos;
	struct sock_fprog filter;
	string PeerMac;
	SADDR peer;
	pbyte tmpbuf;

	//prepare packet filter
	prog_pat[3].k=0	;
	prog_pat[1].k=0	;
	pos = MAC.find(":");
	prog_pat[3].k+= integer(MAC.substr(0,pos), 16) << 8; 
	MAC = MAC.substr(pos+1);
	pos = MAC.find(":");
	prog_pat[3].k+= integer(MAC.substr(0,pos), 16); 
	MAC = MAC.substr(pos+1);
	pos = MAC.find(":");
	prog_pat[1].k+= integer(MAC.substr(0,pos), 16)<< 24; 	
	MAC = MAC.substr(pos+1);
	pos = MAC.find(":");
	prog_pat[1].k+= integer(MAC.substr(0,pos), 16)<< 16; 	
	MAC = MAC.substr(pos+1);
	pos = MAC.find(":");
	prog_pat[1].k+= integer(MAC.substr(0,pos), 16)<< 8; 	
	MAC = MAC.substr(pos+1);
	prog_pat[1].k+= integer(MAC.substr(0,pos), 16); 	
	//install filter
	filter.len = sizeof(prog_pat)/sizeof(prog_pat[0]);
	filter.filter = prog_pat;
	setsockopt(fd , SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter));
	// recv
	tmpbuf = new byte [count+14];
	bzero(tmpbuf, count+14);
	cnt =recvfrom(fd, tmpbuf, count+14, 0 , ( sockaddr *)&peer,(unsigned int *)&sz );
	memcpy(buffer, tmpbuf+14, count);
	delete[] tmpbuf;
	return cnt==-1?-1:cnt-14;
}/*}}}*/


#include <stddef.h>
#include "./Auxiliary/binhandle.h"
typedef unsigned char byte,*pbyte;
static inline void set_mac(pbyte dest, string mac)
{/*{{{*/
	size_t NOTFOUND= std::string::npos;	
	ssize_t pos=0,idx=0;
	byte b;
	if(!dest)
		return ;
	while(NOTFOUND != (pos=mac.find(":")))
	{
		b = integer(mac.substr(0,pos),16);	
		dest[idx++]= b;
		mac=mac.substr(pos+1);
	}
	dest[idx] = integer(mac.substr(0,pos),16);	
}/*}}}*/

// test main / delete afterwards
#include "./Auxiliary/hexdump.h"
#include "./Auxiliary/binhandle.h"
int __main()
{/*{{{*/
	byte buffer [0x10000];
	memset(buffer, 0, 0x10000);
	SADDR peer;
	int sz,shit;

	SOCKET fd=GetRAWSocket("wlp2s0", ETH_P_ALL);
	SOCKET fddg =GetDGRAMSocket("wlp2s0", ETH_P_ALL);
	SendToMAC(fddg, "ac:d5:64:98:ca:4f", (const pbyte)"aaaaaaaa", 8);
	while(1)
	{
		//recvfrom(fd, buffer, 0x1540, 0 , ( sockaddr *)&peer,(unsigned int *)&sz );
		RecvFromMac(fd, "ac:d5:64:98:ca:4f", buffer, 1540 );
		hexdump(buffer,0x150);
		for(int i =0; i<4 ; ++i)
			std::cout << (int)buffer[12+i]<<".";
		std::cout <<'\n';
		for(int i =0; i<4 ; ++i)
			std::cout << (int)buffer[16+i]<<".";
		scanf("%d",&shit);
	}
	hexdump(buffer,0x150);
		return 0;
}/*}}}*/

#include "./ip.h"
void GetDataGramInfo(const void * phdr)
{/*{{{*/
	pbyte buffer = (pbyte)phdr;
	pIPHdr pIP = (pIPHdr)phdr;
	std::cout<<"src IP: ";
	for(int i =0; i<4 ; ++i)
	{
		std::cout << (int)buffer[12+i]<<".";
	}
	std::cout <<'\n';
	std::cout<<"dst IP: ";
	for(int i =0; i<4 ; ++i)
	{
		std::cout << (int)buffer[16+i]<<".";
	}
	std::cout << '\n';
	std::cout << "IP details**************************************\n";
	std::cout << "Version        : "<< (int)pIP->ver << std::endl;
	std::cout << "Hdr Length     : "<< pIP->hdrlen*4 << std::endl;
	std::cout << "TOS            : "<< (int)pIP->tos << std::endl;
	std::cout << "Length         : "<< pIP->tot_len << std::endl;
	std::cout << "ID             : "<< pIP->id << std::endl;
	std::cout << "Fragment offset: "<< pIP->frag_off << std::endl;
	std::cout << "Protocol       : "<< (int)pIP->protocol<<std::endl;
	std::cout << "checksum       : "<< pIP->check <<std::endl;
}/*}}}*/
#endif
