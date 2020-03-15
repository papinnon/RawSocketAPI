#ifndef IP_H
#define IP_H
typedef struct _IP_HEADER 
{
	char   hdrlen:4 ,
	       ver:4;
	char tos;
	short tot_len;
	short id;
	short frag_off;
	char ttl;
	char protocol;
	short check;
	unsigned int saddr;
	unsigned int daddr;
} __attribute__((packed))IPHdr, *pIPHdr ;
#endif

