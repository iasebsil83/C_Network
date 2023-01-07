This program is a simple utility library that allows you to create
and use TCP/UDP networks in IPv4/IPv6 very easily.

Two programs have been made as demonstrations :
> src/1_server.c
> src/2_client.c

The prefix number (1 and 2) corresponds to the order in which programs should be run.
1st the server, 2nd the client.
However, it is not mandatory for several reasons (depending on protocol).

More information about this library is available in lib/network.c.

WARNING TO BEGINNERS: Even if this library simplifies the use of TCP/UDP networks,
the mecanisms hidden behind these networks can be a bit tricky sometimes, especially
in the sendTo()/receiveFrom() functions.
I tried to make all this the more simple as I can but if you are not familiar with
these protocols, their use can be a bit hard to understand.



Contact     : i.a.sebsil83@gmail.com
Youtube     : https://www.youtube.com/user/IAsebsil83
GitHub repo : https://github.com/iasebsil83

Let's Code !                                  By I.A.
