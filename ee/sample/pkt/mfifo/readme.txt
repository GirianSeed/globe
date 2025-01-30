[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

MFIFO sample program

<Description>

This program uses MFIFO to draw objects. DMA transfer is used for SPR->Memory and Memory->GIF. The SPR is used for double buffering and Memory is used for the RingBuffer. The .cmd file is edited to reserve a section for the MFIFO area.

<Files>
	mfifo.cmd

<Execution>

	% make		: compile
	% make run	: run

The program can also be run in the following manner after compilation.
	% dsedb
	> run mfifo.elf

<Controller operations>

	circle/X buttons		: increase/reduce 
					  object count
	triangle/square buttons	: increase/reduce 
					  object size
	R1/R2 buttons			: change spacing 
					  of objects
	left arrow/right arrow (direction keys): displace object 
					  position along 
					  X-axis
	up arrow/down arrow buttons	: displace object 
					  position along 
					  Y-axis
	L1/L2 buttons			: displace object 
					  position along 
					  Z-axis
	start button			: pause
	SELECT+circle, X button		: initialize 
					  object count
	SELECT+R1 button		: set object 
					  spacing to 0
	SELECT+R2 button		: initialize 
					  object spacing
	SELECT+left arrow/right arrow buttons: initialize 
						object 
						position 
						along 
						X-axis
	SELECT+up arrow/down arrow buttons: initialize 
					     object 
					     position along 
					     Y-axis
	SELECT+L1/L2 buttons		: initialize 
				          object position 
					  along Z-axis
	SELECT+START buttons		: initialize 
					  object position 
					  (X, Y and Z axis)

<Notes>
	* MFIFO issues

- The size of the ring buffer (MFIFO_SIZE) must be 2^n qwords. D_RBSR is set to MFIFO_SIZE-0x10. In practice, MFIFO_SIZE is set to sizes such as 0x00200000 or 0x00008000, and D_RBSR is set to 0x001FFFF0 or 0x00007FF0.

- The ring buffer area should be aligned to MFIFO_SIZE. This can be achieved by:

(A) specifying the alignment with a global variable when reserving the area
(B) using the user-accessible area provided by the kernel
(C) editing the .cmd file to reserve a section for the buffer area

  Examples of these methods are given below:

(A)	char base[SIZE] __attribute__ ((aligned(SIZE)));
	
Using aligned, SIZE is restricted to a maximum of 32 KB (0x800). If the ring buffer is 32 KB or larger, the methods described below will be need to be used.

(B)	char* base = (char*)0x00100000;
	
A pointer is specified for the user accessible area (0x00100000 - 0x00200000). The user must manage this area.

(C)	.mfifo		ALIGN(0x40000): { *(.mfifo) }
	
A section is added to the .cmd file, and an area is reserved using the following statement:
		char base[0x40000] __attribute__ ((section(".mfifo")));

- The value (D_RBOR & D_RBSR) must always be 0. This will necessarily be true if the conditions above are met. In practive, when MFIFO_SIZE is 0x00040000, D_RBOR will be 0x00240000 and D_RBSR will be 0x0003FFF0.

- The STR bit of the drain channel will be set after D_RBOR, D_RBSR, and D_CTRL.MFD are set, and before source channel (DMA8) transmission.

- The STR bit of the drain channel will be turned off when ENDtag is sent.

- MFIFO registers are expressed in bytes except for QWC, which is expressed in qwords.

- To determine if the source channel can perform transfers to MFIFO, the size of the writeable area (remain) is calculated using
	remain = (tadr==madr) ? MFIFO_SIZE
		: (tadr+MFIFO_SIZE-madr)&(MFIFO_SIZE-0x10);
  The source channel can perform the transfer if remain is larger than the size of the source channel data.

  * Issues relating to the use of GIF path (path3) for the drain channel

- Since sceGsSwapDBuff switches DoubleBuffer through the GIF, D_CTRL.MFD must be restored to 0 (to terminate MFIFO). Before using MFIFO again, the contents of D8_MADR, D2_TADR, D2_CHCR, etc. must be reset. If going through VIF1 (path2), MFIFO can be left active.

  * Other issues

- Since sceGsSwapDBuff switches DoubleBuffer through the GIF, if the sceGsDBuff variable is cached, the cache must be written back using a function like SyncDCache, after the value is changed.
