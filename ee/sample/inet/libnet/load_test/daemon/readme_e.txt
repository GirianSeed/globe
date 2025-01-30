[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved


Sample Program for Sending/Receiving Packets between the Client
and Server Using TCP in the INET Library (Server program)


< Description of the Sample >
        This sample program allows passive opening of TCP using the
	libnet in EE to send/receive data to/from the connected client.
	As a server, the number of concurrent standby threads can
	be specified using arguments.

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
        comm.c        : Communication code 


< Activating the Program >

        $ make                                  : Compiles the program
        $ dsedb					: Activates dsedb
        > run main.elf <th_num>  		: Executes main.elf

		<th_num>: The number of concurrently established standby
			  connections

On activating the server, establish the connection from the client
program and start communications.

Individual client programs are provided for EE and IOP.
For activating the program, please see the respective documents:

/usr/local/sce/ee/sample/inet/load_test/client/readme_e.txt
/usr/local/sce/iop/sample/inet/load_test/client/readme_e.txt


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
	for development.  Be aware that it is necessary to use
	individually encoded setting files for the titles to be released
	officially. 
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
        3. Activates th_num of standby threads.

	Standby threads are generated each time a connection is
	established.
	The following is carried out in each standby thread;

	4. Performs passive opening of TCP and waits for the connection
	   from the client.
	5. Once the connection is established, receives the send/receive
	   data.
	6. Receives data.
	7. Sends the received data to the client.
	8. Terminated after 1500 times of send/receive operations.
