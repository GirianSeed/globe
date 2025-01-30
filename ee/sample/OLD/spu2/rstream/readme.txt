[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 1.6
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample program showing how to perform stream playback with voices

<Description of sample program>

This sample program uses the librspu2 provisional sound library to perform stream playback from the EE using SPU2 voices.

<Files>
	main.c	
	tr1l_pad.vb (/usr/local/sce/data/sound/wave)
	tr1r_pad.vb (/usr/local/sce/data/sound/wave)

	tr1r_pad.vb

<Execution>
	% make		: compile
	% make run	: run

	The sample program can also be run by 
	entering the following after compilation:
	
	% dsedb
	> run main.elf

	The sample program is operating properly 
	if music is played back in stereo.

<Controller operations>
	none

<Notes>
	none
