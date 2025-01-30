[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Echo Server Sample for Operations in the Libinsck Library


<Description of the sample>

	This sample program is an echo server program which allows passive
	opening	of TCP using the libinsck in EE to send the received data 
	from the connected client as is.
	This is a socket library version of ee/sample/inet/libnet/echo_server.


[ Caution ]
	
  	an986.irx will be required if you are using the USB Ether adapter
	in this sample program.
  	Since the use of an986.irx in titles is prohibited (for details,
  	please see the technical information area of the SCE-NET Web site
	as of Nov. 22, 2000.), an986.irx is not included in the release
	package (under sce/iop/modules). an986.irx can be downloaded from
	"Download" -> "Other common tools" of the SCE-NET Web site. 
  	
<File>
        main.c        : Main program


<Activating the program>

        $ make                                  : Compiles the program
        $ dsedb	                                : Activates dsedb
        > run main.elf                          : Executes main.elf


Make connections using a telnet program such as linux once echo_server
is activated.

        % telnet <saddr> 7                      : <saddr>, server address
        Trying 192.168.8.1...
        Connected to 192.168.8.1.
        Escape character is '^]'.
        hello
        hello
        bye
        bye


As described above, the characters sent from a client are returned from
the server as it is.


<Using the controllers>
        None

<Notes>

	This program is a PlayStation 2 version of the echo server program
	generally reserved in a port #7 of TCP.
	The same operation is carried out in the UNIX machine with
        % telnet <unix_ip_addr> 7.

	This program is intended to use USB Ethernet, HDD Ethernet,
	PPPoE(USB Ethernet), or PPPoE(HDD Ethernet) for the network
	environment.
	To change the connection environment setting, switch the following
	#defines which appear in the beginning of this program.

	#define USB_ETHERNET			: USB Ethernet
	#define USB_ETHERNET_WITH_PPPOE		: PPPoE(USB Ethernet)
	#define HDD_ETHERNET			: HDD Ethernet
	#define HDD_ETHERNET_WITH_PPPOE		: PPPoE(HDD Ethernet)

	To change IP addresses or user names of PPPoE, or to use PPP,
	please make an appropriate change to the setting file indicated
	by the setting name "Combination?"(? is a numerical value)
	for respective connection environment.
	The followings are the combinations of setting names and setting
	files.

	USB Ethernet	         Combination4         net003.cnf
	PPPoE(USB Ethernet)      Combination5         net004.cnf
	HDD Ethernet             Combination6         net005.cnf
	PPPoE(HDD Ethernet)      Combination7         net006.cnf

	In addition, this program refers to the setting file in a plaintext
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

<Operation Overview>

	The program is processed as follows:

	1. Activates the worker thread.
	2. Initializes the program for communications.
	3. Performs passive opening (listen, accept).
	4. Once the connection is established, activates the thread for
	   that connection. Then waits for another connection.
	
	The following is carried out in a thread for each connection;

	5. Waits for the data from the client.
	6. On receiving the data from the client, sends it to the client
	   as it is.
	7. Program ends when disconnected form the client.
