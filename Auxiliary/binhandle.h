#ifndef BINHANDLE_H
#define BINHANDLE_H
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


#endif
