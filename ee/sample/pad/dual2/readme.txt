[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample code showing how to obtain DUALSHOCK2 force-feedback information

<Description>

This program uses the controller library to control the DUALSHOCK2 and obtain force-feedback information.

<Files>
        main.c
        sprt.c
        sprt.h
        pstypes.h

<Execution>

        % make          : compile

        Run dsedb after compiling.
        % dsedb
        > run main.elf
        

<Controller operations>

Pressing any button except the SELECT and START buttons will increase or decrease the gauge depending on how the button is pressed.

<Notes>

Make sure that the controller is plugged into slot 1 (port 0).
Multi-tap (SCPH-1070) connections are not supported.
