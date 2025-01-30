/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 1999, 2000, 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * voicesdr - main.c
 *	main routine
 */

#include <eekernel.h>
#include <sifdev.h>
#include <libgraph.h>
#include <libpad.h>
#include <libsdr.h>

/*
 * �R���p�C�����I�v�V����
 */
#define BATCH_MODE      1	/* �o�b�`�������s��		*/
#define DMA_CB_TEST	1	/* DMA ���荞�݂̃e�X�g		*/
#define IRQ_CB_TEST	1	/* IRQ ���荞�݂̃e�X�g		*/
#define IRQ_ADDR_OFST	0x1000	/* IRQ�̈ʒu (�g�`�擪����� offset) */

/* IOP ������ �� SPU2 ���[�J���������ɓ]�����鎞�� DMA �`�����l�� */
#define DMA_CH 0

#define PRINTF(x) scePrintf x

/*
 * �g�`�f�[�^���
 */
#define VAG_FILENAME	"host0:/usr/local/sce/data/sound/wave/piano.vag"

/* VAG �t�@�C���̃w�b�_�����̃T�C�Y�B�w�b�_�� SPU2 ���[�J�����������ł͕s�v */
#define VAG_HEADER_SIZE 48
/* �]����� SPU2 ���[�J�����������̐擪�A�h���X */
#define VAG_SPU2_ADDR	0x25010

#define	EFFECT_DEPTH 	0x3fff

#if BATCH_MODE
/*
 * �o�b�`�������
 * --- libsdr �ł̓o�b�`�R�}���h�z��̑��v�f�͋�ɂ��Ă������ƁB
 *     ���v�f�͈����̎�n���̂��� libsdr �ɂĎg�p�����B
 */

/* ������: �ݒ�o�b�` */
sceSdBatch gBatchCommonSet [] __attribute__((aligned (64))) = {	/* �A���C�����g�ݒ� */
    {             0,                    0,      0 }, /* sdr �̂ݑ��v�f�� 0 */
    { SD_BSET_PARAM, SD_CORE_0|SD_P_MVOLL, 0x3fff },
    { SD_BSET_PARAM, SD_CORE_0|SD_P_MVOLR, 0x3fff },
    { SD_BSET_PARAM, SD_CORE_1|SD_P_MVOLL, 0x3fff },
    { SD_BSET_PARAM, SD_CORE_1|SD_P_MVOLR, 0x3fff }, /* 4 */
};

/* ������: �m�F�o�b�` */
sceSdBatch gBatchCommonGet [] __attribute__((aligned (64))) = { /* �A���C�����g�ݒ� */
    {             0,                    0, 0 }, /* sdr �̂ݑ��v�f�� 0 */
    { SD_BGET_PARAM, SD_CORE_0|SD_P_MVOLL, 0 },
    { SD_BGET_PARAM, SD_CORE_0|SD_P_MVOLR, 0 },
    { SD_BGET_PARAM, SD_CORE_1|SD_P_MVOLL, 0 },
    { SD_BGET_PARAM, SD_CORE_1|SD_P_MVOLR, 0 },	/* 4 */
    { SD_WRITE_IOP, 0x1234, 0x90000 },
    { SD_WRITE_EE,  0x4321, 0x1000f0 },
    { SD_RETURN_EE, 16, 0x100000 }, /* 7 */
};

/* �{�C�X�������o�b�`: CORE0 */
sceSdBatch gBatchVoiceSet_CORE0 [] __attribute__((aligned (64))) = { /* �A���C�����g�ݒ� */
    {             0,                                0,       0 }, /* sdr �̂ݑ��v�f�� 0 */
    { SD_BSET_PARAM, SD_CORE_0|SD_VOICE_XX|SD_VP_VOLL,  0x1fff },
    { SD_BSET_PARAM, SD_CORE_0|SD_VOICE_XX|SD_VP_VOLR,  0x1fff },
    { SD_BSET_PARAM, SD_CORE_0|SD_VOICE_XX|SD_VP_PITCH, 0x0400 },
    { SD_BSET_ADDR,  SD_CORE_0|SD_VOICE_XX|SD_VA_SSA,   VAG_SPU2_ADDR },
    { SD_BSET_PARAM, SD_CORE_0|SD_VOICE_XX|SD_VP_ADSR1, 
			SD_ADSR1(SD_ADSR_A_EXP, 30, 14, 14) },
    { SD_BSET_PARAM, SD_CORE_0|SD_VOICE_XX|SD_VP_ADSR2, 
			SD_ADSR2(SD_ADSR_S_EXP_DEC, 52, SD_ADSR_R_EXP, 13) }, /* 6 */
};

/* �{�C�X�������o�b�`: CORE1 */
sceSdBatch gBatchVoiceSet_CORE1 [] __attribute__((aligned (64))) = { /* �A���C�����g�ݒ� */
    {             0,                                0,       0 }, /* sdr �̂ݑ��v�f�� 0 */
    { SD_BSET_PARAM, SD_CORE_1|SD_VOICE_XX|SD_VP_VOLL,  0x1fff },
    { SD_BSET_PARAM, SD_CORE_1|SD_VOICE_XX|SD_VP_VOLR,  0x1fff },
    { SD_BSET_PARAM, SD_CORE_1|SD_VOICE_XX|SD_VP_PITCH, 0x0400 },
    { SD_BSET_ADDR,  SD_CORE_1|SD_VOICE_XX|SD_VA_SSA,   VAG_SPU2_ADDR },
    { SD_BSET_PARAM, SD_CORE_1|SD_VOICE_XX|SD_VP_ADSR1, 
			SD_ADSR1(SD_ADSR_A_EXP, 30, 14, 14) },
    { SD_BSET_PARAM, SD_CORE_1|SD_VOICE_XX|SD_VP_ADSR2, 
			SD_ADSR2(SD_ADSR_S_EXP_DEC, 52, SD_ADSR_R_EXP, 13) }, /* 6 */
};

/* �G�t�F�N�g�ݒ�o�b�` */
sceSdBatch gBatchEffectSet [] __attribute__((aligned (64))) = { /* �A���C�����g�ݒ� */
    {              0,                              0, 0 }, /* sdr �̂ݑ��v�f�� 0 */
    { SD_BSET_CORE,   SD_CORE_0 | SD_C_EFFECT_ENABLE, 1 },
    { SD_BSET_PARAM,  SD_CORE_0 | SD_P_EVOLL,         EFFECT_DEPTH },
    { SD_BSET_PARAM,  SD_CORE_0 | SD_P_EVOLR,         EFFECT_DEPTH },
    { SD_BGET_SWITCH, SD_CORE_0 | SD_S_VMIXEL,        0xffffff },
    { SD_BGET_SWITCH, SD_CORE_0 | SD_S_VMIXER,        0xffffff }, /* 5 */
};
#endif

char gDoremi [8] = {36, 38, 40, 41, 43, 45, 47, 48}; 
int gEndFlag = 0;

/* �g�p���� IOP ���W���[�� */
#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"
#define _MOD_SIO2MAN "sio2man.irx"
#define _MOD_PADMAN  "padman.irx"
#define _MOD_LIBSD   "libsd.irx"
#define _MOD_SDRDRV  "sdrdrv.irx"

/* �R���g���[����Ɨp�o�b�t�@ */
u_long128 pad_dma_buf [scePadDmaBufferMax] __attribute__((aligned (64)));

/* �G�t�F�N�g�����ݒ� */
sceSdEffectAttr effect_attr __attribute__((aligned (64)));

#define PROC_BAT_MAX 7
#define PROC_BAT_ITEMS (PROC_BAT_MAX * 24)
#define PROC_BAT_SIZE (sizeof (unsigned int) * (PROC_BAT_MAX * 24))
unsigned int proc_out [PROC_BAT_ITEMS] __attribute__((aligned (64)));

/*
 * load_modules()	���W���[���̃��[�h
 *	����:   �Ȃ�
 *	�Ԃ�l: �Ȃ�
 */
void
load_modules (void)
{
    scePrintf ("loading modules ...\n");

    /*
     *	�K�v�ƂȂ�e���W���[���̃��[�h
     */
    /* �V���A���ʐM��{���W���[�� */
    while (sceSifLoadModule (_MOD_ROOT _MOD_SIO2MAN, 0, NULL) < 0) {
	scePrintf ("  loading %s failed\n", _MOD_SIO2MAN);
    }
    /* �R���g���[�����䃂�W���[�� */
    while (sceSifLoadModule (_MOD_ROOT _MOD_PADMAN, 0, NULL) < 0) {
	scePrintf ("  loading %s failed\n", _MOD_PADMAN);
    }
    /* �჌�x���T�E���h���䃂�W���[�� */
    while (sceSifLoadModule (_MOD_ROOT _MOD_LIBSD, 0, NULL) < 0) {
	scePrintf ("  loading %s failed\n", _MOD_LIBSD);
    }
    /* libsdr �p�h���C�o���W���[�� */
    while (sceSifLoadModule (_MOD_ROOT _MOD_SDRDRV, 0, NULL) < 0) {
	scePrintf ("  loading %s failed\n", _MOD_SDRDRV);
    }

    scePrintf ("loading modules ... done.\n");

    return;
}

/* ----------------------------------------------------------------
 * ���荞�݃n���h��
 * ---------------------------------------------------------------- */
int
IntTrans (int ch, void * common)
{
    int *c = (int *) common;

    (*c) ++;
    scePrintf ("##### interrupt detected. count: %d CORE ch: %d #####\n", *c, ch);

    return 0;	/* ���ݖ��g�p�B�K�� 0 ��Ԃ��B */
}

int
IntFunc (int core, void *common)
{
    int *c = (int *) common;

    (*c) ++;
    scePrintf ("///// interrupt detected (%d). CORE-bit: %d /////\n", *c, core);

    return 0;	/* ���ݖ��g�p�B�K�� 0 ��Ԃ��B */
}

/* ----------------------------------------------------------------
 * �f�[�^�ǂݍ���
 *	����:   �g�`�f�[�^�T�C�Y�̕Ԃ�l
 *	�Ԃ�l: IOP ���̔g�`�f�[�^�ǂݍ��ݗ̈�̐擪�A�h���X
 * ---------------------------------------------------------------- */
char *
set_data (int *size)
{
    int fd;
    char *buffer;

    /* �t�@�C���̃I�[�v�� */
    while ((fd = sceOpen (VAG_FILENAME, SCE_RDONLY)) < 0) {
	scePrintf ("\nfile open failed. %s\n", VAG_FILENAME);
    };
    /* �t�@�C���T�C�Y�̎擾 */
    while ((*size = sceLseek (fd, 0, SCE_SEEK_END)) <= 0) {
	scePrintf ("\nfile seek failed. %s\n", VAG_FILENAME);
    }
    sceLseek (fd, 0, SCE_SEEK_SET);		/* �ǂݍ��݈ʒu��擪�� */
    sceClose (fd);				/* �T�C�Y������ꂽ�̂ŕs�v */
    /* �̈�m�� */
    PRINTF (("allocate IOP heap memory - "));
    while ((buffer = sceSifAllocIopHeap (*size)) == NULL) {
	scePrintf ("\nCan't alloc IOP heap \n");
    }
    PRINTF (("alloced 0x%x  \n", (int)buffer));
    /* �t�@�C���̓��e��ǂݍ��� */
    while (sceSifLoadIopHeap (VAG_FILENAME, buffer) < 0) {
	scePrintf ("\nfile load failed. %s \n", VAG_FILENAME);
    }
    PRINTF (("file load done\n"));

    return buffer;
}

/* ----------------------------------------------------------------
 * ���C������
 * ---------------------------------------------------------------- */

int
sound_test (void)
{
    int v;
    int size;
    char *vagBuffer;
#if BATCH_MODE
    int ret;
#else
    int i, core;
#endif

    PRINTF(("voice start...\n"));

    /* �t�@�C���̓��e�� IOP ���������ɓǂݍ��� */
    if ((vagBuffer = set_data (&size)) == NULL){
	return -1;
    }

    /* �჌�x���T�E���h���C�u�����̏����� */
    sceSdRemote (1, rSdInit, SD_INIT_COLD);

#if BATCH_MODE
    /* �o�b�`����: 4 �̃R�}���h�����s
     * --- libsdr �͑� 4 �����ȍ~�̎w�肪 libsd �ƈقȂ� */
    ret = sceSdRemote (1, rSdProcBatch2, gBatchCommonSet, 4, NULL, 0);
    PRINTF (("rSdProcBatch count = %d \n", ret));

    /* �o�b�`����:
     * 4 �̃R�}���h�����s: �Ԃ�l��̈� proc_out �ɏo��
     * --- libsdr �͑� 4 �����ȍ~�̎w�肪 libsd �ƈقȂ� */
    ret = sceSdRemote (1, rSdProcBatch2, gBatchCommonGet, 4, proc_out, PROC_BAT_SIZE);
    PRINTF ((" check MVOL (L/R): %x\n",
	     proc_out));
#else
    /* �}�X�^�[�{�����[����ݒ� */
    for (i = 0; i < 2; i ++) {
	sceSdRemote (1, rSdSetParam, i | SD_P_MVOLL, 0x3fff);
	sceSdRemote (1, rSdSetParam, i | SD_P_MVOLR, 0x3fff);
    }
#endif

    /*
     * �f�[�^�]��
     */
    PRINTF (("Data transfer ...\n"));
#if DMA_CB_TEST
    /* DMA �]���I�����荞�݃n���h����ݒ� */
    sceSdRemote (1, rSdSetTransIntrHandler,
		 DMA_CH, (sceSdSpu2IntrHandler)IntTrans, (void *) &gEndFlag);
#endif
    /* �]���J�n */
    sceSdRemote (1, rSdVoiceTrans, DMA_CH,
		 SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA,
		 (u_char *)(vagBuffer + VAG_HEADER_SIZE),
		 VAG_SPU2_ADDR, (u_int)size - VAG_HEADER_SIZE);
    PRINTF (("TSA = %x \n",     sceSdRemote (1, rSdGetAddr, DMA_CH | SD_A_TSA)));

#if DMA_CB_TEST
    while (gEndFlag == 0) {	/* ���荞�݃n���h�����Ă΂��܂ő҂� */
	sceGsSyncV (0);
    }
#else
    sceSdRemote (1, rSdVoiceTransStatus, DMA_CH, SD_TRANS_STATUS_WAIT); /* �]���I����҂� */
#endif

#if IRQ_CB_TEST
    /*
     * SPU2 ���荞�݂̐ݒ�
     */
    /* SPU2 ���荞�݂��N����A�h���X��ݒ�
     * ... SPU2 ���[�J�����������́A�g�`�f�[�^�̐擪���� 0x1000 �̈ʒu */
    sceSdRemote (1, rSdSetAddr, SD_CORE_0 | SD_A_IRQA, VAG_SPU2_ADDR + IRQ_ADDR_OFST);
    gEndFlag = 0;
    /* SPU2 ���荞�݃n���h���̓o�^ */
    sceSdRemote (1, rSdSetSpu2IntrHandler, (sceSdSpu2IntrHandler)IntFunc, (void *) &gEndFlag);
    /* SPU2 ���荞�݂�L���� (CORE0 ��) */
    sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_IRQ_ENABLE, 1);
#endif

#if BATCH_MODE
    /* �o�b�`����: �w��{�C�X���ꊇ����
     * --- libsdr �͑� 4 �����ȍ~�̎w�肪 libsd �ƈقȂ�
     * �� 7 ����/0xffffff ... �S�Ẵr�b�g�� 1 = �{�C�X 0 �` 23 ��S�Đݒ� */
    ret = sceSdRemote (1, rSdProcBatchEx2, gBatchVoiceSet_CORE0, 6, NULL,     0,             0xffffff);
    ret = sceSdRemote (1, rSdProcBatchEx2, gBatchVoiceSet_CORE1, 6, proc_out, PROC_BAT_SIZE, 0xffffff);
    PRINTF (("sceSdProcBatchEx count = %d \n", ret));
#else
    for (core = 0; core < 2; core ++) {
	for (v = 0; v < 24; v ++) {
	    /* �{�C�X�̑��������ꂼ��ݒ� */
	    sceSdRemote (1, rSdSetParam, core | (v << 1) | SD_VP_VOLL,  0x1eff);
	    sceSdRemote (1, rSdSetParam, core | (v << 1) | SD_VP_VOLR,  0x1eff);
	    sceSdRemote (1, rSdSetParam, core | (v << 1) | SD_VP_PITCH, 0x0400);
	    sceSdRemote (1, rSdSetAddr,  core | (v << 1) | SD_VA_SSA,   VAG_SPU2_ADDR);
	    sceSdRemote (1, rSdSetParam, core | (v << 1) | SD_VP_ADSR1,
			 SD_ADSR1 (SD_ADSR_A_EXP, 30, 14, 14));
	    sceSdRemote (1, rSdSetParam, core | (v << 1) | SD_VP_ADSR2,
			 SD_ADSR2 (SD_ADSR_S_EXP_DEC, 52, SD_ADSR_R_EXP, 13));
	}
    }
#endif

    /*
     * �G�t�F�N�g�����̐ݒ�
     */
    effect_attr.depth_L  = 0;	/* �ŏ��� 0 �� */
    effect_attr.depth_R  = 0;
    /* effect_attr.delay    = 30;  *//* ���[�h: �z�[���ɂ͕s�v */
    /* effect_attr.feedback = 200; *//* ���[�h: �z�[���ɂ͕s�v */
    /* ���[�h: �z�[�� + ���[�N�G���A�������� */
    effect_attr.mode = SD_REV_MODE_HALL | SD_REV_MODE_CLEAR_WA;
    sceSdRemote (1, rSdSetEffectAttr, SD_CORE_0, &effect_attr);

#if BATCH_MODE
    /* �o�b�`����: 5 �̃R�}���h�����s
     * --- libsdr �͑� 4 �����ȍ~�̎w�肪 libsd �ƈقȂ� */
    ret = sceSdRemote (1, rSdProcBatch2, gBatchEffectSet, 5, NULL, 0);
    PRINTF (("rSdProcBatch count = %d \n", ret));
#else
    /* CORE0: �G�t�F�N�g�L�� */
    sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_EFFECT_ENABLE, 1);
    /* CORE0: �G�t�F�N�g�Z���h�{�����[�� */
    sceSdRemote (1, rSdSetParam,    SD_CORE_0 | SD_P_EVOLL, EFFECT_DEPTH);
    sceSdRemote (1, rSdSetParam,    SD_CORE_0 | SD_P_EVOLR, EFFECT_DEPTH);
    /* CORE0: �S�Ẵ{�C�X�� L/R �o�͂��G�t�F�N�g�ɐڑ�  */
    sceSdRemote (1, rSdSetSwitch,   SD_CORE_0 | SD_S_VMIXEL, 0xffffff);
    sceSdRemote (1, rSdSetSwitch,   SD_CORE_0 | SD_S_VMIXER, 0xffffff);
#endif

    // Ring!
    for (v = 15; v < 23; v ++) {
	unsigned int pitch, c;
#if IRQ_CB_TEST
	/* SPU2 ���荞�݂��N�����ꍇ�A��U�����ɂ��čĐݒ肷�� */
	sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_IRQ_ENABLE, 0);
	sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_IRQ_ENABLE, 1);
#endif
	/* �s�b�`�ݒ� */
	pitch = sceSdRemote (1, rSdNote2Pitch, 60, 0, gDoremi[v - 15], 0);
	sceSdRemote (1, rSdSetParam, SD_CORE_0 | (v << 1) | SD_VP_PITCH, pitch);

	/* ���� */
	sceSdRemote (1, rSdSetSwitch, SD_CORE_0 | SD_S_KON, 1 << v);

	PRINTF ((" ++ key_on  pitchA %x, pitchB %x \n", 
		 pitch,
		 sceSdRemote (1, rSdGetParam, SD_CORE_0 | (v << 1) | SD_VP_PITCH)));
	
	for (c = 0; c < 60; c ++) { /* 1 �b�҂� */
	    sceGsSyncV (0);
	}

	/* ���� */
	sceSdRemote (1, rSdSetSwitch, SD_CORE_0 | SD_S_KOFF, 1 << v);
    }

    /* �O�̂��� CORE0 �S�Ẵ{�C�X������ */
    sceSdRemote (1, rSdSetSwitch, SD_CORE_0 | SD_S_KOFF, 0xffffff);

    PRINTF(("voice completed...\n"));

    return 0;
}

/* ----------------------------------------------------------------
 * �X�^�[�g�֐�
 * ---------------------------------------------------------------- */

int
main (int argc, char *argv [])
{
    int thid;

    /* SIF RPC �̏����� */
    sceSifInitRpc (0);

    /* IOP ���W���[���̃��[�h */
    load_modules ();

    /* �R���g���[���֘A�̏����� */
    scePadInit (0);
    scePadPortOpen (0, 0, pad_dma_buf);

    /* IOP heap �����̏����� */
    sceSifInitIopHeap ();

    thid = GetThreadId ();
    ChangeThreadPriority (thid , 10);

    /* libsdr �̏����� */
    sceSdRemoteInit();
    thid = sceSdRemoteCallbackInit (5);

    /* ���C������ */
    sound_test ();

    /* �R�[���o�b�N�E�X���b�h�̍폜 */
    if (TerminateThread (thid) == thid) {
	DeleteThread (thid);
    }

    return 0;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
