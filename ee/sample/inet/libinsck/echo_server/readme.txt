[SCEI CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample echo server that runs on top of the libinsck library

<Description>
This program is a sample echo server that uses libinsck. It performs a TCP passive open operation from the ee, receives data from a connected client, then sends the data back to the client.
This sample program is the socket library version of ee/sample/inet/libnet/echo_server.

[Notes]
To use a USB Ethernet adapter with this sample program, an986.irx is required.
	Since the use of an986.irx is not permitted within a title (for details, refer to the tech notes area of the developer support website, an986.irx has not been included in the release package (under sce/iop/modules). Download it from the developer support website.

<File>
        main.c:		Main program


<Run Method>
	$ make:		Compile
	$ dsedb:	Start up dsedb
	> run main.elf:	Execute main.elf

After echo_server is started, a connection should be made from a telnet program on linux or another host.

        % telnet <saddr> 7:		<saddr>, server address
        Trying 192.168.8.1...
        Connected to 192.168.8.1.
        Escape character is '^]'.
        hello
        hello
        bye
        bye


As shown above, the characters that were sent from the client are returned as is from the server.


<Controller Operation>
	None

<Remarks>
This program is the PlayStation 2 version of the echo server that is usually reserved for TCP port number 7.
It exhibits the same behavior as when % telnet <unix_ip_addr> 7 is entered on a UNIX machine.

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

In addition, the program accesses a common text configuration file located under /usr/local/sce/conf/net. Note that this is done because this program is used as a development sample and an individually encoded configuration file must be used by an official title.
For information about how to use individually encoded files, refer to the sample configuration application.

Since only one interface is specified per configuration file in the current connection environment configuration files, the program also assumes that multiple devices will not be used simultaneously.
This program runs under the condition that exactly one target device is connected.
Note that multiple connections to the same device are also not considered.

<Processing Summary>
1. Activate worker thread.
2. Perform initialization for communication.
3. Perform a TCP passive open operation (listen and accept).
4. When the connection is established, activate a thread for that connection and wait for a separate connection.
The thread for each connection will:
5. Wait for data from the client.
6. When data is received from the client, send the received data back to the client as is.
7. When the connection from the client is disconnected, processing will be terminated.

