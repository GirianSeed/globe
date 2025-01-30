[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
                  Copyright (C) 2004 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Streaming of waveform data using bypass process of SPU2

<Description of the sample>
	This is a sample for enabling a bypass process of SPU2 to stream
	waveform data in the COREO MEMIN sound data input area of SPU2.

	This sample reads one .inb file (waveform data file interleaved
	in L/R 512 bytes with the data allocated for bypass process),
	enables bypass process of SPU2, and performs streaming process.

	If WAVE_MODE is set to NORMAL_INTERLEAVE, this sample plays
	the normal-interleaved waveform data file (interleaved in L/R
	512 bytes; knot2.int).

	If TRANSFER_LOOP is undefined, transfer process works with
	non-loop operation style.

	If TRANSFER_MODE is set to TRANSFER_HANDLER, the I/O block DMA
	transfer completion handler is enabled.

	[Precautions]
	Sound is output only to optical digital output. Therefore,
	please prepare both of the optical digital output of PlanStation 2
	and an environment such as AV amplifier which can playback the
	optical digital output.

<File>
	main.c	
	knot2.inb (/usr/local/sce/data/sound/wave)
	knot2.int (/usr/local/sce/data/sound/wave)
	knot2.int is the data whose size was modified into the multiple
	of 1024 by adding 0 to knot.int.
	knot2.inb is the converted data based on knot2.int.

<Activating the program>
	% make		:compiles the program
	% make run	:executes the program

	If sound is output through an optical digital output, the program
	operates correctly.

<How to use the controllers>
	None

<Remark>
	1. int2inb.c has been attached as a sample data converter to a
	   straight PCM data for bypass process of PlayStation 2.

	2. With this sample, it is also possible to output bit stream
	   data used in 3D sound for general use to the optical digital
	   output.
	   For the playback of bit stream data, please specify
	   SD_SPDIF_OUT_BITSTREAM instead of SD_SPDIF_OUT_BYPASS to
	   the option for sceSdSetCoreAttr().
	   Please note that a playback of the waveform data used in this
	   sample cannot be done correctly if SD_SPDIF_OUT_BITSTREAM is
	   specified.
