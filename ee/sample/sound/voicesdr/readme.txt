[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
   Copyright (C) 2002 by Sony Computer Entertainment Inc.
                                      All Rights Reserved

voicesdr - Voice generation sample program (using libsdr)

<Description>
	This is a libsdr sample program. It performs SPU2 voice generation from the EE via the IOP.
	Batch processing, end-of-transfer interrupt handler, and SPU2 interrupt handler tests are available through options.

	To create an original VAG file, which contains waveform data, use AIFF2VAG.

	The SPU2 must select the core carefully. Note that core 0 output passes through the main volume of core 1 (in other words, when the core 1 main volume is zero, no sound will be produced by core 0).

	When BATCH_MODE is set to 1 for compilation, batch processing code will be enabled.
	This allows multiple processes that had been performed by independent function calls up to that point to be collectively performed in a batch.

	When IRQ_CB_TEST is set to 1 for compilation, the code that sets up the SPU2 interrupt handler will be enabled.
	This causes an "interrupt detected" message to be output to the console when the address that is 0x1000 bytes (IRQ_ADDR_OFST) after the beginning of the waveform data is accessed by sound generation processing.

	When DMA_CB_TEST is set to 1 for compilation, the code that sets up the end-of-transfer interrupt handler will be enabled.
	This causes an "interrupt detected" message to be output to the console when the VAG file transfer ends.


<Files>
	main.c	

<Execution Method>
	% make		:  Compile
	% make run	:  Execute

	Operation is normal when a one-octave 8-tone scale piano sound is produced.

<Controller Operation>
	None

<Comments>
	Since this sample program uses the sceSifLoadIopHeap() function for reading the contents of files on the IOP, "host1:", which is the IOP-side specification, must be used to specify the filename when reading the file.
	In addition, when make run is invoked, the executable file will be executed by dsifilesv.


