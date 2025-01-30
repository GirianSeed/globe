[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.             			               All Rights Reserved

Gun Controller Sample

<Description>

This program shows how to obtain the gun controller (SCPH-00034) coordinates and display them on screen.

<Files>
        main.c
        sprt.c
        sprt.h
        pstypes.h

<Execution>

	% make :  Compile

After compilation, start up dsedb and execute the following:

	% dsedb
	> run main.elf
	

<Remarks>
 When using the gun controller via a PlayStation 2 multitap, insert it in slot A.

