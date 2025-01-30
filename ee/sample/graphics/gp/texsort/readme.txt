[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2001 Sony Computer Entertainment Inc.
                                       All Rights Reserved


Texture sort sample program


<Introduction>
This sample program uses the basic graphics library to sort and transfer polygons by texture.

<Files>
main.c		Source file
laura.raw	File containing four texture poses of size 128x64
laura-s.raw	File containing four texture poses of size 64x32
laura-t.raw	File containing four texture poses of size 32x16

<Run Method>
        % make run

<Controller Operation>
L1/L2 buttons 		Move camera
up/down direction keys	Rotate camera
SELECT button		Initialize camera position
START button		Pause
Triangle/X buttons	Increase/decrease objects
R1/R2 buttons		Precisely increase/decrease objects
Square button		Output number of objects to console
Circle button		Output number of packets to console

<Description>
This sample program performs a single-pass texture sort of large polygons that use 12 types of textures. It then performs 12 texture transfers, and draws simply by switching the textures 12 times.

It double-buffers packets and chains of the basic graphics library and draws by transferring one buffer while performing calculations and settings using the other buffer.

The textures used by this sample consist of 4 poses (TEXNUM) X 3 levels (TEXLVL) for a total of 12 types of textures. There is a different sized set at each level.
	
Correspondingly, 12 levels of resolution are provided for the chain ordering table.
	
For the texture that each drawing packet (man->sprite) will use, the optimum texture is selected according to the pose at that time and the z-value of the screen. When a packet is registered in the chain, it is registered at the level corresponding to the texture that is used.
	
When all drawing packets have been registered, the relevant texture transfer (textrans) and environment setting (texenv) packets are added to each level.
	
This enables the sample program to avoid complex sorting and to perform high-speed drawing with each texture.

<Notes>
The usable range of 3D processing in this sample program is limited. For information about generic 3D processing, see the sample programs in the gp/zsort,basic3d/ directory.
