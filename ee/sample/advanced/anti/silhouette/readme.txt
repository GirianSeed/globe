[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
       Copyright (C) 2001 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Outline anti-aliasing with 2D filter sample program

<Description>
This program shows how to perform outline anti-aliasing on the GS using 2D filtering.

This process is fast since after drawing the objects, it does not need to use information on object shapes to process the full screen. It also rarely uses an extra frame buffer.

<Files>
	main.c		: Main program
	aa.c		: Functions used in anti-aliasing
	mathfunc.c	: Functions related to camera and 
			  matrix operations
	util.c		: Miscellaneous functions
	packet.dsm	: Drawing object
	basic.vsm	: VU1 microprogram for drawing object
	flower.vsm	: Texture file for object
	ki0.tm2,ki1.tm2	: Background texture files 
			  (for odd/even fields)

<Run Method>
	% make		: compile

After compilation, the program can be executed as follows:
	% dsedb
	> run main.elf

<Controller Operation>
	START button		: Switch filter mode
	SELECT button		: Switch between normal 
				  view and test view
	arrow keys, [circle] [x] [triangle] [square], L1, L2, R1, R2
				: Object operations

<Notes>
In test view, the areas affected by filtering (the object's border and background which are its outline) are displayed in red.

<Advantages of This Method>
- This is a post-filter process, and so does not need the drawing object's outline information.

- Fast: Because it is post-filter, it can handle even complex objects with little change in processing time.

- Doesn't intrude on the frame buffer: works within just a 32x32 pixel work buffer. Moreover, because most drawing will have finished when this filtering is executed, the Z-buffer and texture area can be used as work buffers.

- Doesn't blur anything other than the outline: performs shading only on the edges of polygon objects, where aliasing is most likely to appear, leaving backgrounds and textures within objects untouched and sharp.

<Disadvantages of This Method>
- May not be usable together with "destination alpha" object drawing.

<Processing Overview>
- Set background alpha to 0.
- Set the GS FBA register to 1 (aa.c:fba_change()).
- Draw object as usual.
- Filter entire image using sprite-drawing (described below) (aa.c: aa_draw()).

It is also possible to use multiple layers by applying anti-aliasing to the outline of each layer by clearing the alpha value in the frame buffer after filtering, then drawing the next object.

<Filtering Process Overview>
- After drawing completes, set the exterior of the object in the frame buffer to alpha=0, and the interior to alpha >=0x80 (referred to as "original" below).

- (optional) Depending on the rendered object's alpha value, use the alpha test and frame buffer RGB mask to draw sprites with alpha=0x80 and joggle all alpha>0x80 areas to alpha=0x80 (not done in the sample program).

- Use point sampling to copy as is to work.

- Stack objects on top of each other in work, slightly shifted from the original using alpha testing (pass if A<0x80), alpha blending ((Cs-Cd)*As+Cd), and bilinear sampling.

- Go through 4 iterations of this while changing the shift direction. (The filter properties can be changed by changing the direction and amount, number of iterations, etc. See "About the Filter Modes in This Sample" below for details.

- Stack original in work without shifting, without alpha testing, but with alpha blending ((Cs-Cd)*FIX+Cd) and using point sampling.

- Write work back to frame buffer without alpha testing, alpha blending, or point sampling.

<About the Filter Modes in This Sample>
- In this sample, filtering is performed using a center point and four neighboring points. This can be switched between using the four orthogonally adjacent points in additive mode (+) or the four diagonally adjacent points in multiplicative mode (x). The displacement amount is 1 pixel horizontally and 0.5 pixel vertically.

<Application>
VU1 can also be used to generate packets for filtering.

