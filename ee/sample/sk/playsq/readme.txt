[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Playing score data (SQ data/MIDI) with the standard kit/sound system

<Description>
This program uses the standard kit/sound system to play score data. It shows how to play a MIDI sequence within SQ data. It also provides the simplest example of how to use the standard kit/sound system.

The program reads and performs one SQ file (MIDI sequence) and the corresponding BD and HD files (waveform data and its attribute data). SMF2SQ and JAM, which are tools provided by SCEI, can be used to create these files.
	
<Files>
	main.c	
	overload.sq (/usr/local/sce/data/sound/seq)
	overload.hd (/usr/local/sce/data/sound/wave)
	overload.bd (/usr/local/sce/data/sound/wave)

<Run Method>
	% make		:  Compile
	% make run	:  Execute

	A song will be performed when the program runs properly.

<Controller Operation>
	None

<Notes>
	None

