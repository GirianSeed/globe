[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 2000 Sony Computer Entertainment Inc.
                                        All Rights Reserved


Sample program: Getting system configuration information for the PlayStation 2

<Description>

This program shows how to get "PlayStation 2" system configuration information and output it to the console. The date and time formats from the system configuration are used to display the date and time.

The DTL-T10000 has no internal system configuration. Therefore, a configuration file (T10000.scf) is provided and the function sceScfT10kSetConfig is used to set the system configuration. This is valid only on the DTL-T10000.

The format of the T10000.scf configuration file is shown below.

#DTL-T10000 System Configuration File T10000.scf
#
[SPDIF]         ON              # ON OFF
[ASPECT]        FULL            # 4:3 FULL 16:9
[LANGUAGE]      English         # Japanese English French
				  Spanish German Italian
 				  Dutch Portuguese
[TIMEZONE]      0               # from -720 to 780
[SUMMERTIME]    OFF             # ON OFF
[TIMENOTATION]  24HOUR          # 24HOUR 12HOUR
[DATENOTATION]  YYYY/MM/DD      # YYYY/MM/DD MM/DD/YYYY DD/MM/YYYY

The string after a # is a comment.
Entries enclosed in brackets [] are configuration items, and settings are written after leaving one or more spaces following the configuration items.
The contents that can be set for each configuration item are written in the comment on each line.

If this file cannot be opened, the DTL-T10000 default settings, which are described in the system configuration library overview (libscf.doc), are obtained instead.


<Constant definitions>
Define PAL, when PAL is used for output to a TV screen.

<Files>
     T10000.scf
     scf.c

<Run method>

     	% make:  Compile
     	% make run:  Run
	
     	The following method can also be used to 
	run the program after compilation.

     	% dsedb
     	> run scf.elf

<Controller functions>

     None

