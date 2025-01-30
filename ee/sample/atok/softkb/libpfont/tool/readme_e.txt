[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
                     Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved


Basic Font Library Font Data Creation Tool


<Description>
        This program converts bmp files into gf files that are converted 
        into pb files next, and then integrates the pb files to create 
        pf files.

        bmp     : Bit-map file
        gf      : SCE-NET font data format
        pb      : Font data block for the basic font library
        pf      : Font data for the basic font library


<Files>
        Makefile        : makefile

        bmp2gf.c        : bmp file conversion program source
        bmp2gf.txt      : bmp file conversion program setting sample

        gf2pb.c         : gf file conversion program source
        gf2pb.txt       : gf file conversion program setting sample

        pb2pf.c         : pb file integration program source
        pb2pf.txt       : pb file integration program setting sample

        sample_k.bmp    : Sample data

        ascii.pb        : Latin1 font implemented in PlayStation 2
        jis0201_26.pb   : Half size
        jis0213_0_26.pb : Non-kanji
        jis0213_1_26.pb : The first standard
        jis0213_2_26.pb : The second standard

<Activating the Program>
        % make          : Compile
        % make font     : Create sample data

<Note>
        Refer to the setting sample files for further details regarding
        respective tools.

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
