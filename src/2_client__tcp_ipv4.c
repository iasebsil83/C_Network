printf("Client > This is a basic demonstration of \"network.c/.h\"\n         for TCP communication in IPv4 (client side).\n");




// ---- START ----

//create instance for myself (client)
network* me = network_create(
	NETWORK__CLIENT,
	NETWORK__TCP,
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




	// ---- RECEIVE ----

	//receive reply (no need to precise server, connect() stores server info)
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
	printf("Client > Message \"%s\" received.\n", message);




	// ---- SEND ----

	//get user input
	printf("Client > Write something to send to server : ");
	getUserInput(message, MESSAGE_LENGTH_MAX);

	//send message (no need to precise server, same reason as above)
	network_sendTo(
		me, NULL,
		message, MESSAGE_LENGTH_MAX
	);

	//specific reply : exit request
	if(!strncmp(message,"exit",4)){
		printf("Client > Exit request sent.\n");
		break;
	}
}




// ---- STOP ----

//end connection
network_delete(me);
network_delete(server);
printf("Client > Ended network connection.\n");
