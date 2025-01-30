/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2001, 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <eekernel.h>
#include <sif.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libpad.h>
#include <libgraph.h>		/* sceGsSyncV() */
#include <libsdr.h>
#include <libspu2m.h>

#include <sk/common.h>
#include <sk/errno.h>
#include <sk/sk.h>
#include <sk/sound.h>

/*
 * IOP �g�p���W���[��
 * ---------------------------------------------------------------- */

#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"

#define _MOD_SIO2MAN   "sio2man.irx"
#define _MOD_PADMAN    "padman.irx"
#define _MOD_LIBSD     "libsd.irx"
#define _MOD_SDRDRV    "sdrdrv.irx"
#define _MOD_MODHSYN   "modhsyn.irx"
#define _MOD_MODMIDI   "modmidi.irx"
#define _MOD_SK_SOUND  "sksound.irx"
#define _MOD_SK_HSYNTH "skhsynth.irx"
#define _MOD_SK_MIDI   "skmidi.irx"

/*
 * variables
 * ---------------------------------------------------------------- */

// �X���b�h ID
static int thid_main, thid_sound;

// �Z�}�t�H
int sema_main;

// ���C�������X���b�h�̃X�^�b�N
#define STACK_SIZE_MAIN (512 * 128)
static unsigned char stack_main [STACK_SIZE_MAIN] __attribute__((aligned(16)));

// �]���p EE ���ꎞ�o�b�t�@
#define EEBUF_SIZE (2048 * 64)
static unsigned char eebuf [EEBUF_SIZE] __attribute__((aligned(64)));

// �]���p IOP ���ꎞ�o�b�t�@�̃T�C�Y
#define IOPBUF_SIZE (64 * 1024)			/* 64 KB */

// sceSpu2MemAllocate() �ɂ�� SPU2 ���[�J���������� 32 �̈�m�ۉ\ 
#define SPU2MEM_AREANUM 32
static unsigned char spu2mem_table [SCESPU2MEM_TABLE_UNITSIZE * (SPU2MEM_AREANUM + 1)];

/*
 * functions
 * ---------------------------------------------------------------- */

// �ēǂݍ��݋@�\�t read �֐�
static int
readX (int fd, unsigned char *rbuf, int size)
{
    int req = size;
    int red = 0;
    int ret = 0;

    while (red < size) {	/* �v���ʂ����t�@�C���̓��e��ǂ� */
	ret = sceRead (fd, &(rbuf [red]), req);
	if (ret < 0) {
	    red = -1;
	    break;
	}
	red += ret;
	if (ret < req) {	/* �ǂ߂��o�C�g�����v���ʂ�菭�Ȃ� */
	    req -= ret;		/* �ǂ߂��o�C�g�������v���ʂ����炷 */
	}
    }

    return red;			/* �ŏI�I�ɓǂ߂��o�C�g�� or (-1) */
}

/*
 * �]���֐�
 *	�t�@�C���̓��e �� EE ������ (�ꎞ�o�b�t�@) ��
 *		IOP ������ (�ꎞ�o�b�t�@) �� SPU2 ���[�J��������
 *    ����:
 *	fname:		�t�@�C����
 *	buf:		EE ���������̈ꎞ�o�b�t�@�̐擪�A�h���X
 *      bufsize:	EE ���������̈ꎞ�o�b�t�@�̃T�C�Y
 *	ret_spu_addr:	(�Ԃ�l) �m�ۂ��� SPU2 ���[�J�����������̗̈�̐擪�A�h���X
 *	ret_size:	(�Ԃ�l) �m�ۂ��� SPU2 ���[�J�����������̗̈�̃T�C�Y
 *    �Ԃ�l:
 *	0:		����I��
 *	< 0:		�ُ�I��
 */
static int
sendtoSPU (char *fname,
	   char *buf, unsigned int bufsize,
	   unsigned int *ret_spu_addr, unsigned int *ret_size)
{
    int fd, fsize, red, req;
    unsigned char *iop_addr;
    unsigned int iop_size;
    unsigned int spu2_addr, saddr;
    int sent_size, rest, rsize;
    sceSifDmaData ddata;

    // �I�[�v��
    if ((fd = sceOpen (fname, SCE_RDONLY)) < 0) {
	// Can't open file [fname]
	return (-1);
    }

    // �t�@�C���T�C�Y���擾
    if ((fsize = sceLseek (fd, 0, SCE_SEEK_END)) <= 0) {
	// File read something wrong [fname]
	sceClose (fd);
	return (-2);
    }
    sceLseek (fd, 0, SCE_SEEK_SET); /* �擪�ɖ߂� */

    // IOP �������Ɉꎞ�̈���m��
    iop_size = IOPBUF_SIZE;
    if (iop_size > bufsize) {
	iop_size = bufsize;
    }
    if ((iop_addr = (unsigned char *) sceSifAllocSysMemory (0, iop_size, NULL)) == NULL) {
	// IOP heap, not enough memory [iop_size]
	sceClose (fd);
	return (-3);
    }

    // SPU2 ���[�J���������ɗ̈���m��
    if ((spu2_addr = sceSpu2MemAllocate (fsize)) < 0) {
	// SPU2 local memory, not enough memory [fsize]
	sceSifFreeSysMemory ((void *)iop_addr);	
	sceClose (fd);
	return (-4);
    }

    // �����]��
    sent_size = 0;
    ddata.data = (unsigned int)buf;		// EE top (fixed area)
    ddata.addr = (unsigned int)iop_addr;	// IOP top (fixed area)
    ddata.size = iop_size;			// size
    ddata.mode = 0;				// always 0
    saddr = spu2_addr;
    while (sent_size < fsize) {
	rest = fsize - sent_size;		// �c��T�C�Y���v�Z
	if (rest > iop_size) rest = iop_size;	// �ꎞ�o�b�t�@�̃T�C�Y��
	rsize = readX (fd, buf, rest);		// �f�[�^���o�b�t�@�ɓǂݍ���
	if (rsize < 0) {	// �ǂݍ��ݎ��s
	    // File Read Error
	    sceSifFreeSysMemory ((void *)iop_addr);
	    sceSpu2MemFree (spu2_addr);
	    sceClose (fd);
	    return (-4);
	}
	FlushCache (0);
	ddata.size = rest;			// �]���T�C�Y
	if (sceSifSetDma (&ddata, 1) == 0) {	// �]��
	    // EE -> IOP transfer Error
	    sceSifFreeSysMemory ((void *)iop_addr);
	    sceSpu2MemFree (spu2_addr);
	    sceClose (fd);
	    return (-4);
	}
	sceSkSsSend (SCESK_SEND_IOP2SPU2,	// IOP memory -> SPU2 local memory
		     1,	/* DMA ch */
		     iop_addr, saddr, rest);
	sent_size  += rest;
	saddr      += rest;
    }

    // IOP ���������̈ꎞ�o�b�t�@�̈�����
    sceSifFreeSysMemory ((void *)iop_addr);

    // �N���[�Y
    sceClose (fd);

    // �Ԃ�l
    *ret_spu_addr = (unsigned int)spu2_addr;
    *ret_size     = (unsigned int)fsize;

    return (0);
}

/*
 * �]���֐�
 *	�t�@�C���̓��e �� IOP ������
 *
 *    ����:
 *	fname:		�t�@�C����
 *	ret_iop_addr:	(�Ԃ�l) �m�ۂ��� IOP ���̗̈�̐擪�A�h���X
 *	ret_size:	(�Ԃ�l) �m�ۂ��� IOP ���̗̈�̃T�C�Y
 *    �Ԃ�l:
 *	0:		����I��
 *	< 0:		�ُ�I��
 */
static int
sendtoIOP (char *fname,
	   unsigned int *ret_iop_addr, unsigned int *ret_size)
{
    int fd, fsize, red, req;
    unsigned char *iop_addr;
    int sent_size, rest, rsize;
    sceSifDmaData ddata;

    // �I�[�v��
    if ((fd = sceOpen (fname, SCE_RDONLY)) < 0) {
	// Can't open file [fname]
	return (-1);
    }

    // �t�@�C���T�C�Y���擾
    if ((fsize = sceLseek (fd, 0, SCE_SEEK_END)) <= 0) {
	// File read something wrong [fname]
	sceClose (fd);
	return (-2);
    }
    sceLseek (fd, 0, SCE_SEEK_SET); /* �擪�ɖ߂� */

    // IOP �������ɗ̈���m��
    if ((iop_addr = (unsigned char *) sceSifAllocSysMemory (0, fsize, NULL)) == NULL) {
	// IOP heap not enough memory [fsize]
	sceClose (fd);
	return (-3);
    }

    // �t�@�C���T�C�Y���擾�ł����̂Ńt�@�C�����N���[�Y
    sceClose (fd);

    // �t�@�C���̑S���e�� IOP �������ɓǂݍ���
    if (sceSifLoadIopHeap (fname, iop_addr) < 0) {
	// File read is something wrong
	sceSifFreeSysMemory (iop_addr);
	return (-4);
    }

    // �m�ۂ��� IOP ���������̗̈�̏���Ԃ�
    *ret_iop_addr = (unsigned int)iop_addr;
    *ret_size     = (unsigned int)fsize;

    return (0);
}

/*
 * ���C�������X���b�h
 *	�T�E���h�f�[�^��ǂݍ���ŉ��t�B���t�I����A�X���b�h�I��
 *
 *    ����:
 *	argp:	�����I������ signal ���o���Z�}�t�H ID
 */
static void
main_sound (void *argp)
{
    int ret;
    unsigned int sq_addr, sq_size;
    unsigned int hd_addr, hd_size;
    unsigned int bd_addr, bd_size;
    int hdbd_id, sq_id, track_id;

    /*
     * �f�[�^�̓ǂݍ���
     * ---------------------------------------------------------------- */

    /*
     * SQ �f�[�^�̓ǂݍ���
     */
    if ((ret = sendtoIOP ("host0:/usr/local/sce/data/sound/seq/overload.sq",
			  &sq_addr, &sq_size)) < 0) {
	SignalSema (*(int *)argp);		// main() �ɖ߂�
	return;
    }
    scePrintf ("SQ: (%08x) %d\n", sq_addr, sq_size);

    /*
     * HD �f�[�^�̓ǂݍ���
     */
    if ((ret = sendtoIOP ("host0:/usr/local/sce/data/sound/wave/overload.hd",
			  &hd_addr, &hd_size)) < 0) {
	sceSifFreeSysMemory ((void *)sq_addr);
	SignalSema (*(int *)argp);		// main() �ɖ߂�
	return;
    }
    scePrintf ("HD: (%08x) %d\n", hd_addr, hd_size);

    /*
     * BD �f�[�^�̓ǂݍ��� ... IOP ���������炳��� SPU2 ���[�J���������֓]��
     *                     ... bd_addr: SPU2 ���[�J�����������̃A�h���X
     */
    if ((ret = sendtoSPU ("host0:/usr/local/sce/data/sound/wave/overload.bd",
			  eebuf, EEBUF_SIZE, &bd_addr, &bd_size)) < 0) {
	sceSifFreeSysMemory ((void *)hd_addr);
	sceSifFreeSysMemory ((void *)sq_addr);
	SignalSema (*(int *)argp);		// main() �ɖ߂�
	return;
    }
    scePrintf ("BD: (%08x) %d\n", bd_addr, bd_size);
    
    /*
     * Standard Kit / Sound System ... �e�f�[�^�̓o�^
     * ---------------------------------------------------------------- */

    /*
     * HD/BD �f�[�^�̓o�^
     */
    hdbd_id = sceSkSsBindHDBD (SCESK_AUTOASSIGNMENT,
			       hd_addr, hd_size,  /* HD �A�h���X/�T�C�Y��� */
			       bd_addr, bd_size); /* BD �A�h���X/�T�C�Y��� */

    scePrintf ("BIND HDBD / return: %d\n", hdbd_id);

    /*
     * SQ �f�[�^�̓o�^
     */
    sq_id = sceSkSsBindSQ (MIDI, SCESK_AUTOASSIGNMENT,
			   sq_addr, sq_size);	/* SQ �A�h���X/�T�C�Y��� */

    scePrintf ("BIND SQ (MIDI) / return: %d\n", sq_id);

    /*
     * HD/BD �f�[�^ ID ���A�g���b�N (ID: �������蓖��) �ɓo�^
     */
    track_id = sceSkSsBindTrackHDBD (SCESK_AUTOASSIGNMENT,
				     hdbd_id,
				     0);	/* �o���N�ԍ�: 0 */

    scePrintf ("BIND TRACK (HDBD) / return: %d\n", track_id);

    /*
     * SQ �f�[�^ ID ���AHD/BD �f�[�^ ID ��o�^�����g���b�N�ɓo�^
     */
    ret = sceSkSsBindTrackSQ (MIDI, track_id, sq_id);

    scePrintf ("BIND TRACK (SQ (MIDI)) / return: %d\n", ret);

    /*
     * Standard Kit / Sound System ... ���t
     * ---------------------------------------------------------------- */

    /*
     * MIDI �`�����N�����t
     *		0: MIDI Data Block #
     */
    ret = sceSkSsPlayMIDI (track_id, 0);

    scePrintf ("Play (SQ (MIDI)) %d / return: %d\n", track_id, ret);

    /*
     * �Đ��w�蒼��́AIOP ���̍Đ����n�܂��Ă��Ȃ��ꍇ������B
     * ... ���t�J�n��AsceSkSsStatusMIDI(,SCESK_ISPLAYING) �� 0 �ȊO��
     *     �Ԃ��΁A���t���J�n����Ă���
     */
    while (1) {
	if (sceSkSsStatusMIDI (track_id, SCESK_ISPLAYING) != 0) {
	    break;
	}
	sceGsSyncV (0);
    }

    /*
     * ���t�I����҂�
     */
    while (1) {
	if (! sceSkSsStatusMIDI (track_id, SCESK_ISPLAYING)) {
	    break;
	}
	sceGsSyncV (0);
    }

    /*
     * MIDI �`�����N�̉��t���~
     */
    ret = sceSkSsStopMIDI (track_id);

    scePrintf ("Stop (SQ (MIDI)) %d / return: %d\n", track_id, ret);

    /*
     * Standard Kit / Sound System ... �I������
     * ---------------------------------------------------------------- */

    /*
     * �g���b�N���� HD/BD �f�[�^ ID �̊��蓖�Ă�����
     */
    ret = sceSkSsUnbindTrackHDBD (track_id);

    scePrintf ("UNBIND TRACK (HDBD) / return: %d\n", ret);

    /*
     * �g���b�N���� SQ �f�[�^ ID �̊��蓖�Ă�����
     */
    ret = sceSkSsUnbindTrackSQ (MIDI, track_id);

    scePrintf ("UNBIND TRACK ((SQ (MIDI)) / return: %d\n", ret);

    /*
     * HD/BD �f�[�^ ID ������
     */
    ret = sceSkSsUnbindHDBD (hdbd_id);

    scePrintf ("UNBIND HDBD / return: %d\n", ret);

    /*
     * SQ �f�[�^ ID ������
     */
    ret = sceSkSsUnbindSQ (MIDI, sq_id);

    scePrintf ("UNBIND SQ (MIDI) / return: %d\n", ret);

    /*
     * �̈�����
     * ---------------------------------------------------------------- */

    /*
     * sendtoSPU() ���Ŋm�ۂ����ABD �f�[�^ (SPU2 ���[�J����������) �̗̈�����
     */
    sceSpu2MemFree (bd_addr);

    /*
     * sendtoIOP() ���Ŋm�ۂ����ASQ/HD �f�[�^ (IOP ��������) �̗̈�����
     */
    sceSifFreeSysMemory ((void *)hd_addr);
    sceSifFreeSysMemory ((void *)sq_addr);

    /*
     * ���C�������X���b�h���I��
     * ---------------------------------------------------------------- */

    SignalSema (*(int *) argp);		// main() �ɖ߂�
    return;
}

/*
 * SPU2 �ݒ�
 */
void
setup_spu2 (void)
{
    // ���f�W�^���o�͂̐ݒ�
    sceSkSsSetDigitalOut (SCESK_DOUT_CD_NORMAL);

    // �G�t�F�N�g�̐ݒ�
    sceSkSsSetEffectMode (SCESK_EFFECT_MODE_HALL | SCESK_EFFECT_MODE_CLEAR_WA,
			  0, 0); /* delay, feedback */
    sceSkSsSetEffect (SCESK_EFFECT_ON); 
    sceSkSsSetEffectVolume (SCESK_EFFECT_VOLUME_MAX / 2,  /* L */
			    SCESK_EFFECT_VOLUME_MAX / 2); /* R */

    /*
     * �}�X�^�[�{�����[���ݒ�
     */
    sceSkSsSetMasterVolume (SCESK_MASTER_VOLUME_MAX,  /* L */
			    SCESK_MASTER_VOLUME_MAX); /* R */

    return;
}

#define LOAD_MODULE(mod) \
while (sceSifLoadModule (_MOD_ROOT mod, 0, NULL) < 0) { \
    scePrintf ("  loading %s failed\n", mod); \
}

/*
 * �X�^�[�g�G���g��
 */
int
main (int argc, char *argv [])
{
    struct ThreadParam tp;
    struct SemaParam sp;

    // SIF �֘A�̏�����
    sceSifInitRpc (0);
    sceSifInitIopHeap ();

    // ���W���[���̃��[�h
    LOAD_MODULE (_MOD_SIO2MAN);
    //LOAD_MODULE (_MOD_PADMAN);	/* ���̃T���v���ł̓R���g���[���͖��g�p */
    LOAD_MODULE (_MOD_LIBSD);		/* �჌�x���T�E���h���C�u���� */
    LOAD_MODULE (_MOD_SDRDRV);		/* �჌�x���T�E���h���C�u���� (for EE) */
    LOAD_MODULE (_MOD_MODHSYN);		/* �n�[�h�E�F�A�V���Z�T�C�U */
    LOAD_MODULE (_MOD_MODMIDI);		/* MIDI �V�[�P���T */
    LOAD_MODULE (_MOD_SK_SOUND);	/* �W���L�b�g: �T�E���h�t�@���_�����^�� */
    LOAD_MODULE (_MOD_SK_HSYNTH);	/* �W���L�b�g: �n�[�h�E�F�A�V���Z�T�C�U */
    LOAD_MODULE (_MOD_SK_MIDI);		/* �W���L�b�g: MIDI �V�[�P���T */

    // Standard Kit ... ������
    sceSkInit (0);		/* Standard Kit �S�̂̏����� */
    sceSkSsInit (32);		/* SK/Sound System �̏�����:
				 *   ���� ... sdr ���荞�݃n���h���X���b�h�D��x */
    // SPU2 �������Ǘ��̏�����
    sceSpu2MemInit (spu2mem_table, SPU2MEM_AREANUM, /* �̈搔: SPU2MEM_AREANUM */
		    SCESPU2MEM_USE_EFFECT);	    /* �G�t�F�N�g�͎g�p���� */

    // SPU2 �̑S�ʂɊւ�鑮���̐ݒ�
    setup_spu2 ();

    /*
     * �҂��󂯃Z�}�t�H�̐���
     */
    sp.initCount = 0;
    sp.maxCount  = 1;
    sp.option    = 0;
    if ((sema_main = CreateSema (&sp)) < 0) { /* ���C�������X���b�h�I���p */
	// Can't create semaphore
	return (-1);
    }

    /*
     * ���C�������X���b�h�̐����ƋN��
     */
    tp.stackSize    = STACK_SIZE_MAIN;
    tp.gpReg 	    = &_gp;
    tp.entry 	    = main_sound;
    tp.stack 	    = (void *) stack_main;
    tp.initPriority = 3;
    if ((thid_main = CreateThread (&tp)) < 0) {
	// Can't create thread
	return (-2);
    }
    StartThread (thid_main, (void *)&sema_main);

    // ���C�������X���b�h�̏I����҂�
    WaitSema (sema_main);

    // ���\�[�X�����
    if (TerminateThread (thid_main) == thid_main) {
	DeleteThread (thid_main);
    }
    DeleteSema (sema_main);

    // SPU2 �������Ǘ��̏I��
    sceSpu2MemQuit();

    // Fine ...
    scePrintf ("Fine ...\n");

    return (0);
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
