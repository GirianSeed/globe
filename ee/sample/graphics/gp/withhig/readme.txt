[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2001 Sony Computer Entertainment Inc.
                                       All Rights Reserved


Sample program that combines HiG and the basic graphics library

<Introduction>
This sample program shows how to use HiG together with the basic graphics library.

<Files>
main.c		Main program source
util.c		Utility function group
camera.c	Camera- and light-related function groups
micro.dsm	Microprogram transfer source
vu1cullVo.vsm	Microprogram

<Run Method>
        % make run

<Controller Operation>
Direction keys				Move subwindow, rotate object
Circle/Triangle/Square/X/R1/R2 buttons	Move object
L1/L2 buttons				Rotate object

<Description>
This sample program shows how to use HiG together with the basic graphics library.

Initial processing:
	
Perform initialization for normal HiG drawing. (hig_init(), camera_init(), and light_init())

Perform initialization related to libgp (subwin_init()), as follows.
Create chain for transferring libgp packets. (subwin_dmachain_init())

Create texture transfer and texture environment setting packets. (subwin_texture_init())
Since this program runs in the HiG environment, the GS memory area of the texture transfer destination is allocated with a HiG GS memory service function.
Create alpha environment setting packets for performing alpha blending. (subwin_alpha_init())
All created packets are registered in the chain.

Create libgp drawing packets. (subwin_primitive_init())
Since screen drawing results from HiG are used, general register setting packets are created for flushing the texture cache. (subwin_ad_init())

For each frame:
	
Update libgp packets during HiG drawing. (subwin_main())
Since all data in this sample program is static data, only controller input processing is performed. However, if a dynamic chain is required, the chain would be reset, the packets overwritten, and the packets and subchains would be registered here.

Draw the ligbp chain after HiG drawing has completed. (subwin_draw())

<Notes>
With the current HiP plugin, using PATH3 to draw other packets and simultaneously performing parallel transfers is difficult to implement. Also, in general, page breaks may cause GS overhead when a parallel transfer of PATH3 is performed with PATH1 or PATH2.
	
This sample program does not perform parallel transfers, but draws by shifting the intervals for HiG and the basic graphics library.

<Related Files>
../data/dino.bin HiG data file

