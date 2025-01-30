[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample Program Showing How to Use the inet Library to Trace a Network Route
(tracert Program)

<Description>
	This sample program uses the inet library to trace the route to a target host using the TTL (Time To Live) in the IP packet header (path is used here to mean the routers (gateways) that connect the hosts).
	The intermediate route is traced by checking the ICMP Time Exceeded messages that are returned to the sending host when the TTL is decremented to zero by a router. This sample program was created to provide only the basic functions of the Windows version of tracert, it does not support all of the functions.

[Note]
	To use a USB Ethernet adapter with this sample program, an986.irx is required.
	Since the use of an986.irx is not permitted within a title (for details, see the technote dated 11/22/00 available on the developer support website), an986.irx has not been included in the release package (under sce/iop/modules). Download it from the developer support website .

<Files>

	tracert.c	:  Main program

<Execution Method>

	$ make		:  Compile

	For tracert.irx input/output, open dsicons with TTY0 in another window.

	$ dsicons	:  Open TTY0 input/output

	Execute the program in the original window.

	$ dsidb		:  Start up dsidb
	> source inet.rc.{hdd,hdd.pppoe,usb,usb.pppoe}
			:  Network initialization

	Select the file as the source which is appropriate for the environment being used.

	....
	Wait for a message to be displayed such as:
		 inetctl: (ID=1) [ XXX.XXX.XXX.XXX ]
	(This indicates that an IP address was set by the inet layer)

	> mstart tracert.irx <hostname>
			:  Execute tracert.irx

	<hostname>: Destination host

	tracert.irx input/output is displayed in the window where dsicons was executed.
	To terminate tracert.irx during execution, type Control-C in dsicons.

<Usage>
	tracert.irx has the following options.

	tracert [<option>...] <hostname>
	<option>:
	thpri=<prio> 				:  Set thread priority
	thstack=<stack> 			:  Set thread stack size
	-tty <N> 				:  Specify TTY for tracert.irx input/output (default: TTY0)
	-h <maximum_hops>(default == 30)	:  Maximum number of hops
	-d 					:  Do not perform name resolution when displaying results
	-w <timeout>(default == 5 sec) 		:  Specify timeout period

<Controller Operation>
	None

<Comments>
	USB Ethernet, network adapter, PPPoE (USB Ethernet), or PPPoE (network adapter) are assumed as the connection environment settings in the supplied inet.rc.* files.
	When the IP address or the PPPoE user name are changed, and when PPP is being used, be sure to change the contents of the combination file (netXXX.cnf) that is used to specify the inetctl.irx arguments in each inet.rc.* file.
	In addition, inet.rc references an unencoded text configuration file located in the /usr/local/sce/conf/net directory. Note that this is done because this program is intended as a development sample. An individually encoded configuration file must be used by an official title.
	For information about how to use individually encoded files, refer to the sample configuration application.

	With the current connection environment configuration files, since only one interface is specified for each configuration file, this program further assumes that multiple devices will not be used simultaneously. It will run under the condition that exactly one target device is connected.
	Note that having multiple connections for the same device is also not considered.

<Processing Summary>
	1.  Start up a worker thread.
	2.  Start up threads for sending, receiving, TTY input, and event waiting, respectively.

	Sending thread
		This thread sends an ICMP echo request packet.
		If event waiting is canceled by the receiving thread or by a timeout, this thread increments the TTL by 1 and sends an ICMP echo request packet again.
		The maximum number of retries for each gateway is 3.

	Receiving thread
		This thread repeatedly receives IP packets and if an ICMP time exceeded packet is detected, it displays a message on the TTY. This thread continues receiving IP packets until there is a reply to the ICMP echo request sent by the sending thread, or Control-C is input from the TTY input thread.

	TTY input thread
		This thread waits for input from the TTY and notifies the event waiting thread if a Control-C was input.

	Event waiting thread
		This thread waits for a termination notification from each thread and terminates the program when a termination notification is received.
