#include<string.h>
#include<stdio.h>
#include <stdlib.h>
#include <net/if.h>
#include <linux/if_ether.h>


#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h> //htons
#include <sys/socket.h> //Man 7 packet
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <assert.h>
#include "./Auxiliary/hexdump.h"

void List_IF()
{/*{{{*/
	int sockfd=0;
	struct ifreq ifreq_orig;
	memset(&ifreq_orig, 0 ,sizeof(ifreq_orig));
	if((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP))) == -1)
	{
		perror("socket");
		exit(0);
	}
	for(int i =0; i < 100; ++i)
	{
		ifreq_orig.ifr_ifindex=i;
		if( -1 ==ioctl(sockfd, SIOCGIFNAME, &ifreq_orig))
			continue;
		printf("%d: %20s\n", i,ifreq_orig.ifr_name);
	}
	return;
}/*}}}*/

//int Set_ifPromisc(const char * if_name, int flag)
//{/*{{{*/
//	int sockfd=0;
//	struct ifreq ifreq_orig;
//	memset(&ifreq_orig, 0 ,sizeof(ifreq_orig));
//	if((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP))) == -1)
//	{
//		perror("socket");
//		exit(0);
//	}
//	strncpy(ifreq_orig.ifr_name, if_name, strlen(if_name));
//	if(ioctl(sockfd, SIOCGIFFLAGS, &ifreq_orig) == -1)
//	{
//		perror("ioctl");
//		return -1;
//	}
//#if DEBUG
//	printf("is running: %20s\n", ifreq_orig.ifr_flags & IFF_UP ?"True":"False");
//	printf("is lo: %20s\n", ifreq_orig.ifr_flags & IFF_LOOPBACK ?"True":"False");
//	printf("is promic: %20s\n", ifreq_orig.ifr_flags & IFF_PROMISC ?"True":"False");
//	puts("set flags...");
//#endif
//	if(flag)
//		ifreq_orig.ifr_flags |= IFF_PROMISC;
//	else
//		ifreq_orig.ifr_flags &= ~IFF_PROMISC;
//	if(ioctl(sockfd, SIOCSIFFLAGS, &ifreq_orig) == -1)
//	{
//		perror("ioctl");
//		return -1;
//	}
//	if(ioctl(sockfd, SIOCGIFFLAGS, &ifreq_orig) == -1)
//	{
//		perror("ioctl");
//		return -1;
//	}
//	printf("%s is promisc: %20s\n", if_name,ifreq_orig.ifr_flags & IFF_PROMISC ?"True":"False");
//	close(sockfd);
//	return 0;
//
//}/*}}}*/
//
//void Bind_IF(int sockfd, const char * if_name, int prot)
//{/*{{{*/
//	struct sockaddr_ll sa_ll;
//	struct ifreq ifr;bzero(&sa_ll,sizeof(sa_ll));
//	bzero(&ifr, sizeof(ifr));
//	strncpy(ifr.ifr_name, if_name, IFNAMSIZ);
//	if(-1 == ioctl(sockfd, SIOCGIFINDEX, &ifr))
//	{
//		perror("ioctl");
//		exit(0);
//	}
//	sa_ll.sll_family = AF_PACKET;
//	sa_ll.sll_ifindex= ifr.ifr_ifindex;
//	sa_ll.sll_protocol= htons(prot);
//	if(-1 == bind(sockfd, (struct sockaddr *)&sa_ll, sizeof(sa_ll)))
//	{
//		perror("bind");
//		exit(0);
//	}
//	return;
//}/*}}}*/

#include <sstream>
template<typename T>
long long int integer(const T str,  int type)
{/*{{{*/
	std::stringstream ss;
	long long int ret;
	if(type == 16)
	{
		ss<< std::hex << str;
		ss >>ret;
		return ret;
	}
	else if(type == 10)
	{
		ss<< str;
		ss>> ret;
		return ret;
	}
	else
		return 0;	
}/*}}}*/

#include <string>
#include <stddef.h>
typedef unsigned char byte,*pbyte;
inline void set_mac(pbyte dest, std::string mac)
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

inline int set_ethhdr( EHDR * const hdr, std::string dst, std::string src, unsigned short protocol)
{/*{{{*/
	set_mac(hdr->h_dest, dst);
	set_mac(hdr->h_source, src);
	hdr->h_proto= protocol;
	return 0;
}/*}}}*/

#include <sys/time.h>
uint64_t getCurrentTimestamp()
{/*{{{*/
	struct timeval t;
	
	int code = gettimeofday( &t, NULL );
	assert( code == 0 );
	if ( code != 0 )
	{
		perror( "error calling gettimeofday" );
		assert( 0 );
	}
	
	// Convert seconds to microseconds
	// For the purposes of 802.11 timestamps, we don't care about what happens
	// when this value wraps. As long as the value wraps consistently, we are
	// happy
	uint64_t timestamp = t.tv_sec * 1000000LL;
	timestamp += t.tv_usec;
	
	return timestamp;
}/*}}}*/
