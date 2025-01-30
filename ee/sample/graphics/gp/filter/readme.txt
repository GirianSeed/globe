[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2001 Sony Computer Entertainment Inc.
                                       All Rights Reserved


Filtering sample program that uses the basic graphics library


<Introduction>
This sample program uses basic graphics library functions to filter pictures drawn by the GS.
HiG is used for basic drawing.

<Files>
      main.c		Main program source
      util.c		Utility function group
      camera.c		Camera- and light-related function groups
      micro.dsm		Microprogram transfer source
      vu1cullVo.vsm	Microprogram

<Run Method>
      % make run

<Controller Operation>
R1/R2 buttons				Change filter parameters
Circle/Triangle/Square/Cross buttons	Move drawing object
Direction keys				Move filter window

<Description>
This sample program performs extremely simple filtering with a single sprite. However, by using the GS alpha blending and texture mapping functions, it can also be used for advanced filtering.

Initial processing:

Perform initialization for normal HiG drawing. (hig_init(), camera_init(), and light_init()) 

Perform filtering-related initialization (filter_init()) as follows.
Create a chain for transferring filters. (filter_dmachain_init())

Create texture environment packets in order to use rendered screens as textures. (filter_texture_init())

Update CLAMP register settings to obtain mosaic-type filter results.

Create sprite packets for filtering.
(filter_primitive_init())

Since screen drawing results from HiG are used, general register setting packets are created for flushing the texture cache. (filter_ad_init())
	
For each frame:

Prepare filters during HiG drawing. (filter_main())
Since the screen to be filtered is switched every frame because of double buffering, the texture buffer pointer is adjusted in the texture environment packets.
Perform controller input processing. (filter_ctrl())
Change the values within packets when the D cache is flushed, in preparation for the transfer. (SyncDCache())

Draw the filter chain when HiG drawing has completed. (filter_draw())

<Notes>
The flickering horizontal stripes in the mosaic window occur because the original image is interlaced. To reduce this flicker, it is often necessary to use more complex filtering and configure a work area outside of the drawing area. A combination of the basic graphics library with HiG GS service functions can also be used to change the drawing environment so as to draw to the work area. For details, refer to the multiwin sample program.

If a source file, which includes libgp.h, is compiled using Tool Chain EE 2.9-ee-991111-01 or earlier, the warning "ignoring pragma: }" will be displayed, but it can be ignored.

<Related Files>
../data/shuttle.bin HiG data file
../data/curtain.bin HiG data file

