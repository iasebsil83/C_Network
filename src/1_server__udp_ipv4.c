printf("Server > This is a basic demonstration of \"network.c/.h\"\n         for UDP communication in IPv4 (server side).\n");



// ---- START ----

//create socket
network* nw = network_create(
	NETWORK__SERVER,
	NETWORK__UDP,
	NETWORK__IPV4
);

//bind all addresses
network_bind(nw, PORT);

//create a network instance for the incomming client
network* client = network_create(
	NETWORK__CLIENT,
	NETWORK__UDP,
	NETWORK__IPV4
);



// ---- DATA TRANSFERS ----

//main loop
char message[MESSAGE_LENGTH_MAX];
while(1){



	// ---- SEND ----

	//get user input
	printf("Server > Write something to send to client : ");
	getUserInput(message, MESSAGE_LENGTH_MAX);

	//send message
	//network_setInfo(client, "127.0.0.1", PORT); //local address
	network_sendTo(
		nw, client,
		message, MESSAGE_LENGTH_MAX
	);

	//exit reply
	if(!strncmp(message,"exit",4)){
		printf("Server > Exit request sent.\n");
		break;
	}

	//wait a bit (500ms) in order to let the client receive the message before us
	usleep(2000000);



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
	char* receivedAddress = network_getAddress(client);
	printf("Server > Incomming message [%s] received from \"%c%c.%c%c.%c%c.%c%c\".\n",
		message,
		receivedAddress[0], receivedAddress[1],
		receivedAddress[2], receivedAddress[3],
		receivedAddress[4], receivedAddress[5],
		receivedAddress[6], receivedAddress[7]
	);
	free(receivedAddress);
}



// ---- STOP ----

//end connection
network_delete(nw);
printf("Server > Ended network connection.\n");
