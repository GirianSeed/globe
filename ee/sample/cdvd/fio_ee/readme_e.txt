[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Sample Program to Call the CD/DVD-ROM Drive Command Function 
via Standard I/O

< Description of the Sample >
	This program tests the command function of the CD/DVD-ROM drive.

	1. Initializes items related to the CD/DVD-ROM drive.
	2. Calls the command function.
	3. Displays the result of the command function.
	4. Exchanges media.

	The above processes are repeated in numerical order.

< File >
	main.c

< Activating the Program >
	% dsreset 0 0
	% make		: Compiles the program.

	Place a PlayStation CD on the CD/DVD-ROM drive and activate dsedb.
	% dsedb

	Execute the following.
	> run main.elf

	Exchange media when a message requesting a media exchange is 
	displayed.

< Using the Controller >
	None

< Note >
	If this sample program is executed while the hard disk drive 
	is not connected to the DTL-T10000, it fails to load the
	following modules. However, it does not impair the operation 
	of this sample. 
		host0:/usr/local/sce/iop/modules/atad.irx
		host0:/usr/local/sce/iop/modules/hdd.irx
