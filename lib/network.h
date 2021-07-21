#ifndef NETWORK_H
#define NETWORK_H








// -------------------------------- DEFINITIONS --------------------------------

//standard network types
#include <arpa/inet.h>








// -------------------------------- DEFINITIONS --------------------------------

//owners
#define NETWORK__CLIENT 0x00
#define NETWORK__SERVER 0x80



//protocols
#define NETWORK__TCP 0x00
#define NETWORK__UDP 0x40



//IP types
#define NETWORK__IPV4 0x00
#define NETWORK__IPV6 0x20



//utility macros
#define network_getOwner(n)  ( (n)->o_p_i & 0x80)
#define network_getPrtcl(n)  ( (n)->o_p_i & 0x40)
#define network_getIPType(n) ( (n)->o_p_i & 0x20)



//listen() backlog
#define NETWORK__LISTEN_BACKLOG 5



//type definitions
typedef struct{
	char o_p_i; //XYZ0 0000 (X:owner, Y:protocol, Z:IPType)
	int fd;
	void* sock;
	socklen_t sock_size;
} network;








// -------------------------------- BASICS --------------------------------

//create - delete
network* network_create(char owner, char protocol, char IPType);
void network_delete(network* nw);



//connect - bind - accept
void network_bind(network* nw, unsigned short int port);                   //available for servers only
network* network_accept(network* nw);                                           //available for servers in TCP only
int network_connect(network* nw, char* address, unsigned short int port); //available for clients only



//send - receive
void network_sendTo(network* src, network* dest, char* data, size_t len);      // #data# must not be NULL or unallocated
void network_receiveFrom(network* src, network* dest, char* data, size_t len); // #data# shall be NULL or unallocated//get/set info



//get / set info
char* network_getAddress(network* nw);                                         //will allocate 16 bytes
void network_setInfo(network* nw, char* address, unsigned short int port);








#endif
