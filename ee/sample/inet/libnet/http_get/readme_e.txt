[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Sample Program for Obtaining a Remote File
     Using the Http Protocol in the Libnet Library 


< Description of the Sample >
        This sample program allows the remote http port access
	to download a file using the network library libinet in EE.

[ Caution ]
	an986.irx will be required if you are using the USB Ether adapter
	in this sample program. 
	Since the use of an986.irx in titles is prohibited, an986.irx is
	not included in the release package (under sce/iop/modules).
	For details, please see the technical information area of the
	SCE-NET Web site as of Nov. 22, 2000.  an986.irx can be downloaded
	from "Download" -> "Other common tools" of the SCE-NET Web site.

< File >
        http.c        : Main program


< Activating the Program >
        $ make                          : Compiles the program
        $ dsedb -r run http.elf [saddr] : Executes the program
        
	saddr is a WWW server address (such as www.scei.co.jp).
	After execution, the index.html file available at 
	http://<saddr>/index.html is obtained and stored in the local
	directory.
        
< Using the Controller >
        None

< Note >
	This program is intended to use USB Ethernet, HDD Ethernet,
	PPPoE(USB Ethernet) or PPPoE(HDD Ethernet) as a connection
	environment.
	To change the connection environment setting, switch the following
	#defines which appear in the beginning of this program.

	#define USB_ETHERNET			: USB Ethernet
	#define USB_ETHERNET_WITH_PPPOE		: PPPoE(USB Ethernet)
	#define HDD_ETHERNET			: HDD Ethernet
	#define HDD_ETHERNET_WITH_PPPOE		: PPPoE(HDD Ethernet)

	To change IP address or use PPP, please make an appropriate change
	to the setting file indicated by the setting name "Combination?"
	(? a numerical value) for each connection environment.
	The followings are the combinations of setting names and setting
	files.

	USB Ethernet	         Combination4         net003.cnf
	PPPoE(USB Ethernet)   	 Combination5         net004.cnf
	HDD Ethernet	         Combination6         net005.cnf
	PPPoE(HDD Ethernet)   	 Combination7         net006.cnf

	In addition, this program refers to the setting file in a plaintext
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
        The program is processed as follows;

	1. Loads modules, initializes the INET.
        2. Connects to the remote port.
        3. Sends the GET message.
        4. Receives data.
