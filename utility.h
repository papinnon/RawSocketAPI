#ifndef _utility_h
#define _utility_h
typedef struct ethhdr EHDR;
#include "utility.cpp"
void List_IF();

int Set_ifPromisc(const char * if_name, int flag);

void Bind_IF(int sockfd, const char * if_name, int prot);

template<typename T>
long long int integer(const T str,  int type);

inline int set_ethhdr( EHDR * const hdr, std::string dst, std::string src, unsigned short protocol);

inline void set_mac(pbyte dest, std::string mac);

uint64_t getCurrentTimestamp();

#endif
