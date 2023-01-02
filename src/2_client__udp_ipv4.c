printf("Client > This is a basic demonstration of \"network.c/.h\"\n         for UDP communication in IPv4 (client side).\n");



// ---- START ----

//create socket
network* nw = network_create(
	NETWORK__CLIENT,
	NETWORK__UDP,
	NETWORK__IPV4
);

//create a network instance for the incomming server
network* server = network_create(
	NETWORK__SERVER,
	NETWORK__UDP,
	NETWORK__IPV4
);



// ---- DATA TRANSFERS ----

//main loop
char message[MESSAGE_LENGTH_MAX];
while(1){



	// ---- RECEIVE ----

	//receive reply (message will be reset automatically before reception)
	network_setInfo(server, UDP_ADDRESS_IPV4, PORT);
	network_receiveFrom(
		nw, server,
		message, MESSAGE_LENGTH_MAX
	);

	//exit reply
	if(!strncmp(message,"exit",4)){
		printf("Client > Exit request received.\n");
		break;
	}

	//print reply
	char* receivedAddress = network_getAddress(server);
	printf("Client > Incomming message [%s] received from \"%c%c.%c%c.%c%c.%c%c\".\n",
		message,
		receivedAddress[0], receivedAddress[1],
		receivedAddress[2], receivedAddress[3],
		receivedAddress[4], receivedAddress[5],
		receivedAddress[6], receivedAddress[7]
	);
	free(receivedAddress);



	// ---- SEND ----

	//get user input
	printf("Client > Write something to send to server : ");
	getUserInput(message, MESSAGE_LENGTH_MAX);

	//send message
	network_setInfo(server, UDP_ADDRESS_IPV4, PORT); //local address
	network_sendTo(
		nw, server,
		message, MESSAGE_LENGTH_MAX
	);

	//exit reply
	if(!strncmp(message,"exit",4)){
		printf("Client > Exit request sent.\n");
		break;
	}

	//wait a bit (500ms) in order to let the client receive the message before us
	usleep(500000);
}



// ---- STOP ----

//end connection
network_delete(nw);
printf("Client > Ended network connection.\n");
