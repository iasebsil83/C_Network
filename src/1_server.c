// ---------------- IMPORTATIONS ----------------

//standard
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



//network utility
#include "../lib/network.h"












/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 1_Server [V.V.V] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                  network.h demonstration program : server side

        Blablabla about the subject.

    DD/MM/YYYY > [V.V.V] :
    - Added something.
    - Added something else.

    DD/MM/YYYY > [V.V.V] :
    - Added something that wasn't before.
    - Fixed a bug.
    - Added something else.
    - Added something else again.

    BUGS : Active bugs in last version.
    NOTES : Notes.

    Contact : ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */












// ---------------- DEFINITIONS ----------------

//connection
#define PORT 8080



//message
#define MESSAGE_LENGTH_MAX 100



// >>>>>>>>>>>>>>>>>>>>>>>>>>>> THERE IS 4 MODES FOR THIS EXAMPLE PROGRAM <<<<<<<<<<<<<<<<<<<<<<<<
#define MODE__TCP_IPV4 0x00
#define MODE__TCP_IPV6 0x01
#define MODE__UDP_IPV4 0x02
#define MODE__UDP_IPV6 0x03



//choose the mode you want
#define DEMONSTRATION_MODE MODE__UDP_IPV4












// ---------------- UTILITIES ----------------

//user input
void getUserInput(char* msg, int maxLength){
	bzero(msg, maxLength);

	//append character per character
	for(int i=0; i < maxLength-1; i++){ //-1 to leave the last byte at 0 ('\0')
		msg[i] = getchar();

		//stop input at first line return
		if(msg[i] == '\n'){
			msg[i] = '\0';
			break;
		}
	}
}












// ---------------- EXECUTION ----------------

//main
int main(){

	//select demonstration mode
	switch(DEMONSTRATION_MODE){












		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> EXAMPLE : TCP IPV4 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		case MODE__TCP_IPV4:{
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

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> END OF EXAMPLE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		}break;












		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> EXAMPLE : TCP IPV6 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		case MODE__TCP_IPV6:{
			printf("Server > This is a basic demonstration of \"network.c/.h\"\n         for TCP communication in IPv6 (server side).\n");



			// ---- START ----

			//create socket
			network* nw = network_create(
				NETWORK__SERVER,
				NETWORK__TCP,
				NETWORK__IPV6
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

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> END OF EXAMPLE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		}break;




















		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> EXAMPLE : UDP IPV4 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		case MODE__UDP_IPV4:{
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
				network_setInfo(client, "127.0.0.1", PORT); //local address
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

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> END OF EXAMPLE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		}break;












		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> EXAMPLE : UDP IPV6 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		case MODE__UDP_IPV6:{
			printf("Server > This is a basic demonstration of \"network.c/.h\"\n         for UDP communication in IPv6 (server side).\n");



			// ---- START ----

			//create socket
			network* nw = network_create(
				NETWORK__SERVER,
				NETWORK__UDP,
				NETWORK__IPV6
			);

			//bind all addresses
			network_bind(nw, PORT);

			//create a network instance for the incomming client
			network* client = network_create(
				NETWORK__CLIENT,
				NETWORK__UDP,
				NETWORK__IPV6
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
				network_setInfo(client, "::1", PORT); //local address
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
				usleep(500000);



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
				printf("Server > Incomming message [%s] received from \"%c%c:%c%c:%c%c:%c%c:%c%c:%c%c:%c%c:%c%c\".\n",
					message,
					receivedAddress[ 0], receivedAddress[ 1], receivedAddress[ 2], receivedAddress[ 3],
					receivedAddress[ 4], receivedAddress[ 5], receivedAddress[ 6], receivedAddress[ 7],
					receivedAddress[ 8], receivedAddress[ 9], receivedAddress[10], receivedAddress[11],
					receivedAddress[12], receivedAddress[13], receivedAddress[14], receivedAddress[15]
				);
				free(receivedAddress);
			}



			// ---- STOP ----

			//end connection
			network_delete(nw);
			printf("Server > Ended network connection.\n");

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> END OF EXAMPLE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		}break;

	}

	return EXIT_SUCCESS;
}
