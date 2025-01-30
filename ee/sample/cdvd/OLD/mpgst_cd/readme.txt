[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 1.6
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Playing PlayStation MPEG2 streams with audio (PSS) - Streaming from CD/DVD drive


<Description>
This is a sample program that shows how to play a PlayStation MPEG2 stream with audio. The program plays back video and audio as shown below.

- Video -

Main Profile at Main Level(MP@ML)
Simple Profile at Main Level(SP@ML)
Maximum size         :720x480
Picture structure    : frame structure 
		       (progressive frames recommended)

- Audio -

Format                  : straight PCM
Sample length           : 16 bits
Endianness              : little endian
Sampling rate        	: 48.0 KHz
Number of channels      : 2
(L/R) interleave size 	: 512

The ps2str stream converter is used to multiplex the video and audio. The stream number of the stream to be played back must be 0 for both video and audio. When multiplexing, please specify the stream number as 0.

In the program, reading and decoding are handled in separate threads. The main thread reads using sceRead() while another thread performs MPEG2 decoding.

Audio is read into the EE along with the video. After demultiplexing, the audio is returned to the IOP. In this sample program, the time stamps of the video/audio are not checked. Thus, if one goes out of sync, there is no feature to restore synchronization. In this program, the start of the video and audio are matched up, then playback begins. vblank determines the video playback speed, and the SPU2 playback speed determines the audio playback speed. While there may be some variation between the two, it is not enough to cause problems during normal playback.

If there is a buffer underflow due to slow reads or decoding, the data decoded up to that point will be repeated. In other words, for video, the last decoded image will stay displayed. For audio, the data remaining in the buffer will be looped. This may lead to the audio and video going out of sync. Also, if the buffer is full, no new data will be accepted so there is no chance of the buffer overflowing.

The function sceMpegGetPicture() uses the SPR (Scratch Pad RAM) internally. Thus, if the SPR is to be used elsewhere, it must be freed when sceMpegGetPicture() is called.


<Files>
	audiodec.c
	audiodec.h
	defs.h
	disp.c
	disp.h
	main.c
	read.c
	readbuf.c
	readbuf.h
	util.c
	vibuf.c
	vibuf.h
	videodec.c
	videodec.h
	vobuf.c
	vobuf.h

<Execution>
	% make		: compile

Insert a CD/DVD-ROM containing an MPG file into the local CD/DVD drive.
Run the program by entering the following commands from the console.
        % dsreset 0 0
        $ dsedb
        dsedb S> run main.elf '\\MPGfilename;1'

<Controller operations>

	circle: play back from beginning
	cross: exit

<Notes>
(a) ps2str ver1.00 can only multiplex video encoded with a constant bit rate (CBR). Video encoded with a variable bit rate (VBR) cannot be correctly multiplexed. VBR features will be provided in future versions.
