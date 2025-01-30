[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample that performs conversion on client/server data containing UDP packets using the libnet library.


<Description>
This program performs physical calculations, such as trajectory and impact calculations of balls in a server program running on Linux. The calculation results are then received by each PlayStation 2 client program where an image is drawn. 

[Note]

In this sample an986.irx is required when using a USB Ethernet adapter.

Because the use of an986.irx is not allowed in titles (refer to the tech note on the developer support web site), it is not included with the release package (under sce/iop/modules). Download an986.irx from the developer support web site.

<Files>

/usr/local/sce/ee/sample/inet/ball_game/ee:

        buff0.dsm       :Packet to set triple buffer 
			 transfer state 0
        buff1.dsm       :Packet to set triple buffer 
			 transfer state 1
        buff2.dsm       :Packet to set triple buffer 
			 transfer state 2
        grid.dsm        :Surface model
        m_opt.vsm       :Microcode optimization for 
			 image drawing
        main.c          :Main functions
        mat.dsm         :Surface texture 
        mathfunc.c      :Various numerical value 
			 calculation functions
        mathfunc.h      :Defines various parameters
        matrix.dsm      :Packet template for matrix
        no[1-4].dsm     :Textures from No. 1 to No. 4
        packet.dsm      :Packet data flow to path1/path2
        phys.c          :Function to set ball positions 
			 within calculated packets
        sphere.dsm      :Ball model
        main.h      	:Parameter and prototype 
			 declarations of samples

/usr/local/sce/ee/sample/inet/ball_game/gamed:
        gamed.c         : Server program main function
        gamed.h         : Server program header file
        ntohf.c         : Utility function for endian 
			  conversion
        ntohf.h         : Identical header files 
        physics.c       : Physical calculation process 
			  of trajectory and impact 
			  calculations 
        physics.h       : Identical header file

/usr/local/sce/ee/sample/inet/ball_game/common:
        common.h        : Common header file in server 
			  and client 

<Run method>
        $ make           : Compile

First, start gamed located in the server program. The program gamed runs on Linux.

        $ cd gamed; gamed: Execute server program on Linux

Next, start the client program on another terminal.

        $ cd ee
        $ dsedb -r run main.elf <saddr>
: Execute client. saddr is set to the dhcp server where gamed was started.
        
Subsequently, balls are added each time a client program is executed on more than one client.

<Controller operation>
        START button : Drop ball of the client again 

<Notes>

This program assumes that the connection environment is USB Ethernet or HDD Ethernet.

The connection environment can be changed by switching the following #define statements at the beginning of the program.
	
	#define USB_ETHERNET:		USB Ethernet
	#define HDD_ETHERNET:		HDD Ethernet

To change the IP address or to use PPP, change the configuration file indicated by each connection environment configuration name "Combination?" (where ? is a number) as appropriate. The correspondence between configuration name and configuration file for each connection environment is as follows.

	USB Ethernet          Combination4         net003.cnf
	HDD Ethernet          Combination6         net005.cnf


This program references configuration files under /usr/local/sce/conf/net. This is because this program is for use as a development sample.  Please note that for an official title etc., individually encoded configuration files should be used. For information on how to use individually encoded configuration files, please refer to the sample configuration application.

In the current connection environment configuration files, only one interface is specified per file. Hence, in this sample program, it is assumed that multiple devices are not being used simultaneously. In this program, the target device works under the condition that only a single connection has been made. Please note that no considerations have been made regarding multiple connections for the same device.

<Process summary>
A summary of the process is given below.

        [Client]
        1. Load each module and initialize inet.
        2. Connect to remote port.
        3. Send position request to server.
        4. When "start" is pressed, a position reset 
	   request is sent to the server.
        5. Receive ball information of each client from 
	   the server and draw each image.

        [Server]
        1. Initialize and wait for connection from client.
        2. Start a thread for each connection and set up 
	   a position information request from each 
	   client.
        3. When a position information request is 
	   received, transfer the current position 
	   information to the client.
        4. Advance a unit of time whenever a request is 
	   sent to all connected clients, then 
	   recalculate the position.


