[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
       Copyright (C) 1999 Sony Computer Entertainment Inc.
                                       All Rights Reserved

Simple animation playback using the IPU

<Description>

This sample program shows how to play back simple animation using only the IDEC command of the IPU.

All compressed data is DMA transferred to the IPU.  One IDEC command is issued for each frame, then that frame is decoded. The decoded data is DMA transferred to the GS via PATH3.

The data used by this program consists of a sequence of the required number of macroblocks that will be interpreted by the IDEC.  No inter-frame correlation is made.

This program seldomly uses the EE core, and the VU is not used at all.  Therefore, multiprocessing with processes like polygon processing can be easily performed.  Although this program displays images directly that were transferred to the frame buffer, motions can also be pasted in as texture, by modifying the program.

<Files>
        ezmovie.h
        ldimage.c
        main.c
	ez.ipu (/usr/local/sce/data/movie/ipu)

<Startup>
	% make :  Compile
	% make run :  Execute

The following method can also be used to execute the program after compilation:

	% dsedb
	> run ezmovie ez.ipu

<Controller operation>
	None

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




