printf("Server > This is a basic demonstration of \"network.c/.h\"\n         for TCP communication in IPv4 (server side).\n");



// ---- START ----

//create socket
network* nw = network_create(
	NETWORK__SERVER,
	NETWORK__TCP,
	NETWORK__IPV4
);

//bind all addresses
network_bind(nw, PORT);

//accept the first address wanting to connect
network* client = network_accept(nw);
printf("Server > Accepted a client.\n");



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
		nw, client,
		message, MESSAGE_LENGTH_MAX
	);

	//exit reply
	if(!strncmp(message,"exit",4)){
		printf("Server > Exit request sent.\n");
		break;
	}



	// ---- RECEIVE ----

	//receive reply (message will be reset automatically before reception)
	network_receiveFrom(
		nw, client,
		message, MESSAGE_LENGTH_MAX
	);

	//exit reply
	if(!strncmp(message,"exit",4)){
		printf("Server > Exit request received.\n");
		break;
	}

	//print reply
	printf("Server > Message [%s] received.\n", message);
}



// ---- STOP ----

//end connection
network_delete(nw);
printf("Server > Ended network connection.\n");
