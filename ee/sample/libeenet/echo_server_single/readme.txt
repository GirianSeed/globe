[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
        Copyright (C) 2002 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Single-threaded Echo Server Using the libeenet Library


<Description>
	This program is a single-threaded echo server sample program. 
	The select() function is used to detect the socket operating conditions.
	If the detected socket is a server socket, a new socket is prepared for the receiving client. If it is not a server socket, recv() is used to receive data from the client, then send() is used to send that data as-is. This processing is completely included within a single loop in the main thread. No additional threads are required.

[Note]
	To use a USB Ethernet adapter with this sample program, ent_eth.irx is required.
	Since the use of ent_eth.irx is not permitted within a title, ent_eth.irx has not been included in the release package (under sce/iop/modules). Download it from the developer support website.

<File>
        main.c			:  Main program

<Execution Method>
        $ make			:  Compile
        $ dsedb -r run main.elf	:  Execute

	  Or, the following can be issued after compilation.
	$ make run		:  Execute

Once echo_server is running, a connection should be made from a telnet client.

	% telnet <saddr> 7		: <saddr>, server address
	Trying 192.168.8.1...
	Connected to 192.168.8.1.
	Escape character is '^]'.
	hello
	hello
	bye
	bye


As shown above, the characters sent from the client are returned as-is from the server.

<Controller Operation>
	None

<Comments>
	This program is the PS2 version of the echo server program that is generally reserved for TCP port number 7.
	This program assumes that the connection environment is DHCP (USB Ethernet), DHCP (network adapter), PPPoE (USB Ethernet), PPPoE (network adapter), or PPP (modem).
	The connection environment can be changed by switching the following #define statements at the beginning of the program.

	#define HDD_ETHERNET:			DHCP (network adapter)
	#define HDD_ETHERNET_WITH_PPPOE:	PPPoE (network adapter)
	#define USB_ETHERNET:			DHCP (USB Ethernet)
	#define USB_ETHERNET_WITH_PPPOE:	PPPoE (USB Ethernet)
	#define MODEM:				PPP (modem)

	To use PPP (modem), change the following #define statements, which are NULL strings in iopmodules.h, to match the PPP connection environment and modem that are used.

	#define USE_CONF_NAME		""	: Configuration name within net.db
	#define IOP_MOD_MODEMDRV	""	: Module path name
	#define MODEMDRV_ARG		""	: Argument
	
	To use a USB modem, the USBD module must be loaded in addition to the modules specified above. If additional modules must be loaded because of the type of modem to be used, modify http.c appropriately.

	To change the IP address, to change the PPPoE user name, or to use PPP, change the configuration file indicated by the connection environment configuration name "Combination?" (where ? is a number) as appropriate. The correspondence between configuration name and configuration file for each connection environment is shown below.

	DHCP (network adapter)		Combination6         net005.cnf
	PPPoE (network adapter)		Combination7         net006.cnf
	DHCP (USB Ethernet)		Combination4         net003.cnf
	PPPoE (USB Ethernet)		Combination5         net004.cnf
	PPP (modem)			Combination2         net001.cnf

	In addition, the program accesses an unencoded text configuration file located under /usr/local/sce/conf/net. Note that this is done because this program is used as a development sample and an individually encoded configuration file must be used by an official title.
	For information about how to use individually encoded files, refer to the sample configuration application.

	Since only one interface is specified per configuration file in the current connection environment configuration files, the program also assumes that multiple devices will not be used simultaneously.
	This program runs under the condition that exactly one target device is connected.
	Note that multiple connections to the same device are also not considered.

<Processing Summary>
	1. Initialize libeenet, load various modules.
	2. Initialize the server port.
	3. Use select, enter a loop and detect the socket operating conditions.
	4. If data is present on the server socket, accept is called and a new socket is created that the client will report.
	5. If the socket is not a server socket, operation is considered to be from the client, and data from that client is received and returned as-is.
	6. If the connection is disconnected from the client, the program will terminate.

