[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Single-threaded Echo Server Sample That Runs on Top of the libnet Library


<Description>
	This program is a sample echo server that uses libnet. It performs a TCP passive open operation from the ee, receives data from a connected client, then sends the data directly back to the client.
	The number of waiting threads that will be started up simultaneously can be specified by an argument.

[Notes]
	To use a USB Ethernet adapter with this sample program, an986.irx is required.
	Since the use of an986.irx is not permitted within a title (for details, see the technote dated 11/22/00 available on the developer website), an986.irx has not been included in the release package (under sce/iop/modules). Download it from the developer website under "Download" -> "Other Common Tools."

<Files>
        main.c		:  Main program
        single.c	:  Communications code

<Execution method>

        $ make				:  Compile
        $ dsedb				:  Start up dsedb
        > run main.elf <backlog>	:  Execute main.elf

After echo_server is started, a connection should be made from a telnet program on linux or another host.

        % telnet <saddr> 7: <saddr>, server address
        Trying 192.168.8.1...
        Connected to 192.168.8.1.
        Escape character is '^]'.
        hello
        hello
        bye
        bye

As shown above, the characters sent from the client are returned unchanged from the server.

<Controller operations>
	None

<Remarks>
	This program is the PS2 version of the echo server program that is generally reserved for TCP port number 7.
	It exhibits the same behavior as when % telnet <unix_ip_addr> 7 is entered for a UNIX machine.

	This program assumes that the connection environment is USB Ethernet, HDD Ethernet, PPPoE (USB Ethernet), or PPPoE (HDD Ethernet).
	The connection environment can be changed by switching the following #define statements at the beginning of the program.

	#define USB_ETHERNET:				USB Ethernet
	#define USB_ETHERNET_WITH_PPPOE:		PPPoE (USB Ethernet)
	#define HDD_ETHERNET:				HDD Ethernet
	#define HDD_ETHERNET_WITH_PPPOE:		PPPoE (HDD Ethernet)

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



