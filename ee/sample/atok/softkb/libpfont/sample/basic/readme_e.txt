[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                  All Rights Reserved


Basic Sample Program of Basic Font Library

<Description of the Sample>
        This is the least required sample program to read UTF-8 texts 
        and display the fonts.

<Files>
        Makefile        : Make file
        main.c          : irx initialization, etc.
        sample.c        : Source code of the font drawing program
        sample.h        : Font drawing program header
        sample.pf       : Data for font drawing (up to and including the 
                          second standard Kanji)
        utf8.txt        : Sample texts

<Activating the Program>
        % make          : Compile
        % make run      : Execute

        After compiling, the followings also can be used for execution. 
        However, if a file is specified as an argument, the file will 
        be drawn as texts.

        [Example 1]
        No arguments. Execute the program by reading the text file, 
        'utf8.txt' from the directory as with the 'main.elf.'
        
        % dsedb
        dsedb S> run main.elf

        [Example 2]
        Execute the program by specifying the text file, 'myutf8.txt' 
        located in the directory one layer above of 'main.elf' to the 
        argument.

        % dsedb
        dsedb S> run main.elf host0:../myutf8.txt

<Note>
        % make -e _DEBUG : Compile (debug version)

        If the above is executed at compilation, debugger option; '-g' 
        and '-D _DEBUG' are assigned to generate an application.

        When the compilation option, '-D _DEBUG' is assigned, 
        unregistered characters are checked out, which causes the 
        application to stop.
        The '.../sample/basic/utf8.txt' file contains characters
        that are not registered in the fonts.

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
        4. Initialize fonts.
        5. Set environment for font drawing.
        6. Initialize other drawing environment.

        appMain()
        1. Read texts.
        2. Start a loop.
        3. Synchronize drawing.
        4. Synchronize VSYNC.
        5. Switch the double buffer to the other.
        6. Check the loop completion.
        7. Clear the background image.
        8. Draw texts.
        9. Return to No. 2.

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
