[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Sample program to connect to an HTTP server that requires HTTP 
(BASIC/digest) authentication, using libhttp 

< Description of the Sample >
	This sample program connects to an HTTP server that requires 
	authentication, using the HTTP authentication feature of 
	libhttp.
	libhttp supports both the BASIC and digest 
	authentications.  In this sample program, either of the requested 
	authentications is performed.

	This sample program uses netglue_insck.a as a netglue library to 
	implement libhttp on the inet.  To run this sample 
	program on a stack other than the inet, a netglue library specific 
	to the stack is required separately. 


[ Caution ]
	an986.irx will be required if you are using the USB Ether adapter
	in this sample program.
	Since the use of an986.irx in titles is prohibited, an986.irx is
	not included in the release package (under sce/iop/modules).
	For details, please see the technical information area of the
	SCE-NET Web site as of Nov. 22, 2000.  an986.irx can be downloaded
	from "Download" -> "Other common tools" of the SCE-NET Web site.

< File >
	http_auth.c        : Main program

< Activating the Program >
	$ make                          : Links libhttp by compiling 
					  the program
	$ dsedb -r run http_auth.elf <URL> <user> <passwd> : Executes 
							     the program

	If an HTTP connection is established with a specified URL and an 
	HTTP response requiring authentication is received, an HTTP 
	request is made based on the user name "user" and the password 
	"passwd" to establish a reconnection.

< Using the Controller >
        None

< Note >
	This program assumes the connection environment is USB Ethernet,
	HDD Ethernet, PPPoE (USB Ethernet) or PPPoE (HDD Ethernet).
	To change the connection environment, switch the following
	#define macros which appear in the beginning of this program.

	#define USB_ETHERNET			: USB Ethernet
	#define USB_ETHERNET_WITH_PPPOE		: PPPoE (USB Ethernet)
	#define HDD_ETHERNET			: HDD Ethernet
	#define HDD_ETHERNET_WITH_PPPOE		: PPPoE (HDD Ethernet)

	To change IP address or the user name of PPPoE or use PPP, please 
	make an appropriate change to the setting file indicated by 
	the setting name "Combination?" (? must be a number) for each 
	connection environment.
	The following are the combinations of setting names and setting
	files.

	USB Ethernet	         Combination4         net003.cnf
	PPPoE (USB Ethernet)   	 Combination5         net004.cnf
	HDD Ethernet	         Combination6         net005.cnf
	PPPoE (HDD Ethernet)   	 Combination7         net006.cnf

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

	1. Loads various modules to use inet, and initializes inet.
	2. Establishes an HTTP connection with the target URL, and 
	   receives an HTTP response.
	3. Creates authentication data based on the user name "user" and 
	   the password "passwd", establishes an HTTP connection again, 
	   and receives an HTTP response, if an HTTP response "Not 
	   authenticated (401)" has returned.
	4. Displays the results.
