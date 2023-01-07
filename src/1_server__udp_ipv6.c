printf("Server > This is a basic demonstration of \"network.c/.h\"\n         for UDP communication in IPv4 (server side).\n");




// ---- START ----

//create instance for myself (server)
network* me = network_create(
	NETWORK__SERVER,
	NETWORK__UDP,
	NETWORK__IPV6
);

//bind all addresses
network_bind(me, PORT);

//create instance for any incomming client
network* client = network_create(
	NETWORK__CLIENT,
	NETWORK__UDP,
	NETWORK__IPV6
);




// ---- DATA TRANSFERS ----

//main loop
char message[MESSAGE_LENGTH_MAX];
while(1){




	// ---- RECEIVE ----

	//receive reply from any client (the variable will be set to the sender's info)
	network_receiveFrom(
		me, client,
		message, MESSAGE_LENGTH_MAX
	);

	//special reply : exit request
	if(!strncmp(message, "exit", 4)){
		printf("Server > Exit request received.\n");
		break;
	}

	//print raw reply
	char* receivedAddress = network_getAddress(client);
	printf(
		"Server > Incomming message \"%s\" received from [%s].\n",
		message,
		receivedAddress
	);
	free(receivedAddress);




	// ---- SEND ----

	//get user input
	printf("Server > Write something to send to client : ");
	getUserInput(message, MESSAGE_LENGTH_MAX);

	/* /!\ IMPORTANT PRECISION /!\
	|
	| UDP allows data reception from anybody at any moment.
	| That means that any client can send you data and you have to analyze the received info to
	| know who is talking.
	|
	|We do not have this in TCP because we only receive from a specified client (that has been
	|accepted before using accept()).
	|
	| Here, 'client' info is fulfilled by the previous receiveFrom() (RECEPTION block above).
	| So we do not have to set information on our 'client' manually.
	*/

	//send message (precise destination instead of TCP)
	network_sendTo(
		me, client,
		message, MESSAGE_LENGTH_MAX
	);

	//special reply : exit request
	if(!strncmp(message, "exit", 4)){
		printf("Server > Exit request sent.\n");
		break;
	}
}




// ---- STOP ----

//end connection
network_delete(me);
network_delete(client);
printf("Server > Ended network connection.\n");
