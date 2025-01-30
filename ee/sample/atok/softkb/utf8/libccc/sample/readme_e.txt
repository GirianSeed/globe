[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library libccc Version 1.1
                  Copyright (C) 2003 Sony Computer Entertainment Inc.
                                                     All Rights Reserved


The character code conversion sample to Shift JIS and UTF8

<Description of the Sample>
        Using the libccc library, this sample converts a text file written in 
        character code Shift JIS into character code UTF8 and output it to 
        a file, and then converts the file converted into character code UTF8 
        into character code Shift JIS and output it to a file. 

<Files>
        Makefile        : Make file
        main.c          : Sample souce
        test.txt        : Sample text (Sift JIS)

<Activating the Program>
        % make          : Compile
        % make run      : Execute

        After compiling, the followings also can be used for execution. 
        % dsedb
        dsedb S> run main.elf

        This sample achieves success if test.utf8.txt (UTF8) and 
        test.sjis.txt (Shift JIS) are output after test.txt is read.

<Using the Controller>
        None

<Note>
        None

