printf("Client > This is a basic demonstration of \"network.c/.h\"\n         for UDP communication in IPv4 (client side).\n");




// ---- START ----

//create instance for myself (client)
network* me = network_create(
	NETWORK__CLIENT,
	NETWORK__UDP,
	NETWORK__IPV4
);

//try to connect to server (after this operation, server is no longer used
network* server = NULL; //  except for holding its data structure allocated)
while(!server){         //  WARNING: Do NOT use server->fd because it is set to -1 (not to use).
	server = network_connect(me, SERVER_ADDRESS_IPV4, PORT);
	usleep(250000);
}
char* serverAddress = network_getAddress(server);
printf("Client > Connected to server [%s].\n", serverAddress);
free(serverAddress);




// ---- DATA TRANSFERS ----

//main loop
char message[MESSAGE_LENGTH_MAX];
while(1){




	// ---- SEND ----

	//get user input
	printf("Client > Write something to send to server : ");
	getUserInput(message, MESSAGE_LENGTH_MAX);

	//send message (no need to precise server, connect() already stores it)
	network_sendTo(
		me, NULL,
		message, MESSAGE_LENGTH_MAX
	);

	//specific reply : exit request
	if(!strncmp(message,"exit",4)){
		printf("Client > Exit request sent.\n");
		break;
	}




	// ---- RECEIVE ----

	//receive reply from server (no need to precise as well for the same reason)
	network_receiveFrom(
		me, NULL,
		message, MESSAGE_LENGTH_MAX
	);

	//specific reply : exit request
	if(!strncmp(message,"exit",4)){
		printf("Client > Exit request received.\n");
		break;
	}

	//print raw reply
	char* receivedAddress = network_getAddress(server);
	printf(
		"Client > Incomming message \"%s\" received from [%s].\n",
		message,
		receivedAddress
	);
	free(receivedAddress);
}




// ---- STOP ----

//end connection
network_delete(me);
network_delete(server);
printf("Client > Ended network connection.\n");
