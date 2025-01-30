[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved


Sample Program for Sending/Receiving Packets between the Client and Server
Using TCP in the INET Library (Client program)


< Description of the Sample >
        This sample program is for establishing the server - TCP
	connections using the libnet in EE to send/receive the specified
	size of data.
	The data size and the number of concurrently activated threads
	can be specified using arguments.  The corresponding program must
	be activated in the server prior to the activation of the program.

[ Caution ]
	an986.irx will be required if you are using the USB Ether adapter
	in this sample program.
	Since the use of an986.irx in titles is prohibited, an986.irx is
	not included in the release package (under sce/iop/modules).
	For details, please see the technical information area of the
	SCE-NET Web site as of Nov. 22, 2000.  an986.irx can be downloaded
	from "Download" -> "Other common tools" of the SCE-NET Web site.

< File >
        main.c        : Main program

< Activating the Program >
Activate the server program on a server.
Two types of server programs are provided for Linux and EE.

For activating the program, please see the respective documents:
/usr/local/sce/ee/sample/inet/load_test/linux_daemon/readme_e.txt
/usr/local/sce/ee/sample/inet/load_test/daemon/readme_e.txt

Activate the client program when the server is ready.

        $ make                                  : Compiles the program
        $ dsedb					: Activates dsedb
        > run main.elf <saddr> <th_num> <pktsize>
                                                : Executes main.elf
        <saddr>: server address
	<th_num>: The number of concurrently established connections
	<pktsize>: Send/Receive data size

< Using the Controller >
        None

< Note >
	This program assumes the connection environment is USB Ethernet,
	HDD Ethernet, PPPoE(USB Ethernet) or PPPoE(HDD Ethernet).
	To change the connection environment setting, switch the following
	#defines which appear in the beginning of this program.

	#define USB_ETHERNET			: USB Ethernet
	#define USB_ETHERNET_WITH_PPPOE		: PPPoE(USB Ethernet)
	#define HDD_ETHERNET			: HDD Ethernet
	#define HDD_ETHERNET_WITH_PPPOE		: PPPoE(HDD Ethernet)

	To change IP address or use PPP, please make an appropriate change
	to the setting file indicated by the setting name "Combination?"
	(? must be a number) for each connection environment.
	The followings are the combinations of setting names and setting
	files.

	USB Ethernet	         Combination4         net003.cnf
	PPPoE(USB Ethernet)   	 Combination5         net004.cnf
	HDD Ethernet	         Combination6         net005.cnf
	PPPoE(HDD Ethernet)   	 Combination7         net006.cnf

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
	device is connected.  It does not support multiple connections of
	the same device model, either.

< Operation Overview >
        The program is processed as follows:

        1. Activates the worker thread.
	2. Initializes the program for communications.
        3. Activates th_num of communication threads.

	The following is carried out in each communication thread;

	4. Establishes the TCP connection with the server
	5. Sends the send/receive data size.
	6. Sends the pktsize-bite(s) data to the server.
	7. Receives data from the server.
	8. Confirms the identity of data.
	9. Terminated after 1500 times of send/receive operations.
