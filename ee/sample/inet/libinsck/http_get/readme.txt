[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample that gets remote files using the http protocol and the libinsck library

<Description>
This sample program connects to a remote http port and downloads files using the socket library for INET libinsck from the EE. 
	This is the socket library version of ee/sample/inet/http_get.

[Notes]
In this sample program, an986.irx is required when using a USB Ethernet adapter.

Because the use of an986.irx is not allowed in titles (refer to the tech note on the developer support web site), it is not included with the release package (under sce/iop/modules). Download an986.irx from the developer support web site.

<Files>
        http.c        : Main program


<Run method>
        $ make                          : Compile 
        $ dsedb -r run http.elf [saddr]   : Execute
        
        saddr is a WWW server address (www.scei.co.jp etc.).
        When the program is executed, HTTP is used to get the http://<saddr>/index.html file, then it is saved locally.

<Controller operation>
        None

<Note>
The connection environment can be changed by switching the following #define statements at the beginning of the program.

	#define USB_ETHERNET:			USB Ethernet
	#define USB_ETHERNET_WITH_PPPOE:	PPPoE (USB Ethernet)
	#define HDD_ETHERNET:			HDD Ethernet
	#define HDD_ETHERNET_WITH_PPPOE:	PPPoE (HDD Ethernet)

To change the IP address or the PPPoE user name, or to use PPP, change each configuration file indicated by the connection environment configuration name "Combination?" (where ? is a number) as appropriate. The correspondence between configuration name and configuration file for each connection environment is as follows.

	USB Ethernet		Combination4         net003.cnf
	PPPoE (USB Ethernet)	Combination5         net004.cnf
	HDD Ethernet		Combination6         net005.cnf
	PPPoE (HDD Ethernet)	Combination7         net006.cnf


This program references configuration files under /usr/local/sce/conf/net. This is because this program is for use as a development sample.  Please note that for an official title etc., individually encoded configuration files should be used. For information on how to use individually encoded configuration files, please refer to the sample configuration application.

In the current connection environment configuration files, only one interface is specified per file. Hence, in this sample program, it is assumed that multiple devices are not being used simultaneously. In this program, the target device works under the condition that only a single connection has been made. Please note that no considerations have been made regarding multiple connections for the same device.

<Process summary>
A summary of the process is given below.

        1. Load each module and initialize inet.
        2. Connect to remote port.
        3. Send GET message.
        4. Receive data.

