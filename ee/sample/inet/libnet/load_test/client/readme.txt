[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample that uses TCP to send and receive packets between a client and server using libnet. (client program)


<Description>
This program uses libnet to establish a TCP connection with a server from the EE, then sends and receives data of a specified size.
The data size and number of simultaneous connections to startup can be specified in arguments.
The corresponding program must be started on the server first.

[Precautions]
In this sample an986.irx is required when using a USB Ethernet adapter.

Because the use of an986.irx is not allowed in titles(refer to the tech note on the developer support web site), it is not included with the release package (under sce/iop/modules). Download an986.irx from the developer support web site.


<Files>
        main.c        : Main program


<Run method>
First start the server program on the server. There are two types of server programs that run on Linux and the EE. Refer to the following documents for details on starting each program.

/usr/local/sce/ee/sample/inet/load_test/linux_daemon/readme.txt
/usr/local/sce/ee/sample/inet/load_test/daemon/readme.txt

Once server preparations are complete start the client program.

        $ make                       : Compile
        $ dsedb		             : Start dsedb
        > run main.elf <saddr> <th_num> <pktsize>
                                     : Execute main.elf
        <saddr>: Server address
	<th_num>: Number of connections to establish simultaneously
	<pktsize>: Data size to send and receive

<Controller operation>
        None

<Note>
This sample program assumes that the connection environment is USB Ethernet, HDD Ethernet, PPPoE (USB Ethernet), or PPPoE (HDD Ethernet).

The connection environment can be changed by switching the following #define statements at the beginning of the program.

	#define USB_ETHERNET:			USB Ethernet
	#define USB_ETHERNET_WITH_PPPOE:	PPPoE (USB Ethernet)
	#define HDD_ETHERNET:			HDD Ethernet
	#define HDD_ETHERNET_WITH_PPPOE:	PPPoE (HDD Ethernet)

To change the IP address or the PPPoE user name, or to use PPP, change each configuration file indicated by the connection environment configuration name "Combination?" (where ? is a number) as appropriate. The correspondence between configuration name and configuration file for each connection environment is as follows.

	USB Ethernet		Combination4         net003.cnf
	PPPoE (USB Ethernet)	Combination5         net004.cnf
	HDD Ethernet		Combination6         net005.cnf
	PPPoE (HDD Ethernet)	Combination7         net006.cnf


This program references configuration files under /usr/local/sce/conf/net. This is because this program is for use as a development sample.  Please note that for an official title etc., individually encoded configuration files should be used. For information on how to use individually encoded configuration files, please refer to the sample configuration application.

In the current connection environment configuration files, only one interface is specified per file. Hence, in this sample program, it is assumed that multiple devices are not being used simultaneously. In this program, the target device works under the condition that only a single connection has been made. Please note that no considerations have been made regarding multiple connections for the same device.

<Process summary>
A summary of the process is shown below.

        1. Start work thread.
	2. Perform initialization of communication.
        3. Start th_num communication threads.
	For each thread,
        4. Establish a TCP connection with the server.
        5. Send data size to send and receive.
        6. Send pktsize bytes of data to the server.
        7. Receive data from the server.
	8. Check data to make sure that is the same.
	9. Exit after repeating send and receive 1500 times.
