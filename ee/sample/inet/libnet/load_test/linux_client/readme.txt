[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2000 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample that uses TCP to send and receive packets between a client and server on UNIX. (client program)

<Description>

This program is a UNIX version of the sample program that uses libnet to establish a TCP connection with the server from the EE and send and receive data of a specified size. The program has been verified to run on Linux.
The data size and number of connections to startup simultaneously can be specified in arguments.
The corresponding programs must be started on the server first. 
The operation of this program is the same as /usr/local/sce/ee/sample/inet/load_test/client.

<Files>
        main.c        : Main program


<Run method>

        $ make                                   : Compile
        $ ./load_test <saddr> <th_num> <pktsize> : Execute load_test
        <saddr>: Server address
	<th_num>: Number of connections to be established simultaneoulsy. 
	<pktsize>: Data size for send and receive

<Notes>
A comparison can be made between the inet API and the BSD socket API.
