[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved


Common Network Configuration Interface Library Sample Program (convauth)


<Description>
	This sample program shows how to use the sceNetcnfifConvAuthname() function whose use is described in the document, "Usage Conventions for Your Network Configuration Files."


<Files>
	Makefile	:  Makefile
	convauth.c	:  Main program

<Usage>

	1. Run make.

	your-host > make


	2. Start up the program as follows.

	dsedb S> reset;run ./convauth.elf aaa


	3. Program operation is correct if the string that was input is output as-is, as shown below.

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
