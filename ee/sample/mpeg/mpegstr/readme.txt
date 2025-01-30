[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
        Copyright (C) 2000 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample program: Playing a PlayStation MPEG2 stream with audio (PSS) 

<Description>
-------------

    == Overview ==

Mpegstr is a sample program for playing a PlayStation MPEG2 stream with audio (PSS). Mpegstr plays images and audio that satisfy the following requirements.

    - Images -

Main Profile at Main Level (MP@ML)
Simple Profile at Main Level (SP@ML)
Maximum size:  720x576
Picture structure:  Frame structure (progressive frame is desirable)

    - Audio -

Format:  Straight PCM
Sample length:   16 bits
Endian:  Little endian
Sampling rate:   48.0 KHz
Number of channels:  2
(L/R) interleave size:  512

Mpegstr plays a PSS file that is located on the hard disk drive of the host or on the CD/DVD drive of the DTL-T10000. The base playback device is specified as the device name (host0: or cdrom0:) at the beginning of the filename.

The stream converter ps2str is used to multiplex images and audio. The stream number of a stream to be played by this program must be 0 when images and audio are played together. Specify 0 for the stream number when images and audio are multiplexed.


    == GS memory ==
	
The program first initializes GS memory then sets up the normal double buffer.

(a) clearGsMem function
First, GS memory is initialized.
This is accomplished by establishing the entire memory as one large drawing area, then drawing one sprite.

		 -----------------------
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|	Frame		|
		|	 Buffer		|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		 -----------------------
(b) sceGsSetDefDBuff function
Next, sceGsSetDefDBuff() is used to set up GS memory again as 2 frame buffers and a texture buffer. A Z buffer is not used in this program.

		 -----------------------
		|			|
		|	Frame		|
		|	 Buffer0	|
		|			|
		 -----------------------
		|			|
		|	Frame		|
		|	 Buffer1	|
		|	 		|
		 -----------------------
		|			|
		|	Texture		|
		|	 Buffer		|
		|			|
		|			|
		 -----------------------
		|			|
		|			|
		|			|
		 -----------------------

    == Thread configuration ==

The program consists of three threads.

(a) main thread 
The main thread is the thread in which main() is executed. main thread first performs initialization, creates and starts up the other threads, then performs the following processing.

+ Reads data from the media to readBuf.
+ Demultiplexes the readBuf data and transfers the picture to viBuf of videoDec, and the audio to the buffer within audioDec.
+ Uses DMA to transfer the picture to the IPU.
+ Transfers the audio to the IOP.

(b) video decode thread 
The video decode thread uses the IPU to decode the picture.

+ Uses the IPU to decode the picture.
+ Stores the decoding results in voBuf.

(c) default thread 
The default thread is used for debugging purposes. Although control reaches this thread in a similar manner as it does for the other threads, this thread normally does not do anything.


    == Interrupts ==

This program uses two interrupts on the user side.

(a) vblankHandler
This function is called when vblank starts. The following processing is performed within this function.

- sceGsSetHalfOffset is used to adjust the half pixel offset that occurs between the odd and even fields.
- sceGsSwapDBuff is used to swap double buffers.
- sceDmaSend is used to transfer image data and data required for drawing to the GS.

Since image data is transferred to the GS only once in the time that vblank is generated twice, the same image will be referenced as a texture by the odd and even fields. However, since the double buffer is swapped for every vblank, the operating rate will be 60 frames/s.

(b) handler_endimage
When a DMA transfer via path3 completes, an interrupt is generated and this function is called. Within the main loop, image data is transferred and a DMA transfer via path3 is used within sceGsSwapDBuff. Since these transfers are distinguished from each other by a flag, when the DMA transfer from sceGsSwapDBuff completes, this function does nothing and exits. When the image data transfer completes, the flag is raised and this function performs the following processing.

- Decrement by 1 the counter that indicates the number of images within VoBuff.
- Raise the flag that indicates that frame drawing has ended.

The number of image data entries in VoBuf are managed in this way.


    == General program flow ==

Since this program uses three threads and two interrupt handlers, the program flow is difficult to understand. However, a brief description is presented below. Refer to this together with the section "Data flow and buffer configuration" below.

(a) Perform SIF initialization and load required modules in IOP memory.

(b) Initialize GS memory, then set up the frame buffers and texture buffer.

(c) Allocate the readBuf buffer for storing the data (audio + image) gotten from the disk.
	
(d) Create the audio and image decoders (vidoDec and audioDec).

(e) Allocate the VoBuf buffer for storing the image data after it has been decoded by the IPU.

(f) Create two threads (default and video decode). (For details, see the description presented above.)

(g) Open the file for performing streaming.

(h) Register two interrupt handlers (vblankHandler and handler_endimage). (For details, see the description presented above.)

(i) Transfer data from the disk to readBuf.

(j) Use sceMpegDemuxPssRing to demultiplex the data into audio and image data.

(k) Send the audio data back to the IOP.

(l) Use sceMpegGetPicture to decode the image data.

(m) Create packets for sending the decoded image data and drawing data to the GS.

(n) Raise the flag and set it so that vblankHandler processing is performed.

(o) Start decoding audio data.

(p) Swap the drawing and display buffer for each vblank and transfer packets to the GS.

(q) Repeat steps (i) to (p) until there is no more data.


    == Data flow and buffer configuration ==

The following figure shows the data from the time the data is read from the CD/DVD until it is played.


            audioDec  readBuf   viBuf           voBuf
            +---+     +---+     +---+           +---+
            | A |     | V |---->| V |  Video    |   |
            +---+     +---+     +---+  decode   | V |
            | A |<----| A |     | V |--> IPU -->|   |--> GS
            +---+     +---+     +---+           +---+
              |       | V |     | V |           |   |
              |       +---+     +---+           | V |
              |       | A |     |-------------| |   |
              |       +---+       videoDec      +---+
              |       | V |                    
              |       +---+                                        EE 
              |         ^
            --|---------|-----------------------------------------------
              V         |
            +---+       |                                          IOP
     SPU2<--| A |     CD/DVD/HD
            +---+
            | A |
            +---+

(a) Data that was read from the CD/DVD/HD is first placed in the readBuf buffer in the EE.
(b) The readBuf data is demultiplexed and separated into sound and pictures.
(c) Pictures are sent to the viBuf video input buffer within the videoDec module.
(d) The viBuf data is decoded by the IPU and the decoded pictures are placed in the voBuf video output buffer.
(e) Pictures in voBuf are sent to the GS and synchronized with vblank interrupts, then displayed on the monitor.
(f) Sound that was demultiplexed in (b) is stored in the buffer within the audioDec module.
(g) The sound that was stored in audioDec is transferred to the (SMEM) buffer on the IOP.
(h) The sound in SMEM is transferred by a DMA transfer to the local memory of the SPU2, where it is played.


    == Miscellaneous ==

This sample program does not check video and audio timestamps. Therefore, there is no return operation if the video and audio are not synchronized. In this program, playback starts after the beginning of the video and audio are aligned. The video playback speed is determined by vblank. The audio playback speed is determined by the SPU2 playback speed. Although there may be a slight discrepancy between these two speeds, it is not large enough to cause a problem during normal play.

When pause and resume are executed, a maximum of 512 samples (0.0107 sec at 48 KHz) may be lost.

If a buffer underflow occurs during playback because reading or decoding did not occur in time, the data that had been decoded up to that time is repeatedly played. That is, for video, the picture that was decoded last is displayed directly. For audio, the data remaining in the buffer is repeatedly played. At this time, the audio and video may not be synchronized. Also, since no new data is taken in as long as the buffer is full, a buffer overflow will not occur.

The function sceMpegGetPicture() uses the Scratch Pad RAM (SPR). Therefore, if the SPR is to be used by another source, the SPR must be released during execution of sceMpegGetPicture().

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
	strfile.c
	strfile.h
	util.c
	vibuf.c
	vibuf.h
	videodec.c
	videodec.h
	vobuf.c
	vobuf.h

<Run methods>

% make:  Compile
% make run:  Run (with audio)
% make noaudio:  Run (without audio)
% make help:  Display help information (to confirm how to specify the filename)


For example, if the PSS file is on the hard disk drive of the host, the following would be executed.

% dsreset 0 0
% dsedb
dsedb S> run main.elf 'host0:XXX.pss'

Or, if the PSS file were on the CD/DVD drive of the DTL-T10000, the following would be executed.

% dsreset 0 0
% dsedb
dsedb S> run main.elf 'cdrom0:\\XXX.PSS;1'


<Controller functions>

	Circle button:  Play from the beginning
	X button:  End
	Square button:  Pause or resume

<Remarks>

(a) When the DTL-T10000 is used to perform streaming from the host side, playback may be distorted due to the network load. Note the following points when streaming via the network.

	Use a 100Base-T connection
	Do not pass through a hub

(b) Part of the image is distorted in the sample data /usr/local/sce/data/movie/mpeg/sample.pss.



