[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
        Copyright (C) 2002 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Connecting to an HTTP Server and Aborting a Transfer using libhttp.

<Description>
This sample program uses libhttp to connect to an HTTP server, then aborts the connection or transfer five seconds after the connection is made. libhttp allows either blocking or non-blocking mode to be selected when the HTTP connection is made, however, this sample program makes the connection using non-blocking mode.

To run this program using a stack other than the one provided by inet and libeenet, the netglue library for that stack must be provided separately.

[Note]
To use a USB Ethernet adapter with this sample program, an986.irx (inet) or ent_eth.irx (libeenet) is required.
Since the use of these is not permitted within a title (for details, see the technote dated 11/22/00 available on the developer support website), they have not been included in the release package (under sce/iop/modules). Download them from the developer support website.

<Files>
	http_abort.c		:  Main program

<Execution Method>

        $ cd libinsck
		or
        $ cd libeenet

	$ make			:  Compile

	$ dsedb -r run http_abort.elf command <URL> [post-data/file]
				:  Execute

An HTTP connection will be made to the specified URL, and after five seconds, the connection or transfer will be aborted.

The command parameter specifies the HTTP method. The following three methods are supported.
		-get		GET method
		-head		HEAD method
		-post		POST method

For POST, the data to be posted can be specified with the post-data parameter. For GET, the name of the file for receiving the body part of the reply can also be specified.

<Controller Operation>
        None

<Comments>
This program assumes that the connection environment is USB Ethernet, network adapter PPPoE (USB Ethernet), or PPPoE (HDD Ethernet).
The connection environment can be changed by switching the following #define statements at the beginning of the program.

	#define USB_ETHERNET:				USB Ethernet
	#define USB_ETHERNET_WITH_PPPOE:		PPPoE (USB Ethernet)
	#define HDD_ETHERNET:				Network adapter
	#define HDD_ETHERNET_WITH_PPPOE:		PPPoE (Network adapter)

To change the IP address, to change the PPPoE user name, or to use PPP, change the configuration file indicated by the connection environment configuration name "Combination?" (where ? is a number) as appropriate. The correspondence between configuration name and configuration file for each connection environment is shown below.

	USB Ethernet		Combination4         net003.cnf
	PPPoE (USB Ethernet)	Combination5         net004.cnf
	Network adapter		Combination6         net005.cnf
	PPPoE (Network adapter)	Combination7         net006.cnf

In addition, the program accesses an unencoded text configuration file located under /usr/local/sce/conf/net. Note that this is done because this program is used as a development sample and an individually encoded configuration file must be used by an official title.
For information about how to use individually encoded files, refer to the sample configuration application.

Since only one interface is specified per configuration file in the current connection environment configuration files, the program also assumes that multiple devices will not be used simultaneously.
This program runs under the condition that exactly one target device is connected.
Note that multiple connections to the same device are also not considered.

<Processing Summary>
	1. Initialize the network library and load various modules.
	2. Use sceHTTPSetOption() to specify non-blocking mode.
	3. Make an HTTP connection to the target URL and get an HTTP reply.
	4. Five seconds after the connection is made, abort the connection or transfer.
