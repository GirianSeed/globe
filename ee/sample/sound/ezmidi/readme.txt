[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 2000 Sony Computer Entertainment Inc.
                                        All Rights Reserved

EzMIDI - Playing MIDI tunes and sound effects

<Description>

csl_hsyn, csl_midi, and csl_msin are used to play tunes and sound effects.

This sample program is associated with an IOP sample program, and the two programs work together. The EE program will automatically load the IOP program, so the IOP module (iop/sample/sound/ezmidi) needs to be built beforehand.

libsd can be used to play sound effects, but this sample program uses csl_hsyn. csl_msin is used on the EE side to generate a midi-stream, which is then transferred to the IOP where it is played by csl_hsyn.

The advantages of using this method are that JAM files can be used and that SPU2 voice management does not need to be performed by the EE. By using the maximum number of voices and the highest priority for the two BGM and sound effects midi-stream input ports, it is possible to provide a feature that allows "up to n sounds to be used for sound effects and the remaining (48-n) to 48 voices for playing tunes."

To minimize EE-IOP communications, a midi-stream buffer for sound effects is transferred to the IOP once in one frame only when a new sound effect is requested. The results of this are not noticed by the EE. Voice management is handled by csl_hsyn, and there is no possibility of dropped data in the buffer since the IOP loop operates much faster (240 Hz).

EzBGM handles split loading of BD. Thus, large BDs can be transferred to the SPU2 local memory without an impact on IOP memory.

DMA channel 1 is used to transfer BDs to SPU2 memory. When performing input/output block transfers (sceSdBlockTrans), etc. at the same time, make sure that there are no channel conflicts.

<Files>
	main.c
	midi_rpc.c
	ezmidi_i.h
	sakana.sq (/usr/local/sce/data/sound/seq)
	sakana.hd (/usr/local/sce/data/sound/wave)
	sakana.bd (/usr/local/sce/data/sound/wave)
	eff.hd    (/usr/local/sce/data/sound/wave)
	eff.bd    (/usr/local/sce/data/sound/wave)


<Execution>
	% make		: compile for IOP 
			  (iop/sample/sound/ezmidi/)
	% cd  ???	: change to EE directory 
			  (ee/sample/sound/ezmidi/)
	% make		: compile for EE
	% make run	: run

A tune is played and pressing arrow keys will play sound effects.

<Controller operations>
        x button     	:   Begin playing tune
        square button     	:   Stop playing tune
	up arrow (direction key):   Sound effect (telephone)
	right arrow     	:   Sound effect (comedy, right)
	left arrow      	:   Sound effect (comedy, left)
	down arrow      	:   Sound effect (glass)
        START button  		:   Pause/resume playing tune
	SELECT button 		:   Exit program

<Notes>
MIDI messages are a musical format and have the following disadvantages when used for controlling sound effects.

* Voices are handled with only channel numbers and key numbers, so the same key cannot be issued repeatedly while keying off individual keys.
* Controls such as the pan pot affect the entire channel.

"Extended MIDI messages" provide special extensions to overcome these problems. 
For information about this format, refer to the CSL overview and for information about the API (sceMSIn_PutHsMsg, etc.) refer to the CSL MIDI Stream generation reference.

