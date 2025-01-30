[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
	Copyright (C) 2003 Sony Computer Entertainment Inc.
				       All Rights Reserved

Listing Modules

<Overview>
	This program displays a list of modules currently loaded in memory.

<Files>
	Makefile
	modlist.c

<Build Method>

	% make

<Execution Method>

	% make run

<Description>
	This sample program uses the sceErxGetModuleIdList() and sceErxReferModuleStatus() functions of liberx to display a list of modules currently loaded in memory.

	A similar display can also be obtained by using the dselist utility or by using dsedb's mlist command.

