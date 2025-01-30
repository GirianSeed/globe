[SCEI CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
    Copyright (C) 2001-2002 Sony Computer Entertainment Inc.
	                                 All Rights Reserved


Sample program that combines the basic graphics library with HiG GS service functions

<Introduction>
This sample program uses HiG GS service functions to control the drawing environment (create multiwindows), and uses the basic graphics library to draw.
In addition, it uses subchains to draw the same packet multiple times.

<Files>
      main.c	Main program source
      util.c	Utility function source

<Run Method>
        % make run

<Controller Operation>
Circle/Triangle/Square/X buttons	Switch drawing object
Direction keys				Move green window

<Description>
Initial processing:
	
Create the drawing environment with the HiG GS service function group. Create four subwindows.

Separately create two drawing subchains (red triangle and white line strip) using the basic graphics library.

For each frame:

The subwindow settings are registered at an even level of the main chain ordering table. (add_to_gppacket()) 

One subchain is registered in the main chain using sceGpAddChain().

The other subchain is registered in three locations using sceGpCallChain() so that it is drawn using the remaining three windows.

A subchain is registered at an odd level so that the subwindow settings that were registered at the preceding level will become valid.

The main chain is drawn with sceGpKickChain().

Although the main chain is reset and recreated for each frame, the subchains and drawing packets are not changed. Therefore, the chains that were initially created are used asis.

