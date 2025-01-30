[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
	Copyright (C) 2003 Sony Computer Entertainment Inc.
				       All Rights Reserved

Displaying the EE's SIF Software Register

<Overview>

	This program displays the EE's copy of the SIF software register.

<Files>
	Makefile
	sifsreg.c


<Build Method>

	% make

<Execution Method>

	% make run

<Description>
	This sample program uses the sceSifGetSreg() function of libkernl.a to display the SIF software register.

	There is a SIF software register located on both the EE and the IOP. Each side can update and read the SIF software register on the other side.

	To read the EE's SIF software register, the sceSifGetSreg() function must be called from a program running on the EE. To do this with a conventional monolithic ELF program, debugging code that called the sceSifGetSreg() function had to be incorporated in advance into the body of the ELF program. However, in the ERX module environment provided by liberx, a program containing the sceSifGetSreg() function call can be dynamically linked and run, so debugging code does not need to be incorporated into the program body ahead of time.

        This technique is useful when you want to execute debugging code without having to rebuild the program.

