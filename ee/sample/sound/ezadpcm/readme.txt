[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 2001 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Playing back BGM by streaming ADPCM data from EzADPCM disk

<Description>

.VB (ADPCM) files are played back by streaming from a disk. Stereo BGM is created by simultaneously playing back two .VB files.

In this program, streaming is performed from the host's hard disk or from the local CD/DVD disk. When using a DTL-T10000 connected to a network, network delays will sometimes prevent smooth playback.

The waveform data files (in this sample, files with a .vb extension) that can be used have the leading 48 bytes of the VAG files removed. VAG files are created with AIFF2VAG.

When the original VAG file for this .vb file is created, the head and tail of the file must form a loop. Also, the size of the created .vb file has to be an integral multiple of the buffer size that is set in SPU2 local memory.

A minimum of 0x4000 bytes (16 KB) should be allocated per channel for the size of the buffer that is set in SPU2 local memory. If the operation is performed with a buffer size smaller than this, then it may not work properly when IOP interrupts have been masked.

Furthermore, depending upon conditions when IOP interrupts are masked, a buffer size larger than this may be required. Hence, the final size of the buffer should be determined in accordance with actual usage conditions.

This program is for the IOP-side and it operates together with an EE-side sample program. Because the EE-side program automatically loads the IOP side, performa Make in advance for the IOP-side module (iop/sample/sound/ezadpcm).

<Files>
	main.c	
	rpc.c
	ezadpcm.h

<Execution>
	% make		: IOP-side compilation 
			  (iop/sample/sound/ezadpcm/)
	% cd ???	: Change to EE-side directory
			  (ee/sample/sound/ezadpcm/)
	% make		: EE-side compilation
	% make run	: Execution

After make run, press the START button to read the .VB file (sampling's /usr/local/sce/data/sound/wave/tr1l(r)_pad.vb) from the host's hard disk and play back the ADPCM data.

For example, if a .VB file is in the local CD/DVD drive, execution is as follows:

	% dsreset 0 0
	% dsedb
	dsedb S> run main.elf 'cdrom0:\\TR1L_PAD.VB;1' 'cdrom0:\\TR1R_PAD.VB;1'

On the dsedb command line, execution is as follows (for a host-side hard disk):

	dsedb S> run main.elf 				host0:/usr/local/sce/data/sound/wave/tr1l_pad.vb		host0:/usr/local/sce/data/sound/wave/tr1r_pad.vb

	Be sure to specify two .VB (ADPCM) files in the argument.

<Controller Operation>
        Up direction key  : Increases the volume.
        Down direction key: Lowers the volume.
        START button      : Starts (resumes) or stops file 
			    being played.
	SELECT button     : Exits the program.

<Remarks>
	None

