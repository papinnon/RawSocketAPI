#ifndef _RAWSOCKET_H_
#define _RAWSOCKET_H_

#include "./RawSocket.cpp"
SOCKET GetDGRAMSocket(const char * IfaceName, int Protocol ) ;
SOCKET GetRAWSocket(const char * IfaceName, int Protocol ) ;
int SendToMAC(SOCKET fd, const string MAC, const pbyte buffer, int count );
int RecvFromMac(SOCKET fd,  string MAC, pbyte * pbuffer, int count);

#endif
