[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2001 Sony Computer Entertainment Inc.
                                       All Rights Reserved


Z-sort sample program that uses the basic graphics library

<Introduction>
This sample program performs packet sorting using an ordering table to Z-sort individual drawing packets. This enables semitransparent objects to be drawn properly.

<Files>
      zsort.c	Main program source
      vu0.c	vu0 microprogram source

<Run Method>
        % make run

<Controller Operation>
SELECT button				Turn Z-sort on/off
up/down/left/right direction keys	Rotate object
Circle/Triangle/X/Square buttons	Rotate camera
	
<Description>
This program reads object data, then draws using separate drawing packets for each internally defined data block. It uses the average of the z-values in a drawing packet as the z-value of that packet and determines the position in the Z-sort ordering chain at which to add the packet, which corresponds to the size of that z-value.

Since drawing is performed in order of object data blocks when Z-sorting is turned off, the internal red ball will be different than when Z-sorting is turned on.

The texture data is read as a tim2-formatted file, and is managed and used together with textureArg.

<Notes>
To set values in R-system drawing packets (reglist), vu0.c uses core multimedia instructions.

