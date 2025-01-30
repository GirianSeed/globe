[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Sample program showing how to decode multiple sets of compressed data

<Description>

This program is an extended version of the sample program in ipu/ezcube inside the sample directory.

Multiple sets of compressed data are decoded one by one by the IPU, DMA-transferred to the texture buffer, and displayed as textures.

By using the DMA termination interrupt from the IPU, once a set of data has been decoded, the next set can be decoded immediately, thus providing efficient use of the IPU.
	
<Files>
	main.c
	ldimage.c
	ezcube.c
	ezcube.h
	ezmovie.h
	ez.ipu    (/usr/local/sce/data/movie/ipu)
	rugby.ipu (/usr/local/sce/data/movie/ipu)
	456.ipu   (/usr/local/sce/data/movie/ipu)
	1_6.ipu   (/usr/local/sce/data/movie/ipu)
	wada.ipu  (/usr/local/sce/data/movie/ipu)
	yama.ipu  (/usr/local/sce/data/movie/ipu)

	yama.ipu

<Execution>
	% make		: compile
	% make run	: run

The program can also be run using the following method after compilation.
	% dsedb
	> run main.elf

<Controller operations>
	up-arrow/down-arrow (direction keys)	: rotate around X-axis
	left/right buttons	: rotate around Y-axis
	L1/L2 buttons		: rotate around Z-axis
	R1/R2 buttons		: displace along Z-axis of cube
	triangle/X buttons	: rotate camera around X-axis
	circle/square buttons	: rotate camera around Y-axis
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




