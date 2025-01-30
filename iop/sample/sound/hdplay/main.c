/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: main.c,v 1.51 2003/05/21 07:22:27 kaol Exp $
 */
/* 
 *              I/O Processor Library Sample Program
 * 
 *                            
 *                          Version 1.0 
 * 
 *       Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 * 
 *                             main.c
 *
 *       Play the BD data with HD information by SdHd fuctions
 */

#include <kernel.h>
#include <stdio.h>
#include <sys/file.h>
#include <sceerrno.h>
#include <stdlib.h>
#include <libsd.h>
#include <sdhd.h>


/* �]����� SPU2 ���[�J�����������̐擪�A�h���X */
#define  SPU2_ADDR	0x15010

/* IOP ������ �� SPU2 ���[�J���������ɓ]�����鎞�� DMA �`�����l�� */
#define  DMA_CH         0
#define  BASE_priority  60

/* �T�E���h�f�[�^ */
#define  BD_NAME       "host1:/usr/local/sce/data/sound/wave/ivory.bd"
#define  HD_NAME       "host1:/usr/local/sce/data/sound/wave/ivory.hd"

unsigned char *HD_ADDR;         
unsigned char *BD_ADDR;         

/* ���荞�݃n���h���ɓn�����f�[�^�F�]���I���t���O( > 0 �]���I�� ) */
volatile int gEndFlag = 0;           

/* �����p�����[�^�\���� */
typedef struct {
    int   core;              /* �R�A�ԍ�             */
    int   voiceNumber;       /* �{�C�X�ԍ�           */
    int   velocity;          /* �׃��V�e�B           */
    int   baseNote;          /* �x�[�X�m�[�g         */
    int   note;              /* �m�[�g               */
    int   detune;            /* �f�B�`���[��         */
    int   ADSR1;             /* ADSR1                */
    int   ADSR2;             /* ADSR2                */
    int   panpot;            /* �p���|�b�g           */
    int   vagOffsetAddr;     /* �g�`�f�[�^�I�t�Z�b�g */
    int   vagSampleRate;     /* �T���v�����O���[�g   */
} KeyOnParam; 

static int core_voice[2];

#define NO_CINDEX    (-1)    /* �m�C�Y�����g�p */
/* ---------------------------------------------------------------
 * �f�[�^�ǂݍ���
 * ---------------------------------------------------------------- */
char *set_data (char *FILENAME, int *size)
{
    char *buffer;
    int  oldstat;
    int  fd;
    
    if ((fd = open (FILENAME, O_RDONLY)) < 0){
	printf("Can not open file : %s.\n", FILENAME);
	return NULL;
    }
    
    *size = lseek (fd, 0, SEEK_END);	/* �t�@�C���T�C�Y�̎擾 */
    printf("FILE SIZE : %d\n", *size);
    if (*size <= 0){
	printf("\nCan't load  file to iop heap\n");
	return NULL;
    }    
    lseek(fd, 0, SEEK_SET);		/* �ǂݍ��݈ʒu��擪�� */
   
    CpuSuspendIntr(&oldstat);
    
    buffer = AllocSysMemory(SMEM_Low, *size, NULL); /* �f�[�^�̈� */
    
    CpuResumeIntr(oldstat);
    
    Kprintf("allocate IOP heap memory - ");    

    if (buffer == NULL){
	printf("Can't alloc heap \n");
	return NULL;
    }
    printf("alloced 0x%x  \n", (int)buffer);
    
    /* �t�@�C���̓��e��ǂݍ��� */	
    if (read(fd, buffer, *size) != *size) return NULL;
    close(fd);  
    
    return buffer; 
    
}
/* ----------------------------------------------------------------
 * ���荞�݃n���h��
 * ---------------------------------------------------------------- */
int IntTrans (int ch, void * common)
{
    int *c =( int *)common;

    (*c)++;
    Kprintf("##### interrupt detected. count: %d CORE ch: %d #####\n", *c, ch);
    Kprintf("##### DMA transfer finished                     #####\n");
    
    return 0;	/* ���ݖ��g�p�B�K�� 0 ��Ԃ��B */
}

/* ----------------------------------------------------------------
 * IOP��BD�f�[�^��SPU2�ɓ]��
 * ---------------------------------------------------------------- */
void spu2_transfer (int size)
{
    int   ret;
       
    /* DMA �]���I�����荞�݃n���h����ݒ� */
    sceSdSetTransIntrHandler(DMA_CH, (sceSdSpu2IntrHandler)IntTrans, 
			      (void *) &gEndFlag);
    gEndFlag = 0;
    /* �]���J�n */
    ret = sceSdVoiceTrans(DMA_CH, SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA,
			   (u_char *)BD_ADDR, SPU2_ADDR, size);
    /* DMA_CB_TEST */
    while (gEndFlag == 0){   
        /* ���荞�݃n���h�����Ă΂��܂ő҂� : �C���^�[�o��: 0.1 �b */
	DelayThread(100 * 1000);
    } 
    
    return;
}
/* ----------------------------------------------------------------
 * ������
 * ---------------------------------------------------------------- */
int initialize (void)
{
    int core, voice;
    
    /* �჌�x���T�E���h���C�u�����̏����� */
    if (sceSdInit(0) != SCE_OK) return -1;

    for (core = 0; core < 2; core++){

	 /* �}�X�^�[�{�����[���� �I�t */
	sceSdSetParam(core | SD_P_MVOLL, 0);
	sceSdSetParam(core | SD_P_MVOLR, 0);
	
	for (voice = 0; voice < 24; voice++){
	    /* �e�{�C�X�̃{�����[���A�s�b�`�̏����� */
	    sceSdSetParam(core | (voice << 1) | SD_VP_VOLL,  0);
	    sceSdSetParam(core | (voice << 1) | SD_VP_VOLR,  0);
	    sceSdSetParam(core | (voice << 1) | SD_VP_PITCH, 0x1000);
	}
	 /* �}�X�^�[�{�����[�����Z�b�g */
	sceSdSetParam(core | SD_P_MVOLL, 0x3fff);
	sceSdSetParam(core | SD_P_MVOLR, 0x3fff);
    }
  
    return 0;
}

/* ----------------------------------------------------------------
 * panpot ����Volume�l�ւ̌v�Z
 * ---------------------------------------------------------------- */
void set_pan (KeyOnParam *voice)
{      
    int  vol_L, vol_R;
    int  panpot;

    /* �p���|�b�g�̑�� :  �t���Ȃ�Ε������] */
    if (voice->panpot < 0)panpot = - voice->panpot;
    else panpot = voice->panpot;    
  
    if (panpot == 64){ 
	/* panpot = 64 */
	vol_R = vol_L = (0x3fff * voice->velocity) / 128;
    } else if (panpot > 64){
	/* panpot : 64 --- 127 */
	vol_L = ((0x3fff * (128 - panpot) * voice->velocity) / 64) / 128;
	vol_R = (0x3fff * voice->velocity) / 128;
    } else if (panpot < 64){
	/* panpot : 0  ---  64 */
	vol_L = (0x3fff * voice->velocity) / 128;
	vol_R = ((0x3fff * panpot * voice->velocity) / 64) / 128;
    }
      
    if (voice->panpot < 0){
	/* �p���|�b�g���t���̎��A15bit�L�����ŕ��̒l���� */
	vol_L = (- vol_L) & 0x7fff;
	vol_R = (- vol_R) & 0x7fff;
    }
    
    /* �{�C�X�{�����[�� L/R �̐ݒ� */
    sceSdSetParam(voice->core | (voice->voiceNumber << 1) | SD_VP_VOLL, vol_L);
    sceSdSetParam(voice->core | (voice->voiceNumber << 1) | SD_VP_VOLR, vol_R);
    
    return;
}
/* ----------------------------------------------------------------
 * �׃��V�e�B�����W�̃N���X�t�F�C�h�̌v�Z 
 * ---------------------------------------------------------------- */
int cal_velRangeEffect (int velocity, SceSdHdSampleParam *sample)
{
    int value;

    /* �׃��V�e�B�� 0 �Ȃ�ΏI�� */
    if(velocity <= 0) return 0;
    value = velocity;

    if (((sample->velRange).low & 0x80) == 0x80){
	/*  �׃��V�e�B�����W Low �� �r�b�g7 �� 1 */
	if (((sample->velRange).low & 0x7f) > velocity){
	    /* �׃��V�e�B�͂׃��V�e�B�����W�͈̔͊O */
	    return 0;
	}
	if ((sample->velRange).crossFade > velocity){
	    /* �׃��V�e�B�̃N���X�t�F�C�h���ʗʂ��Z�o */
	    value = ((velocity - ((sample->velRange).low & 0x7f))* velocity)/(
		(sample->velRange).crossFade - ((sample->velRange).low & 0x7f));
	}
    }
    if (((sample->velRange).high & 0x80) == 0x80){
	/* �׃��V�e�B�����W High �� �r�b�g7 �� 1 */
	if (((sample->velRange).high & 0x7f) < velocity){
	  /* �׃��V�e�B�͂׃��V�e�B�����W�͈̔͊O */
	    return 0;
	}
	if ((sample->velRange).crossFade < velocity){
	    /* �׃��V�e�B�̃N���X�t�F�C�h���ʗʂ��Z�o */
	    value = ((((sample->velRange).high & 0x7f) - velocity)* velocity)/
		(((sample->velRange).high & 0x7f) - (sample->velRange).crossFade);
	}
    }
    return  value;
}
/* ----------------------------------------------------------------
 * �X�v���b�g�̃N���X�t�F�C�h�̌v�Z 
 * ---------------------------------------------------------------- */
int cal_splitRangeEffect (int note, int velocity, SceSdHdSplitBlock *split)
{
    int value;

    /* �׃��V�e�B�� 0 �Ȃ�ΏI�� */
    if (velocity <= 0) return 0;
    value = velocity;

    if (((split->range.low) & 0x80) == 0x80){
	/* �X�v���b�g�����W Low �� �r�b�g7 �� 1 */
	if ((((split->range).low) & 0x7f) > note){
	    /* �m�[�g�̓X�v���b�g�����W�͈̔͊O */
	    return 0;
	}
	if ((split->range).crossFade > note){
	    /* �m�[�g�ɂ��x���V�e�B�̃N���X�t�F�C�h���ʗʂ��Z�o */
	    value = (((note - ((split->range).low & 0x7f)) * velocity)/
		     (((split->range).crossFade) - (((split->range).low) & 0x7f)));
	}
    }
   
    if ((((split->range).high) & 0x80) == 0x80){
	/* �X�v���b�g�����W High �� �r�b�g7 �� 1 */
	if ((((split->range).high) & 0x7f) < note){
	    /* �m�[�g�̓X�v���b�g�����W�͈̔͊O */
	    return 0;
	} 
	if ((split->range).crossFade < note){
	    /* �m�[�g�ɂ��x���V�e�B�̃N���X�t�F�C�h���ʗʂ��Z�o */
	    value = (((((split->range).high & 0x7f) - note)* velocity)/
		     (((split->range).high & 0x7f) - (split->range).crossFade));
	}
    }
    return value;
}
/* ----------------------------------------------------------------
 * �R�A���̃{�C�X�i���o�[�Ǘ�
 * ---------------------------------------------------------------- */
void set_voiceNumber (KeyOnParam *voice, int voicecount)
{
    int i;
    
    for(i = 0; i < voicecount; i++){
	/* �w�肵���R�A�̃{�C�X�i���o�[�����蓖�Ă� : 0 - 23 */
	voice[i].voiceNumber = core_voice[voice[i].core]++;
	if(core_voice[voice[i].core] >= 24) core_voice[voice[i].core] = 0;
    }
      
    return;
}
/* ----------------------------------------------------------------
 * ��������
 * ---------------------------------------------------------------- */
void  key_on (KeyOnParam *voice, int voicecount)
{
    int voiceBit[2];
    int i;
    int pitch;
    
    /* �R�A���̃{�C�X�i���o�[���v�Z����{�C�X�r�b�g�z��̏����� */
    voiceBit[0] = voiceBit[1] = 0;
    for(i = 0; i < voicecount; i++){
	/* �R�A���ɔ�������{�C�X�̃r�b�g���P�ɂ��� */
	voiceBit[(voice[i].core)] |=  1 << voice[i].voiceNumber;
	/* �p���|�b�g�̐ݒ� */
	set_pan(&voice[i]); 

	/* �s�b�`�̐ݒ� */
	pitch = sceSdNote2Pitch(voice[i].baseNote, voice[i].detune, voice[i].note, 0);
	sceSdSetParam(voice[i].core | (voice[i].voiceNumber << 1) | SD_VP_PITCH,      
		       (0x3fff & ((pitch * voice[i].vagSampleRate) / 48000)));
	
        /* ADSR�̐ݒ� */
	sceSdSetParam(voice[i].core | (voice[i].voiceNumber << 1) | SD_VP_ADSR1,
		      voice[i].ADSR1);
	sceSdSetParam(voice[i].core | (voice[i].voiceNumber << 1) | SD_VP_ADSR2,
		      voice[i].ADSR2);
        /* _SSA: �g�`�f�[�^�̐擪�A�h���X = ( BD_ADDR + vagi->vagOffsetAddr ) */
	sceSdSetAddr(voice[i].core | (voice[i].voiceNumber << 1) | SD_VA_SSA, 
		     SPU2_ADDR + voice[i].vagOffsetAddr);
    }
    /* ���� */
    sceSdSetSwitch(SD_CORE_0 | SD_S_KON, voiceBit[0]);
    sceSdSetSwitch(SD_CORE_1 | SD_S_KON, voiceBit[1]);
    
    return;
}
/* ----------------------------------------------------------------
 * ��������
 * ---------------------------------------------------------------- */
void  key_off (KeyOnParam *voice, int voicecount)
{
   
	int voiceBit[2];
	int i;

	voiceBit[0] = 0;
	voiceBit[1] = 0;
	
 	for(i = 0; i < voicecount; i++){
	    /* �R�A���ɔ�������{�C�X�̃r�b�g���P�ɂ��� */
	    voiceBit[voice[i].core] |=  1 << voice[i].voiceNumber;
	}
	/* ���� */
 	sceSdSetSwitch(SD_CORE_0 | SD_S_KOFF, voiceBit[0]);	
 	sceSdSetSwitch(SD_CORE_1 | SD_S_KOFF, voiceBit[1]);
	
	return;
}
/* ----------------------------------------------------------------
 * ���擾���@ ���̂P
 *  �v���O�����ɓo�^����Ă���g�`�����ԂɌ�������������B 
 * �v���O������񂩂珇��HD�̊K�w�\�������ǂ�A�ŏI�I�ɂT��
 * �����擾����B���̏�����ɔg�`�̔������s���B    
 * ---------------------------------------------------------------- */
int GetHDdata_case1(void)
{
    int   program, split, sample; /* ���[�v�ϐ� */
    int   maxnum, ret, sampleNumber;
      
    /*  SdHd */
    SceSdHdProgramParam      programParam;
    SceSdHdSplitBlock        splitBlock;
    SceSdHdSampleSetParam    sampleSetParam; 
    SceSdHdSampleParam       sampleParam;
    SceSdHdVAGInfoParam      vagInfoParam;
    KeyOnParam               voice; 
   
    maxnum = sceSdHdGetMaxProgramNumber(HD_ADDR);
    if (maxnum <= (-1)) return -1;  

    for (program = 0; program <= maxnum; program++){ 
	/* �v���O�������̎擾 */
	ret = sceSdHdGetProgramParam(HD_ADDR, program, &programParam);
	if (ret != SCE_OK){
	    printf("Can not GetProgramParam %x.\n", program);
	    continue;
	}
	for (split = 0; split < programParam.nSplit; split++){
	    /* �X�v���b�g�u���b�N���̎擾 */
	    ret = sceSdHdGetSplitBlock(HD_ADDR, program, split, &splitBlock);
	    if (ret != SCE_OK){
		printf("Can not GetSplitBlock %x.\n", split);
		continue;
	    }
	    /* �T���v���Z�b�g���̎擾 */
	    ret = sceSdHdGetSampleSetParam 
		(HD_ADDR, splitBlock.sampleSetIndex, &sampleSetParam);
	    if (ret != SCE_OK){
		printf("Can not GetSampleSetParam %x.\n", 
		       splitBlock.sampleSetIndex);
		continue;
	    }
	    for (sample = 0; sample < sampleSetParam.nSample; sample++){
                /* �T���v���Z�b�g�i���o�[�ƃT���v���C���f�b�N�X�i���o�[����
		   �T���v���i���o�[���擾 */
		sampleNumber = sceSdHdGetSampleNumberBySampleIndex
		    (HD_ADDR, splitBlock.sampleSetIndex, sample);
		if (sampleNumber <= (-1)){
		    printf("Can not GetSampleNumber.\n");
		    continue;
                }
                /* �T���v�����̎擾 */
		ret = sceSdHdGetSampleParam
		    (HD_ADDR, sampleNumber, &sampleParam);
		if (ret != SCE_OK){
		    printf("Can not GetSampleParam %x.\n", sampleNumber);
		    continue;
		}		
		/* sampleParam.vagIndex �� NO_CINDEX( -1): �m�C�Y�������g�p */
		if (sampleParam.vagIndex == NO_CINDEX){
		    printf("Sound source is NOISE.\n");
		    continue;
		}
		/* VAGInfo���̎擾 */
		ret = sceSdHdGetVAGInfoParam
		    (HD_ADDR, sampleParam.vagIndex, &vagInfoParam);		
		if (ret != SCE_OK){
		    printf("Can not GetVAGInfoParam %x.\n", 
			   sampleParam.vagIndex);
		    continue;
		}
		
                /* �����p�����[�^�\���̂ɒl��ݒ� */
                /* �������s���R�A�w�� */
		switch (sampleParam.spuAttr & 0xf0){/* spuAttr �̃R�A�r�b�g���� */
		case SCEHD_SPU_CORE_0: /* �R�A 0 �g�p */
		    voice.core = SD_CORE_0;
		    break;
		case SCEHD_SPU_CORE_1: /* �R�A 1 �g�p */
		    voice.core = SD_CORE_1;
		    break;
		default:
		    voice.core = SD_CORE_0;
		    break;
		}
		voice.baseNote       = sampleParam.common.baseNote;
		voice.note           = sampleParam.common.baseNote;
		voice.detune         = sampleParam.common.detune;
		voice.panpot         = sampleParam.common.panpot;
		/* �׃��V�e�B�̓T���v���{�����[���Ƀv���O�����{�����[�������������l���̗p*/
		voice.velocity       = (sampleParam.common.volume * programParam.common.volume) / 128;
		voice.ADSR1          = sampleParam.ADSR.ADSR1;
		voice.ADSR2          = sampleParam.ADSR.ADSR2;
		voice.vagOffsetAddr  = vagInfoParam.vagOffsetAddr;
		voice.vagSampleRate  = vagInfoParam.vagSampleRate;
		printf("Program #%2d, Split #%2d, Sample #%2d ,vagInfo #%2d : offset %5x\n",
		       program, split, sampleNumber, sampleParam.vagIndex, vagInfoParam.vagOffsetAddr);

		/* �{�C�X�i���o�[�̐ݒ� */
		set_voiceNumber(&voice, 1);
                /* �������� */
		key_on(&voice, 1);
		DelayThread(500 * 1000); /* 0.5�b�҂� */
		/* ���� */
		key_off(&voice, 1);
	    }
	}
    }
    return 0;
}
/* ----------------------------------------------------------------
 * ���擾���@ ���̂Q
 *
 * sceSdHdGetSampleParamByNoteVelocity, sceSdHdGetVAGInfoParamByNoteVelocity 
 * ���g�����g�`���̎擾�Ǝ擾�����g�`�̔��������B 
 *
 *  �m�[�g�Ƃ׃��V�e�B�ƃv���O�����i���o�[���甭���̂��߂�
 * �Œ���̏����擾���A�ł����Ȃ��H���Ŕg�`�����擾�A��������B
 *
 *   �����Ɋւ����{���̓T���v������VAGInfo���̒l���̗p�B
 * ---------------------------------------------------------------- */
int  GetHDdata_case2(void)
{
    int   program, sample, vag, voice; /* ���[�v�ϐ� */
    int   ret;
    int   note, velocity;
    int   programcount, samplecount, vagcount;
    int   oldstat;
  

    /* SdHd */
    SceSdHdProgramParam      programParam;
    SceSdHdSplitBlock        *splitPtr      = (SceSdHdSplitBlock *)NULL;
    SceSdHdSampleSetParam    *sampleSetPtr  = (SceSdHdSampleSetParam *)NULL; 
    SceSdHdSampleParam       *samplePtr     = (SceSdHdSampleParam *)NULL;
    SceSdHdVAGInfoParam      *vagInfoPtr    = (SceSdHdVAGInfoParam *)NULL;
    KeyOnParam               *voicePtr      = (KeyOnParam *)NULL;
    int                      *programNumber = (int *)NULL;


    /* �L���ȃv���O�����i���o�[�̐����擾 */
    ret = sceSdHdGetValidProgramNumberCount(HD_ADDR);
    if (ret < 1){
	printf("Can not Find ValidProgramNumber\n");
	 return -1; 
    } else {
	/* �L���ȃv���O�����i���o�[���i�[����̈���m�� */
	CpuSuspendIntr(&oldstat);
	programNumber = AllocSysMemory
	    (SMEM_Low, sizeof ( int )*ret, NULL);
	CpuResumeIntr(oldstat);                 
	if (programNumber == NULL){
	    printf("Can not allocate!\n"); 
	     return -1;
	}
    }
    /* �L���ȃv���O�����i���o�[�������擾 */
    programcount = sceSdHdGetValidProgramNumber(HD_ADDR, programNumber);    
    if (programcount < 1){
	printf("Can not get ValidProgramNumber\n");
	 goto FreeParam;
    }
    
    /* ��{���̑g��������l������HD���̊e����̌�␔���擾 */
    /* �X�v���b�g�u���b�N���̌�␔���擾 */
    ret = sceSdHdGetMaxSplitBlockCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SplitBlock\n");
	 goto FreeParam;
    } else {
	/* �f�[�^���i�[����̈���m�� */
	CpuSuspendIntr(&oldstat);
	splitPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSplitBlock)*ret, NULL);
	CpuResumeIntr(oldstat);                 
	if (splitPtr == NULL){
	    printf("Can not allocate!\n"); 
	    goto FreeParam;
	}
    }	
    /* �T���v���Z�b�g���̌�␔���擾 */
    ret = sceSdHdGetMaxSampleSetParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleSetParam\n");
	goto FreeParam;
    } else {
	/* �f�[�^���i�[����̈���m�� */
	CpuSuspendIntr(&oldstat);
	sampleSetPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSampleSetParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (sampleSetPtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }
    /* �T���v�����̌�␔���擾 */
    ret = sceSdHdGetMaxSampleParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleParam\n");
	goto FreeParam;
    } else {
	/* �f�[�^���i�[����̈���m�� */
	CpuSuspendIntr(&oldstat);
	samplePtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSampleParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (samplePtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
	/* �����p�����[�^�\���̂��T���v�����̌�␔���m�� */
	CpuSuspendIntr(&oldstat);
	voicePtr = AllocSysMemory
	    (SMEM_Low, sizeof (KeyOnParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (voicePtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }
    /* VAGInfo���̌�␔���擾 */
    ret = sceSdHdGetMaxVAGInfoParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleSetParam\n");
	goto FreeParam;
    } else {
	/* �f�[�^���i�[����̈���m�� */
	CpuSuspendIntr(&oldstat);
	vagInfoPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdVAGInfoParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (vagInfoPtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }

    /* HD���̎擾 */
    for (program = 0; program < programcount; program++){
	/* �R�A���̔����{�C�X���Ǘ�����{�C�X�i���o�[�J�E���^�������� */
	core_voice[0] = core_voice[1] = 0;
	/* �v���O�������̎擾 */
	ret = sceSdHdGetProgramParam(HD_ADDR, programNumber[program], &programParam);
	if (ret != SCE_OK){
	    printf("Can not GetProgramParam %x.\n", programNumber[program]);
	    continue;
	}
	for (note = 0; note < 128; note++){ /* �m�[�g�����W�F0 - 127 */
	    for (velocity = 5; velocity < 128; velocity += 60){ /* �x���V�e�B�����W�F0 - 127 (0 = Key off) */
		/* �{�C�X�J�E���^�i��������g�`�̐����J�E���g�j�̏����� */
		voice = 0;
		/* �v���O�����i���o�[�A�m�[�g�A�׃��V�e�B�ɊY������
		   �T���v�������擾 */
		samplecount = sceSdHdGetSampleParamByNoteVelocity
		    (HD_ADDR, programNumber[program], note,
		     velocity, SCESDHD_IGNORE, samplePtr);
		
		/* �i���o�[�A�׃��V�e�B�ɊY������VAGInfo�����擾 */
		vagcount = sceSdHdGetVAGInfoParamByNoteVelocity
		    (HD_ADDR, programNumber[program], note, velocity, 
		     SCESDHD_IGNORE, vagInfoPtr);
		
		/* �����p�����[�^�\���̂ɒl��ݒ� */
		if (vagcount > 0){
		    for (vag = 0, sample = 0; sample < samplecount; sample++){
		
			/* �T���v������sampleParam.vagIndex �� NO_CINDEX(�m�C�Y�������g�p)
			   �Ȃ�΁A�T���v�����̔z���i�܂��� */
			if (samplePtr[sample].vagIndex == NO_CINDEX){
			    printf("Sound source is NOISE.\n");
			    continue;
			}		
			/* �������s���R�A�w�� */
			switch (samplePtr[sample].spuAttr & 0xf0){
			    /* spuAttr �̃R�A�r�b�g���� */
			 case SCEHD_SPU_CORE_0: /* �R�A 0 �g�p */
			     voicePtr[voice].core = SD_CORE_0;
			     break;
			case SCEHD_SPU_CORE_1: /* �R�A 1 �g�p */
			    voicePtr[voice].core = SD_CORE_1;
			    break;
			default:
			    voicePtr[voice].core = SD_CORE_0;
			    break;
			}
			/* �׃��V�e�B�̓T���v���{�����[���ƃv���O�����{�����[�������� */
			voicePtr[voice].velocity      = 
			    (velocity * samplePtr[sample].common.volume * programParam.common.volume)/(128 * 128);
			voicePtr[voice].baseNote      = samplePtr[sample].common.baseNote;
			voicePtr[voice].note          = note;
			/* �f�B�`���[���̓T���v�����̒l���̗p */
			voicePtr[voice].detune        = samplePtr[sample].common.detune; 
			/* �p���|�b�g�� �T���v�����̒l���̗p */
			voicePtr[voice].panpot        = samplePtr[sample].common.panpot; 
			voicePtr[voice].ADSR1         = samplePtr[sample].ADSR.ADSR1; 
			voicePtr[voice].ADSR2         = samplePtr[sample].ADSR.ADSR2; 
			voicePtr[voice].vagOffsetAddr = vagInfoPtr[vag].vagOffsetAddr;
			voicePtr[voice].vagSampleRate = vagInfoPtr[vag].vagSampleRate;
			vag++;
			voice++;
		    }
		    
		    /* �����g�`�̐��� > 0 �Ȃ�� ���� */
		    if (voice >0){
			printf("Program #%3d note %3d velocity %3d  : ", 
			       programNumber[program], note, velocity);
			printf("%2d Sample(s) / %2d vag(s)\n", samplecount, voice);

			/* �{�C�X�i���o�[�̐ݒ� */
			set_voiceNumber(voicePtr, voice);
			/* ���� */			    
			key_on(voicePtr, voice);
			DelayThread(400 * 1000); /* 0.4�b�҂� */
			/* ���� */			    
			key_off(voicePtr, voice);
		    }
		}  
	    }
	}
    }

  FreeParam:
    /* �m�ۂ������̈�̉�� */
    if (programNumber !=( int *) NULL) { 
	FreeSysMemory(programNumber);
    }
    if (splitPtr !=(SceSdHdSplitBlock *) NULL) { 
	FreeSysMemory(splitPtr);
    }
    if (sampleSetPtr !=(SceSdHdSampleSetParam *) NULL) { 
	FreeSysMemory(sampleSetPtr);
    }
    if (samplePtr !=(SceSdHdSampleParam *) NULL) { 
	FreeSysMemory(samplePtr);
    }
    if (vagInfoPtr !=(SceSdHdVAGInfoParam *) NULL) { 
	FreeSysMemory(vagInfoPtr);
    }


    return 0;
}
/* ----------------------------------------------------------------
 * ���擾���@ ���̂R
 *
 *   sceSdHdGetSplitBlockByNote, sceSdHdGetSampleSetParamByNote
 *   sceSdHdGetSampleParamByVelocity, sceSdHdGetVAGInfoParamByVelocity
 * ���g���Ĕg�`�����擾�B�擾�����������p���m�[�g�ƃ{�����[��
 * �̃N���X�t�F�C�h�̌��ʂ��v�Z���Ĕ��������B
 *
 *   �m�[�g�ƃv���O�����i���o�[����A�X�v���b�g�u���b�N����
 * �T���v���Z�b�g���B�X�v���b�g�u���b�N�����̃T���v���Z�b�g�i���o�[�ւ�
 * �C���f�b�N�X�Ƃ׃��V�e�B����A�T���v������VAGInfo���̎擾�B
 *   �X�v���b�g�u���b�N���A�T���v�����ɓ��̃m�[�g�ƃ{�����[��
 * �̃����W����N���X�t�F�C�h�̌��ʂ��v�Z���A��������{�����[����ݒ�B
 *
 *   ���̑��̔����Ɋւ����{���̓T���v������VAGInfo���̒l���̗p�����B
 * ---------------------------------------------------------------- */
int GetHDdata_case3(void)
{
    int   program, split, sample, vag, voice; /* ���[�v�ϐ� */
    int   ret;
    int   note, velocity;
    int   programcount, splitcount, samplesetcount, samplecount, vagcount;
    int   oldstat;
  
    
    /* SdHd */
    SceSdHdProgramParam      programParam;
    SceSdHdSplitBlock        *splitPtr      = (SceSdHdSplitBlock *)NULL;
    SceSdHdSampleSetParam    *sampleSetPtr  = (SceSdHdSampleSetParam *)NULL; 
    SceSdHdSampleParam       *samplePtr     = (SceSdHdSampleParam *)NULL;
    SceSdHdVAGInfoParam      *vagInfoPtr    = (SceSdHdVAGInfoParam *)NULL;
    KeyOnParam               *voicePtr      = (KeyOnParam *)NULL;
    int                      *programNumber = (int *)NULL;

    /* �L���ȃv���O�����i���o�[�̐����擾 */
    ret = sceSdHdGetValidProgramNumberCount(HD_ADDR);
    if (ret < 1){
	printf("Can not Find ValidProgramNumber\n");
	return -1; 
    } else {
	/* �L���ȃv���O�����i���o�[���i�[����̈���m�� */
	CpuSuspendIntr(&oldstat);
	programNumber = AllocSysMemory
	    (SMEM_Low, sizeof ( int )*ret, NULL);
	CpuResumeIntr(oldstat);                 
	if (programNumber == NULL){
	    printf("Can not allocate!\n"); 
	    goto FreeParam;
	}
    }
    /* �L���ȃv���O�����i���o�[�������擾 */
    programcount = sceSdHdGetValidProgramNumber(HD_ADDR, programNumber);    
    if (programcount < 1){
	printf("Can not get ValidProgramNumber\n");
	goto FreeParam;
    }
    
    /* ��{���̑g��������l������HD���̊e����̌�␔���擾 */
    /* �X�v���b�g�u���b�N���̌�␔���擾 */
    ret = sceSdHdGetMaxSplitBlockCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SplitBlock\n");
	goto FreeParam;
    } else {
	/* �f�[�^���i�[����̈���m�� */
	CpuSuspendIntr(&oldstat);
	splitPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSplitBlock)*ret, NULL);
	CpuResumeIntr(oldstat);                 
	if (splitPtr == NULL){
	    printf("Can not allocate!\n"); 
	    goto FreeParam;
	}
    }	
    /* �T���v���Z�b�g���̌�␔���擾 */
    ret = sceSdHdGetMaxSampleSetParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleSetParam\n");
	goto FreeParam;
    } else {
	/* �f�[�^���i�[����̈���m�� */
	CpuSuspendIntr(&oldstat);
	sampleSetPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSampleSetParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (sampleSetPtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }
    /* �T���v�����̌�␔���擾 */
    ret = sceSdHdGetMaxSampleParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleParam\n");
	goto FreeParam;
    } else {
	/* �f�[�^���i�[����̈���m�� */
	CpuSuspendIntr(&oldstat);
	samplePtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSampleParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (samplePtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
	/* �����p�����[�^�\���̂��T���v�����̌�␔���m�� */
	CpuSuspendIntr(&oldstat);
	voicePtr = AllocSysMemory
	    (SMEM_Low, sizeof (KeyOnParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (voicePtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }
    /* VAGInfo���̌�␔���擾 */
    ret = sceSdHdGetMaxVAGInfoParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleSetParam\n");
	goto FreeParam;
    } else {
	/* �f�[�^���i�[����̈���m�� */
	CpuSuspendIntr(&oldstat);
	vagInfoPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdVAGInfoParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (vagInfoPtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }

    /* HD���擾 */
    for (program = 0; program < programcount; program++){  
	/* �R�A���̔����{�C�X���Ǘ�����{�C�X�i���o�[�J�E���^�������� */
	core_voice[0] = core_voice[1] = 0;
	ret = sceSdHdGetProgramParam(HD_ADDR, programNumber[program], &programParam);
	if (ret != SCE_OK){
	    printf("Can not GetProgramParam %x.\n", program);
	    continue;
	}
	for (note = 0; note < 128; note++){/* �m�[�g�����W�F0 - 127 */
	    /* �v���O�����i���o�[�A�m�[�g�ɊY������X�v���b�g�u���b�N�����擾 */
	    splitcount = sceSdHdGetSplitBlockByNote
		(HD_ADDR, programNumber[program], note, splitPtr);
	    /* �v���O�����i���o�[�A�m�[�g�ɊY������
	       �T���v���Z�b�g�����擾 */
	    samplesetcount = sceSdHdGetSampleSetParamByNote
		(HD_ADDR, programNumber[program], note, sampleSetPtr);
	    /* �x���V�e�B�����W�F0 - 127 (0 = Key off) */
	    for (velocity = 5; velocity < 128; velocity += 60){
		/* �{�C�X�J�E���^�i��������g�`�̐����J�E���g�j�̏����� */
		voice = 0;
		for (split = 0; split < splitcount; split++){
		    /* �T���v���Z�b�g�i���o�[�A�׃��V�e�B�ɊY������
		       �T���v�������擾 */
		    samplecount = sceSdHdGetSampleParamByVelocity
			(HD_ADDR, splitPtr[split].sampleSetIndex, velocity, 
			 SCESDHD_IGNORE, samplePtr);
		    /* �T���v���Z�b�g�i���o�[�A�׃��V�e�B�ɊY������
		       VAGInfo�����擾 */
		    vagcount = sceSdHdGetVAGInfoParamByVelocity
			(HD_ADDR, splitPtr[split].sampleSetIndex, velocity, 
			 SCESDHD_IGNORE, vagInfoPtr);
		 
		    /* �����p�����[�^�\���̂ɒl��ݒ� */
		    if (vagcount > 0){
			for (vag = 0, sample = 0; sample < samplecount; sample++){
			  
			    /* �T���v������sampleParam.vagIndex �� NO_CINDEX(�m�C�Y�������g�p)
			       �Ȃ�΁A�T���v�����̔z���i�܂��� */
			    if (samplePtr[sample].vagIndex == NO_CINDEX){
				printf("%d sampleParam data Sound source is NOISE.\n", sample);
				continue;
			    }
			    
			    /* �������s���R�A�w�� */
			    switch (samplePtr[sample].spuAttr & 0xf0){
				/* spuAttr �̃R�A�r�b�g���� */
			    case SCEHD_SPU_CORE_0: /* �R�A 0 �g�p */
				voicePtr[voice].core = SD_CORE_0;
				break;
			    case SCEHD_SPU_CORE_1: /* �R�A 1 �g�p */
				voicePtr[voice].core = SD_CORE_1;
				break;
			    default:
				voicePtr[voice].core = SD_CORE_0;
				break;
			    }

			    /* �m�[�g�Ƃ׃��V�e�B�̃����W�ɑ΂���N���X�t�F�C�h���ʂ̌v�Z */

			    /* �׃��V�e�B�����W�̃N���X�t�F�C�h�̌v�Z */
			    ret = cal_velRangeEffect(velocity, &samplePtr[sample]);

			    if (ret <= 0) continue;
			    /* �׃��V�e�B�̓T���v���{�����[���ƃv���O�����{�����[�������� */
			    voicePtr[voice].velocity = 
				(ret * samplePtr[sample].common.volume * programParam.common.volume)/(128 * 128);
			    /* �X�v���b�g�̃N���X�t�F�C�h�̌v�Z */
			    ret = cal_splitRangeEffect(note, velocity, &splitPtr[split]);
			    if (ret <= 0) continue;
                             /* �׃��V�e�B�̓T���v���{�����[���ƃv���O�����{�����[�������� */
			     voicePtr[voice].velocity = 
				(ret * samplePtr[sample].common.volume * programParam.common.volume)/(128 * 128);

				
			
			    voicePtr[voice].baseNote      = samplePtr[sample].common.baseNote;
			    voicePtr[voice].note          = note;
			    /* �f�B�`���[���̓T���v�����̒l���̗p */
			    voicePtr[voice].detune        = samplePtr[sample].common.detune; 
                            /* �p���|�b�g�� �T���v�����̒l���̗p */
			    voicePtr[voice].panpot        = samplePtr[sample].common.panpot; 
			    voicePtr[voice].ADSR1         = samplePtr[sample].ADSR.ADSR1; 
			    voicePtr[voice].ADSR2         = samplePtr[sample].ADSR.ADSR2; 
			    voicePtr[voice].vagOffsetAddr = vagInfoPtr[vag].vagOffsetAddr;
			    voicePtr[voice].vagSampleRate = vagInfoPtr[vag].vagSampleRate;
			    vag++;
			    voice++;
			}
		    }
		}
		/* �����g�`�̐��� > 0 �Ȃ�� ���� */
		if (voice > 0){
		    printf("Program #%3d note %3d velocity %3d  : ",
			   programNumber[program], note, velocity);
		    printf("%2d Split(s) / %2d vag(s)\n", splitcount, voice);

		    /* �{�C�X�i���o�[�̐ݒ� */
		    set_voiceNumber(voicePtr, voice);
		    /* ���� */			    
		    key_on(voicePtr, voice);
		    DelayThread(400 * 1000); /* 0.4�b�҂� */
		    /* ���� */			    
		    key_off(voicePtr, voice);
		}
	    }
	}
    }

  FreeParam:
    /* �m�ۂ������̈�̉�� */
    if (programNumber !=( int *) NULL) { 
	FreeSysMemory(programNumber);
    }
    if (splitPtr !=(SceSdHdSplitBlock *) NULL) { 
	FreeSysMemory(splitPtr);
    }
    if (sampleSetPtr !=(SceSdHdSampleSetParam *) NULL) { 
	FreeSysMemory(sampleSetPtr);
    }
    if (samplePtr !=(SceSdHdSampleParam *) NULL) { 
	FreeSysMemory(samplePtr);
    }
    if (vagInfoPtr !=(SceSdHdVAGInfoParam *) NULL) { 
	FreeSysMemory(vagInfoPtr);
    }

    return 0;
}
/* ----------------------------------------------------------------
 * ���C������
 * ---------------------------------------------------------------- */
int sound_start (void)
{
    int   hd_size, bd_size;

    printf ("sound_start...\n");

    /* �t�@�C���̓��e�� IOP ���������ɓǂݍ��� */
    if ((HD_ADDR = set_data (HD_NAME, &hd_size)) == NULL) return -1;
    printf("IOP load : HD_ADDR %p ", HD_ADDR);
    printf(".... HD data SIZE %d\n", hd_size);
    if ((BD_ADDR = set_data (BD_NAME, &bd_size)) == NULL) goto finalize;

    printf("IOP load : BD_ADDR %p ", BD_ADDR);
    printf(".... BD data SIZE %d\n", bd_size);

    /* ������ */
    if (initialize() != 0){
	printf("Can not Initialize!\n");
	goto finalize;
    }  

    /* IOP��BD�f�[�^��SPU2�ɓ]�� */
    spu2_transfer(bd_size);
      
    /* sdhd ���g���� HD���̎擾 */
    if (GetHDdata_case1() != 0) goto finalize; /* ���̂P */  
    if (GetHDdata_case2() != 0) goto finalize; /* ���̂Q */
    if (GetHDdata_case3() != 0) goto finalize; /* ���̂R */

  finalize:
    if (HD_ADDR !=( unsigned char *) NULL) { 
	FreeSysMemory(HD_ADDR);
    }
    if (BD_ADDR !=( unsigned char *) NULL) { 
	FreeSysMemory(BD_ADDR);
    }
    printf("sound_start completed...\n");
     
    return 0;
    
}
/* ----------------------------------------------------------------
 * �X�^�[�g�֐�
 * ---------------------------------------------------------------- */
int start (int argc, char *argv [])
{
    struct ThreadParam     param;
    int                    thid;
    int                    ret;
  
    /* Create thread */
    param.attr         = TH_C;
    param.entry        = sound_start;
    param.initPriority = BASE_priority;
    param.stackSize    = 0x800;
    param.option       = 0;

    thid = CreateThread(&param);
  
    printf("thid ID %x\n", thid);
  
    if (thid > 0){
	printf("Start Thread\n");
   
	ret = StartThread(thid, 0);	/* �X���b�h�̋N�� */
   
	printf(".....Fine!\n");
    
	return RESIDENT_END;	/* program is resident */
    
    } else {
	printf("Program is deleted\n");
    
	return NO_RESIDENT_END;	/* program is deleted */
    }
}
/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
