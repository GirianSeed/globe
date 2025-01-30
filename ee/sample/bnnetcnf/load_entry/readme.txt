[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved


PlayStation BB Navigator Network Configuration File Read Library
Sample Program (load_entry)


<Description>
	This sample program reads the contents of a PlayStation BB Navigator Network Configuration file, converts it to a Your Network Configuration file, then uses it to make a connection.
	Note that this sample program cannot read a PlayStation BB Navigator Network Configuration file that was created or saved on another PlayStation 2.

<Files>

	Makefile		:  Makefile
	load_entry.c		:  Main program
	load_module.c		:  Load module program
	load_module.h		:  Load module program header
	poweroff.c		:  Program for performing power off processing
	poweroff.h		:  Program header for performing power off processing

<Usage>

	1.  Connect the PlayStation BB Unit to the DTL-T10000(H) and connect a LAN cable to the network adapter.

	2.  Use the Your Network Configuration File Check Tool Version 2.0 to create a PlayStation BB Navigator Network Configuration file on the PlayStation BB Unit.

	3.  Run make in the /usr/local/sce/ee/sample/bnnetcnf/load_entry directory.

	   > cd /usr/local/sce/ee/sample/bnnetcnf/load_entry
	   > make

	4.  Start up dsedb.

	   > dsedb

	5.  Execute the following command.

	   dsedb S> reset;run ./load_entry.elf

	6.  The connection will have been made when the following message is displayed.

	   address: "xxx.xxx.xxx.xxx"
	   [load_entry] complete

<Operational Overview>

	This sample program operates as shown in the following figure.

	      +------------+      +------------+      +----------+
	      | bnnetcnf.a |----->| netcnfif.a |----->| libnet.a |
	      +------------+      +------------+      +----------+
	EE          ^    Transfer      | ^ Get address     | Set up and 
	            |   configuration  V |                 V start connection
	=============================================================
	IOP         ^                  | |                 V
	            |                  | |            +------------+
	            |                  | |            | libnet.irx |
	            |                  | |            +------------+
	            |                  V |                 V
	            |            +--------------+    +===================+
	            |            | netcnfif.irx |--->| INET TCP/IP Stack |
	            |            +--------------+    +===================+
	            |                                      V
	PlayStation BB Navigator                        Connection
	Network Configuration file

<Notes>
	Although sceMount() is used by load_entry.c to mount devices, this program does not perform any processing for unmounting devices.

