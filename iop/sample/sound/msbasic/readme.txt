[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2002 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Playing back a MIDI Stream with the Hardware Synthesizer

<Description>
This is a MIDI stream generation (modmsin) and hardware synthesizer (modhsyn) sample program.

The program reads a set of hd/bd bank binary files (timbre) and produces sound for the sequentially generated MIDI streams.

libsd.irx, modhsyn.irx, and modmsin.irx must be running on the IOP. Since dsicons can display the load status, if the sample program does not run properly, confirm that these modules were loaded properly.

<Files>
main.c	
eff.hd (/usr/local/sce/data/sound/wave)
eff.bd (/usr/local/sce/data/sound/wave)

<Execution>
% make		: Compile
% make run		: Execute

Processing is normal when sound is sequentially generated for the "comedy," "glass," and "telephone" sound effects.

<Controller Operations>
None

<Remarks>
None

