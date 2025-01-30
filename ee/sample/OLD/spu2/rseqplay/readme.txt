[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 1.6
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program showing how to play back MIDI sequences

<Description of the sample program>

This sample program uses the librspu2 provisional sound library to play back MIDI sequences from the EE.

In the define section of main.c, set

		SEQ_FILENAME to an SEQ file,
		VH_FILENAME  to a vh file, and
		VB_FILENAME  to a vb file.

The file formats for these are completely identical to the current PlayStation so files should be generated using the current tools.

<Files>
	main.c	
	fuga.seq  (/usr/local/sce/data/sound/seq)
	simple.vh (/usr/local/sce/data/sound/wave)
	simple.vb (/usr/local/sce/data/sound/wave)

	simple.vh
	simple.vb

<Execution>
	% make		: compile
	% make run	: run

	The sample program can also be run by 
	entering the following after compilation:

	% dsedb
	> run main.elf

	The sample program is operating properly 
	if fuga is played back as piano sounds.

<Controller operations>
	none

<Notes>
	none
