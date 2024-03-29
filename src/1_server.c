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
#define PORT 5000

//message
#define MESSAGE_LENGTH_MAX 100

// >>>>>>>>>>>>>>>>>>>>>>>>>>>> THERE ARE 4 MODES FOR THIS EXAMPLE PROGRAM <<<<<<<<<<<<<<<<<<<<<<<<
#define MODE__TCP_IPV4 0x00
#define MODE__TCP_IPV6 0x01
#define MODE__UDP_IPV4 0x02
#define MODE__UDP_IPV6 0x03

// <<<<<<<<<<<<<<<<<<<<<<<<<<<< CHOOSE the one you want
#define DEMONSTRATION_MODE MODE__UDP_IPV6








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

		case MODE__TCP_IPV4:{
			#include "1_server__tcp_ipv4.c"
		}break;

		case MODE__TCP_IPV6:{
			#include "1_server__tcp_ipv6.c"
		}break;

		case MODE__UDP_IPV4:{
			#include "1_server__udp_ipv4.c"
		}break;


		case MODE__UDP_IPV6:{
			#include "1_server__udp_ipv6.c"
		}break;

	}

	return EXIT_SUCCESS;
}
