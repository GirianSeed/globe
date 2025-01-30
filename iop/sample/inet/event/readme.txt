[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved


Inet Event Sample Program


<Description>

This sample program gets events from network connections or from the insertion/removal of hardware and displays these events on-screen.


<Files>

	Makefile	:  Makefile
	dump.c		:  Configuration contents dump program
	dump.h		:  Configuration contents dump program headers
	event.c		:  Main program
	get_event.c	:  Event management program
	get_event.h	:  Event management program headers
	if_id.c		:  Interface ID management program
	if_id.h		:  Interface ID management program headers
	up_down.c	:  Connection/disconnection program
	up_down.h	:  Connection/disconnection program headers


<Usage>

1. First, execute make in the directory /usr/local/sce/iop/sample/inet/event to create event.irx.

	   > cd /usr/local/sce/iop/sample/inet/event
	   > make

2. Execute make in the directory /usr/local/sce/ee/sample/inet/event.

	   > cd /usr/local/sce/ee/sample/inet/event
	   > make

3. Start up dsedb.

	   > dsedb

4. Execute the following.

	   dsedb R> reset;run ./event.elf

5. After startup, the following events are displayed on-screen each time they occur.

(When the START button is pressed, a connection begins, and when the START button is pressed again, the connection is released.)

	   Attach	:  Hardware was attached
	   Detach	:  Hardware was detached
	   Start	:  Interface was brought up and became available
	   Stop		:  Interface was taken down and became unavailable
	   Error	:  Interface reported an error (error other than Busy_Retry, Timeout, Authfailed, or Busy)
	   Up		:  Request was made to bring up an interface that can be brought up
	   Down		:  Request was made to take down an interface that can be taken down
	   Retry	:  Retry processing is in progress
	   Busy_Retry	:  Preparing to dial the next telephone number because the destination is busy
	   Timeout	:  Disconnected due to a timeout
	   Authfailed	:  Authorization failed
	   Busy		:  Busy (*)
	   NoCarrier	:  Destination's hardware did not respond, or the line is not connected (*)

(*)  Events marked by an asterisk are hardware dependent


<Precautions>

This program uses the following network configuration files when attempting to make a connection. As a result, to properly connect to the network, these network configuration files must be properly set in advance.

1. If #define MODEM is enabled in event.c

	     /usr/local/sce/conf/net/net001.cnf

2. If #define MODEM is disabled in event.c

	     /usr/local/sce/conf/net/net002.cnf

In addition, this program uses the USB autoloader to load the hardware device driver. As a result, to properly load the driver, the following USB autoloader configuration file must be set in advance.

	   /usr/local/sce/conf/usb/usbdrvho.cnf

