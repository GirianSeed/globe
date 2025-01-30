[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                        All Rights Reserved

EzBGM - Sample program showing streaming playback of background music (BGM) from disk

<Description>

This program performs streaming playback of wav files from disk. The program allows two wav files to be simultaneously and asynchronously played back. The playback of each stream can be controlled through the controller.

In this sample, streaming is performed from the hard disk or from the local CD/DVD disc. Note that when using a DTL-T10000 connected to the network, network latency often prevents smooth playback.

Windows-format, 16-bit, 48 KHz, uncompressed, stereo/mono wav files can be used. The stereo/mono mode is determined from the header information and the playback mode is selected accordingly.

In stereo wavs, L/R interleaving is done at every one-sample. Since 512-byte L/R interleaving is required when using on the SPU2, these wavs cannot be played directly. In this sample program, the IOP is used to rearrange the wav PCM into an SPU2-formatted PCM. IOP overhead is about 2% per file.

This sample program is for the EE, and it works together with an IOP-side program. The EE program will automatically load the IOP code, so make sure the IOP module (iop/sample/sound/ezbgm) has been built before running the program.

<Files>
	main.c	
	bgm_rpc.c
	bgm_i.h
	m_stereo.wav (/usr/local/sce/data/sound/wave)
	ps_mono.wav  (/usr/local/sce/data/sound/wave)

<Execution>
	% make		: compile IOP code 
			 (iop/sample/sound/ezbgm/)
	% cd  ???	: change to EE directory 
			  (ee/sample/sound/ezbgm/)
	% make		: compile EE code
	% make run	: run

As described above, the WAV file is read from the host HD and played back.

When the WAV file is in the local CD/DVD drive, it is run as follows:

	% dsreset 0 0
	$ dsedb
	dsedb S> run main.elf 'cdrom0:\\M_STEREO.WAV;1' 'cdrom0:\\PS_MONO.WAV;1'

	dsedb is run from the command line as follows (with host HD).
	
	dsedb S> run main.elf host0:/usr/local/sce/data/sound/wave/m_stereo.wav 	host0:/usr/local/sce/data/sound/wave/ps_mono.wav
	
<Controller operations>
        circle button    :   start playing first wav file
        triangle button	 :   stop playing first wav file
        X button    	 :   start playing second wav file
        square button    :   stop playing second wav file
        START button     :   pause/resume file being played
	SELECT button    :   exit program

<Notes>
	none

