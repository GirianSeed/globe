[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2001 Sony Computer Entertainment Inc.
                                      All Rights Reserved

Minimal program for playing a PS2 MPEG2 stream (PSS) with audio

<Description>
ezmpegstr is a sample program for playing a PSS file. The playback method is similar to that of mpegstr, but ezmpegstr uses a simpler architecture.

The biggest differences between ezmpegstr and mpegstr are as follows.

1) ezmpegstr uses only one thread for all playback processing. Sufficient performance is ensured by using timer 0, and no interrupt handler is necessary.

2) The player component group, which does not allocate memory itself, depends on the allocated resource list that is passed to the player core function playPss. The player .bss section has been made as small as possible.

In addition, the core player has no function for reading file data. Instead, a non-blocking function must be provided for obtaining stream data when the player is called.

The sample callback function allocates an IOP memory area ffor buffering.

3) Pause and stop play functions are not supported.

<Files>
	audiodec.c
	audiodec.h
	ezmpegstr.h
	ldimage.c
	main.c
	playpss.c
	playpss.h
	strfile.c
	strfile.h
	sample.pss (/usr/local/sce/data/movie/mpeg)

<Run Method>
	% make		:  Compile
	% make run	:  Execute

When the PSS file resides on the hard disk drive of the host, the following procedure can be used.

	% dsreset 0 0
	% dsedb
	dsedb S> run ezmpegstr.elf 'host0:XXX.pss'

When the PSS file resides on the CD/DVD disk drive of the DTL-T10000, the following procedure can be used.

	% dsreset 0 0
	% dsedb
	dsedb S> run ezmpegstr.elf 'cdrom0:\\XXX.PSS;1'

The default option for the sample program assumes that the "cdrom0:" device is a CD, not a DVD.

<Controller Operation>
	None

<Notes>
(a) Playback in PAL mode is supported by uncommenting out the '#define PAL' line of ezmpegstr.h, then recompiling.

(b) Playback using a DVD instead of a CD as the "cdrom0:" device is supported by commenting out the '#define MEDIA_CD' line of ezmpegstr.h, then recompiling.

(c) The work area width is set to 720 pixels for playing the sample animation. Since the frame buffer width is only 640 pixels, memory and file space can be saved by reducing this work area and using a stream that was encoded with a horizontal resolution of 640.
	
(d) When the video buffer becomes empty, the end of a multiplexed program stream is detected by testing whether or not all receive data was processed by the call to sceDemuxPss.

When a single stream callback that reports a buffer full state causes a deadlock condition, this test returns an incorrect result. This deadlock occurs when the video buffer (demuxBuff, demuxBuffSize) is extremely small or when the audio stream is far ahead of the video stream.

(e) Members of the playPssRsrcs structure are shown below.

mpegBuff:
		libmpeg reference image work area
		The SCE_MPEG_BUFFER_SIZE macro of libmpeg.h can be used to determine the required size.


mpegBuffSize:
	mpegBuff byte size

rgb32:
	The 64-byte aligned buffer size for decoded video macroblocks must be equal to sizeof(sceIpuRGB32)*number of macroblocks.

path3tag:
	64-byte aligned buffer of tags for transferring decoded video macroblocks to GS memory
	The PATH_3_TAG_SIZE macro of ezmpegstr.h can be used to determine the required size.

demuxBuff:
	64-byte aligned buffer for demultiplexed video data
	The ideal buffer size depends on the video stream throughput characteristic.

demuxBuffSize:
	demuxBuf byte size (must be a multiple of 16)

audioBuff:
	64-byte aligned EE memory buffer for demultiplexed audio data

audioBuffSize:
	audioBuff byte size

iopBuff:
	IOP memory buffer for storing demultiplexed audio data

iopBuffSize:
	iopBuff byte size

zeroBuff:
	64-byte EE memory buffer for uploading silent data
	The size must be ZERO_BUFF_SIZE.

iopZeroBuff:
	IOP memory buffer for silent data
	The size must be ZERO_BUFF_SIZE.

