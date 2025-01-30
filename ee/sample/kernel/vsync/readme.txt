[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program: VSyncStart interrupt handler

<Description>

This program can be used to gain an understanding of the operation of threads and the VSyncStart interrupt handler.

<Thread operation>

main (priority 1):
Creates and starts up ReadThread, synchronizes to the VSyncStart interrupt, and outputs a string to the dsedb console.

ReadThread (priority 2):
Reads a file only while main is waiting for the MainSema semaphore.
When the thread terminates, it releases the ReadEnd semaphore and deletes itself so that main can terminate.

<Interrupt>

Whenever VSyncStart occurs, the MainSema semaphore will be released.

<Semaphores>

main       -- MainSema    Semaphore for putting main 
			  in RUN state during each interrupt
ReadThread -- ReadEndSema Semaphore for signaling the end of 
			  a file read

<Program operation>

The program has two threads called main and ReadThread, and creates a mechanism by which main is started whenever the VSyncStart interrupt handler is active. This requires that the priority of the main thread be set lower than that of ReadThread.

Handler     V          V          V          V
main        |-------   |-------   |-------   |
ReadThread  |       ---|       ---|       -- |

The PollingSema function within main is used to confirm whether or not ReadThread processing has completely terminated.

The following two methods can be used for setting main to WAIT state.

    WaitSema
    SleepThread

Although either can be used to achieve the same result, the semaphore method offers greater flexibility, so it is used in this program.

The main thread is set to RUN state by calling iSignalSema within the VSyncStart interrupt handler.

The main thread is set to WAIT state before the VSyncStart interrupt is enabled. This is done to prevent main from continuing in RUN state without entering WAIT state, when it should properly be in WAIT state. This would happen if iSignalSema were called within the handler when main was in RUN state.

After reading the file, ReadThread will release the ReadEnd semaphore which will terminate main. At this time, if a VSyncStart interrupt is generated before ExitDeleteThread is called, control will return to main and main will terminate. To terminate ReadThread, the priority of main is lowered below that of ReadThread.


<File>

	vsync.c

<Run method>

        % make:  Compile
        % make run:  Run

        The following method can also be used 
	to run the program after compilation.

        % dsedb
        > run vsync.elf

<Controller functions>

	None

