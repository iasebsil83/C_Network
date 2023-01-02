printf("Client > This is a basic demonstration of \"network.c/.h\"\n         for TCP communication in IPv4 (client side).\n");






// ---- START ----

//create socket
network* nw = network_create(
	NETWORK__CLIENT,
	NETWORK__TCP,
	NETWORK__IPV4
);

//try to connect
while( !network_connect(nw, "127.0.0.1", PORT) ){ //local address
	usleep(250000);
}
printf("Client > Connected to server.\n");






// ---- DATA TRANSFERS ----

//main loop
char message[MESSAGE_LENGTH_MAX];
while(1){

	// ---- RECEIVE ----

	//receive reply (message will be reset automatically before reception)
	network_receiveFrom(
		nw, NULL,
		message, MESSAGE_LENGTH_MAX
	);

	//exit reply
	if(!strncmp(message,"exit",4)){
		printf("Client > Exit request received.\n");
		break;
	}

	//print reply
	printf("Client > Message [%s] received.\n", message);






	// ---- SEND ----

	//get user input
	printf("Client > Write something to send to server : ");
	getUserInput(message, MESSAGE_LENGTH_MAX);

	//send message
	network_sendTo(
		nw, NULL,
		message, MESSAGE_LENGTH_MAX
	);

	//exit reply
	if(!strncmp(message,"exit",4)){
		printf("Client > Exit request sent.\n");
		break;
	}
}






// ---- STOP ----

//end connection
network_delete(nw);
printf("Client > Ended network connection.\n");
