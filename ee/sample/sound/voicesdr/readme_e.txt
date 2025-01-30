[SCE CONFIDENTIAL DOCUMENT]
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                   Copyright (C) 2002 by Sony Computer Entertainment Inc.
                                                      All Rights Reserved

voicesdr - Sample of Generated Sound of Voice (libsdr used)


< Description of the Sample Program >
	This program is a libsdr sample, which produces SPU2 voice 
	sound from EE to IOP. 
	There are options that allow for experiments of batch processing, 
	transfer termination interrupt handler, and SPU2 interrupt handler.
	
	Use AIFF2VAG to create the waveform data, VAG file independently.
	
	In selecting a core for SPU2, note that the core 0 is output 
	through the main volume of the core 1 (i.e. if the main volume 
	of the core 1 is zero, then no sound is output for the core 0).
	
	If BATCH_MODE is set to 1 to compile the data, the codes for 
	batch processing become enabled. This permits multiple processes 
	consist of individual function calls to be operated in one batch 
	processing.
	
	If IRQ_CB_TEST is set to 1 to compile the data, the codes for 
	SPU2 interrupt handler become enabled. Then the message, 
	"interrupt detected" is output to the console when the generated 
	sound processing accesses to the latter 0x1000 bytes 
	(IRQ_ADDR_OFST) of the waveform data.
	
	If DMA_CB_TEST is set to 1 to compile the data, the codes for 
	the transfer termination interrupt handler become enabled. 
	The message, "interrupt detected" will be issued on the console
	when the transfer of VAG file terminates.
	1

< File >
	main.c	


< Activating the Program >
	% make		: Compiles the program
	% make run	: Executes the program

	The operation is normal if one octave (8 scales) is generated 
	on the piano sound.


< Using the Controller >
	None


< Notes >
	This program uses sceSifLoadIopHeap(), which reads the file 
	contents on IOP, and therefore, "host1:" specified on the IOP 
	is used to determine file name on reading file. 
	Also, dsifilesv runs the execution file when make run is issued.
