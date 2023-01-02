#executables
run: network.o 1_server.o 2_client.o
	gcc -Wall -o 1_run 1_server.o network.o -O2
	gcc -Wall -o 2_run 2_client.o network.o -O2
	rm -f *.o



#example programs
1_server.o: src/1_server.c lib/network.h
	gcc -Wall -c src/1_server.c -O2

2_client.o: src/2_client.c lib/network.h
	gcc -Wall -c src/2_client.c -O2



#network library (TCP/UDP - IPv4/IPv6)
network.o: lib/network.c lib/network.h
	gcc -Wall -c lib/network.c -O2
