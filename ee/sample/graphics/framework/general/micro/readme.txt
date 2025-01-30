[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Sample script showing how to create *.vsm from *.vcl using the VCL (VU Command Line).

<Description>
A sample script to create *.vsm from *.vcl is shown below.

vertex_color:
	If loop optimization is selected, vcl will create a very long prologue and epilogue for a spotlight sample. Therefore, as a default, only linear optimization code is generated. In order to create faster code, delete the -L option for the vu1spotV.vcl sample in the script and delete the NOP that is not required in the prologue and epilogue as well.
			
lambert:
	vcl creates correct loop code, but it can take an extremely long time.


phong:
	vcl loop optimization fails in the current vcl version. Therefore, only linear optimization code is created.

<Run method>

	% ./vertex_color: Creates a vertex_color shading microcode sample.
	% ./lambert	: Creates a lambert shading microcode sample.
	% ./phong	: Creates a phong shading microcode sample.

<Controller operation>
	None

<Notes>
	None

