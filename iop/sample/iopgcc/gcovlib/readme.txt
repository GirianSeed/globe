[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
        Copyright (C) 2003 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Coverage Test Support Library and Support Utilities for iop-gcc and gcov

<Description>
If the "-O0 -fprofile-arcs -ftest-coverage" compile-time options are specified with iop-gcc, code will be included in the generated object that counts the execution frequency for each line of program source code, in memory.

libiopgov.a is a support library used for writing this line-by-line execution count data to a file. iopgcovout.irx and iopgcovctl.irx are support utilities.
        
<Files>
        iopgcov.h       Public header
        (libiopgcov.a)  Library created in this directory

        Makefile        Makefile for creating library and utilities
        iop_bb.c        Source code of object registered in libiopgcov.a
        iopgcovout.c    Support module to be made resident in IOP memory
        iopgcovctl.c    Program for controlling the resident support module
        iopgcovout.h    Header file of internal structures required by 
                        iop_bb.c, iopgcovout.c, and iopgcovctl.c
        gcov_io.h       GNU-derived subroutine declaration required by iopgcovout.c
        readme.text     This file

<Compilation>

        % make

<Installation>

        % su
        # make install

<Usage>
For specific operations, see the usage sample in the ../gcovsample directory.

iopgcovout.irx is a resident library that provides support for IOP program modules linked to libiopgov.a.
Make sure it is started up and made resident in IOP memory.

Once started, iopgcovout.irx finds all modules that reside in memory, locates those modules for which the execution frequency is being counted, and places them under its management.

Modules that are loaded after iopgcovout.irx is already running are automatically placed under iopgcovout.irx management by a routine in libiopgov.a.

Note:  While iopgcovout.irx is made resident, it must be possible for the iop to make file accesses to "host1:". To ensure that this happens, either start up dsidb or dsifilesv in advance.

iopgcovctl.irx is a command that enables the functions of the resident library iopgcovout.irx to be called.

By executing the commands shown below, you can write the count data of a module whose execution frequencies are being counted in IOP memory to a file, display a list of modules, and cancel the resident status of iopgcovout.irx.

       * Writing count data
         To write the count data of all modules under iopgcovout.irx management, start up iopgcovctl.irx as follows.

          % dsistart iopgcovctl.irx

To write the count data of a specific module under iopgcovout.irx management, specify the module name as follows.

          % dsistart iopgcovctl.irx <module-name>

Count data is accumulated in the file SOURCENAME.da. When data is written, the counter in memory is reset.  When data is written again, the new count value subsequent to the previous time data was written is added to the count value that was accumulated in the file.

       * Listing modules
To display a list of modules that are under iopgcovout.irx management, issue the following command.

          % dsistart iopgcovctl.irx -list

       * Canceling the resident status of iopgcovout.irx
To cancel the resident status of iopgcovout.irx, issue the following command.

          % dsistart iopgcovctl.irx -stop

<Function Specifications>

===========================================================================
sceIopgcovOutputData                                             [Function]
===========================================================================
                                                Output execution count data
[Syntax]
        #include <iopgcov.h>
        void sceIopgcovOutputData();

[Calling Conditions]
Cannot be called from an interrupt handler
Can be called from a thread
Multithread safe (must be called in an interrupt-enabled state)

[Arguments]
None

[Return Value]
None

[Description]
This function adds the execution count data in memory to the data in the file and clears the execution count data in memory.

===========================================================================
sceIopgcovDoGlobalCtors                                          [Function]
===========================================================================
          Perform initialization so that execution count data can be output
[Syntax]
        #include <iopgcov.h>
        void sceIopgcovDoGlobalCtors(void);

[Calling Conditions]
Can be called from a module's start routine (must be called in an interrrupt-enabled state)

[Arguments]
None

[Return Value]
None

[Description]
This function must be called inside of a module's start routine.

===========================================================================
sceIopgcovDoGlobalDtors                                          [Function]
===========================================================================
                       Output execution count data and terminate processing
[Syntax]
        #include <iopgcov.h>
        void sceIopgcovDoGlobalDtors(void);

[Calling Conditions]
Can be called from a module's stop routine (must be called in an interrrupt-enabled state)

[Arguments]
None

[Return Value]
None

[Description]
This function must be called inside of a module's stop routine.



<Notes>
This directory does not contain sample programs. It contains a library and utilities whose source code has been made publicly available.

