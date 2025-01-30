[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved


High-speed transfer of textures using the PSMCT32 format

<Description>

When using textures in the GS, textures and CLUTs are transferred from main memory to the frame buffer, where they are used.

To speed up the texture rendering performed by the GS, bit conversions are performed during transfer from main memory to the GS, and the data is stored in the GS frame buffer using this bit format. The bit conversions are performed according to certain rules. 

Thus, texture transfers using the PSMT4, PSMT8, and PSMCT16 bit patterns will be slower than the PSMCT32 bit pattern even when the data sizes (not resolution) are the same. (see gsvpu.ppt: "Texture transfers").

When performing operations such as replacing textures to be rendered from the frame buffer, performance for 4/8-bit textures can be improved by converting them beforehand to bit patterns that can be transferred using PSMCT32 and then accessing this data as PSMT4/8.

This sample program provides the following:

- An off-line bit conversion sample program for converting PSMT4/8 textures to a format that can be transferred to the GS as PSMCT32.
- Comparison of performance between transferring as PSMT4/8 and performing PSMCT32 conversion before transferring.

For details about the bit patterns in GS local memory, please refer to the "Details on GS Local Memory" document in the FAQ's, etc. area of the developer support web site.

<Files>

      bitconv/
bitconv is a sample off-line program that performs bit conversion to allow PSMT4/8 raw-image to be transferred as PSMCT32. Under linux, PSMT4, PSMT8 raw-image data is converted into a format that allows transfer with PSMCT32.

        bitconv <number of bits for source image (4/8)> 
		<width of input image> <height of input 
		image> <input image> <output image>

<Limitations>
- Both the width and height of textures must be powers of two.
- Textures smaller than the texture page size are sometimes not converted properly.

      4bit/
A sample program for transferring 4-bit 256x256 textures as 32-bit 128x64 textures.

#define __32BIT__ is set to transfer textures converted to 32 bits.
The transfer size is 32768 bytes, and this is transferred 100 times and the H-count is displayed.

When transferring to TBP=6720,
        PSMT4   : 221
        PSMCT32 : 98
As can be seen, PSMCT32 provides faster transfers.
For PSMT4, setting up TBP to values other than on a page boundary (multiples of 16), causes performance to be reduced significantly.

      8bit/
A sample program for transferring 8-bit 256x256 textures as 32-bit 128x128 textures.

#define __32BIT__ is set to transfer textures converted to 32 bits.
The transfer size is 65536 bytes, and this is transferred 100 times and the H-count is displayed.

When transferring to TBP=6720,
        PSMT8   : 205
        PSMCT32 : 178
As can be seen, PSMCT32 provides faster transfers.
For PSMT8, setting up TBP to values other than on a page boundary (multiples of 16), causes performance to be reduced significantly.

<Execution>
	% make		: compile
	% make run	: run
	Alternatively, the following method can be used:
	% dsedb
	> run main.elf

<Controller operations>
	None

