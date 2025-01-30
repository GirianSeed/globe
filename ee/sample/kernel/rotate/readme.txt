[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program: Thread ready queue rotation

<Description>

This program can be used to gain an understanding of the operation of the thread ready queue.

<Thread operation>

main (priority 1):
Creates and starts up PrintThread1, PrintThread2, and PrintThread3, puts each of them in READY state, and lowers the priority to 10.

PrintThread1 (priority 1):
Outputs the thread ID to the dsedb console and repeatedly rotates the thread ready queue.

PrintThread2 (priority 1):
Outputs the thread ID to the dsedb console and repeatedly rotates the thread ready queue.

PrintThread3 (priority 1):
Outputs the thread ID to the dsedb console and repeatedly rotates the thread ready queue.

<Program operation>

PrintThread1, PrintThread2, and PrintThread3, which were started up from main, are in READY state. Since their priorities are the same, they are registered in the thread ready queue in the order they were started up.

When the priority of main is lowered to 10, PrintThread1 transitions to RUN state, outputs the thread ID, and rotates the thread ready queue. When PrintThread1 moves to the end of the ready queue, PrintThread2 transitions to RUN state. This is executed up to PrintThread3 and terminates after it is repeated four times. When the last PrintThread3 terminates, control returns to main and the program terminates normally.

<File>

	rotate.c

<Run method>

        % make:  Compile
        % make run:  Run

        The following method can also be used to 
	run the program after compilation.

        % dsedb
        > run rotate.elf

<Controller functions>

	None

