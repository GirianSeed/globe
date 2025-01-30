[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Sample program (in blocking mode) to connect to an HTTP server using 
libhttp

< Description of the Sample >
	This sample program connects to an HTTP server using libhttp.
	libhttp can select between blocking and non-blocking 
	modes when establishing an HTTP connection.  This sample enables a 
	connection in blocking mode.

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
	http_blk.c        : Main program

< Activating the Program >
	$ make                          : Links libhttp by compiling 
					  the program
	$ dsedb -r run http_blk.elf command <URL> [post-data] : Executes 
								the program
	
	This program establishes an HTTP connection with a specified URL, 
	and displays an HTTP response.
	"command" specifies the following three HTTP methods:
		-get	GET method
		-head	HEAD method
		-post	POST method
	Only the POST method can specify data to be posted as "post-data".
	
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
	2. Uses sceHTTPSetOption() to specify blocking.
	3. Establishes an HTTP connection with the target URL, and 
	   receives an HTTP response.
	4. Displays the HTTP response.

