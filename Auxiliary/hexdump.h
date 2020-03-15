#ifndef HEXDUMP_H
#define HEXDUMP_H
#include <iostream>
//#include <format>
#include <stdio.h>
#include <stdarg.h>
#include <memory>
#include <string.h>
#define sizet int
#define PAD_FOR_UNPRINT '_'
int canPrint(unsigned char c)
{/*{{{*/
	return (c>=32&&c<127);
}/*}}}*/

std::string string_format(const std::string fmt_str, ...) 
{/*{{{*/
    int final_n, n = ((int)fmt_str.size()) * 2; 
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return std::string(formatted.get());
}/*}}}*/

void hexdump(unsigned char const * buf, sizet cnt)
{/*{{{*/
	int lines= cnt/16;
	for(int i=0; i<lines; ++i)
	{
//printf("%020x    ",i*16);
//		std::cout << std::format("{:0>x}    ",i*16);
		std::cout <<string_format("%016x    ",i*16);    
		for(int j =0; j<16; ++j)
			std::cout <<string_format("%02x",(int)buf[i*16+j])<<" ";
		for(int j =0; j<16; ++j)
			std::cout << (char)(canPrint(buf[i*16+j])?(char)buf[i*16+j]:PAD_FOR_UNPRINT);
		std::cout<<"\n";
	}
//	printf("%020x    ",lines*16);
//	std::cout << std::format("{:0>x}    ",lines*16);
	std::cout <<string_format("%016x    ",lines*16);    
	for(int j =0; j<cnt%16; ++j)
		std::cout <<string_format("%02x",(int)buf[lines*16+j])<<" ";
	for(int j =0; j<cnt%16; ++j)
		std::cout << (char)(canPrint(buf[lines*16+j])?(char)buf[lines*16+j]:PAD_FOR_UNPRINT);
	std::cout<<"\n";
	return;
}/*}}}*/

//int main()
//{
//	unsigned char buf[] = {0xa1,0xa2,0xb3,0xc4,0xd5,0xe6,0xf7,0x08};
//	hexdump(buf,8);
//}
#endif
