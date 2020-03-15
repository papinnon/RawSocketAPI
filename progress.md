# Raw Socket Dev log

为了正确发送构造的ip数据包。应该设计好透明的能向各端口发送数据的API。  

__man packet__  

负责接收SOCK_RAW socket 类型包的函数:
* bpf filter
* 使用  tcpdump -dd ether src xx:xx:xx:xx:xx:xx 生成filter
* 使用 tcpdump -d ... 反汇编filter
* linux/filter.h
* https://www.kernel.org/doc/Documentation/networking/filter.txt

一种较为优雅的拼接ip头部的方式。在漏洞利用中，似乎只需要在乎 长度，身份标识，和校验，标志，其他采用默认？  
class IP_hdr  
	char * pIP;  
	struct ipv4hdr  
	IP_hdr(src, dst, id,  ..)  
	func // to fill length  
	func // to set flags  
	

