[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 2000 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program showing how to control controllers on a multitap

<Description>

This program controls and gets information about controllers using the controller library and the multitap library. Actuator control is compatible with the DUALSHOCK only.

<Files>
        main.c

<Execution>

        % make          : compile

        After compiling, run using dsedb.
        % dsedb
        > run main.elf

<Controller operations>

        (with DUALSHOCK connected)
        up arrow        :       Run small motor
        down arrow      :       Stop small motor
        left arrow      :       Run large motor
        right arrow     :       Stop large motor

<Notes>

Plug the multitap into controller port 1 (port 0) and then plug the controllers into the appropriate controller ports *-A-D.
