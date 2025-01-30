[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 2000 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample program: Thread priority

<Description>

This program can be used to gain an understanding of thread priorities.

<Thread operation>

main (priority 1):
Creates and starts up ReadThread and PrintThread, puts each of them in READY state, lowers the priority to 10, and transitions to READY state.

ReadThread (priority 1):
Outputs the thread ID to the dsedb console and continues to read the file.

PrintThread (priority 2):
Continues to output the thread ID to the dsedb console.

<Semaphores>

ReadEndSema -- Semaphore indicating the end of file reading

<Program operation>

ReadThread and PrintThread, which were started up from main, are in READY state.

When the priority of main becomes lower than that of ReadThread and PrintThread, the higher priority ReadThread transitions to RUN state. When ReadThread opens and reads the file, it temporarily enters WAIT state. When ReadThread transitions to WAIT state, the lower priority PrintThread transitions to RUN state.

When ReadThread finishes reading the file, it releases ReadEndSema and terminates PrintThread. After ReadThread is terminated, main terminates.


<File>

	priority.c

<Run method>

        % make:  Compile
        % make run:  Run

        The following method can also be used to 
	run the program after compilation.

        % dsedb
        > run priority.elf

<Controller functions>

	None
