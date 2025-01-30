[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample that uses TCP to send and receive packets between a client and server using libnet. (server program)

<Description>
This program performs a TCP passive open from the EE using libnet and sends and receives data with a connected client.
The number of simultaneously waiting threads can be specified in an argument for the server.

[Caution]
In this sample program, an986.irx is required when using a USB Ether adapter.

Because the use of an986.irx is not allowed in titles (refer to the tech note on the developer support web site), it is not included with the release package (under sce/iop/modules). Download an986.irx from the developer support web site.
       

<Files>
        main.c        : Main program
        comm.c        : Code of communication portion


<Run method>

        $ make                    : Compile
        $ dsedb			  : Start dsedb
        > run main.elf <th_num>	  : Execute main.elf

	<th_num>:Number of simultaneous connections waiting to be established

Once the server starts, a connection will be established from the client program and communication will start.
A client program that runs on the EE and the IOP should be prepared in advance.

Refer to the following documents.
/usr/local/sce/ee/sample/inet/load_test/client/readme.txt
/usr/local/sce/iop/sample/inet/load_test/client/readme.txt

<Controller operation>
        None

[Notes]
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
4. Perform TCP passive open and wait for connection from client.
5. Once the connection is established, receive the data size for send and receive.
6. Receive data.
7. Send received data to the client.
8. Exit after send and receive are repeated 1500 times.
