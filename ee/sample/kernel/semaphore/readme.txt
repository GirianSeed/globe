[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 2000 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program: Semaphores

<Description>

This program shows how to control the operation of threads using the VSync interrupt and semaphores.

<Thread operation>

main (priority 1):
Creates semamain, sema1, sema2, and sema3.
Creates and starts up PrintThread1, PrintThread2, and PrintThread3. Enters READY state.  Subsequently, enables the VSyncStart interrupt and transitions to WAIT state in WaitSema.

PrintThread1 (priority 1):
After the sema1 semaphore is acquired, repeatedly output the thread ID to the dsedb console.

PrintThread2 (priority 1):
After the sema2 semaphore is acquired, repeatedly output the thread ID to the dsedb console.

PrintThread3 (priority 1):
After the sema3 semaphore is acquired, repeatedly output the thread ID to the dsedb console.

<Interrupt>

Each time this interrupt occurs, the sema1, sema2, sema3 semaphores are returned.

<Semaphores>

Corresponding functions
main         -- semamain    Semaphore for terminating main
PrintThread1 -- sema1       Semaphore for activating PrintThread1
PrintThread2 -- sema2       Semaphore for activating PrintThread2
PrintThread3 -- sema3       Semaphore for activating PrintThread3

<Program operation>

After PrintThread1, PrintThread2, and PrintThread3 are terminated normally, main will be terminated normally by lowering the priority of the main thread below that of PrintThread1, PrintThread2, and PrintThread3.

The main thread will enter WAIT state after the VSyncStart interrupt is enabled.

After the main thread has transitioned to WAIT state, the VSyncStart interrupt will be generated. Each time this interrupt occurs, the sema1, sema2, sema3 semaphores are returned.

PrintThread1, PrintThread2, and PrintThread3, which were activated from the main thread, will go to RUN state and will enter WAIT state from the WaitSema function. Each time the VSyncStart interrupt occurs, the sema1, sema2, sema3 semaphores will be returned. Since the priorities of PrintThread1, PrintThread2, and PrintThread3 are the same, when control returns from the interrupt handler, PrintThread1, PrintThread2, and PrintThread3 are registered in the thread ready queue in the order that they entered READY state and are thus executed sequentially.

This sequence of operations will be repeated four times, and PrintThread3, which entered RUN state last, will release the semamain semaphore of the main thread before calling the ExitDeleteThread function. As a result, after PrintThread3 is terminated, control will return to the lower priority main thread and the program will be terminated normally.

<File>

	semaphore.c

<Run method>

        % make:  Compile
        % make run:  Run

        The following method can also be used 
	to run the program after compilation.

        % dsedb
        > run semaphore.elf

<Controller functions>

	None

