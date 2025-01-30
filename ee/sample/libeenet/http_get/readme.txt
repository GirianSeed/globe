[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2001 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program that uses the libinsck library to get a remote file using the http protocol

<Description>
This sample program uses the INET socket library libinsck from the EE to connect to a remote http port and download a file.
This program is the socket library version of ee/sample/inet/http_get.

[Notes]
To use a USB Ethernet adapter with this sample program, an986.irx is required.
Since the use of an986.irx is not permitted within a title (see the tech notes area of the developer support website), an986.irx has not been included in the release package (under sce/iop/modules). Download it from the developer support website.
	

<File>
        http.c:			Main program


<Run Method>
        $ make:					Compile
        $ dsedb -r run http.elf [saddr]:	Execute

        saddr is a WWW server address (such as www.scei.co.jp).
        When the program is executed, HTTP is used to get the file http://<saddr>/index.html and save it locally.

<Controller Operation>
	None

<Remarks>
This sample program assumes that the connection environment is USB Ethernet, HDD Ethernet, PPPoE (USB Ethernet), or PPPoE (HDD Ethernet).
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

In addition, the program accesses a common text configuration file located under /usr/local/sce/conf/net. Note that this is done because this program is used as a development sample and an individually encoded configuration file must be used by an official title.
For information about how to use individually encoded files, refer to the sample configuration application.

Since only one interface is specified per configuration file in the current connection environment configuration files, the program also assumes that multiple devices will not be used simultaneously.
This program runs under the condition that exactly one target device is connected.
Note that multiple connections to the same device are also not considered.

<Processing Summary>
1. Load each type of module and perform inet initialization.
2. Connect to the remote port.
3. Send a GET message.
4. Receive data.

