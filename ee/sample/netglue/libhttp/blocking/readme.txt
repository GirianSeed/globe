[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2001 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program that uses libhttp to connect to an HTTP server (blocking version).

<Description>
This is a sample program that uses libhttp or libhttps to connect to an HTTP server.
libhttp enables blocking or non-blocking to be selected when making an HTTP connection. This sample program makes the connection using blocking.

To run this sample program using a stack other than inet and libeenet,
a netglue library for that stack must be provided separately. 

[Note]
To use a USB Ethernet adapter with this sample program, an986.irx (inet) or ent_eth.irx (libeenet) is required.
Since the use of these is not permitted within a title (for details, see the technote dated 11/22/00 available on the developer support website), they have not been included in the release package (under sce/iop/modules). Download them from the developer support website.

<File>
        http_blk.c		: Main program

<Run Method>
	$ cd libinsck
 	or
	$ cd libeenet
	$ make		: Compile 

	$ dsedb -r run http_blk.elf command <URL> [post-data]: Execute

An HTTP connection is made to the specified URL and the HTTP response is displayed.

command specifies the HTTP method to be used. The following three commands can be specified.
		-get		GET method
		-head		HEAD method
		-post		POST method
For the POST method, data to be POSTed can be specified with the post-data parameter.	

<Controller Operation>
	None

<Remarks>
This program assumes that the connection environment is USB Ethernet, HDD Ethernet, PPPoE (USB Ethernet), or PPPoE (HDD Ethernet).
The connection environment can be changed by switching the following #define statements at the beginning of the program.

	#define USB_ETHERNET:			USB Ethernet
	#define USB_ETHERNET_WITH_PPPOE:	PPPoE (USB Ethernet)
	#define HDD_ETHERNET:			HDD Ethernet
	#define HDD_ETHERNET_WITH_PPPOE:	PPPoE (HDD Ethernet)

To change the IP address or to change the PPPoE user name when PPP is used, change the configuration file indicated by the connection environment configuration name "Combination?" (where ? is a number). The correspondence between configuration name and configuration file for each connection environment is shown below.

	USB Ethernet		Combination4         net003.cnf
	PPPoE (USB Ethernet)	Combination5         net004.cnf
	HDD Ethernet		Combination6         net005.cnf
	PPPoE (HDD Ethernet)	Combination7         net006.cnf

The configuration file used by this program is an ordinary text file, located in /usr/local/sce/conf/net. This is because this program is a sample for use in development. Note that official titles must use individually encoded configuration files.
For information about how to use individually encoded files, refer to the sample configuration application.

With the current connection environment configuration files, only one interface is specified per configuration file. Therefore, this program assumes that only one device will be used at any one time.
This program runs under the condition that exactly one target device is connected.
Note that multiple connections from the same device are also not considered.

<Processing Summary>
1. Initialize the network library and load various modules.
2. Use sceHTTPSetOption() to specify blocking.
3. Make an HTTP connection to the target URL and obtain an HTTP response.
4. Display the HTTP response.

