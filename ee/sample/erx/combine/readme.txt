[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2003 Sony Computer Entertainment Inc.
				     All Rights Reserved

Restructuring a Static Library as an ERX Module

<Overview>

	This sample program shows how to use a tool such as erx-gcc (ld, eelibld, eefixup) to restructure a static library (.a file) as an ERX module (.erx file).

<Files>
	Makefile
	modname.c


<Build Method>

	% make


<Execution Method>

	This sample program is intended only for building.

<Description>
	This sample program shows how to create ipumpeg.erx, which contains a combination of functions from libipu.a and libmpeg.a.

	It also illustrates how libraries that will be used at roughly the same time, such as libipu and libmpeg, can be combined so as to reduce the number of modules that are loaded and thereby reduce the loading time.


