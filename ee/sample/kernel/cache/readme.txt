[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved


Sample program of EE core cache control

<Notes>
This sample program was created from information about the EE that is not publicly available. This information is expected to be made publicly available on the developer support website in the near future.

<Description>
The instructions that are used to control the cache have restrictions and will not work properly unless they are used in accordance with those restrictions.
This sample program shows concretely how to use those instructions so that no problems will occur.

The following functions are provided by the program.
- Locking of a data cache line
- Determining whether or not a hit occurred in the data cache
- Cancelling the lock of the data cache line and writing data back to memory

Before using these cache control instructions, the DI function of the EE kernel API should be used to disable interrupts.
This is necessary because the cache instruction and the accompanying sync.l or sync.p instruction must be executed as a set.

<File>
	cache.c:		Main program

<Run Method>
	% make:  Compile

The following method can also be used to run the program after it is compiled.
	% dsedb
	> run cache..elf

<Controller Operation>
	None


