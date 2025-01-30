[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
        Copyright (C) 2003 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Coverage Test Sample for iop-gcc

<Description>
If the "-O0 -fprofile-arcs -ftest-coverage" compile-time options are specified with iop-gcc, code will be included in the generated object that counts the execution frequency for each line of program source code, in memory.

These directories include a sample program showing how to use this function, a support library, and support utilities.

<Directories>
   gcovlib/
Contains a library to be linked to the program that will be the target of the coverage test, a support resident library, and support utilities.
   covsample/
Contains a sample showing how to code, compile and execute a coverage test target program.

<gcov Command>
The gcov command is normally included in the GNU C compiler package.
Since the gcov command is a post-processing program for analyzing coverage data, it basically does not depend on the compiler target and is compatible with programs included in any target-oriented gcc.

Although the gcov command was not included in the gcc binary package for the iop that was released from SCEI as of February 2003, since Linux normally comes with the gcov command installed as a standard component, please use that version of gcov.

If, by chance, the gcov command has not been installed as a standard component, the gcov command that comes with the gcc package for the ee should be used instead.

We plan to include the gcov command in the package when the new gcc for the iop is released.

<Note>
The libraries and sample programs in these directories are dependent on iop-gcc.

They cannot be used with a C compiler for the IOP from another company.



