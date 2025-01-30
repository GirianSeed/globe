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
#include <sk/stream.h>

#define Xprintf scePrintf

// #define DEBUG
#ifdef DEBUG
#define PRINTF(x) Xprintf x
#define DBGPROC(x) x
#else
#define PRINTF(x)
#define DBGPROC(x)
#endif

/*
 * IOP �g�p���W���[��
 * ---------------------------------------------------------------- */

#define LOAD_MODULE(mod) \
while (sceSifLoadModule (_MOD_ROOT mod, 0, NULL) < 0) { \
    scePrintf ("  loading %s failed\n", mod); \
}

#define DATA_DIR     "host0:/usr/local/sce/data/sound/wave/"
#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"

#define _MOD_SIO2MAN     "sio2man.irx"
#define _MOD_PADMAN      "padman.irx"
#define _MOD_LIBSD       "libsd.irx"
#define _MOD_STRADPCM    "stradpcm.irx"
#define _MOD_SDRDRV      "sdrdrv.irx"
#define _MOD_MODHSYN     "modhsyn.irx"
#define _MOD_MODMIDI     "modmidi.irx"
#define _MOD_SK_SOUND    "sksound.irx"
#define _MOD_SK_STRADPCM "sksadpcm.irx"

/*
 * �萔�}�N��
 * ---------------------------------------------------------------- */

// �X�g���[�����
#define N_STREAM	3 // BGM 2 channels + SE 1 channel

// �o�b�t�@�T�C�Y
#define SPU_BUF_SIZE	0x4000	/* SPU2 ���[�J���o�b�t�@�� */
#define IOP_BUF_SIZE	(SPU_BUF_SIZE * 8) /* IOP �������������O�o�b�t�@ */
#define CPY_BUF_SIZE	(SPU_BUF_SIZE * 1) /*  */

#define EE_UNCACHE_MASK 0x20000000

// �T�E���h�X���b�h�w���p�R�}���h
#define _IDLE     0
#define _PRELOAD  1
#define _PLAY     2
#define _STOP     3
#define _PAUSE    4
#define _MUTE     5
#define _PCHANGE  6
#define _QUIT     (-1)
#define _PLAY_ALL 1000

// �X�g���[���`�����l��
#define _TR_CH_BGM_L 0
#define _TR_CH_BGM_R 1
#define _TR_CH_ONE   2

// �X�g���[���`�����l�� (�r�b�g�\��)
#define _TR_BGM		((1 << _TR_CH_BGM_L) | (1 << _TR_CH_BGM_R))
#define _TR_ONE		 (1 << _TR_CH_ONE)
#define _STREAM_ALL	 (_TR_BGM | _TR_ONE)

// ID �����蓖��
#define _NO_ID (-1)

// �T���v������: �X�g���[�������Đ���L���ɂ��ăR���p�C�� & ���s
#define STREAM_AUTOPLAY 1
// �T���v������: �G�t�F�N�g��L���ɂ��ăR���p�C�� & ���s
#define STREAM_EFFECT   0

#if STREAM_AUTOPLAY != 0
#define STREAM_OPT_AUTOPLAY SCESK_STRADPCM_OPTION_AUTOPLAY
#else
#define STREAM_OPT_AUTOPLAY 0
#endif
#if STREAM_EFFECT != 0
#define STREAM_OPT_EFFECT SCESK_STRADPCM_OPTION_EFFECT
#else
#define STREAM_OPT_EFFECT 0
#endif

// �T���v������: �X�g���[���I�v�V����
#define STREAM_OPTIONS (STREAM_OPT_AUTOPLAY | STREAM_OPT_EFFECT)

// �T���v������: �|�[�Y�w���Ń~���[�g����
#define PAUSE_IS_MUTE 0

// �X�g���[���{�����[��: 10% ���}���Ă���
#define VOLUME_L ((SCESK_STRADPCM_VOLUME_MAX * 90) / 100)
#define VOLUME_R ((SCESK_STRADPCM_VOLUME_MAX * 90) / 100)

// �R���g���[������
#define _PAD_PRELOAD_ONE SCE_PADRdown
#define _PAD_PRELOAD_BGM SCE_PADRright
#if STREAM_AUTOPLAY == 0
#define _PAD_PLAY_ONE    SCE_PADRleft
#define _PAD_PLAY_BGM    SCE_PADRup
#endif
#define _PAD_PAUSE_ONE   SCE_PADR1
#define _PAD_MUTE_ONE    SCE_PADL1
#define _PAD_MUTE_BGM    SCE_PADL2
#define _PAD_STOP_ONE    SCE_PADLdown
#define _PAD_STOP_BGM    SCE_PADLright
#define _PAD_PCHANGE_ONE SCE_PADLleft
#define _PAD_PCHANGE_BGM SCE_PADLup
#define _PAD_PLAY_ALL    SCE_PADstart
#define _PAD_QUIT        SCE_PADselect

// ADPCM �u���b�N�� LOOP �t���O�� LOOP �݂̂ɂ���
#define ADPCM_FLAG_LOOP  0x02
#define ADPCM_SETFLAG_LOOP(a) { \
  *((unsigned char *)((a)+1)) = ADPCM_FLAG_LOOP; \
}

/*
 * �ϐ�
 * ---------------------------------------------------------------- */

// �R���g���[����Ɨp�o�b�t�@
static u_long128 pad_dma_buf [scePadDmaBufferMax] __attribute__((aligned(64)));

// �X���b�h ID
static int thid_main;		// ���C�������X���b�h
static int thid_sound;		// �T�E���h�����X���b�h

// �Z�}�t�H
int sema_main;			// ���C�������X���b�h�I���p�Z�}�t�H
int sema_sound;			// �T�E���h�����X���b�h�p�Z�}�t�H
int sema_sound_send;		// �T�E���h�����R�}���h�p�Z�}�t�H
int sema_sound_complete;	// �T�E���h�����R�}���h�I���҂��󂯗p�Z�}�t�H

// ���C�������X���b�h�̃X�^�b�N
#define STACK_SIZE_MAIN (1024 * 128)
static unsigned char stack_main [STACK_SIZE_MAIN] __attribute__((aligned(64)));

// �T�E���h�����X���b�h�̃X�^�b�N
#define STACK_SIZE_SOUND (512 * 128)
static unsigned char stack_sound [STACK_SIZE_SOUND] __attribute__((aligned(64)));

// sceSpu2MemAllocate() �ɂ�� SPU2 ���[�J���������� 32 �̈�m�ۉ\ 
#define SPU2MEM_N_AREAS 32
static unsigned char spu2mem_table [SCESPU2MEM_TABLE_UNITSIZE * (SPU2MEM_N_AREAS + 1)];

// �X�g���[�����\����
SceSkSsStrAdpcmEnv strenv;

// �X�g���[����� (�r�b�g�p�^�[��)
unsigned int stream_bit  = 0;	// ���݉ғ����Ă���X�g���[��
unsigned int stop_bit    = 0;	// ��~������X�g���[��
unsigned int pause_bit   = 0;	// �|�[�Y����Ă���X�g���[��
unsigned int mute_bit    = 0;	// �~���[�g����Ă���X�g���[��
unsigned int pchange_bit = 0;	// �������ύX����Ă���X�g���[��

// �X�g���[�� ID, �g���b�N ID
int stradpcm_id = _NO_ID;
int track_id    = _NO_ID;

// EE ���������f�[�^�ǂݍ��݃o�b�t�@
unsigned int xaddr [N_STREAM][CPY_BUF_SIZE] __attribute__((aligned(64)));

// �T�E���h�����X���b�h�E�R�}���h���M
static int sound_command = _IDLE;
static int sound_index;
static int sound_value;
static int sound_return;

typedef struct stream_sound_info {
    char *fname;	/* �t�@�C����		*/
    int   fd;		/* file descriptor	*/
    int   size;		/* �t�@�C���T�C�Y	*/
    int   loop;		/* 1:���[�v / 0:1 �V���b�g */
    int   end;		/* 1:�f�[�^�I�� / 0:�ǂݍ��ݒ� (1 �V���b�g�f�[�^�̂�) */
} stream_sound_info_t;

// �X�g���[���g�`�f�[�^���
stream_sound_info_t stream_sound_info [N_STREAM] = {
    { DATA_DIR "tr1l_pad.vb", -1, 0, 1, 0 }, /* ���[�v */
    { DATA_DIR "tr1r_pad.vb", -1, 0, 1, 0 }, /* ���[�v */
    { DATA_DIR "ps_mono.vb",  -1, 0, 0, 0 }, /* 1 �V���b�g */
};

/*
 * �֐�
 * ---------------------------------------------------------------- */

/*
 *	�t�@�C���̃I�[�v���ƃT�C�Y�擾
 */
static void
open_file_ch (int ch)
{
    int fd, size;

    while ((fd = sceOpen (stream_sound_info [ch].fname, SCE_RDONLY)) < 0) {
	scePrintf ("[[ Error: bad filename %s ]]\n",
		   stream_sound_info [ch].fname);
    }
    while ((size = sceLseek (fd, 0, SCE_SEEK_END)) < 0) {
	scePrintf ("[[ Error: cannot seek %s ]]\n",
		   stream_sound_info [ch].fname);
    }
    while (sceLseek (fd, 0, SCE_SEEK_SET) < 0) {
	scePrintf ("[[ Error: cannot rewind %s ]]\n",
		   stream_sound_info [ch].fname);
    }
    stream_sound_info [ch].fd   = fd;
    stream_sound_info [ch].size = size;
}

static void
open_file (void)
{
    int ch;

    for (ch = 0; ch < N_STREAM; ch ++) {
	open_file_ch (ch);
    }
}

/*
 *	�t�@�C���̃N���[�Y
 */
static void
close_file_ch (int ch)
{
    sceClose (stream_sound_info [ch].fd);
}

static void
close_file (void)
{
    int ch;

    for (ch = 0; ch < N_STREAM; ch ++) {
	close_file_ch (ch);
    }
}

/*
 *	�X�g���[���f�[�^�̃v�����[�h
 */
static void
pre_read_file_ch (int ch)
{
    int rsize;

    stream_sound_info [ch].end = 0;
    while (sceLseek (stream_sound_info [ch].fd, 0, SCE_SEEK_SET) < 0) {
	scePrintf ("[[ Error: cannot seek %s ]]\n",
		   stream_sound_info [ch].fname);
    }
    // EE ���������f�[�^�ǂݍ��݃o�b�t�@���A�ǂݍ���
    rsize = sceRead (stream_sound_info [ch].fd,
		     (u_char *)strenv.buffer [ch].addr, CPY_BUF_SIZE);
    strenv.buffer [ch].validsize = CPY_BUF_SIZE;
}

static void
pre_read_file (unsigned int str_bit)
{
    int ch;

    for (ch = 0; ch < N_STREAM; ch ++) {
	if (str_bit & (1 << ch)) {
	    pre_read_file_ch (ch);
	}
    }
}

/*
 *	�X�g���[���f�[�^�̓ǂݍ��݈ʒu��擪�Ɋ����߂�
 */
static void
rewind_file_ch (int ch)
{
    while (sceLseek (stream_sound_info [ch].fd, 0, SCE_SEEK_SET) < 0) {
	scePrintf ("[[ Error: cannot seek %s ]]\n",
		   stream_sound_info [ch].fname);
    }
}

static void
rewind_file (int str_bit)
{
    int ch;
    for (ch = 0; ch < N_STREAM; ch ++) {
	if (str_bit & (1 << ch)) {
	    rewind_file_ch (ch);
	}
    }
}

/*
 *	�X�g���[���f�[�^�̃��[�h
 */
int
fill_data_ch (int ch)
{
    int ret;

    ret = 0;
    if (strenv.buffer [ch].validsize == 0) {
	unsigned char *addr = (unsigned char *)strenv.buffer [ch].addr;
	unsigned int   size = CPY_BUF_SIZE;
	int rsize;

	if (stream_sound_info [ch].end == 0) {
	    // 1 �V���b�g: �g�`�f�[�^�������܂œ��B���Ă��Ȃ�
	    //     ���[�v: ��� .end �� 0
	    rsize = sceRead (stream_sound_info [ch].fd, addr, size);
	    if (rsize < size) {
		if (stream_sound_info [ch].loop == 1) {
		    int xsize;
		    // ���[�v: �f�[�^�擪�܂Ŋ����߂�
		    if (rsize < 0) rsize = 0;
		    if (rsize >= 0x10) {
			// �ŏI�u���b�N�� LOOP �t���O�� LOOP �݂̂�
			ADPCM_SETFLAG_LOOP (addr + rsize - 0x10);
		    }
		    while (sceLseek (stream_sound_info [ch].fd, 0, SCE_SEEK_SET) < 0) {
			scePrintf ("[[ Error: cannot seek %s ]]\n",
				   stream_sound_info [ch].fname);
		    }
		    xsize = sceRead (stream_sound_info [ch].fd,
				     addr + rsize, size - rsize);
		    if (xsize >= 0x10) {
			// �擪�u���b�N�� LOOP �t���O�� LOOP �݂̂�
			ADPCM_SETFLAG_LOOP (addr + rsize);	// �擪�u���b�N
		    }
		    if (xsize >= 0x20) {
			// (�擪 + 1) �u���b�N�� LOOP �t���O�� LOOP �݂̂�
			ADPCM_SETFLAG_LOOP (addr + rsize + 0x10);
		    }
		} else {
		    // 1 �V���b�g: �g�`�f�[�^�ǂݍ��݂������܂œ��B����
		    stream_sound_info [ch].end ++;
		}
	    }
	    strenv.buffer [ch].validsize = CPY_BUF_SIZE;
	    ret = 1;
	}
    }

    return (ret);
}

unsigned int
fill_data (unsigned int str_bit)
{
    int ch;
    unsigned int ret_bit;

    ret_bit = 0;
    for (ch = 0; ch < N_STREAM; ch ++) {
	if (str_bit & (1 << ch)) {
	    ret_bit |= fill_data_ch (ch) << ch;
	}
    }

    // �f�[�^���ǂݍ��܂ꂽ�X�g���[���̃r�b�g��
    return (ret_bit);
}

/*
 *	SPU2 �ݒ�
 */
void
set_spu2 (void)
{
    /*
     * ���f�W�^���o�͂̐ݒ�
     */
    sceSkSsSetDigitalOut (SCESK_DOUT_CD_NORMAL);

    /*
     * �G�t�F�N�g�̐ݒ�
     */
    sceSkSsSetEffectMode (SCESK_EFFECT_MODE_HALL | SCESK_EFFECT_MODE_CLEAR_WA,
			  0, 0); /* delay, feedback */
    sceSkSsSetEffect (SCESK_EFFECT_ON); 
    sceSkSsSetEffectVolume (SCESK_EFFECT_VOLUME_MAX / 4,  /* L */
			    SCESK_EFFECT_VOLUME_MAX / 4); /* R */

    /*
     * �}�X�^�[�{�����[���ݒ�
     */
    sceSkSsSetMasterVolume (SCESK_MASTER_VOLUME_MAX,  /* L */
			    SCESK_MASTER_VOLUME_MAX); /* R */

    return;
}

/*
 *	�X�g���[�����̊e ID ���蓖��
 */
void
set_stream (void)
{
    int ret;

    /*
     * ADPCM stream �f�[�^�̓o�^
     *     �� ADPCM stream �f�[�^ ID ���Ԃ�
     */
    stradpcm_id = sceSkSsBindSTRADPCM (SCESK_AUTOASSIGNMENT);
    PRINTF (("Bind stream %d\n", stradpcm_id));

    /*
     * ADPCM stream �f�[�^ ID ���A�g���b�N (ID: �������蓖��) �ɓo�^
     *     �� �g���b�N id ���Ԃ�
     */
    track_id = sceSkSsBindTrackSTRADPCM (SCESK_AUTOASSIGNMENT, stradpcm_id);
    PRINTF (("Bind track %d\n", track_id));

    /*
     * ���\���� env �̏�����
     */
    ret = sceSkSsInitEnvironmentSTRADPCM (track_id, &strenv);

    stream_bit = 0;
}

/*
 *	�X�g���[�����̊e ID �̊��蓖�ĉ���
 */
void
unset_stream (void)
{
    int ret;

    ret = sceSkSsUnbindTrackSTRADPCM (track_id);
    PRINTF (("Unbind track %d\n", ret));

    ret = sceSkSsUnbindSTRADPCM (stradpcm_id);
    PRINTF (("Unbind stream %d\n", ret));
}

void
set_env_ch (int ch)
{
    int          spu2_addr;
    unsigned int iop_addr;

    /*
     * SPU2 �{�C�X���
     */
    strenv.adpcm.channel [ch].voice.core    = SD_CORE_0;	// SPU2 core.
    strenv.adpcm.channel [ch].voice.no      = ch;		// SPU2 voice no.
    strenv.adpcm.channel [ch].voice.options = STREAM_OPTIONS;
    // SPU2 voice volume L/R
    switch (ch) {					
    case _TR_CH_BGM_L:
	strenv.adpcm.channel [ch].voice.voll = VOLUME_L;
	strenv.adpcm.channel [ch].voice.volr = 0;
	break;
    case _TR_CH_BGM_R:
	strenv.adpcm.channel [ch].voice.voll = 0;
	strenv.adpcm.channel [ch].voice.volr = VOLUME_R;
	break;
    case _TR_CH_ONE:
	strenv.adpcm.channel [ch].voice.voll = VOLUME_L;
	strenv.adpcm.channel [ch].voice.volr = VOLUME_R;
	break;
    }
    // SPU2 voice pitch
    strenv.adpcm.channel [ch].voice.pitch = SCESK_STRADPCM_PITCH_1X;
    // SPU2 voice envelope ... See common/include/sdmacro.h and `SPU2 overview'
    strenv.adpcm.channel [ch].voice.adsr1 = SD_ADSR1 (SD_ADSR_A_LINEAR,	/* attack curve */
						      0,		/* attack rate */
						      0xf,		/* decay rate */
						      0xf);		/* sustain level */
    strenv.adpcm.channel [ch].voice.adsr2 = SD_ADSR2 (SD_ADSR_S_EXP_DEC, /*sustain curve */
						      0x7f,		/* sustain rate */
						      SD_ADSR_R_LINEAR,	/* release curve */
						      0xa);		/* release rate */

    /*
     * �o�b�t�@���
     */

    // SPU2 ���[�J����������
    while ((spu2_addr = sceSpu2MemAllocate (SPU_BUF_SIZE)) < 0) {
	scePrintf ("[[ No enough memory in SPU2! ]]\n");
    }
    strenv.adpcm.channel [ch].spu2.addr      = spu2_addr;
    strenv.adpcm.channel [ch].spu2.size      = SPU_BUF_SIZE;
    strenv.adpcm.channel [ch].spu2.validsize = 0;

    // IOP �������������O�o�b�t�@
    while ((iop_addr = (unsigned int)sceSifAllocIopHeap (IOP_BUF_SIZE)) == (int)NULL) {
	scePrintf ("[[ No enough memory in IOP! ]]\n");
    }
    strenv.adpcm.channel [ch].iop.addr      = iop_addr;
    strenv.adpcm.channel [ch].iop.size      = IOP_BUF_SIZE;
    strenv.adpcm.channel [ch].iop.validsize = 0;

    // (IOP ��������) EE ������ -> IOP �������]���o�b�t�@
    while ((iop_addr = (unsigned int)sceSifAllocIopHeap (CPY_BUF_SIZE)) == (int)NULL) {
	scePrintf ("[[ No enough memory in IOP! ]]\n");
    }
    strenv.adpcm.channel [ch].buffer.addr      = iop_addr;
    strenv.adpcm.channel [ch].buffer.size      = CPY_BUF_SIZE;
    strenv.adpcm.channel [ch].buffer.validsize = 0;

    // EE ���������f�[�^�ǂݍ��݃o�b�t�@
    // ... �A�h���X���A���L���b�V���̈�ɕϊ�
    strenv.buffer [ch].addr      = ((unsigned int)xaddr [ch]) | EE_UNCACHE_MASK;
    strenv.buffer [ch].size      = CPY_BUF_SIZE;
    strenv.buffer [ch].validsize = 0;
}

static void
set_env (void)
{
    int ch;

    strenv.adpcm.version  = 0;		/* �o�[�W����: ���ݕK�� 0 */
    strenv.adpcm.trans_ch = SD_CORE_0;  /* SPU2 �]���`�����l�� */
    for (ch = 0; ch < N_STREAM; ch ++) {
	set_env_ch (ch);
    }

    // �ݒ�� IOP ���ɔ��f: �S�X�g���[���`�����l��
    sceSkSsSetupSTRADPCM (track_id, &strenv, _STREAM_ALL);
}

static void
unset_env_ch (int ch)
{
    // SPU2 ���[�J����������
    sceSpu2MemFree (strenv.adpcm.channel [ch].spu2.addr);

    // (IOP ��������) EE ������ -> IOP �������]���o�b�t�@
    sceSifFreeIopHeap ((void *)strenv.adpcm.channel [ch].buffer.addr);
    // IOP �������������O�o�b�t�@
    sceSifFreeIopHeap ((void *)strenv.adpcm.channel [ch].iop.addr);
}

void
unset_env (void)
{
    int ch;

    for (ch = 0; ch < N_STREAM; ch ++) {
	unset_env_ch (ch);
    }

    // track_id �Ɋւ��ݒ�𖳌���
    sceSkSsUnsetupSTRADPCM (track_id, &strenv);
}

/*
 *	�T�E���h�����X���b�h
 */
void
main_sound (void *argp)
{
    int ret;
    int isstop = 0;
    int note_id;
    int ch;
    unsigned int bit;

    sound_command = _IDLE;
    note_id = 0;

    while (isstop == 0) {
	WaitSema (sema_sound);	/* �R�}���h��҂��󂯂� */
	
	sound_return = 0;
	ret = -1;
	switch (sound_command) {
	case _PRELOAD:
	    if ((stream_bit & sound_value) == 0) {
		pre_read_file (sound_value);
		ret = sceSkSsPreloadSTRADPCM (track_id, &strenv, sound_value);
		if (ret == SCESK_OK) {
		    stream_bit |= sound_value;
		}
	    }
	    break;
#if STREAM_AUTOPLAY == 0
	case _PLAY:
	    if ((stream_bit & sound_value) == sound_value) {
		ret = sceSkSsStartSTRADPCM (track_id, &strenv, sound_value);
	    }
	    break;
#endif
	case _STOP:
	    if ((stream_bit & sound_value) == sound_value) {
		ret = sceSkSsTerminateSTRADPCM (track_id, &strenv, sound_value);
		if (ret == SCESK_OK) {
		    rewind_file (sound_value);
		    stop_bit |= sound_value;
		}
	    }
	    break;

	case _PAUSE:
	    if ((stream_bit & sound_value) == sound_value) {
		bit = pause_bit & sound_value;
		if (bit == sound_value) {
		    // depause
		    ret = sceSkSsDepauseSTRADPCM (track_id, &strenv, sound_value);
		    pause_bit &= ~sound_value;
		} else {
		    // pause
		    ret = sceSkSsPauseSTRADPCM (track_id, &strenv, sound_value);
		    pause_bit |= sound_value;
		}
	    }
	    break;

	case _MUTE:
	    if ((stream_bit & sound_value) == sound_value) {
		bit = mute_bit & sound_value;
		if (bit == sound_value) {
		    // demute
		    ret = sceSkSsDemuteSTRADPCM (track_id, &strenv, sound_value);
		    mute_bit &= ~sound_value;
		} else {
		    // mute
		    ret = sceSkSsMuteSTRADPCM (track_id, &strenv, sound_value);
		    mute_bit |= sound_value;
		}
	    }
	    break;

	case _PCHANGE:
	    if ((stream_bit & sound_value) == sound_value) {
		bit = pchange_bit & sound_value;
		if (bit == sound_value) {
		    // pitch 1x
		    for (ch = 0; ch < N_STREAM; ch ++) {
			if (sound_value & (1 << ch)) {
			    strenv.adpcm.channel [ch].voice.mask  = SCESK_STRADPCM_ATTR_PITCH;
			    strenv.adpcm.channel [ch].voice.pitch = SCESK_STRADPCM_PITCH_1X;
			}
		    }
		    ret = sceSkSsChangeAttrSTRADPCM (track_id, &strenv, sound_value);
		    pchange_bit &= ~sound_value;
		} else {
		    // pitch 120% up
		    for (ch = 0; ch < N_STREAM; ch ++) {
			if (sound_value & (1 << ch)) {
			    strenv.adpcm.channel [ch].voice.mask  = SCESK_STRADPCM_ATTR_PITCH;
			    strenv.adpcm.channel [ch].voice.pitch = (SCESK_STRADPCM_PITCH_1X * 120) / 100;
			}
		    }
		    ret = sceSkSsChangeAttrSTRADPCM (track_id, &strenv, sound_value);
		    pchange_bit |= sound_value;
		}
	    }
	    break;

#if STREAM_AUTOPLAY != 0
	case _PLAY_ALL:
	    bit = stream_bit & _STREAM_ALL;
	    if (bit == _STREAM_ALL) {
		// terminate
		ret = sceSkSsTerminateSTRADPCM (track_id, &strenv, bit);
		if (ret == SCESK_OK) {
		    stream_bit &= ~bit;
		    rewind_file (bit);
		}
	    } else {
		bit = (~stream_bit) & _STREAM_ALL;
		pre_read_file (bit);
		ret = sceSkSsPreloadSTRADPCM (track_id, &strenv, bit);
		if (ret == SCESK_OK) {
		    stream_bit |= bit;
		}
	    }
	    break;
#endif

	case _QUIT:
	    ret = SCESK_OK;
	    isstop ++;
	    break;
	}
	if (ret < 0) {
	    PRINTF (("something is wrong...\n"));
	}
	sound_return = ret;
	sound_command = _IDLE;
	sound_value   = 0;
	if (isstop == 0) {	/* Not QUITing ... */
	    SignalSema (sema_sound_send);
	}
	SignalSema (sema_sound_complete);
    }

    return;
}

/*
 *	�X�g���[���f�[�^�� IOP �]���ƃX�e�[�^�X�X�V
 */
static void
stream_update (void)
{
    int ret;

    if (stream_bit != 0) {
	ret = fill_data (stream_bit);
	DBGPROC (if (ret > 0) scePrintf ("Filled %x\n", ret));
	sceSkSsUpdateSTRADPCM (track_id, &strenv);
    }
}

/*
 *	�X�g���[���f�[�^�̓]���m�F
 */
static void
stream_sync (void)
{
    int ret;

    if (stream_bit != 0) {
	ret = sceSkSsSyncSTRADPCM (track_id, &strenv);
	DBGPROC (if (ret > 0) scePrintf ("Sync %x\n", ret));
	if (stop_bit != 0) {
	    stream_bit &= ~stop_bit;
	    stop_bit = 0;
	}
    }
}

/*
 *	�T�E���h�����X���b�h�ւ̃R�}���h���o�p�֐�
 *	... ����: ���ϐ������������ăR�}���h�Ƃ��Ă���̂ŁA
 *		  multi-thread safe �ł͖������Ƃɒ��ӁB
 */
int
command_x (int idx, int cmd, int value)
{
    int ret;

    ret = WaitSema (sema_sound_send); // or PollSema()
    if (ret >= 0) {
	sound_index   = idx;
	sound_command = cmd;
	sound_value   = value;
	SignalSema (sema_sound);
	WaitSema (sema_sound_complete);
    } else {
	return (-1);
    }

    return (sound_return);
}

/*
 *	���C������
 */
void
main_loop (void)
{
    int ret, c;
    int note_id, old_note_id;
    unsigned int paddata, oldpad;
    unsigned char rdata [32];

    /* �e��t���O */
    int isstop  = 0;		/* �X���b�h�I���t���O */

    scePadPortOpen (0, 0, pad_dma_buf);

    c = 0;
    oldpad = paddata = 0;
    note_id = old_note_id = -1;
    while (isstop == 0) {
	/* �R���g���[�����̓ǂݍ��� */
	if (scePadRead (0, 0, rdata) > 0) paddata = (0xffff ^ ((rdata[2] << 8) | rdata[3]));
	else                              paddata = 0;

	if (paddata != oldpad) {
	    if (paddata & _PAD_QUIT) { /* main_loop() �̏I�� */
		isstop ++;
	    }
	    else if (paddata & _PAD_PRELOAD_ONE) {
		ret = command_x (0, _PRELOAD, _TR_ONE);		PRINTF (("PRELOAD 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_PRELOAD_BGM) {
		ret = command_x (0, _PRELOAD, _TR_BGM);		PRINTF (("PRELOAD 2: %d\n", ret));
	    }
#if STREAM_AUTOPLAY == 0
	    else if (paddata & _PAD_PLAY_ONE) {
		ret = command_x (0, _PLAY, _TR_ONE);		PRINTF (("PLAY 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_PLAY_BGM) {
		ret = command_x (0, _PLAY, _TR_BGM);		PRINTF (("PLAY 2: %d\n", ret));
	    }
#endif
	    else if (paddata & _PAD_STOP_ONE) {
		ret = command_x (0, _STOP, _TR_ONE);		PRINTF (("STOP 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_STOP_BGM) {
		ret = command_x (0, _STOP, _TR_BGM);		PRINTF (("STOP 2: %d\n", ret));
	    }
	    else if (paddata & _PAD_PAUSE_ONE) {
		ret = command_x (0, _PAUSE, _TR_ONE);		PRINTF (("PAUSE 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_MUTE_ONE) {
		ret = command_x (0, _MUTE, _TR_ONE);		PRINTF (("MUTE 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_MUTE_BGM) {
		ret = command_x (0, _MUTE, _TR_BGM);		PRINTF (("MUTE 2: %d\n", ret));
	    }
	    else if (paddata & _PAD_PCHANGE_ONE) {
		ret = command_x (0, _PCHANGE, _TR_ONE);		PRINTF (("PITCH CHANGE 1: %d\n", ret));
	    }
	    else if (paddata & _PAD_PCHANGE_BGM) {
		ret = command_x (0, _PCHANGE, _TR_BGM);		PRINTF (("PITCH CHANGE 2: %d\n", ret));
	    }
#if STREAM_AUTOPLAY != 0
	    else if (paddata & _PAD_PLAY_ALL) {
		ret = command_x (0, _PLAY_ALL, 0);		PRINTF (("PLAY ALL: %d\n", ret));
	    }
#endif
	}
	c ++;

	stream_update ();
	sceGsSyncV (0);
	stream_sync ();

	oldpad = paddata;
	DBGPROC(if (c % 60 == 0) scePrintf (".\n"));
    }

    PRINTF (("c = %d\n", c));

    // �T�E���h�����X���b�h���I��������
    sound_command = _QUIT;
    SignalSema (sema_sound);
    WaitSema (sema_sound_complete);
}

/*
 *	���C�������X���b�h
 */
static void
main_main (void *argp)
{
    // �R���g���[���ɂ��Đ����R���g���[������
    main_loop ();

    SignalSema (*(int *)argp);		// main() �ɖ߂�
    return;
}

/*
 *	�e�평��������
 */
static void
initialize (void)
{
    struct SemaParam sp;

    /*
     * SIF �֘A�̏�����
     */
    sceSifInitRpc (0);
    sceSifInitIopHeap ();

    /*
     * ���W���[���̃��[�h
     */
    LOAD_MODULE (_MOD_SIO2MAN);
    LOAD_MODULE (_MOD_PADMAN);
    LOAD_MODULE (_MOD_LIBSD);		/* �჌�x���T�E���h���C�u���� */
    LOAD_MODULE (_MOD_SDRDRV);		/* �჌�x���T�E���h���C�u���� (for EE) */
    LOAD_MODULE (_MOD_STRADPCM);	/* ADPCM �X�g���[�~���O���C�u���� */
    LOAD_MODULE (_MOD_SK_SOUND);	/* �W���L�b�g: �T�E���h�t�@���_�����^�� */
    LOAD_MODULE (_MOD_SK_STRADPCM);	/* �W���L�b�g: ADPCM �X�g���[�~���O */

    /*
     * �R���g���[�����C�u�����̏�����
     */
    scePadInit (0);

    /*
     * �W���L�b�g + �W���L�b�g/�T�E���h�V�X�e�� ... ������
     */
    sceSkInit (0);	/* �W���L�b�g�S�̂̏�����: ���� ... �K�� 0 */
    sceSkSsInit (32);	/* �W���L�b�g/�T�E���h�V�X�e���̏�����:
			 *   ���� ... sdr ���荞�݃n���h���X���b�h�̃v���C�I���e�B */

    /*
     * �W���L�b�g�T�E���h�V�X�e�� ... ADPCM �X�g���[�~���O
     */
    sceSkSsInitSTRADPCM (); /* SK/Sound System ADPCM �X�g���[�~���O�̏����� */

    /*
     * SPU2 �������Ǘ��̏�����
     */
    sceSpu2MemInit (spu2mem_table, SPU2MEM_N_AREAS, /* �̈搔: SPU2MEM_N_AREAS */
		    SCESPU2MEM_USE_EFFECT);	    /* �G�t�F�N�g�͎g�p���� */

    /*
     * SPU2 �̑S�ʂɊւ�鑮���̐ݒ�
     */
    set_spu2 ();

    set_stream ();
    set_env ();
    open_file ();

    /*
     * �҂��󂯃Z�}�t�H�̐���
     */
    sp.initCount = 0;
    sp.maxCount  = 1;
    sp.option    = 0;
    // ���C�������X���b�h�I���p�Z�}�t�H
    while ((sema_main = CreateSema (&sp)) < 0) {
	scePrintf ("[[ Error: can't create semaphore.\n]]");
    }
    // �T�E���h�����X���b�h�p�Z�}�t�H
    while ((sema_sound = CreateSema (&sp)) < 0) {
	scePrintf ("[[ Error: can't create semaphore.\n]]");
    }
    // �T�E���h�����R�}���h�I���҂��󂯗p�Z�}�t�H
    while ((sema_sound_complete = CreateSema (&sp)) < 0) {
	scePrintf ("[[ Error: can't create semaphore.\n]]");
    }
    sp.initCount = 1;		/* ��� WaitSema() ���s�� */
    // �T�E���h�����R�}���h�p�Z�}�t�H
    while ((sema_sound_send = CreateSema (&sp)) < 0) {
	scePrintf ("[[ Error: can't create semaphore.\n]]");
    }

    return;
}

/*
 *	�e��I������
 */
static void
finalize (void)
{
    if (stream_bit != 0) {
	sceSkSsTerminateSTRADPCM (track_id, &strenv, stream_bit);
	stream_update ();
	stream_sync ();
    }
	
    close_file ();
    unset_env ();
    unset_stream ();

    // SPU2 �������Ǘ��̏I��
    sceSpu2MemQuit();

    /*
     * �҂��󂯃Z�}�t�H�̍폜
     */
    DeleteSema (sema_sound);
    DeleteSema (sema_sound_send);
    DeleteSema (sema_sound_complete);
    DeleteSema (sema_main);

    sceSkSsQuitSTRADPCM (); /* SK/Sound System ADPCM �X�g���[�~���O�̏I�� */

    return;
}

/*
 *	�X�^�[�g�G���g��
 */
int
main (int argc, char *argv [])
{
    struct ThreadParam tp;

    /*
     *	����������
     */
    initialize ();

    /*
     * ���C�������X���b�h�̐����ƋN��
     */
    tp.stackSize    = STACK_SIZE_MAIN;
    tp.gpReg 	    = &_gp;
    tp.entry 	    = main_main;
    tp.stack 	    = (void *)stack_main;
    tp.initPriority = 3;
    if ((thid_main = CreateThread (&tp)) < 0) {
	// Can't create thread
	return (-2);
    }
    StartThread (thid_main, (void *)&sema_main);

    /*
     * �T�E���h�����X���b�h�̐����ƋN��
     */
    tp.stackSize    = STACK_SIZE_SOUND;
    tp.gpReg 	    = &_gp;
    tp.entry 	    = main_sound;
    tp.stack 	    = (void *)stack_sound;
    tp.initPriority = 4;
    thid_sound = CreateThread (&tp);
    if (thid_sound < 0) {
	return 0;		// Can't create thread
    }
    StartThread (thid_sound, (void *)&sema_sound);

    /*
     * ���C�������X���b�h�̏I����҂�
     */
    WaitSema (sema_main);

    /*
     * ���\�[�X�����
     */
    if (TerminateThread (thid_sound) == thid_sound) {
	DeleteThread (thid_sound);
    }
    if (TerminateThread (thid_main) == thid_main) {
	DeleteThread (thid_main);
    }

    /*
     *	�I������
     */
    finalize ();

    // Fine ...
    PRINTF (("Fine ...\n"));

    return (0);
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
