[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved


Sample Program for Server-Client Data Exchange 
via UDP Packets in the Libnet Library

< Description of the Sample >
        This sample program is for the Linux server program which enables
	physical calculations to find orbit or collision of balls.
	The result is received by the PlayStation 2 client programs
	then drawing operations are carried out.

[ Caution ]
	an986.irx will be required if you are using the USB Ether adapter
	in this sample program. 
	Since the use of an986.irx in titles is prohibited, an986.irx
	is not included in the release package (under sce/iop/modules).
	For details, please see the technical information area of the
	SCE-NET Web site as of Nov. 22, 2000.  an986.irx can be downloaded
	from "Download" -> "Other common tools" of the SCE-NET Web site.

< File >

/usr/local/sce/ee/sample/inet/ball_game/ee:

        buff0.dsm       : Packet for setting the transfer state 0 of
			  the triple buffer
        buff1.dsm       : Packet for setting the transfer state 1 of
			  the triple buffer
        buff2.dsm       : Packet for setting the transfer state 2 of
			  the triple buffer
        grid.dsm        : Ground model
        m_opt.vsm       : Optimization for drawing microcode
        main.c          : Main functions
        mat.dsm         : Ground texture
        mathfunc.c      : Operation function 
        mathfunc.h      : Parameter definitions
        matrix.dsm      : Packet template for matrices
        no[1-4].dsm     : No. 1 through 4 textures
        packet.dsm      : Packet data for path1/path2
        phys.c          : Function for calculating the ball position
			  and set it in the packet
        sphere.dsm      : Model of a ball
        main.h      	: Parameters and prototypes declarations of
			  this sample
/usr/local/sce/ee/sample/inet/ball_game/gamed:
        gamed.c         : Main function of the server program
        gamed.h         : Header file for the server program
        ntohf.c         : Utility function for endian conversions
        ntohf.h         : Header file for the utility function
        physics.c       : Physical calculation (e.g. orbit, collision)
        physics.h       : Header file for the physical calculation

/usr/local/sce/ee/sample/inet/ball_game/common:
        common.h        : Common header file for server and client

< Activating the Program >
        $ make              : Compiles the program.

Activates the server program, gamed. The gamed operates on Linux.

        $ cd gamed; gamed   : Executes the server program on Linux.

Activates the client program on another terminal.

	$ cd ee
        $ dsedb -r run main.elf <saddr>
                            : Executes client.  saddr is the server which
			      gamed is activating.
        
        Each time additional client program is executed for multiple
	clients, the ball is added.

< Using the Controller >
        START button: The own client ball falls again.

< Note >
	This program assumes the connection environment is USB Ethernet
	and HDD Ethernet.
	To change the connection environment setting, switch #define which
	appears in the beginning of this program.

	#define USB_ETHERNET		: USB Ethernet
	#define HDD_ETHERNET		: HDD Ethernet

	To change IP address or use PPP, please make an appropriate
	change to the setting file indicated by the setting name
	"Combination?" (? must be a number) for each connection
	environment.
	The followings are the combinations of setting names and setting
	files.

	USB Ethernet        Combination4      net003.cnf
	HDD Ethernet        Combination6      net005.cnf

	In addition, this program refers to the setting file in plaintext
	form stored under /usr/local/sce/conf/net for the use of samples
	for development. Be aware that it is necessary to use individually
	encoded setting files for the titles to be released officially.
	For the directions for using the individually encoded files, see
	the application samples for setting.
	
	Since the current setting file for connection environment
	specifies only 1 interface to 1 setting file, this program is not
	intended to use multiple devices at the same time.
	This program operates under the condition where only 1 target
	device is connected. It does not support multiple connections of
	the same device model, either.

< Operation Overview >
        The program is processed as follows;

        [Client]
        1. Loads modules, initializes the INET
        2. Connected to the remote port 
        3. Request position to the server
        4. Requests position reset to the server by pressing the
	   START button
        5. Receives the client ball information from the server
	   to carry out drawings.

        [Server]
        1. Initialized and waits for the connection with clients. 
        2. Threads are activated every time the connection is made in
	   order to respond the location information request from clients.
	3. Sends the current location information on receiving the request
	   from clients.
	4. Each time responding to the client requests, a calculation is
	   carried out again so that individual response timings are
	   adjusted.
