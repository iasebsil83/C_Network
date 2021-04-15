// ---------------- IMPORTATIONS ----------------

//standard
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//network utility
#include "network.h"




















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




















// ---------------- EXECUTION ----------------

//main
int main(){
	//presentation
	printf("Server > This is a basic demonstration of \"network.c/.h\" (server side).\n");






	//TCP - IPv4 : START

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






	//TCP - IPv4 : DATA TRANSFERS

	//main loop
	char message[MESSAGE_LENGTH_MAX];
	while(1){



		//SEND

		//reset message
		bzero(message, MESSAGE_LENGTH_MAX);

		//get user input
		printf("Server > Write something to send to client : ");
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
			nw, client,
			message, MESSAGE_LENGTH_MAX
		);

		//exit reply
		if(!strncmp(message,"exit",4)){
			printf("Server > Exit request sent.\n");
			break;
		}
		printf("Server > Message [%s] sent.\n", message);






		//RECEIVE

		//reset message
		bzero(message, MESSAGE_LENGTH_MAX);

		//receive reply
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






	//TCP - IPv4 : STOP

	//end connection
	network_delete(nw);
	printf("Server > Ended network connection.\n");






	return EXIT_SUCCESS;
}
