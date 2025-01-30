[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2000 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Sample Program for Sending/Receiving Packets between the Client
and Server Using TCP on UNIX (Client program)


< Description of the Sample >
	This is a UNIX version sample program for establishing the server
	- TCP connection using the libnet in EE to send/receive the
	specified size of data.  The operation has been tested on Linux.
	The data size and the number of concurrently activated threads
	can be	specified using	arguments.  The corresponding program
	must be activated in the server prior to the activation of the
	program.

	The operation of this program is the same as in the case of 
	/usr/local/sce/ee/sample/inet/load_test/client.


< File >
        main.c        : Main program


< Activating the Program >

        $ make                                   : Compiles the program
        $ ./load_test <saddr> <th_num> <pktsize> : Executes load_test
        <saddr>: server address
		<th_num>: The number of concurrently established connections
		<pktsize>: Send/Receive data size

< Note >
        This sample program can be used to perform a comparison between
	the INET API and BSD socket API.

