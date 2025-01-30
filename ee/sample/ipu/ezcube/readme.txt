[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program showing how to paste a texture that has been decoded using the IPU onto a three-dimensional polygon

<Description of sample program>

This sample program pastes sample data in the ipu/ezmovie directory as a texture onto the sample data in pkt/cube.

For a description of the specific operations, please refer to the cube sample program. For a description of the animation, please refer to the ezmovie sample program. In this sample program, data decoded by the IPU is transferred via DMA to the texture buffer and then displayed as a texture.
		
<Files>
	main.c
	ldimage.c
	ezcube.c
	ezcube.h
	1_6.ipu (/usr/local/sce/data/movie/ipu)
	ezmovie.h

<Execution>
	% make		: compile
	% make run	: run

	The sample program can also be run by performing the following operations after compilation.
	% dsedb
	> run main.elf

<Controller operations>
	up-arrow/down-arrow (direction keys)	: rotate around x-axis
	left/right buttons	: rotate around y-axis
	L1/L2 buttons		: rotate around z-axis
	R1/R2 buttons		: move along z-axis of cube
	triangle/X buttons	: rotate camera around x-axis
	circle/square buttons	: rotate camera around y-axis
	START button		: reset

<Notes>
For the decoding process, which is applied to data encoded by changing the quantization matrix, it is necessary to issue the SETIQ command as shown in the following program after resetting the IPU.

	------ Configure matrix for intrablock

        // Put IQ table in IPU_in_FIFO
        {
         volatile u_char iqtable[64];
         for(i = 0;i < 64;i++){
          iqtable[i] = 32;
         }

            //send IQ table to IPU
            FlushCache(WRITEBACK_DCACHE);
            {
                DPUT_D4_MADR((volatile u_int)iqtable);
                DPUT_D4_QWC((volatile int)4);
                DPUT_D4_CHCR((volatile int)1<<8);
            }
            while((DGET_D4_CHCR()>>8) == 1);

            sceIpuSETIQ(SCE_IPU_SETIQ_INTRA, 0);
            sceIpuSync(0, 0);

            sceIpuBCLR(0);                      // reset IPU_in_FIFO
            sceIpuSync(0, 0);
        }

	------ Configure matrix for non-intrablock

        // Put IQ table in IPU_in_FIFO
        {
         volatile u_char iqtable[64];
         for(i = 0;i < 64;i++){
          iqtable[i] = 16;
         }

            //send IQ table to IPU
            FlushCache(WRITEBACK_DCACHE);
            {
                DPUT_D4_MADR((volatile u_int)iqtable);
                DPUT_D4_QWC((volatile int)4);
                DPUT_D4_CHCR((volatile int)1<<8);
            }
            while((DGET_D4_CHCR()>>8) == 1);

            sceIpuSETIQ(SCE_IPU_SETIQ_NONINTRA, 0);
            sceIpuSync(0, 0);

            sceIpuBCLR(0);                      // reset IPU_in_FIFO
            sceIpuSync(0, 0);
        }




