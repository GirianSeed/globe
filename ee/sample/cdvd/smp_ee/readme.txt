[SCEI CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
     Copyright (C) 1999-2002 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample program showing how to call CD/DVD-ROM drive command functions

<Description>

This is a test program for CD/DVD-ROM command functions.

The following steps are repeated in order by the program:

	1. Initialize CD/DVD-ROM related items
	2. Call command function
	3. Display results from command function
	4. Media exchange 

<Files>
	main.c

<Execution>
	% dsreset 0 0
	% make		: compile

Mount a PlayStation CD in the CD/DVD-ROM drive and run dsedb
	% dsedb

In the window with the EE sample program, run
	> run main.elf

When a message is displayed prompting the user to exchange media, please exchange the media.

<Controller operations>
	None

<Notes>
	The following macros are defined in main.c.

	#define MEDIA_CD	Specify the read media as a CD disc
	#define MEDIA_DVD_DUAL	Specify the read media as a DVD Dual Layer disc

	If neither of the above macro definitions is specified, the default specification is DVD Single Layer disc.

