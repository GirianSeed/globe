[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

VU0 sample programs

<Description>
The VU0 sample programs contain the following examples.

vu0/tballs :  Three-dimensional sphere motion
vu0/tfog   :  Non-directional light source generation and 
	      object clipping
vu0/tmip   :  MiMe linear interpolation and MIPMODEL 
	      processing
vu0/tskin  :  Displaying a single model multiple times 
	      with a matrix
vu0/tsquare:  VU0 macro using the inline assembler
vu0/tdiff  :  Polygon clipping

<Files>
vu0/tballs:	object.c
		object.h
		objfast.c
		objfastA.c
		objnorm.c
		objsimpl.c
		sce_pp3.c
		tballs.c
		util.c
vu0/tdiff:
		object.c
		object.h
		objfast.c
		objfastA.c
		objnorm.c
		objpart.c
		objsimpl.c
		sce_pp3.c
		tdiff.c
		util.c

vu0/tfog:
		object.c
		object.h
		objfast.c
		objfastA.c
		objnorm.c
		objpart.c
		objsimpl.c
		sce_pp3.c
		tfog.c
		util.c

vu0/tmip:
		object.c
		object.h
		objfast.c
		objfastA.c
		objnorm.c
		objpart.c
		objsimpl.c
		sce_pp3.c
		tmip.c
		util.c

vu0/tskin:
		object.c
		object.h
		objfast.c
		objfastA.c
		objnorm.c
		objpart.c
		objsimpl.c
		sce_pp3.c
		tskin.c
		util.c

vu0/tsquare:
		object.c
		object.h
		objfast.c
		objfastA.c
		objnorm.c
		objpart.c
		objsimpl.c
		sce_pp3.c
		tsquare.c
		util.c

<Startup>
	% make :  Compile
	% make run :  Execute

The following method can also be used to execute the program after compilation.
	% dsedb
	> run *.elf

<Controller operation>
	None

<Remarks>
	None

