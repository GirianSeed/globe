[SCEI CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
   Copyright (C) 2001-2002 by Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample program for calling CD/DVD-ROM drive command functions with standard I/O

<Description>
This is a CD/DVD-ROM command function test program. It sequentially carries out the following actions repeatedly.

	1. CD/DVD-ROM-related initialization
	2. Command function calling
	3. Command function result display
	4. Media exchange processing

<File>
	main.c

<Run method>
	% dsreset 0 0
	% make :  Compile

Insert the "PlayStation" CD in the CD/DVD-ROM Drive and start dsedb. Execute the following.
	% dsedb
	> run main.elf
	

When a message is displayed prompting the user to exchange media, please exchange the media.

<Controller operation>
	None

<Remarks>
When executing this program on a DTL-T10000 to which there is no hard disk connected, loading the modules indicated below will fail. However, this will not affect execution of the program.
	host0:/usr/local/sce/iop/modules/atad.irx
	host0:/usr/local/sce/iop/modules/hdd.irx



