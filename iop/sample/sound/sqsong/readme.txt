[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
Copyright (C) 1999, 2000 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Playing a SONG with the hardware synthesizer

<Description>
This is a sample program that shows how to use the MIDI sequencer (modmidi) and hardware synthesizer (modhsyn).

The program reads a set consisting of an sq file (score data) and hd/bd files (waveform data) and plays the SONG chunks in the sq file. JAM, which is a tool provided by SCE, can be used to create the sq file and the SONG chunks within it.

<Files>
	main.c	
	overlo_s.sq (/usr/local/sce/data/sound/seq)
	overlo_s.hd (/usr/local/sce/data/sound/wave)
	overlo_s.bd (/usr/local/sce/data/sound/wave)

<Run Method>
	% make		:  Compile
	% make run	:  Execute

The program runs properly if the same tune is performed as the "overload" tune performed by sqhard.

<Controller Operation>
	None

<Notes>
	None
