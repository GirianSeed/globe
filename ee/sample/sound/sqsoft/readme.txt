[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 1999 Sony Computer Entertainment Inc.
                                        All Rights Reserved

MIDI playback with the software synthesizer

<Description>

This sample program demonstrates the operation of the MIDI sequencer (modmidi), the software synthesizer (modssyn), and EE line out (liblout).

This program works together with the corresponding IOP-side sample code. Since the EE side automatically loads the IOP side, build the IOP module (iop/sample/sound/sqsoft) in advance.

The /usr/local/sce/data/sound/ssyn/sce.ssb file provides phoneme data for the software synthesizer. This is still a trial version and additional refinements are planned.

Song data based on GM or GS can be played directly but since "drum parts" aren't implemented, a BankChange(MSB) = 120 setting must be made for the "drum part" channel. For more information, please refer to the phoneme data readme.

The program works as follows.

* The EE program starts and the necessary modules are loaded to the IOP.

* The EE program is initialized.

* The user programs are loaded from the EE to the IOP and started. The EE then enters into a wait state for data from the IOP.

* The IOP loads sq and starts the sequence. When the modmidi output Stream MIDI is input to modssyn, a time code is added, and it is transferred to the EE.  (Data is in original format.)  

* When MIDI data is received, the EE performs operations based on the data and outputs it as Stream-PCM data.

* The PCM stream data is input to liblout and then transferred to the I/O block of the SPU2.  The libsdr feature is used internally.

<Files>
	main.c	

<Execution>
	% make		:Compiles IOP sample 
			 (iop/sample/sound/sqsoft)
	% cd  ???	:Transfers to the EE sample 
			 directory 
			 (ee/sample/sound/sqsoft)
	% make		:Compiles EE sample  
	% make run	:Executes the program

	The program plays a song if it is working correctly.

<Controller operations>
	None

