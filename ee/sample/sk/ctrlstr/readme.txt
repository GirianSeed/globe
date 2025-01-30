[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Playing Back ADPCM Stream Data Using the Standard Kit/Sound System

<Description>
	This sample program plays back ADPCM stream data using the standard kit/sound system.

	The program reads monaural one-shot sound stream data along with a set of stereo stream data and uses the controller to control the playback of the stream.

	The waveform data files that can be used (files with a .vb extension in this sample program) are VAG files without the leading 48 bytes. VAG files are created using AIFF2VAG. AIFF2VAG can be found in the "Sound Artist Tools" area of SCE-NET.
	When a VAG file which is based on this .vb file, is created, looping is required at the beginning and end. In addition, the size of the created .vb file must be an integer multiple of the buffer size that is set up in SPU2 local memory.

	Be sure to allocate at least 0x4000 bytes (16 KB) per channel for the buffer size that is set up in SPU2 local memory. If this sample program is run using a smaller buffer size, it may not run properly when IOP interrupts are masked, for example. Since a larger size may also be required depending on the conditions when interrupts are masked by the IOP, the ultimate buffer size should be determined according to actual usage conditions.

<Files>
	main.c	
	tr1l_pad.vb (/usr/local/sce/data/sound/wave)
	tr1r_pad.vb (/usr/local/sce/data/sound/wave)
	ps_mono.vb  (/usr/local/sce/data/sound/wave)

<Execution Method>
	% make		:  Compile
	% make run	:  Execute

	The following operations can be performed after executing make run.

	[Controlling and playing back monaural streams]

	Pressing the X button preloads the monaural stream.
	If STREAM_AUTOPLAY is set to 1 within the code, playback begins right away. If STREAM_AUTOPLAY is not 1, playback begins when the Square button is pressed. Playback stops when the Down-arrow button is pressed. If the R1 button is pressed during playback, playback will be paused. If the L1 button is pressed during playback, playback will be muted. If the Left-arrow button is pressed during playback, the playback pitch will be changed.

	[Controlling and playing back stereo streams]

	Pressing the Circle button preloads the stereo stream.
	If STREAM_AUTOPLAY is set to 1 within the code, playback begins right away. If STREAM_AUTOPLAY is not 1, playback begins when the Triangle button is pressed. Playback stops when the Right-arrow button is pressed. If the L2 button is pressed during playback, playback will be muted. If the Up-arrow button is pressed during playback, the playback pitch will be changed.

	If a stereo stream were to be paused, the left and right channels might get out of phase. Therefore, pausing is not performed for stereo streams.

<Controller Operation>
        X button			:  Preload monaural stream
        Square button			:  Begin monaural stream playback (STREAM_AUTOPLAY!=1)
        Circle button			:  Preload stereo stream
        Triangle button			:  Begin stereo stream playback (STREAM_AUTOPLAY!=1)
        Down-arrow (direction keys)	:  Stop monaural stream
        Right-arrow 			:  Stop stereo stream
        Left-arrow 			:  Change monaural stream playback pitch
        Up-arrow 			:  Change stereo stream playback pitch
        L1 button			:  Mute/unmute monaural stream
        L2 button			:  Mute/unmute stereo stream
        R1 button			:  Pause/unpause monaural stream
        START button			:  Play/stop entire stream
	SELECT button			:  Terminate program

<Comments>
	If a stereo stream were to be paused, the left and right channels might get out of phase. Therefore, be sure not to perform pause processing for multiple streams that need to have their phases properly aligned, such as a stereo stream.

