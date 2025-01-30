[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program showing how to play an MPEG2 stream using the IPU

<Description>

This program shows how to play MPEG2 (including MPEG1) streams using the IPU.  The MPEG2 streams that can be played are

	    Main Profile at Main Level(MP@ML)
	    Simple Profile at Main Level(SP@ML)

and only streams having frame-structure pictures.  Playing of movies with field-structure pictures is not currently supported. Streams with attached MPEG2 system information are not supported, either.

Operation of the program is as follows.  

First, all MPEG2 data is temporarily read into memory.  Then the data read by the function sceMpegAddBs() is sent to the IPU. Next, the function sceMpegGetPicture() is used to decode one picture at a time. Since the decoded data is expanded in main memory, it can be DMA transferred to the GS and displayed on the screen.

A picture represented by a frame structure has even and odd fields interleaved within one frame.  Since the fields are switched every 1/60 second, the picture itself must be switched every 1/30 second.  Therefore, the program is designed so that the picture is switched once every 2 vblanks.

Internally, the function sceMpegGetPicture() uses the Scratch Pad RAM (SPR).  Therefore, if another function wants to use the SPR, it must only be released between invocations of sceMpegGetPicture().

<Files>
        ezmeg.h
        display.c
        ldimage.c
        main.c
	ez.m2v (/usr/local/sce/data/movie/mpeg)

<Execution>
	% make:  Compile
	% make run:  Execute

This program can also be executed by the following method after compilation.
	% dsedb
	> run ezmpg.elf ez.m2v

<Controller operation>
	None

<Notes>
	None
