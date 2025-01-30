[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

echo server sample using libnet library

<Description>
This sample program is an echo server that uses libnet to perform TCP passive opens from the EE.  The program receives data from a connected client and sends the data back asis to the client.
The number of waiting threads started simultaneously can be specified in an argument.

[Precautions]
In this sample an986.irx is required when using a USB Ethernet adapter.

Because the use of an986.irx is not allowed in titles(refer to the tech note on the developer support web site), it is not included with the release package (under sce/iop/modules). Download an986.irx from the developer support web site.

<Files>
        main.c        : Main program
        comm.c        : Communication code


<Run method>

        $ make                     : Compile
        $ dsedb			   : Start dsedb
        > run main.elf <th_num>    : Execute main.elf
		<th_num>: number of waiting threads started simultaneously.

Once echo_server starts, a connection can be established from a telnet program on Linux, etc.

        % telnet <saddr> 7        : <saddr>, server address
        Trying 192.168.8.1...
        Connected to 192.168.8.1.
        Escape character is '^]'.
        hello
        hello
        bye
        bye

As described above, characters sent from a client are returned from the server as is. 


<Controller operation>
None

<Notes>
The PlayStation 2 version of this echo server program generally reserves TCP port 7.

For a UNIX machine, it has the same behavior as:
	% telnet <unix_ip_addr> 7.

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


This program references configuration files under /usr/local/sce/conf/net. This is because this program is for use as a development sample.  Please note that for an official title etc., individually encoded configuration files should be used. For information on how to use individually encoded configuration files, please refer to the sample configuration application.

In the current connection environment configuration files, only one interface is specified per file. Hence, in this sample program, it is assumed that multiple devices are not being used simultaneously. In this program, the target device works under the condition that only a single connection has been made. Please note that no considerations have been made regarding multiple connections for the same device.

<Process summary>
A summary of the process is given below.

1. Start work thread.
2. Perform initialization of communication.
3. Start th_num waiting threads. Waiting threads are newly created each time a connection is established. 
For each waiting thread,
4. Execute a passive open for TCP port 7 and wait for a connection from the client.
5. Once a connection is established, wait for data from the client. 
6. When data is received from the client, the received data is sent back to the client as is.
7. Exit when the connection from the client is terminated.
