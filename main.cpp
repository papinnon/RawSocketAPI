#include "RawSocket.h"
#include "ip.h"
int main()
{
	IPHdr hdr;
	byte buf[1540];
	byte dt[]="\x45\x00" \
		   "\x00\x28\x77\x7c\x40\x00\x40\x06\x1d\x26\xc0\xa8\x00\x69\x8c\xd2" \
		   "\x58\x4a\xdd\xd0\x01\xbb\x0d\x9e\x4d\x7c\xa5\xfd\x67\xae\x50\x10" \
		   "\x01\xf5\xbf\x5f\x00\x00";
	memcpy(&hdr, dt, 20);	
	SOCKET fd = GetRAWSocket("wlp2s0", ETH_P_ALL);
	SendToMAC(GetDGRAMSocket("wlp2s0",ETH_P_ALL), "11:22:33:44:55:66", (pbyte)&hdr, 20);
	RecvFromMac(fd, "ac:d5:64:98:ca:4f", buf, 1540 );
	hexdump(buf, 152);
	GetDataGramInfo(buf);
}
