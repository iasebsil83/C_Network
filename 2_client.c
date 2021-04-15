// ---------------- IMPORTATIONS ----------------

//standard
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



//network utility
#include "network.h"




















/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 2_Client [V.V.V] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                  network.h demonstration program : client side

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
#define ADDRESS "::1" //localhost for IPv6. For IPv4, use "127.0.0.1"
#define PORT 8080



//message
#define MESSAGE_LENGTH_MAX 100




















// ---------------- EXECUTION ----------------

//main
int main(){
	//presentation
	printf("Client > This is a basic demonstration of \"network.c/.h\" (client side).\n");






	//TCP - IPv4 : START

	//create socket
	network* nw = network_create(
		NETWORK__CLIENT,
		NETWORK__TCP,
		NETWORK__IPV6
	);

	//try to connect
	while( !network_connect(nw, ADDRESS, PORT) ){
		usleep(250000);
	}
	printf("Client > Connected to server.\n");






	//TCP - IPv4 : DATA TRANSFERS

	//main loop
	char message[MESSAGE_LENGTH_MAX];
	while(1){



		//RECEIVE

		//reset message
		bzero(message, MESSAGE_LENGTH_MAX);

		//receive reply
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






		//SEND

		//reset message
		bzero(message, MESSAGE_LENGTH_MAX);

		//get user input
		printf("Client > Write something to send to server : ");
		for(int i=0; i < MESSAGE_LENGTH_MAX-1; i++){ //-1 to leave the last byte at 0 ('\0')
			message[i] = getchar();

			//stop input at first line return
			if(message[i] == '\n'){
				message[i] = '\0';
				break;
			}
		}

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
		printf("Client > Message [%s] sent.\n", message);
	}






	//TCP - IPv4 : STOP

	//end connection
	network_delete(nw);
	printf("Client > Ended network connection.\n");






	return EXIT_SUCCESS;
}