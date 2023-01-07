printf("Server > This is a basic demonstration of \"network.c/.h\"\n         for TCP communication in IPv4 (server side).\n");




// ---- START ----

//create instance for myself (server)
network* me = network_create(
	NETWORK__SERVER,
	NETWORK__TCP,
	NETWORK__IPV4
);

//bind all addresses
network_bind(me, PORT);

//accept the first incomming client
network* client        = network_accept(me);
char*    clientAddress = network_getAddress(client);
printf("Server > Accepted client [%s].\n", clientAddress);
free(clientAddress);



// ---- DATA TRANSFERS ----

//main loop
char message[MESSAGE_LENGTH_MAX];
while(1){




	// ---- SEND ----

	//get user input
	printf("Server > Write something to send to client : ");
	getUserInput(message, MESSAGE_LENGTH_MAX);

	//send message
	network_sendTo(
		me, client,
		message, MESSAGE_LENGTH_MAX
	);

	//specific reply : exit request
	if(!strncmp(message,"exit",4)){
		printf("Server > Exit request sent.\n");
		break;
	}




	// ---- RECEIVE ----

	//receive reply (receive only from this specific client)
	network_receiveFrom(
		me, client,
		message, MESSAGE_LENGTH_MAX
	);

	//specific reply : exit request
	if(!strncmp(message,"exit",4)){
		printf("Server > Exit request received.\n");
		break;
	}

	//print raw reply
	printf("Server > Message \"%s\" received.\n", message);
}




// ---- STOP ----

//end connection
network_delete(me);
network_delete(client);
printf("Server > Ended network connection.\n");
