[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved


scis_atest:  Sample Program Showing How to Perform Scissoring Using an Alpha Test 

<Description>

	This sample program shows how to perform scissoring using an alpha test.

	This is a technique in which cutting parameters are set up for the polygon vertex alpha values, then cutting is performed using the alpha test function of the GS.

	Since it is not necessary to subdivide polygons when drawing, this technique has the advantage in that the processing cost associated with scissoring is small.

	Differences with this technique compared to the sample programs scsi_vu0 and scsi_vu1 are as follows.
	Advantages
		The drop in performance is smaller for both calculation and drawing
		Programming is simpler
		No distortion or gaps are generated because of polygon subdivision

	Disadvantages
		Cannot be used together with drawing that uses the source alpha for some other purpose
		If a polygon is too big, the cutting plane may be distorted
		The cutting plane shapes are limited
		    Examples of valid cutting planes:  Only one plane or two parallel planes with sufficiently flat surfaces relative to the polygon size
		    Examples of invalid cutting planes:  Multiple planes intersecting at angles such as orthogonal planes (like a rectangular parallelepiped)

	For processing details, see the comments below.

<Files>

	main.c		:  Main program
	scis_atest.vsm	:  VU1 microprogram (cutting using one arbitrary plane)
	scis_atest2.vsm	:  VU1 microprogram (cutting using two planes perpendicular to the x-axis)
	packet.dsm	:  Drawing object
	mathfunc.c	:  Camera and matrix manipulation functions
	util.c		:  Miscellaneous functions
	flower.dsm	:  Object texture file

<Execution Method>

	% make		:  Compile

	After compilation, the sample program can be executed as follows.
	% dsedb
	> run main.elf

<Controller Operation>

	START button		:  Turn scissoring off
	SELECT button		:  Change cutting plane calculation function (LINEAR or QUADRATIC)
	Triangle/X/Square/Circle buttons	:  Rotate object

    LINEAR mode:
        R1/R2 buttons				:  Adjust alpha value inclination angle (r)
	Up/Down/Left/Right (direction keys)	:  Rotate cutting plane (a0,b0,c0)
        L1/L2 buttons				:  Move cutting plane (d)

    QUADRATIC mode:
	Left/Right 		:  Move cutting plane center position (f)
	Up/Down 		:  Adjust alpha value gap (e)
	L1/L2 buttons		:  Adjust alpha base value (g)

	The symbols a to g and a0 to c0 in parentheses correspond to symbols in the formulas described below.

<Comments>
    * Processing details

	In this technique, cutting parameters are set for polygon vertex alpha values by VU1, and cutting is performed by GS alpha test processing.

	Processing details are explained below.

	GS settings are made such that if a texture is present, no texture alpha is used (TCC=0) and drawing is performed by Gouraud shading (IIP=1).

	In addition, alpha testing is enabled with alpha values over 128 being invalid.
	(ATE=1, ATST=LEQUAL, AREF=0x80, AFAIL=KEEP)

	Besides normal perspective transformation, the following processing is also performed by VU1.

	For LINEAR mode (see scis_atest.vsm)

	The following calculations are performed for:

		scissor_param[] (vf28) = (a,b,c,d) 
		Vertex before perspective transformation (localvertex, vf20) = (x,y,z,w)

		tmpA = ax+by+cz+d
		if (tmpA<0) A=0
		else if (tmpA>255) A=255
		else A=tmpA

	The A value obtained in this way is set as the vertex alpha value.

	When the following formula is considered:

		(a,b,c) = r*(a0,b0,c0) (where, (a0,b0,c0) is a unit vector of length 1)

	the meanings of the parameters are as follows.

		(a0,b0,c0):  Cutting plane direction (normal)
		r:  Alpha variation rate
		d:  Cutting plane position (offset)

	For QUADRATIC mode (see scis_atest2.vsm)

	By using the quadratic function, the section enclosed by two planes is extracted.

	The following calculations are performed for:

		scissor_param[] (vf28) = (a,b,c,-) 
		Vertex after perspective transformation (vf25) = (x,y,z,w)


		tmpA = axx+bx+c
		if (tmpA<0) A=0
		else if (tmpA>255) A=255
		else A=tmpA

	The A value obtained in this way is set as the vertex alpha value.
	The part of the quadratic function tmpA = axx+bx+c for which tmpA < 128.0 is drawn.

	To operate the controller, it is easier to deal with the parameters (e,f,g) by converting them to (a,b,c) and passing them to the microcode using the following formula.

	axx+bx+c = e(x+f)^2 + g

	At this time, e, f, and g have the following meanings.
		e:  Quadratic function difference condition
		f:  x-direction offset
		g:  Quadratic function base height
	The distance between the two planes and the alpha variation rate can be set with combinations of e and g.


    *  Setting the alpha variation rate
	 The following effects result from setting the alpha variation rate.

	    * If the variation rate is too large, the alpha at the edge of the polygon to be cut (tmpA) will be outside of the range (0 to 255) causing saturation to occur, and slippage of the cutting plane.

	    * If the variation rate is too small, the alpha resolution will be reduced, and the cutting plane error will become large.

	    * To obtain the ideal variation rate and avoid the two phenomena described above, make the difference between the alphas (tmpA) at both edges of the largest polygon that can be cut as large as possible but within a range not to exceed 127.


    * Note
	When a large polygon is inclined and drawn, a distortion of the perspective transformation may appear in the cutting plane.

