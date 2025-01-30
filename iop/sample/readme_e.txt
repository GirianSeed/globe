[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

                                                        Sample Structure
========================================================================

Sample Directory Structure
------------------------------------------------------------------------
sce/iop/sample/
|--hello
|--kernel
|  |--module
|  +--thread
|--sif
|  |--sifcmd
|  +--sifrpc
+--spu2
    |--autodma
    |--seqplay
    |--stream
    +--voice


Sample Index  
------------------------------------------------------------------------
Sample codes are shown as below.

hello:
	hello		Sample to display "hello !" 
kernel:
	kernel/module   Sample to show how to create a resident library
			module
	kernel/thread	Sample to generate a thread and boot-up, 
			Operate a thread priority, Synchronize threads 
sif:
	sif/sifcmd	SIF CMD protocol sample
	sif/sifrpc	SIF RPC protocol sample
spu2:
	spu2/autodma	Sample to produce the sound by the straight PCM 
			input using the	AutoDMA transfer function of the
			Interim sound library(libspu2)
	spu2/seqplay	Sample to reproduce the MIDI sequence on the IOP
			using the Interim sound library(libspu2, 
			libsnd2)
	spu2/stream	Sample to reproduce the voice stream by the SPU2 
			on the IOP using the Interim sound library
			(libspu2)
	spu2/voice	Sample to produce the SPU2 voice on the IOP using
			the Interim sound library(libspu2)


Preliminary Arrengements for Sample Compiling   
----------------------------------------------
Move to the sce/iop/lib directory and execute the following:

 % make

Compiler path etc. are established.

