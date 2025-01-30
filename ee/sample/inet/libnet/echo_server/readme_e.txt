[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Echo Server Sample for Operations in the Libnet Library


< Description of the Sample >
        This sample program is an echo server program which allows passive
	opening	of TCP using the libnet in EE to send the received data 
	from the connected client as is.  The number of concurrent standby
	threads	can be specified using arguments.

[ Caution ]
	an986.irx will be required if you are using the USB Ether adapter
	in this sample program.
	Since the use of an986.irx in titles is prohibited, an986.irx is not
	included in the release package (under sce/iop/modules). 
	For details, please see the technical information area of the
	SCE-NET Web site as of Nov. 22, 2000.  an986.irx can be downloaded
	from "Download" -> "Other common tools" of the SCE-NET Web site.

< File >
        main.c        : Main program
        comm.c        : Communication code


< Activating the Program >

        $ make                                  : Compiles the program
        $ dsedb					: Executes dsedb
        > run main.elf <th_num> 		: Executes main.elf
		<th_num>: The number of concurrently established standby
			  threads.

On activating the echo_server, make a connection from the program such as
a telnet in Linux. 


        % telnet <saddr> 7                    : <saddr>, server address
        Trying 192.168.8.1...
        Connected to 192.168.8.1.
        Escape character is '^]'.
        hello
        hello
        bye
        bye


Characters sent from the client are returned to the server as is.


< Using the Controller >
        None

< Note >
	This program is a PlayStation 2 version of the echo server program
	that is reserved in the port #7 of general TCPs.
	The same operation is carried out in the UNIX machine with the
        following.
	% telnet <unix_ip_addr> 7 
	
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
	for development. Be aware that it is necessary to use
	individually encoded setting files for the titles to be released
	officially. 
	For the directions for using the individually encoded files, see
	the application samples for setting.
	
	Since the current setting file for connection environment
	specifies only 1 interface to 1 setting file, this program is not
	intended to use multiple devices at the same time.
	This program operates under the condition where only 1 target
	device is connected. It does not support multiple connections of
	the same device model, either.

< Operation Overview >
        The program is processed as follows:

        1. Activates the worker thread.
	2. Initializes the program for communications.
        3. Activates th_num of standby threads.

	Standby threads are generated each time a connection is
	established.  
	The following is carried out in each standby thread;

	4. Performs passive opening of the port #7 of TCP and waits for
	   the connection from the client.
	5. Once the connection is established, waits for the data from the
	   client.
	6. On receiving the data from the client, sends it to the client
	   as is.
	7. Program ends when disconnected from the client.
