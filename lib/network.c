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












/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Network [0.1.1] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

    BUGS : .
    NOTES : .

    Contact     : i.a.sebsil83@gmail.com
    Youtube     : https://www.youtube.com/user/IAsebsil83
    GitHub repo : https://github.com/iasebsil83

    Let's Code !                                  By I.A.
*********************************************************************************
    LICENCE :
    C_Network
    Copyright (C) 2021  Sebastien SILVANO
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.
    If not, see <https://www.gnu.org/licenses/>.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */












// -------------------------------- BASICS --------------------------------

//create - delete
network* network_create(char owner, char protocol, char IPType){

	//create instance
	network* nw = malloc(sizeof(network));
	if(nw == NULL){
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
	if(nw->sock == NULL){
		printf("FATAL ERROR > network.c : network_create() : Computer refuses to give more memory.\n");
		exit(EXIT_FAILURE);
	}

	//reset socket structure
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
	if(nw == NULL){
		printf("RUNTIME ERROR > network.c : network_delete() : Network instance is NULL.\n");
		return;
	}

	//flush data in socket
	while( close(nw->fd) == -1 );

	//free structure
	if(nw->sock != NULL){
		free(nw->sock);
	}
	free(nw);
}






//connect - bind - accept
void network_bind(network* nw, unsigned short int port){ //available for servers only

	//error case
	if(nw == NULL){
		printf("RUNTIME ERROR > network.c : network_bind() : Network instance is NULL.\n");
		return;
	}
	if( !network_getOwner(nw) ){
		printf("RUNTIME ERROR > network.c : network_bind() : Network instance is set as \"CLIENT\" and clients cannot use network_bind().\n");
		return;
	}

	//reset socket structure
	bzero(nw->sock, nw->sock_size);

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

network* network_accept(network* nw){ //available for servers in TCP only

	//error case
	if(nw == NULL){
		printf("RUNTIME ERROR > network.c : network_accept() : Network instance is NULL.\n");
		return NULL;
	}
	if( !network_getOwner(nw) ){
		printf("RUNTIME ERROR > network.c : network_accept() : Network instance is set as \"CLIENT\" and clients cannot use network_accept().\n");
		return NULL;
	}
	if( network_getPrtcl(nw) ){
		printf("RUNTIME ERROR > network.c : network_accept() : Network instance is set as \"UDP\" protocol and only \"TCP\" protocols can use network_accept().\n");
		return NULL;
	}

	//create new client instance
	network* client_nw = network_create(
		NETWORK__CLIENT,
		network_getPrtcl (nw),
		network_getIPType(nw)
	);

    //accept new client
    client_nw->fd = accept(
		nw->fd,
		(struct sockaddr*)(client_nw->sock),
		&(nw->sock_size)
	);

	return client_nw;
}

int network_connect(network* nw, char* address, unsigned short int port){ //available for clients in TCP only

	//error cases
	if(nw == NULL){
		printf("RUNTIME ERROR > network.c : network_connect() : Network instance is NULL.\n");
		return 0;
	}
	if(address == NULL){
		printf("RUNTIME ERROR > network.c : network_connect() : Address is NULL.\n");
		return 0;
	}
	if( network_getOwner(nw) ){
		printf("RUNTIME ERROR > network.c : network_connect() : Network instance is set as \"SERVER\" and servers cannot use network_connect().\n");
		return 0;
	}
	if( network_getPrtcl(nw) ){
		printf("RUNTIME ERROR > network.c : network_connect() : Network instance is set as \"UDP\" protocol and only \"TCP\" protocols can use network_connect().\n");
		return 0;
	}

	//reset socket structure
	bzero(nw->sock, nw->sock_size);

	//set socket structure
	if( network_getIPType(nw) ){

		//set formatted address
		int errorCode = inet_pton(
			AF_INET6,
			address,
			&(
				( (struct sockaddr_in6*)(nw->sock) )->sin6_addr
			)
		);
		if(errorCode != 1){
			printf("RUNTIME ERROR > network.c : network_connect() : Invalid IPv6 address given \"%s\" [0x%02x].\n", address, errorCode);
			return 0;
		}

		//set the other fields
		( (struct sockaddr_in6*)(nw->sock) )->sin6_family = AF_INET6;
		( (struct sockaddr_in6*)(nw->sock) )->sin6_port   = htons(port);

	}else{

		//set formatted address
		int errorCode = inet_pton(
			AF_INET,
			address,
			&(
				( (struct sockaddr_in*)(nw->sock) )->sin_addr
			)
		);
		if(errorCode != 1){
			printf("RUNTIME ERROR > network.c : network_connect() : Invalid IPv4 address given \"%s\" [0x%02x].\n", address, errorCode);
			return 0;
		}

		//set the other fields
		( (struct sockaddr_in*)(nw->sock) )->sin_family = AF_INET;
		( (struct sockaddr_in*)(nw->sock) )->sin_port   = htons(port);
	}

	//connect
	int errorCode = connect(
		nw->fd,
		(struct sockaddr*)(nw->sock),
		nw->sock_size
	);
	if(errorCode){
		printf("RUNTIME ERROR > network.c : network_connect() : Could not connect to server [0x%02x].\n", errorCode);
		return 0;
	}

	//connection established
	return 1;
}






//sendTo - receiveFrom
void network_sendTo(network* src, network* dest, char* data, size_t len){ // #data# must not be NULL or unallocated

	//error cases
	if(src == NULL){
		printf("RUNTIME ERROR > network.c : network_sendTo() : Source network instance is NULL.\n");
		return;
	}
	if(data == NULL){
		printf("RUNTIME ERROR > network.c : network_sendTo() : Data is NULL.\n");
		return;
	}
	int errorCode = 0;

	//send data over UDP
	if( network_getPrtcl(src) ){
		if(dest == NULL){
			printf("RUNTIME ERROR > network.c : network_receiveFrom() : Destination network instance is NULL.\n");
			return;
		}

		//server & client
		errorCode = sendto(
			src->fd,
			data,
			len,
			0, //flags
			(struct sockaddr*)(dest->sock), 
			dest->sock_size
		);
	}

	//send data over TCP
	else{

		//server
		if( network_getOwner(src) ){
			if(dest == NULL){
				printf("RUNTIME ERROR > network.c : network_sendTo() : Destination network instance is NULL.\n");
				return;
			}
			errorCode = send(
				dest->fd,
				data,
				len,
				0       //flags
			);
		}

		//client
		else{
			errorCode = send(
				src->fd,
				data,
				len,
				0       //flags
			);
		}
	}

	//errors
	if(errorCode != len){
		printf("RUNTIME ERROR > network.c : network_sendTo() : Error sending data [0x%02x].\n", errorCode);
	}
}

void network_receiveFrom(network* src, network* dest, char* data, size_t len){ // #data# shall be NULL or unallocated

    //error cases
	if(src == NULL){
		printf("RUNTIME ERROR > network.c : network_receiveFrom() : Source network instance is NULL.\n");
		return;
	}
	int errorCode = 0;

	//receive data over UDP
	if( network_getPrtcl(src) ){
		if(dest == NULL){
			printf("RUNTIME ERROR > network.c : network_receiveFrom() : Destination network instance is NULL.\n");
			return;
		}

		//reset data
		bzero(data, len);

		//server & client
		errorCode = recvfrom(
			src->fd,
			data,
			len,
			0,                              //flags
			(struct sockaddr*)(dest->sock),
			&(dest->sock_size)
		);
	}

	//receive data over TCP
	else{

		//server
		if( network_getOwner(src) ){
			if(dest == NULL){
				printf("RUNTIME ERROR > network.c : network_receiveFrom() : Destination network instance is NULL.\n");
				return;
			}

			//reset data
			bzero(data, len);

			//reception
			errorCode = recv(
				dest->fd,
				data,
				len,
				0       //flags
			);
		}

		//client
		else{

			//reset data
			bzero(data, len);

			//reception
			errorCode = recv(
				src->fd,
				data,
				len,
				0       //flags
			);
		}
	}

	//errors
	if(errorCode != len){
		printf("RUNTIME ERROR > network.c : network_receiveFrom() : Error receiving data [0x%02x].\n", errorCode);
	}
}






//get / set info
static char network_byteToChar(char b){
	switch(b){

		//numbers
		case 0x0:
			return '0';
		case 0x1:
			return '1';
		case 0x2:
			return '2';
		case 0x3:
			return '3';
		case 0x4:
			return '4';
		case 0x5:
			return '5';
		case 0x6:
			return '6';
		case 0x7:
			return '7';
		case 0x8:
			return '8';
		case 0x9:
			return '9';

		//letters
		case 0xa:
			return 'a';
		case 0xb:
			return 'b';
		case 0xc:
			return 'c';
		case 0xd:
			return 'd';
		case 0xe:
			return 'e';
		case 0xf:
			return 'f';
	}
	return ' ';
}

char* network_getAddress(network* nw){ //will allocate 16 bytes

	//error cases
	if(nw == NULL){
		printf("RUNTIME ERROR > network.c : network_getAddress() : Network instance is NULL.\n");
		return NULL;
	}

	//prepare result
	char* address = malloc(32);
	if(address == NULL){
		printf("FATAL ERROR > network.c : network_getAddress() : Computer refuses to give more memory.\n");
		exit(EXIT_FAILURE);
	}
	bzero(address, 32);

	//IPv6
	if( network_getIPType(nw) ){

		//get address
		uint8_t* ipv6_address = (
			(
				(struct sockaddr_in6*)(nw->sock)
			)->sin6_addr
		).s6_addr;

		//copy in allocated result
		address[ 0] = network_byteToChar(  ipv6_address[0x0] & 0x0f       );
		address[ 1] = network_byteToChar( (ipv6_address[0x0] & 0xf0) >> 4 );

		address[ 2] = network_byteToChar(  ipv6_address[0x1] & 0x0f       );
		address[ 3] = network_byteToChar( (ipv6_address[0x1] & 0xf0) >> 4 );

		address[ 4] = network_byteToChar(  ipv6_address[0x2] & 0x0f       );
		address[ 5] = network_byteToChar( (ipv6_address[0x2] & 0xf0) >> 4 );

		address[ 6] = network_byteToChar(  ipv6_address[0x3] & 0x0f       );
		address[ 7] = network_byteToChar( (ipv6_address[0x3] & 0xf0) >> 4 );

		address[ 8] = network_byteToChar(  ipv6_address[0x4] & 0x0f );
		address[ 9] = network_byteToChar( (ipv6_address[0x4] & 0xf0) >> 4 );

		address[10] = network_byteToChar(  ipv6_address[0x5] & 0x0f );
		address[11] = network_byteToChar( (ipv6_address[0x5] & 0xf0) >> 4 );

		address[12] = network_byteToChar(  ipv6_address[0x6] & 0x0f );
		address[13] = network_byteToChar( (ipv6_address[0x6] & 0xf0) >> 4 );

		address[14] = network_byteToChar(  ipv6_address[0x7] & 0x0f );
		address[15] = network_byteToChar( (ipv6_address[0x7] & 0xf0) >> 4 );

		address[16] = network_byteToChar(  ipv6_address[0x8] & 0x0f );
		address[17] = network_byteToChar( (ipv6_address[0x8] & 0xf0) >> 4 );

		address[18] = network_byteToChar(  ipv6_address[0x9] & 0x0f );
		address[19] = network_byteToChar( (ipv6_address[0x9] & 0xf0) >> 4 );

		address[20] = network_byteToChar(  ipv6_address[0xa] & 0x0f );
		address[21] = network_byteToChar( (ipv6_address[0xa] & 0xf0) >> 4 );

		address[22] = network_byteToChar(  ipv6_address[0xb] & 0x0f       );
		address[23] = network_byteToChar( (ipv6_address[0xb] & 0xf0) >> 4 );

		address[24] = network_byteToChar(  ipv6_address[0xc] & 0x0f       );
		address[25] = network_byteToChar( (ipv6_address[0xc] & 0xf0) >> 4 );

		address[26] = network_byteToChar(  ipv6_address[0xd] & 0x0f       );
		address[27] = network_byteToChar( (ipv6_address[0xd] & 0xf0) >> 4 );

		address[28] = network_byteToChar(  ipv6_address[0xe] & 0x0f       );
		address[29] = network_byteToChar( (ipv6_address[0xe] & 0xf0) >> 4 );

		address[30] = network_byteToChar(  ipv6_address[0xf] & 0x0f       );
		address[31] = network_byteToChar( (ipv6_address[0xf] & 0xf0) >> 4 );
	}

	//IPv4
	else{

		//get address
		uint32_t ipv4_address = (
			(
				(struct sockaddr_in*)(nw->sock)
			)->sin_addr
		).s_addr;

		//copy in allocated result
		address[ 0] = network_byteToChar( (ipv4_address & 0x000000f0) >>  4 );
		address[ 1] = network_byteToChar(  ipv4_address & 0x0000000f        );

		address[ 2] = network_byteToChar( (ipv4_address & 0x0000f000) >> 12 );
		address[ 3] = network_byteToChar( (ipv4_address & 0x00000f00) >>  8 );

		address[ 4] = network_byteToChar( (ipv4_address & 0x00f00000) >> 20 );
		address[ 5] = network_byteToChar( (ipv4_address & 0x000f0000) >> 16 );

		address[ 6] = network_byteToChar( (ipv4_address & 0xf0000000) >> 28 );
		address[ 7] = network_byteToChar( (ipv4_address & 0x0f000000) >> 24 );

		address[ 8] = 0; address[ 9] = 0;
		address[10] = 0; address[11] = 0;
		address[12] = 0; address[13] = 0;
		address[14] = 0; address[15] = 0;
		address[16] = 0; address[17] = 0;
		address[18] = 0; address[19] = 0;
		address[20] = 0; address[21] = 0;
		address[22] = 0; address[23] = 0;
		address[24] = 0; address[25] = 0;
		address[26] = 0; address[27] = 0;
		address[28] = 0; address[29] = 0;
		address[30] = 0; address[31] = 0;
	}

	return address;
}

void network_setInfo(network* nw, char* address, unsigned short int port){

	//error cases
	if(nw == NULL){
		printf("RUNTIME ERROR > network.c : network_setInfo() : Network instance is NULL.\n");
		return;
	}
	if(address == NULL){
		printf("RUNTIME ERROR > network.c : network_setInfo() : Address is NULL.\n");
		return;
	}

	//IP type
	int errorCode = 1;
	if( network_getIPType(nw) ){

		//IPv6
		errorCode = inet_pton(
			AF_INET6,
			address,
			&(
				( (struct sockaddr_in6*)(nw->sock) )->sin6_addr
			)
		);
		( (struct sockaddr_in6*)(nw->sock) )->sin6_family = AF_INET6;
		( (struct sockaddr_in6*)(nw->sock) )->sin6_port   = htons(port);
	}else{

		//IPv4
		errorCode = inet_pton(
			AF_INET,
			address,
			&(
				( (struct sockaddr_in*)(nw->sock) )->sin_addr
			)
		);
		( (struct sockaddr_in*)(nw->sock) )->sin_family = AF_INET;
		( (struct sockaddr_in*)(nw->sock) )->sin_port   = htons(port);
	}

	//error
	if(errorCode != 1){
		printf("RUNTIME ERROR > network.c : network_setInfo() : Could not set address [0x%02x].\n", errorCode);
	}
}
