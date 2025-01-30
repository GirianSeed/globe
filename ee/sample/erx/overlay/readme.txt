[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
                Copyright (C) 2003 Sony Computer Entertainment Inc.
                                                   All Rights Reserved

Program Sample to Switch from the Overlay Environment to the ERX
Environment

<Description of the Sample>
        This sample program describes the building procedure required for
        switching from the overlay environment to the ERX environment.

<Files>
        Makefile.erx            Makefile for the ERX environment
        Makefile.ovl            Makefile for the overlay environment
        main.c
        over.cmd                Linker script for the overlay environment
        overlay.tbl
        baseelf.tbl
        balls/Makefile
        balls/balltex.s
        balls/balls.c
        ezmpeg/Makefile
        ezmpeg/display.c
        ezmpeg/ezmpeg.h
        ezmpeg/ldimage.c
        ezmpeg/ezmpeg.c
        iga/Makefile
        iga/devinit.c
        iga/tex.h
        iga/work.dsm
        iga/work.vsm
        iga/iga.c

<Building the Program>
        % make -f Makefile.erx  Building in the ERX environment
         or
        % make -f Makefile.ovl  Building in the overlay environment

<Executing the Program>
        % make -f Makefile.erx run
         or
        % make -f Makefile.ovl run

<Option Setting during Building>
        -DAUTO    : Auto-execution mode
        -DCDROM   : Read data from the CDROM

<Notes>
        This sample program describes the procedure for switching from the
        overlay environment to the ERX environment.  Although the source
        program is the same, there are many points at which the switch can
        be made using the compilation macro USE_ERX or USE_OVERLAY and 
        processing unique to each are separated.

<Using the Controller>
        None. Processing will be performed automatically.

