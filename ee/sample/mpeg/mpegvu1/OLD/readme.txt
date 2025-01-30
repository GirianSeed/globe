[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 1.6
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Decoding PlayStation MPEG2 streams (PSS) and performing color conversion using VU1

<Description>
mpegvu1 is a sample program showing how to play PlayStation MPEG2 streams (PSS).The program performs color conversion (YCbCr->RGB) using VU1 instead of IPU.

Color difference (CbCr) components in IPU color conversion are treated as four adjacent points having the same value. For progressive frames, the vertical and horizontal directions are sampled at the same time so this usually doesn't cause any problems.

However, when displaying interlaced frames and pictures in the field structure, fields from different times are arranged alternately along the vertical direction. If this kind of color conversion is performed, fine color offsets or stripes will appear in the picture.

mpegvu1 performs high-precision color conversion by using VU1 instead of the IPU. If VU1 is available during MPEG2 decoding, this method will provide sharp decoded MPEG2 images.

mpegvu1 plays video as shown below.

	- Video -

	    Main Profile at Main Level(MP@ML)
	    Simple Profile at Main Level(SP@ML)
	    Maximum size               :720x480

mpegvu1 classifies picture structures into the following three categories, and uses different VU1 microcode to perform color conversion for each category.

	    (1) progressive frame structure
	    (2) interlace frame structure
	    (3) field structure

The category to which a bit stream belongs can be determined by looking at the members of the sceMpeg structure after decoding (see the section where csct is calculated in videodec.c).

mpegvu1 plays back the PSS file on either the host hard disk drive or the DTL-T10000 CD/DVD drive. The device used for playback is specified in the device name at the start of the file name (host0:/cdrom0:).

When pausing or resuming, a maximum of 512 audio samples (0.0107 sec at 48KHz) may be dropped out.

<Files>
	audiodec.c
	audiodec.h
	cscvu1.c
	cscvu1.h
	defs.h
	disp.c
	disp.h
	main.c
	read.c
	readbuf.c
	readbuf.h
	strfile.c
	strfile.h
	util.c
	vibuf.c
	vibuf.h
	videodec.c
	videodec.h
	vobuf.c
	vobuf.h
	vu1.dsm
	yuvfl.dsm
	yuvfl.vsm
	yuvfrfl.dsm
	yuvfrfl0.vsm
	yuvfrfl1.vsm
	yuvprg.dsm
	yuvprg0.vsm
	yuvprg1.vsm

<Execution>
	% make		: compile
	% make run	: run
	% make noaudio  : run (no audio) 
	% make help     : help information display 
			 (the method for specifying the 
			  file name can be confirmed)

When the PSS file is in the host hard disk drive, the following execution can be carried out:

	% dsreset 0 0
	% dsedb
	dsedb S> run main.elf 'host0:XXX.pss'

When the PSS file is in the DTL-T10000 CD/DVD drive, the following execution can be carried out:

	% dsreset 0 0
	% dsedb
	dsedb S> run main.elf 'cdrom0:\\XXX.PSS;1'

<Controller operations>

	circle button	: Play back from the beginning
	X button	: Exit
	square button	: pause/resume

<Notes>

(a) When streaming from a host using the DTL-T10000, playback may be disrupted due to network overhead. When streaming over a network, note the following:
		Connect with 100Base-T
		Do not use a hub

(b) The sample data in /usr/local/sce/data/movie/sample.pss contains some image corruption.
