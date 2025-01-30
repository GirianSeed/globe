[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved


PlayStation BB Navigator Network Configuration File Read Library
Sample Program (add_entry)


<Description>
	This sample program reads the contents of a PlayStation BB Navigator Network Configuration file, converts it to a Your Network Configuration file, then saves it onto a PS2 memory card.
	Note that this sample program cannot read a PlayStation BB Navigator Network Configuration file that was created or saved on another PlayStation 2.

<Files>

	Makefile		:  Makefile
	add_entry.c		:  Main program
	load_module.c		:  Load module program
	load_module.h		:  Load module program header
	poweroff.c		:  Program for performing power off processing
	poweroff.h		:  Program header for performing power off processing

<Usage>

	1.  Connect the PlayStation BB Unit to the DTL-T10000(H).

	2.  Use the Your Network Configuration File Check Tool Version 2.0 to create a PlayStation BB Navigator Network Configuration file on the PlayStation BB Unit.

	3.  Insert a PS2 memory card which does not have a Your Network Configuration file into MEMORY CARD Slot 1.

	4.  Run make in the /usr/local/sce/ee/sample/bnnetcnf/add_entry directory.

	   > cd /usr/local/sce/ee/sample/bnnetcnf/add_entry
	   > make

	5.  Start up dsedb.

	   > dsedb

	6.  Execute the following command.

	   dsedb S> reset;run ./add_entry.elf

	7.  The operation will have completed when the following message is displayed.

	   [add_entry] complete


<Operational Overview>

	This sample program operates as shown in the following figure.

	          +-------------+            +-------------+
	          | bnnetcnf.a  | ---------> | netcnfif.a  |
	          +-------------+            +-------------+
	EE          ^                              V
	===============================================
	IOP         ^                              V
                    |                       +--------------+
                    |                       | netcnfif.irx |
                    |                       +--------------+
                    |                              V
                    |                       +------------+
                    |                       | netcnf.irx |
                    |                       +------------+
                    |                              V
	PlayStation BB Navigator    Your Network Configuration file
	Network Configuration file     


<Note>
	Although sceMount() and sceMcInit() are used by add_entry.c to mount devices and initialize the memory card environment, this program does not perform the corresponding processing for unmounting devices or terminating the memory card environment.


