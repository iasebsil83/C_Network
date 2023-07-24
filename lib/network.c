// -------------------------------- IMPORTATIONS --------------------------------

//standard
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

//network
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//own header
#include "network.h"








/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Network [0.1.2] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                 Network by I.A.

        Network is just an utility program that allows you to create and join
    networks using TCP or UDP in IPv4 or IPv6.

    15/04/2021 > [0.1.0] :
    - Created network.c/.h.
    - Implemented TCP in IPv4.
    - Implemented TCP in IPv6.
    - Started implementation of UDP in IPv4.
    - Started implementation of UDP in IPv6.

    28/04/2021 > [0.1.1] :
    - Finished implementation of UDP in IPv4.
    - Finished implementation of UDP in IPv6.

    07/01/2023 > [0.1.2] :
    - Fixed everything in TCP & UDP for both IPv4 & IPv6.

    BUGS : .
    NOTES : .

    Contact     : i.a.sebsil83@gmail.com
    Youtube     : https://www.youtube.com/user/IAsebsil83
    GitHub repo : https://github.com/iasebsil83

    Let's Code !                                  By I.A.
*********************************************************************************
    LICENCE :

    C_Network
    Copyright (C) 2023 Sebastien SILVANO

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.

    If not, see <https://www.gnu.org/licenses/>.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */








// -------------------------------- BASICS --------------------------------

//create - delete
network* network_create(char owner, char protocol, char IPType){

	//create instance
	network* nw = malloc(sizeof(network));
	if(!nw){
		printf("FATAL ERROR > network.c : network_create() : Computer refuses to give more memory.\n");
		exit(EXIT_FAILURE);
	}

	//set values
	nw->o_p_i = 0x00;
	nw->o_p_i |= owner;
	nw->o_p_i |= protocol;
	nw->o_p_i |= IPType;

	//set IP type & sock size
	int IP_type;
	if( network_getIPType(nw) ){
		IP_type = AF_INET6;

		//set sock size
		nw->sock_size = sizeof(struct sockaddr_in6);
	}else{
		IP_type = AF_INET;

		//set sock size
		nw->sock_size = sizeof(struct sockaddr_in);
	}

	//allocate socket structure
	nw->sock = malloc(nw->sock_size);
	if(!(nw->sock)){
		printf("FATAL ERROR > network.c : network_create() : Computer refuses to give more memory.\n");
		exit(EXIT_FAILURE);
	}
	bzero(nw->sock, nw->sock_size);

	//set socket type
	int sock_type;
	if( network_getPrtcl(nw) ){
		sock_type = SOCK_DGRAM;
	}else{
		sock_type = SOCK_STREAM;
	}

	//create socket
	nw->fd = socket(IP_type, sock_type, 0);
	if(nw->fd == -1){
		printf("RUNTIME ERROR > network.c : network_create() : Could not create socket.\n");
		free(nw);
		return NULL;
	}

    return nw;
}

void network_delete(network* nw){

	//error cases
	if(!nw){
		printf("RUNTIME ERROR > network.c : network_delete() : Network instance is NULL.\n");
		return;
	}

	//flush data in socket
	if(nw->fd != -1){
		while( close(nw->fd) == -1 );
	}

	//free structure
	if(nw->sock){
		free(nw->sock);
	}
	free(nw);
}




//connect - bind - accept
void network_bind(network* nw, unsigned short int port){ //available for servers only

	//error case
	if(!nw){
		printf("RUNTIME ERROR > network.c : network_bind() : Network instance is NULL.\n");
		return;
	}
	if( !network_getOwner(nw) ){
		printf("RUNTIME ERROR > network.c : network_bind() : Network instance is set as \"CLIENT\" and clients cannot use network_bind().\n");
		return;
	}

	//set socket structure
	int errorCode = 0;
	if( network_getIPType(nw) ){
		( (struct sockaddr_in6*)(nw->sock) )->sin6_family = AF_INET6;
		( (struct sockaddr_in6*)(nw->sock) )->sin6_addr   = in6addr_any;
		( (struct sockaddr_in6*)(nw->sock) )->sin6_port   = htons(port);

		//bind
		errorCode = bind(
			nw->fd,
			(struct sockaddr*)(nw->sock),
			nw->sock_size
		);
	}else{
		  ( (struct sockaddr_in*)(nw->sock) )->sin_family        = AF_INET;
		( ( (struct sockaddr_in*)(nw->sock) )->sin_addr ).s_addr = htonl(INADDR_ANY); //all addresses are bound
		  ( (struct sockaddr_in*)(nw->sock) )->sin_port          = htons(port);

		//bind
		errorCode = bind(
			nw->fd,
			(struct sockaddr*)(nw->sock),
			nw->sock_size
		);
	}

	//bind error
	if(errorCode == -1){
		printf("RUNTIME ERROR > network.c : network_bind() : Could not bind all addresses [0x%02x].\n", errorCode);
		return;
	}

	//listen
	if( !network_getPrtcl(nw) ){
		errorCode = listen(nw->fd, NETWORK__LISTEN_BACKLOG);
		if(errorCode){
			printf("RUNTIME ERROR > network.c : network_bind() : Could not listen [0x%02x].\n", errorCode);
		}
	}
}

network* network_accept(network* server_nw){ //available for servers in TCP only

	//error case
	if(!server_nw){
		printf("RUNTIME ERROR > network.c : network_accept() : Network instance is NULL.\n");
		return NULL;
	}
	if( !network_getOwner(server_nw) ){
		printf("RUNTIME ERROR > network.c : network_accept() : Network instance is set as \"CLIENT\" and clients cannot use network_accept().\n");
		return NULL;
	}
	if( network_getPrtcl(server_nw) ){
		printf("RUNTIME ERROR > network.c : network_accept() : Network instance is set as \"UDP\" protocol and only \"TCP\" protocols can use network_accept().\n");
		return NULL;
	}

	//create new client instance
	network* client_nw = network_create(
		NETWORK__CLIENT,
		network_getPrtcl (server_nw), //same protocol
		network_getIPType(server_nw)  //same IP type
	);

	//no need to have socket file descriptor => reset it
	close(client_nw->fd);	                 // (we should set it at -1 as well in theory
	                                         //  but the following accept() will overwrite it)
    //accept new client
    client_nw->fd = accept(
		server_nw->fd,
		(struct sockaddr*)(client_nw->sock),
		&(client_nw->sock_size)
	);

	return client_nw;
}

network* network_connect(network* client_nw, char* address, unsigned short int port){ //available for clients only (will return the server found)

	//error cases
	if(!client_nw){
		printf("RUNTIME ERROR > network.c : network_connect() : Network instance is NULL.\n");
		return NULL;
	}
	if(!address){
		printf("RUNTIME ERROR > network.c : network_connect() : Address is NULL.\n");
		return NULL;
	}
	if( network_getOwner(client_nw) ){
		printf("RUNTIME ERROR > network.c : network_connect() : Network instance is set as \"SERVER\" and servers cannot use network_connect().\n");
		return NULL;
	}

	//create new server instance
	network* server_nw = network_create(
		NETWORK__SERVER,
		network_getPrtcl (client_nw), //same protocol
		network_getIPType(client_nw)  //same IP type
	);

	//no need to have socket => reset file descriptor
	close(server_nw->fd);
	server_nw->fd = -1;

	//set server socket structure
	if( network_getIPType(server_nw) ){

		//set formatted address
		int errorCode = inet_pton(
			AF_INET6,
			address,
			&( ((struct sockaddr_in6*)(server_nw->sock))->sin6_addr )
		);
		if(errorCode != 1){
			printf("RUNTIME ERROR > network.c : network_connect() : Invalid IPv6 address given \"%s\" [0x%02x].\n", address, errorCode);
			return NULL;
		}

		//set the other fields
		( (struct sockaddr_in6*)(server_nw->sock) )->sin6_family = AF_INET6;
		( (struct sockaddr_in6*)(server_nw->sock) )->sin6_port   = htons(port);

	}else{

		//set formatted address
		int errorCode = inet_pton(
			AF_INET,
			address,
			&( ((struct sockaddr_in*)(server_nw->sock))->sin_addr )
		);
		if(errorCode != 1){
			printf("RUNTIME ERROR > network.c : network_connect() : Invalid IPv4 address given \"%s\" [0x%02x].\n", address, errorCode);
			return NULL;
		}

		//set the other fields
		( (struct sockaddr_in*)(server_nw->sock) )->sin_family = AF_INET;
		( (struct sockaddr_in*)(server_nw->sock) )->sin_port   = htons(port);
	}

	//connect
	int errorCode = connect(
		client_nw->fd,
		(struct sockaddr*)(server_nw->sock),
		server_nw->sock_size
	);
	if(errorCode){
		printf("RUNTIME ERROR > network.c : network_connect() : Could not connect to server [0x%02x].\n", errorCode);
		return NULL;
	}

	//connection established
	return server_nw;
}




//sendTo - receiveFrom
void network_sendTo(network* src, network* dst, char* data, size_t len){ // #data# must be not NULL and allocated

	//error cases
	if(!src){
		printf("RUNTIME ERROR > network.c : network_sendTo() : Source network instance is NULL.\n");
		return;
	}
	if(!data){
		printf("RUNTIME ERROR > network.c : network_sendTo() : Data is NULL.\n");
		return;
	}
	int errorCode = 0;

	//send data over UDP
	if( network_getPrtcl(src) ){

		//server
		if( network_getOwner(src) ){
			if(!dst){
				printf("RUNTIME ERROR > network.c : network_sendTo() : Destination network instance is NULL.\n");
				return;
			}
			errorCode = sendto(
				src->fd,
				data, len,
				0,                             //flags
				(struct sockaddr*)(dst->sock),
				dst->sock_size
			);
		}

		//client
		else{
			errorCode = sendto(
				src->fd,
				data, len,
				0,         //flags
				NULL, sizeof(struct sockaddr_in) //destination is not required for clients (connect() stores the server info)
			);
		}
	}

	//send data over TCP
	else{

		//server
		if( network_getOwner(src) ){
			if(!dst){
				printf("RUNTIME ERROR > network.c : network_sendTo() : Destination network instance is NULL.\n");
				return;
			}
			errorCode = send(
				dst->fd,
				data, len,
				0          //flags
			);
		}

		//client
		else{
			errorCode = send(
				src->fd,
				data, len,
				0          //flags
			);
		}
	}

	//errors
	if(errorCode != len){
		printf("RUNTIME ERROR > network.c : network_sendTo() : Error sending data [0x%02x].\n", errorCode);
	}
}

void network_receiveFrom(network* src, network* dst, char* data, size_t len){ // #data# must be not NULL and unallocated

    //error cases
	if(!src){
		printf("RUNTIME ERROR > network.c : network_receiveFrom() : Source network instance is NULL.\n");
		return;
	}
	int errorCode = 0;

	//receive data over UDP
	if( network_getPrtcl(src) ){

		//server
		if( network_getOwner(src) ){
			if(!dst){
				printf("RUNTIME ERROR > network.c : network_sendTo() : Destination network instance is NULL.\n");
				return;
			}
			bzero(data, len);
			errorCode = recvfrom(
				src->fd,
				data, len,
				0,                             //flags
				(struct sockaddr*)(dst->sock),
				&(dst->sock_size)
			);
		}

		//client
		else{
			bzero(data, len);
			errorCode = recvfrom(
				src->fd,
				data, len,
				0,         //flags
				NULL, NULL //destination is not required for clients (connect() stores the server info)
			);
		}
	}

	//receive data over TCP
	else{

		//server
		if( network_getOwner(src) ){
			if(!dst){
				printf("RUNTIME ERROR > network.c : network_receiveFrom() : Destination network instance is NULL.\n");
				return;
			}
			bzero(data, len);
			errorCode = recv(
				dst->fd,
				data, len,
				0          //flags
			);
		}

		//client
		else{
			bzero(data, len);
			errorCode = recv(
				src->fd,
				data, len,
				0          //flags
			);
		}
	}

	//errors
	if(errorCode != len){
		printf("RUNTIME ERROR > network.c : network_receiveFrom() : Error receiving data [0x%02x].\n", errorCode);
	}
}




//user information
char* network_getAddress(network* nw){ //will allocate 16 bytes for IPv4, 40 bytes for IPv6
	                                   //IPv4 may be displayed on less characters (7-15 chars + \0)
	//error cases                      //IPv6 will always be displayed on 38 bytes (+ \0)
	if(!nw){
		printf("RUNTIME ERROR > network.c : network_getAddress() : Network instance is NULL.\n");
		return NULL;
	}
	char* addressStr;

	//IPv6
	if( network_getIPType(nw) ){

		//prepare result
		addressStr = malloc(40);
		if(!addressStr){
			printf("FATAL ERROR > network.c : network_getAddress() : Computer refuses to give more memory.\n");
			exit(EXIT_FAILURE);
		}

		//get address
		uint8_t* ipv6_address = (
			(
				(struct sockaddr_in6*)(nw->sock)
			)->sin6_addr
		).s6_addr;

		//write address in result (hex format, byte per byte)
		sprintf(
			addressStr,
			"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
			ipv6_address[ 0], ipv6_address[ 1], ipv6_address[ 2], ipv6_address[ 3],
			ipv6_address[ 4], ipv6_address[ 5], ipv6_address[ 6], ipv6_address[ 7],
			ipv6_address[ 8], ipv6_address[ 9], ipv6_address[10], ipv6_address[11],
			ipv6_address[12], ipv6_address[13], ipv6_address[14], ipv6_address[15]
		);
		addressStr[39] = '\0';
	}

	//IPv4
	else{

		//prepare result
		addressStr = malloc(16);
		if(!addressStr){
			printf("FATAL ERROR > network.c : network_getAddress() : Computer refuses to give more memory.\n");
			exit(EXIT_FAILURE);
		}

		//get address
		uint32_t ipv4_address_uint32 = (
			(
				(struct sockaddr_in*)(nw->sock)
			)->sin_addr
		).s_addr;
		uint8_t* ipv4_address = (uint8_t*)&ipv4_address_uint32;
//		uint32_t ipv4_address_1 = (int)ipv4_address[0];
//		uint32_t ipv4_address_2 = (int)ipv4_address[1];
//		uint32_t ipv4_address_3 = (int)ipv4_address[2];
//		uint32_t ipv4_address_4 = (int)ipv4_address[3];

		//write address in result (dec format, byte per byte)
		bzero(addressStr, 16);
		sprintf(
			addressStr,
			"%i.%i.%i.%i",
			ipv4_address[0], ipv4_address[1],
			ipv4_address[2], ipv4_address[3]
		);
	}

	return addressStr;
}
