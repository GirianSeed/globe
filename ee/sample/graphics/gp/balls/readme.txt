[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2001 Sony Computer Entertainment Inc.
                                       All Rights Reserved


Balls sample program that uses the basic graphics library


<Introduction>
This program uses basic graphics library functions to recreate the balls sample program of the packet library. To improve performance, the ordering chain and drawing packets have been placed in the scratchpad.

<Files>
	balls.c		Main program source

<Run Method>
        % make run

<Controller Operation>
	up/down-arrow (direction keys)	Increase and decrease balls

<Description>
Initial processing:

Texture transfer, texture environment setting, and alpha environment setting packets are used to make initial settings.
Drawing packets are added to chains that were reset and parts that are constant (other than coordinate values) are set.

For each frame:

Ball coordinate values are updated and chains to display the number of balls at the required rate, are kicked.

<Notes>
This program differs from the balls program of the packet library as follows.
- No DMA transfers are performed to the scratchpad for the ball database.
- Transfers are performed using Path3.
- The text display method is different.

If the source file, which includes libgp.h, is compiled using Tool Chain EE 2.9-ee-991111-01 or earlier, the warning "ignoring pragma: }" will be displayed, but it can be ignored.



