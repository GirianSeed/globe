[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 1.6
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program showing how to generate voices

<Description of sample program>

This sample program uses the librspu2 provisional sound library to generate SPU2 voices from the EE. Options allow experimentation with DMA callbacks and IRQ callbacks.

To change tone data, specify a VAG file in VAG_FILENAME and a file size in VAG_SIZE in the define lines of the main.c. The format of the VAG file is fully identical to the one used in the current PlayStation, so files can be created using the current tools.

The usage of the API is similar to the current PlayStation but core selection must be performed with SPU2. Note that output from core 0 passes through the main volume control of core 1 (i.e., if the main volume setting of core 1 is zero, core 0 sounds will not be heard).

If IRQ_CB_TEST is set to 1, IRQ callbacks are enabled. When address 0x1000 from the VAG header (IRQ_ADDR_OFST) is accessed, an "IRQ interrupt detected" message will be output to the console.

If DMA_CB_TEST is set to 1, DMA transfer termination callbacks are enabled. When transfer of a VAG file is completed, a "DMA interrupt detected" message will be output to the console.

<File>
	main.c	
	piano.vag (/usr/local/sce/data/sound/wave)

<Execution>
	% make		: compile
	% make run	: run

	The sample program can also be run by 
	entering the following after compilation:

	% dsedb
	> run main.elf

	If the sample program is operating properly, 
	a one-octave, eight-note musical scale will 
	be played as a piano.

<Controller operations>
	none

<Notes>
	none
