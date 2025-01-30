[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2000 Sony Computer Entertainment Inc.
                                                     All Rights Reserved


Sample Program for Sending/Receiving Packets between the Client
and Server Using TCP on UNIX (Server program)


< Description of the Sample >
        This is a UNIX version sample program which allows passive opening
	of TCP to send/receive data with the connected client.
	The operation has been tested on Linux.  

	The operation of this program is the same as in the case of 
	/usr/local/sce/ee/sample/inet/load_test/daemon.



< File >
        main.c        : Main program


< Activating the Program >

        $ make              : Compiles the program
        $ ./load_testd      : Activates the server program


On activating the server, establish the connection from the client
program and start communications. 

Individual client programs are provided for EE and IOP.
For activating the program, please see the respective documents:

/usr/local/sce/ee/sample/inet/load_test/client/readme_e.txt
/usr/local/sce/iop/sample/inet/load_test/client/readme_e.txt


< Note >
        This sample program can be used to perform a comparison between
	the INET API and BSD socket API.