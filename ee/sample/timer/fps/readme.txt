[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2003 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Using libtimer to Measure frames/sec in Each Screen Mode

<Description>
	This sample program uses the libtimer library to measure frequency from the length of VSYNC and HSYNC in each screen mode.

	The program allocates a libtimer virtual counter and uses polling to measure the length of 100 VSYNCs. It also measures the length of 100 HSYNCs using SetAlarm(). In each case, the calculated value of the frequency is displayed in Hz or KHz.

	Measurements are performed for a total of five different screen modes, namely NTSC (interlaced, non-interlaced, and progressive) and PAL (interlaced and non-interlaced).

<Files>
	main.c
	Makefile
	Makefile.erx

<Run Method>
	% make		:  Compile
	% make run	:  Run

<Controller Operation>
	None

<Notes>
	Polling is used to detect VSYNCs and SetAlarm() is used to detect HSYNCs. Both of these methods produce a margin of error in the result.

	Errors can occur in libtimer for the following reasons.

	- CPU interrupt processing is delayed because a bus master other than the MIPS core, such as DMA, uses the bus
	- Timer interrupt processing is delayed because an interrupt gets processed with a higher priority than the interrupt used by libtimer

	Since there isn't any way to prevent these kinds of errors, be aware that they may also occur in an application. This must be taken into consideration when programming.


	Note that in both NTSC interlaced and progressive modes, the VSYNC frequency is approximately 59.94 Hz. However, in interlaced mode, the HSYNC frequency is approximately 15.75 KHz, whereas it is 2X that value in progressive mode, or approximately 31.5 KHz. This means that a timer that uses HSYNC as its clock source such as the SetAlarm() function of libkernl.a, will count approximately two times faster in progressive mode than in interlaced mode. Consequently, when timeouts are implemented with the SetAlarm() function, the screen mode must be carefully considered.

