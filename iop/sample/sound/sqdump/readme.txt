[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2003 Sony Computer Entertainment Inc.
                                      All Rights Reserved


SqDump Tool (Sample Program for sdsq Module)

<Description>
    This sample program is a tool that uses the sdsq module to dump an SQ file.

    The program arranges the messages for the MIDI and SONG chunks of an SQ file based on the analysis results from the sdsq module and outputs them to a file.

    The execution parameters of the program are shown below. Start up dsidb, run the program and specify the necessary parameters.
    See the Makefile for a simple command for executing the program with restricted parameters.

    Usage: sqdump.irx -o output_file [OPTIONS] SQfile.sq
      OPTIONS
        -o :  Specifies the output file (required).
        -n :  Controls the Note On/Off display (off if -n is specified).
        -m :  Specifies the MIDI chunk to be dumped.
        -s :  Specifies the SONG chunk to be dumped.
        -D :  Performs debug printing.
        -u :  Displays Usage.

<Files>
    main.c	:  Main program (processes parameters, etc.)
    mainstr.h	:  String definitions for main program
    sqdump.c	:  Dumps an SQ file using the sdsq module
    sqdump.h	:  sqdump definitions
    sqdumpstr.h	:  sqdump string definitions
    myassert.h	:  Assert processing for debugging

    sqdump_log.txt	:  This is the default output file.
                      Its name is specified in the Makefile.
                      The file included with the sample program was produced from the following input file (overlo_s.sq).

    overlo_s.sq		:  This is an SQ file located under  .../data/sound/seq.
                      It is not included in the project.

<Run Method>

    % make run	:  Build and dump../../../../data/sound/seq/overlo_s.sq.

    % make host1:./test/Loop_SQ.sq
                :  Build and dump ./test/Loop_SQ.sq.

    % make -e SQDUMPOP="-D -m 2 -s 0" host1:./test/Loop_SQ.sq
                :  Build and dump MIDI chunk 2 and SONG chunk 0 of ./test/Loop_SQ.sq (with debug printing).

    % make usage
                :  Build and output Usage.

    % make -e OPTION=DEBUG
                :  Build for debugging (enable assertions).


    After the program is built, the following method can be used to run the program.

    % dsidb
    dsidb R> reset 0 2
    dsidb R> mstart ../../../../iop/modules/sdsq.irx

    dsidb R> mstart sqdump.irx -o host1:sqdump_log.txt host1:./test/Loop_SQ.sq

<Controller Operation>
    None

<Notes>
    The assert processing in myassert.h is strictly for debugging.
    Never use assertions in a master.



