[SCE CONFIDENTIAL DOCUMENT]
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                  All Rights Reserved


Common Network Configuration Interface LibrarySample Program (convauth)


< Description of the Sample Program >

	This program describes how to use sceNetcnfifConvAuthname() 
	function specified in the provision of Your Network 
	Configuration File.

	
< Files >

	Makefile		: Make file
	convauth.c		: Main program


<Using the Program >

	1. Execute make.

	   your-host > make

	2. Activate the program as follows.

	   dsedb S> reset;run ./convauth.elf aaa

	3. The execution is successful if the string is output exactly 
	   as it was input as shown below.
	   
 	EE DECI2 Manager version 0.06 Feb 19 2002 10:08:18
	CPUID=2e14, BoardID=4126, ROMGEN=2002-0319, 32M

	Loading program (address=0x00100000 size=0x00008da4) ...
	Loading program (address=0x00108e00 size=0x00002550) ...
	Loading 593 symbols ...
	Entry address = 0x00100008
	GP value      = 0x00113370

	"aaa" -> "aaa"
	*** End of Program
	*** retval=0x00000000
