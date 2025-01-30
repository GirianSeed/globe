[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2003 Sony Computer Entertainment Inc.
				     All Rights Reserved

Displaying the Amount of Free Space in liberx-Managed Memory

<Overview>
	This sample program displays the amount of free space for liberx.

<Files>
	Makefile
	freemem.c


<Build Method>

	% make

<Execution Method>

	% make run


<Description>
	This sample program uses the sceErxQueryFreeMemSize() function of liberx to display the amount of free space in memory managed by liberx.

	The program uses the sceErxRecordMemConsumption() function, and also calls sceErxQueryFreeMemSticky() so that the amount of free space at the time that the maximum amount of liberx-managed memory was used can also be displayed.

