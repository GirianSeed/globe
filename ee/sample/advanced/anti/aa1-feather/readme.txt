[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
         Copyright (C) 2000 Sony Computer Entertainment Inc.
                                        All Rights Reserved


Sample program: Anti-aliasing (No Z sort)

<Description>
This program demonstrates how to achieve an anti-aliasing effect with AA1.

To show the maximum AA1 effect, the polygons to be drawn must be sorted from the interior of the screen towards the front. However, sorting takes time, and in some cases, this is not a realistic solution.

The program shows that when sorting cannot be performed, a sufficient effect can be obtained in practice by turning off AA1 in relation to "corrupted" polygons for which damage is particularly conspicuous.

There is also the problem that AA1 LINE does not write a value to the Z Buffer. The solution is to turn AA1 off and draw to the Z buffer the first time, then perform a Z test the second time while drawing AA1 LINE. This enables the Z Buffer to be used.

There is also another function implemented as an AA method which performs alpha Blend drawing to the current frame buffer, while applying a bilinear filter to the contents of the frame buffer from the preceding field.

<Files>
	aa1.c:  Main program
	devinit.c:  Device initialization routine
	make_packet.c:  Packet creation routine
	*.h:  Texture data, etc.
	work.dsm:  Object data (DMA packet)
	work.vsm:  VU1 microprogram

<Run method>
	% make:  Compile

	The following method can also be used to run 
	the program after compilation.
	% dsedb
	> run aa1.elf

<Controller functions>
	up, down (direction keys):  Change viewpoint
	Circle button:  Increase number of objects
	Cross button:  Decrease number of objects
	Triangle button:  Decrease light (becomes darker)
	Square button:  Increase light (becomes brighter)

	R1 button:  Switch AA1 ON/OFF
	R2 button:  Switch object between triangle/line

	L1 button:  Perform AA by blending with image 
		    of preceding field
	L2 button:  Perform AA by blending with image 
		    of preceding field (low light)

<Remarks>
The effect of the L1 and L2 buttons is independent of the AA1 function.

Also, to eliminate the side effects of AA1, turn off AA1 for polygons that appear corrupted when observed from the viewpoint and for rear-facing polygons (relative to the viewpoint). The code for this is included in work.vsm.

The calculations are summarized below.

1. Calculate the vertex coordinates in the view coordinate system for VF05.
2. Calculate the polygon normal line in the view coordinate system for VF06.
3. Calculate the inner product VF05*VF06 and save it in VF29x.
4. Substitute the distance from the viewpoint to the vertex by using VF05z and save it in VF08x.
5. Calculate VF29x*8.0f - VF08x.
6. In VF07, prepare a command that turns PRMODE AA1 ON or OFF according to the sign.
7. Turn AA1 ON or OFF in PRMODE each time one vertex is drawn.

When the cosine of the angle between the line of sight vector and surface normal line is 1/8 or less, turn AA1 OFF.



