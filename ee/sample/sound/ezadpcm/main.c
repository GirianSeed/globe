/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *              Emotion Engine Library Sample Program
 *                          Version 0.11
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *	                  EzADPCM - main.c
 *		            main routine
 *
 *	Version		Date		Design	Log
 *  --------------------------------------------------------------------
 *	0.10		Feb. 3, 2000	kaol
 *	0.11		Feb.27, 2000	kaol	BUFFER_SIZE is modified
 */

#include <eekernel.h>
#include <string.h>	/* for strcpy() */
#include <sifdev.h>
#include <libgraph.h>	/* for sceGsSyncV() */
#include <libsdr.h>
#include <sdrcmd.h>
#include <libpad.h>
#include "ezadpcm.h"

/* IOP ���̔g�`�f�[�^�p�o�b�t�@�T�C�Y */
#define BUFFER_SIZE      499712

/* ADPCM �X�g���[�~���O�̏�� */
#define _RUN  1				/* �ғ��� */
#define _STOP 0				/* ��~�� */
/* LR �`�����l���w�� */
#define _L 0
#define _R 1

/* �{�����[�� */
#define _VOLUME 0x3fff			/* �{�����[�������l */
#define _LR(x) ((x) << 16 | (x))	/* �{�����[���w��p�ȈՃ}�N�� */

/* �g�`�f�[�^�t�@�C���� */
char gFilename[2][64] __attribute__((aligned (64)));
					/* gFilename[0]: L �`�����l���p
					   gFilename[1]: R �`�����l���p */

/* �O���֐� (rpc.c) */
extern int ezAdpcm_RPCinit (void);
extern int ezAdpcm_RPC (int command, int arg);

/* �g�p���� IOP ���W���[�� */
#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"
#define _MOD_SIO2MAN "sio2man.irx"
#define _MOD_PADMAN  "padman.irx"
#define _MOD_LIBSD   "host0:/usr/local/sce/iop/modules/libsd.irx"
#define _MOD_EZADPCM "host0:/usr/local/sce/iop/sample/sound/ezadpcm/ezadpcm.irx"

/* �R���g���[����Ɨp�o�b�t�@ */
u_long128 pad_dma_buf [scePadDmaBufferMax]  __attribute__((aligned (64)));

/*
 * load_modules()	���W���[���̃��[�h
 *	����:   �Ȃ�
 *	�Ԃ�l: �Ȃ�
 */
void
load_modules (void)
{
    /* SIF RPC �̏����� */
    sceSifInitRpc (0);

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
    while (sceSifLoadModule(_MOD_LIBSD, 0, NULL) < 0) {
	scePrintf ("  loading libsd.irx failed\n");
    }
    /* EzADPCM �p�h���C�o���W���[�� */
    while (sceSifLoadModule (_MOD_EZADPCM, 0, NULL) < 0) {
	scePrintf ("  loading ezadpcm.irx failed\n");
    }

    scePrintf ("loading modules ... done.\n");

    return;
}

/*
 * init_adpcm()		IOP ���̃Z�b�g�A�b�v:
 *			�t�@�C���I�[�v���A�{�C�X�w��A�{�����[���ݒ�Ȃ�
 *	����:   �Ȃ�
 *	�Ԃ�l:
 *		0 ... �Z�b�g�A�b�v����I��
 *	       -1 ... �G���[
 */
int
init_adpcm (void)
{
    int info;

    /* SIF RPC �ڑ��̊m�� */
    if (ezAdpcm_RPCinit () < 0)
	return (-1);

    /* IOP ���჌�x���T�E���h���W���[���̏����� */
    ezAdpcm_RPC (EzADPCM_SDINIT, SD_INIT_COLD); /* = sceSdInit() */
    /* �}�X�^�[�{�����[���̐ݒ� */
    ezAdpcm_RPC (SD_CORE_0 | EzADPCM_SETMASTERVOL, _LR (0x3fff)); /* CORE0 master vol */
    ezAdpcm_RPC (SD_CORE_1 | EzADPCM_SETMASTERVOL, _LR (0x3fff)); /* CORE1 master vol */

    /* IOP ���Ŕg�`�f�[�^�t�@�C�����I�[�v�� */
    info = ezAdpcm_RPC (_L | EzADPCM_OPEN, (int) gFilename[_L]); /* L �`�����l���p */
    scePrintf ("  VB:%d %s  %d byte(body)\n", _L, gFilename[_L], info);

    info = ezAdpcm_RPC (_R | EzADPCM_OPEN, (int) gFilename[_R]); /* L �`�����l���p */
    scePrintf ("  VB:%d %s  %d byte(body)\n", _R, gFilename[_R], info);

    /* IOP ���Ŕg�`�f�[�^�p�̃������̈���m�� */
    ezAdpcm_RPC (EzADPCM_INIT, BUFFER_SIZE);

    /* ADPCM �X�g���[�~���O�Ŏg�p����{�C�X���w��
     * ���݂̎����ł� CORE0 ���� */
    ezAdpcm_RPC (_L | EzADPCM_SETVOICE, 22); /* �{�C�X 22 */
    ezAdpcm_RPC (_R | EzADPCM_SETVOICE, 23); /* �{�C�X 23 */
    ezAdpcm_RPC (EzADPCM_SETVOL, _LR(_VOLUME)); /* �{�����[�������l���w�� */

    return (0);
}

/*
 * main_loop()		�����̃��C�����[�v:
 *			�R���g���[�����͂��� IOP �� RPC
 *	����:   �Ȃ�
 *	�Ԃ�l: �Ȃ�
 */
void
main_loop (void)
{
    int run_status;		/* IOP ���̃X�g���[�~���O�� */
    int running = _STOP;	/*  EE ���̃X�g���[�~���O�w���� */
    u_int paddata, oldpad;
    u_char rdata [32];
    int vol = _VOLUME;		/* �{�����[�������l */
    int isstop = 0;		/* �v���O�����I���t���O */

    oldpad = paddata = 0;
    while (1) {
	/* �R���g���[�����̓ǂݍ��� */ 
#define _PAD_DATA (0xffff ^ ((rdata[2] << 8) | rdata[3]))
	if (scePadRead (0, 0, rdata) > 0) paddata = _PAD_DATA;
	else                              paddata = 0;

#ifdef DEBUG
	if (paddata != 0) scePrintf ("%08x\n", paddata);
#endif

	/* isstop:
	   0      ... �Z���N�g�{�^����������Ă��Ȃ� �� �ʏ퓮��
	   0 �ȊO ... �Z���N�g�{�^���������ꂽ       �� �v���O�����I���� */
	if (isstop == 0) {
	    /*
	     * �R���g���[�����ɂ�鏈��
	     */
	    if (paddata != oldpad) {
		/* IOP ���̃X�g���[�~���O�ғ���Ԃ̎擾 */
		run_status = ezAdpcm_RPC (EzADPCM_GETSTATUS, 0);
		if (paddata & SCE_PADstart) {
		    // �X�g���[�~���O���J�n/��~
		    if (running == _STOP) {
			/* �X�g���[�~���O���J�n */
			ezAdpcm_RPC (EzADPCM_PRELOADSTART, 0);
			running = _RUN;
		    } else {
			if (run_status == EzADPCM_STATUS_RUNNING) {
			    /* �X�g���[�~���O���~ */
			    ezAdpcm_RPC (EzADPCM_STOP, 0);
			    running = _STOP;
			}
		    }
		} else if (paddata & SCE_PADselect) {
		    /* �v���O�����I�� */
		    ezAdpcm_RPC (EzADPCM_SETVOL, 0);	/* �{�����[���� 0 �� */
		    ezAdpcm_RPC (EzADPCM_STOP,   0);	/* �X�g���[�~���O���~ */
		    isstop ++;				/* �I�������֑J�� */
		}
	    }
#define _VOL_DELTA 0x80
	    if (paddata & SCE_PADLup) {
		/* �{�����[�����グ�� */
		vol += _VOL_DELTA;
		if (vol > 0x3fff) vol = 0x3fff;
		ezAdpcm_RPC (EzADPCM_SETVOLDIRECT, _LR (vol)); /* �{�����[���ݒ� */
	    } else if (paddata & SCE_PADLdown) {
		/* �{�����[���������� */
		vol -= _VOL_DELTA;
		if (vol < 0) vol = 0;
		ezAdpcm_RPC (EzADPCM_SETVOLDIRECT, _LR (vol)); /* �{�����[���ݒ� */
	    }

	} else {
	   /* �Z���N�g�{�^���������ꂽ �� �v���O�����I���� */

	    if (ezAdpcm_RPC (EzADPCM_GETSTATUS, 0) == EzADPCM_STATUS_IDLE) {
		/* IOP ���̏������I�����AIDLE ��ԂɂȂ��� */
		ezAdpcm_RPC (EzADPCM_CLOSE, 0); /* �g�`�f�[�^�t�@�C�����N���[�Y */
		/* �}�X�^�[�{�����[���� 0 �� */
		ezAdpcm_RPC (SD_CORE_0 | EzADPCM_SETMASTERVOL, 0);
		ezAdpcm_RPC (SD_CORE_1 | EzADPCM_SETMASTERVOL, 0);
		/* IOP ���I������ */
		ezAdpcm_RPC (EzADPCM_QUIT, 0);
		return;
	    }
	}
	sceGsSyncV(0);
	oldpad = paddata;
    }
    return;
}

/*
 * main()		���C���֐�
 */
int
main (int argc, char *argv [])
{
    /* IOP ���W���[���̃��[�h */
    load_modules ();

    /* �R���g���[���֘A�̏����� */
    scePadInit (0);
    scePadPortOpen (0, 0, pad_dma_buf);

    scePrintf ("EzADPCM initialize ...\n");

    /* �v���O�����N�����̈����̐����`�F�b�N */
    if (argc != 3) {
	scePrintf ("\n\nusage: %s <L-ch file>.vb <R-ch file>.vb\n\n", argv [0]);
	return 0;
    }

    /* �g�`�f�[�^�t�@�C���� */
    strcpy (gFilename [_L], argv [_L + 1]);
    strcpy (gFilename [_R], argv [_R + 1]);

    /* IOP ���̃Z�b�g�A�b�v */
    init_adpcm ();

    scePrintf ("EzADPCM initialize ... done\n");

    /* �X�g���[�~���O�������R���g���[���Ő��䂷�� */
    main_loop ();

    scePrintf ("EzADPCM finished...\n");
    return 0;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
