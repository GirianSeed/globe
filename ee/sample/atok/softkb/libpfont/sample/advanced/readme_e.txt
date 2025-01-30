[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                  All Rights Reserved


Basic Font Library Sample Program

<Description of the Sample>
        This is a sample program which displays UTF-8 fonts.
        This program reads the font data and the sample texts (utf8.txt) 
        from the host before drawing.

<Files>
        Makefile        : Make file
        main.c          : irx initialization, etc.
        sample.h        : Main program header 
        sample.c        : Source code of the main program
        sample.pf       : Font data
        utf8.txt        : Sample texts

        xvifpk.h        : Packet construction inline version 
        xvifpk.c        : 
        mfifo.h         : mfifo management 
        mfifo.c         : 
        misc.h          : Others 
        misc_inl.c      : 

        debug.h         : Debug output 
        debug.c         : 
        libpkdbg.h      : Packet dump 
        libpkdbg.c      : 

<Activating the Program>
        % make          :Compile
        % make run      :Execute

       After compiling, the following also can be used for execution.
        % dsedb
        > run main.elf

<Note>
        The large volume of codes for debugging contained in this program 
        enables you to trace easily how packets are constructed.

<Procedure Summary>
        The followings are the procedure summaries for respective files.

        ===== main.c =====
        1. Initialize irx, etc.
        2. Call appInit from sample.c.
        3. Call appMain from sample.c.
        4. Call appExit from sample.c.
        5. Complete.

        ===== sample.c =====
        appInit()
        1. Initialize the work.
        2. Initialize packets.
        3. Read texts.
        4. Read fonts.
        5. Initialize fonts.
        6. Set environment for font drawing.
        7. Initialize other drawing environment.

        appMain()
        1. Start a loop.
        2. Synchronize drawing.
        3. Synchronize VSYNC.
        4. Switch the double buffer to the other.
        5. Check the loop completion.
        6. Clear the background image.
        7. Draw texts.
        8. Return to No. 2.

        appExit()
        1. Free all resources.

-----------------------------------------------------------------------
Copyrights
-----------------------------------------------------------------------
Sample Font Data: (C)2002 Sony Computer Entertainment Inc.
These sample font data ("Data") and any materials created from the Data 
may be used for products of each licensee, 
provided that the Data and the materials created from the Data should be 
used for PlayStation 2 application only. 
Unauthorized modification, assignment, public transmission (including 
making it transmittable), or lending of the Data, is prohibited.  
No reference to this license is required on its products.
 
