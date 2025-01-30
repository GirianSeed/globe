[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Playing various kinds of score data with the standard kit/sound system

<Description>
This program uses the standard kit/sound system to play various kinds of score data.

The program reads two SQ files (MIDI sequences) and three sets of BD and HD files (waveform data and its attribute data), then plays the MIDI sequences (score data), plays the sound effect sequences, and generates one-shot sounds. SMF2SQ and JAM, which are tools provided by SCE, can be used to create these files.

<Files>
	main.c	
	overload.sq (/usr/local/sce/data/sound/seq)
	overload.hd (/usr/local/sce/data/sound/wave)
	overload.bd (/usr/local/sce/data/sound/wave)
	sesample2.sq (/usr/local/sce/data/sound/seq)
	sesample2.hd (/usr/local/sce/data/sound/wave)
	sesample2.bd (/usr/local/sce/data/sound/wave)

<Run Method>
	% make		:  Compile
	% make run	:  Execute

	The following operations can be performed after make run is executed.

	[Perform MIDI sequence]
	Data related to the MIDI sequence is transferred by pressing the Left-arrow key. Then, the MIDI sequence performance is started by pressing the Right-arrow key. The Down-arrow key can be used to stop the performance. The volume can be changed by using L1/L2, and the tempo can be changed with START + L1/L2.

	[Perform sound effect sequence]
	Data related to the sound effect sequence is transferred by pressing the Square button. Then, the sound effect sequence performance is started by pressing the Circle button. The Circle button can be held down to perform a maximum of 32 sound effect sequences.

	[Generate one-shot sound]
	Data related to one-shot sound generation is transferred by pressing the Up-arrow key. Then, one-shot sound generation is started by pressing the Triangle button. The panpot will move from right to left during the performance. The Triangle button can be pressed continuously to generate a maximum of 48 one-shot sounds in a range permitted by the SPU2's voices. This includes other performances as well.

<Controller Operation>
        Left-arrow (direction key):  Transfer data related to MIDI sequence (one time only)
        Right-arrow 	:  Start MIDI sequence performance
        Down-arrow 	:  Stop MIDI sequence
        Square 		:  Transfer data related to sound effect sequence (one time only)
        Circle 		:  Start sound effect sequence performance
        Up-arrow 	:  Transfer data related to one-shot sound generation (one time only)
        Triangle 	:  Start one-shot sound generation
        L1 		:  Increase MIDI sequence performance volume
        L2 		:  Decrease MIDI sequence performance volume
        L1 + START 	:  Increase MIDI sequence performance tempo
        L2 + START 	:  Decrease MIDI sequence performance tempo
	SELECT 		:  Quit program

<Notes>
For convenience, the program has functions that are not included as sample code such as the stopping of one-shot sound generation.


