/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                     I/O Processor Library
 *                          Version 2.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                         sfxbasic - main.c
 *                        MIDI & SE Stream Basic Sample
 *
 *     Version   Date          Design   Log
 *  --------------------------------------------------------------------
 *               Feb.2002      masae     for release
 */

#include <kernel.h>
#include <stdio.h>

#include <libsd.h>
#include <csl.h>
#include <cslmidi.h>
#include <cslse.h>
#include <modhsyn.h>
#include <modmsin.h>
#include <modsein.h>

#define USE_MAKEMSG

volatile int main_sema;		// �T�E���h������҂Z�}�t�H
sceHSyn_VoiceStat vstat;	// modhsyn ��ԃ��j�^�o�b�t�@

/* ----------------------------------------------------------------
 * �^�C�}�[
 * ---------------------------------------------------------------- */

// 4167 micro sec. = 1000 * 1000 / 240 = 1/240 sec
const int ONE_240TH = 4167;

typedef struct TimerCtx {
    int thread_id;
    int timer_id;
    int count;
} TimerCtx;

/* ----------------
 * ���荞�݃n���h��
 * ---------------- */
unsigned int
timer_handler (void *common)
{
    TimerCtx *tc = (TimerCtx *) common;

    iWakeupThread (tc->thread_id); // wakeup ATick()

    return (tc->count); // �V���Ȕ�r�l��ݒ肵�J�E���g�𑱍s
}


/* ----------------
 * �^�C�}�[�ݒ�
 * ---------------- */
int
set_timer (TimerCtx *timer)
{
    struct SysClock clock;
    int timer_id;

    // 1/240 sec = ??? sysclock
    USec2SysClock (ONE_240TH, & clock);
    timer->count = clock.low;	/* within 32-bit */

    // Use sysclock timer
    if ((timer_id = AllocHardTimer (TC_SYSCLOCK, 32, 1)) <= 0) {
	printf ("Can NOT allocate hard timer ...\n");
	return (-1);
    }
    timer->timer_id = timer_id;

    if (SetTimerHandler (timer_id, timer->count,
			 timer_handler, (void *) timer) != KE_OK) {
	printf ("Can NOT set timeup timer handler ...\n");
	return (-1);
    }

    if (SetupHardTimer (timer_id, TC_SYSCLOCK, TM_NO_GATE, 1) != KE_OK) {
	printf ("Can NOT setup hard timer ...\n");
	return (-1);
    }

    if (StartHardTimer (timer_id) != KE_OK) {
	printf ("Can NOT start hard timer ...\n");
	return (-1);
    }

    return 0;
}

/* ----------------
 * �^�C�}�[�폜
 * ---------------- */
int
clear_timer (TimerCtx *timer)
{
    int ret;

    ret = StopHardTimer (timer->timer_id);
    if (! (ret == KE_OK || ret == KE_TIMER_NOT_INUSE)) {
	printf ("Can NOT stop hard timer ...\n");
	return (-1);
    }

    if (FreeHardTimer (timer->timer_id) != KE_OK) {
	printf ("Can NOT free hard timer ...\n");
	return (-1);
    }
    return 0;
}

/* ----------------------------------------------------------------
 * �萔�}�N��/�ϐ�
 * ---------------------------------------------------------------- */
/* ----------------
 * �T�C�Y/�������z�u
 * ---------------- */
// �g�`�f�[�^���
#define HD0_ADDRESS	((void *) 0x120000)
#define BD0_ADDRESS	((void *) 0x130000)
#define BD0_SIZE	(127536+16) // 64 �̔{����

// BD �� SPU2 ���[�J�����������擪�A�h���X
#define SPU_ADDR0	((void *) 0x5010)

/* ----------------------------------------------------------------
 * CSL �֘A
 * ---------------------------------------------------------------- */
// CSL buffer group �w��
const  int IN_BUF = 0;		// Buffer Group 0: ���� / ��(�I�v�V����)
const  int OUT_BUF = 1;		// Buffer Group 1: �o��
// CSL buffer context �w��
const  int DATA_BUF = 0;		// context (N*2):   N �Ԃ̃o�b�t�@
const  int ENV_BUF = 1;		// context (N*2)+1: N �Ԃ̃o�b�t�@�ɑΉ������

/* ----------------------------------------------------------------
 * modmsin �֘A
 * ---------------------------------------------------------------- */
// �o�̓|�[�g
#define OUT_PORT  0  //SE & MIDI �X�g���[���� OUT_PORT �͑��P��, �����̂�!
#define OUT_Stream0  0       // �S�Ă� MIDI �X�g���[���͂����ɏo��
#define OUT_Stream1  1	// �S�Ă� SE �X�g���[���͂����ɏo��
#define OUT_PORT_NUM  1	// �o�̓|�[�g�̑���

/* CSL context */
static sceCslCtx msinCtx;

/* CSL buffer group */
static sceCslBuffGrp msinGrp [2]; // ��ɗv�f���� 2 ... 0/����, 1/�o��

/* CSL buffer context */
static sceCslBuffCtx msinBfCtx [OUT_PORT_NUM];// �o�̓|�[�g�̑��� = 1

/*
 * stream buffer
 */
#define  STREAMBUF_SIZE  1024   // �o�b�t�@�T�C�Y
#define  STREAMBUF_SIZE_ALL  (sizeof (sceCslMidiStream) + STREAMBUF_SIZE)
                                //           ����          + �o�b�t�@
/* �\����sceCslMidiStream���\����sceCslSeStream�������f�[�^�������Ă���B
   �����o�� data[]�͓���--> sizeof(**�\����)�͓����ɂȂ�܂��B  */

// �e���o�̓|�[�g���g���o�b�t�@: ���̓|�[�g�̑���
static char streamBf [OUT_PORT_NUM * 2][STREAMBUF_SIZE_ALL];


/* ----------------------------------------------------------------
 * modsein �֘A
 * ---------------------------------------------------------------- */

/* CSL context */
static sceCslCtx seinCtx;

/* CSL buffer group */
static sceCslBuffGrp seinGrp [2]; // ��ɗv�f���� 2 ... 0/����, 1/�o��

/* CSL buffer context */
static sceCslBuffCtx seinBfCtx [1];// �o�̓|�[�g�̑��� = 1
                                   // [OUT_PORT_NUM]     1

/* ----------------------------------------------------------------
 * modhsyn �֘A
 * ---------------------------------------------------------------- */
// ���̓|�[�g
const int HS_PORT_0 = 0;		// OUT_Stream0 �̏o�͂����
const int HS_PORT_1 = 1;		// OUT_Stream1�̏o�͂����
const int HS_PORT_NUM = 2;   	        // ���̓|�[�g�̑���
const int HS_BANK_0 = 0;		// HD0/BD0 �� HS_PORT_0 �ł̃o���N�ԍ�

/* CSL context */
static sceCslCtx synthCtx;

/* CSL buffer group */
static sceCslBuffGrp synthGrp;

/* CSL buffer context */
static sceCslBuffCtx sInBfCtx [2 * 2]; // ���̓|�[�g�̑���(= 2) x 2
static sceHSynEnv    synthEnv [2];     // ���̓|�[�g�̑���(= 2)
                                       // [HS_PORT_NUM]   2   	
/* ----------------------------------------------------------------
 * �e Mudule Context �̐ݒ�
 * ---------------------------------------------------------------- */
void
set_module_context (void)
{
    /*
     * modmsin
     */
   // CSL context
   msinCtx.extmod     = NULL;
   msinCtx.callBack   = NULL;
   msinCtx.buffGrpNum = 2;	// modmsin �͏�� 2
   msinCtx.buffGrp    = msinGrp;
      // CSL buffer group: ����
      msinGrp [ IN_BUF].buffNum = 0;
      msinGrp [ IN_BUF].buffCtx = NULL;
      // CSL buffer group: �o��
      msinGrp [OUT_BUF].buffNum = OUT_PORT_NUM;	// �o�̓|�[�g��
      msinGrp [OUT_BUF].buffCtx = msinBfCtx;
        // �o�̓|�[�g 0
	msinBfCtx [OUT_PORT].sema = 0;
	msinBfCtx [OUT_PORT].buff = &(streamBf [OUT_Stream0]); // modhsyn �Ƌ��L

     /*
     * modsein
     */
    // CSL context
    seinCtx.extmod     = NULL;
    seinCtx.callBack   = NULL;
    seinCtx.buffGrpNum = 2;	// modsein �͏�� 2
    seinCtx.buffGrp    = seinGrp;
    
      // CSL buffer group: ����
      seinGrp [ IN_BUF].buffNum = 0;
      seinGrp [ IN_BUF].buffCtx = NULL;
      // CSL buffer group: �o��
      seinGrp [OUT_BUF].buffNum = OUT_PORT_NUM;	// �o�̓|�[�g��
      seinGrp [OUT_BUF].buffCtx = seinBfCtx;
        // �o�̓|�[�g 0
	seinBfCtx [OUT_PORT].sema = 0;
	seinBfCtx [OUT_PORT].buff = &(streamBf [OUT_Stream1]); // modhsyn �Ƌ��L

    /*
     * modhsyn
     */

    // CSL context
    synthCtx.extmod   	= NULL;
    synthCtx.callBack 	= NULL;
    synthCtx.buffGrpNum = 1;	// modhsyn �͏�� 1
    synthCtx.buffGrp    = &synthGrp;
      // CSL buffer group: ����
      synthGrp.buffNum = HS_PORT_NUM * 2;	// ���̓|�[�g�� x 2
      synthGrp.buffCtx = sInBfCtx;
      
     //MIDI Stream PORT
        //Stream data
        sInBfCtx [HS_PORT_0 * 2 + DATA_BUF].sema = 0;
	//        sInBfCtx [0].sema = 0;
	sInBfCtx [HS_PORT_0 * 2 + DATA_BUF].buff = &(streamBf [OUT_Stream0]); 
	//	sInBfCtx [0].buff = &(streamBf [OUT_Stream0]); 
        //Env data
	sInBfCtx [HS_PORT_0 * 2 +  ENV_BUF].sema = 0;
	//	sInBfCtx [1].sema = 0;
	sInBfCtx [HS_PORT_0 * 2 +  ENV_BUF].buff = &(synthEnv [HS_PORT_0]);
	//	sInBfCtx [1].buff = &(synthEnv [HS_PORT_0]);
    //SE Stream PORT
	//Stream data
	sInBfCtx [HS_PORT_1 * 2 + DATA_BUF].sema = 0;
	//	sInBfCtx [2].sema = 0;
	sInBfCtx [HS_PORT_1 * 2 + DATA_BUF].buff = &(streamBf [OUT_Stream1]); 
	//	sInBfCtx [2].buff = &(streamBf [OUT_Stream1]); 
	//Env data
	sInBfCtx [HS_PORT_1 * 2 +  ENV_BUF].sema = 0;
	//	sInBfCtx [3].sema = 0;
	sInBfCtx [HS_PORT_1 * 2 +  ENV_BUF].buff = &(synthEnv [HS_PORT_1]);
	//	sInBfCtx [3].buff = &(synthEnv [HS_PORT_1]);

    /*
     * MIDI-stream buffer
     */
    ((sceCslMidiStream *) &(streamBf [OUT_Stream0]))->buffsize  = STREAMBUF_SIZE_ALL;
    ((sceCslMidiStream *) &(streamBf [OUT_Stream0]))->validsize = 0;
    /*
     * SE-stream buffer
     */
    ((sceCslSeStream *) &(streamBf [OUT_Stream1]))->buffsize  = STREAMBUF_SIZE_ALL;
    ((sceCslSeStream *) &(streamBf [OUT_Stream1]))->validsize = 0;

    return;
}

/* ----------------------------------------------------------------
 * Hardware Synthesizer �Z�b�g�A�b�v
 * ---------------------------------------------------------------- */
int
set_hsyn (sceHSyn_VoiceStat* pVstat)
{
    // modhsyn �S�̂̏�����
    if (sceHSyn_Init (&synthCtx, ONE_240TH) != sceHSynNoError) { // 1/240sec
      printf ("sceHSyn_Init Error here it is?\n");
      return (-1);
    }

    /* ���̓|�[�g 0 �� HD0/BD0 �̐ݒ�
     * ---------------------------------------------------------------- */

    // modhsyn ���̓|�[�g 0 �̐ݒ�
    synthEnv [HS_PORT_0].priority       = 0;
    synthEnv [HS_PORT_0].portMode       = sceHSynModeHSyn; // �|�[�g�� MIDI ���[�h
    synthEnv [HS_PORT_0].waveType       = sceHSynTypeHSyn;  // �g�`�f�[�^�͏]���ʂ�
    synthEnv [HS_PORT_0].lfoWaveNum 	= 0;
    synthEnv [HS_PORT_0].lfoWaveTbl 	= NULL;
    synthEnv [HS_PORT_0].velocityMapNum = 0;
    synthEnv [HS_PORT_0].velocityMapTbl = NULL;
    /* ���̓|�[�g 1 �� HD0/BD0 �̐ݒ�
     * ---------------------------------------------------------------- */

    // modhsyn ���̓|�[�g 0 �̐ݒ�
    synthEnv [HS_PORT_1].priority       = 0;
    synthEnv [HS_PORT_1].portMode       = sceHSynModeSESyn; // �|�[�g�� SE ���[�h
    synthEnv [HS_PORT_1].waveType       = sceHSynTypeHSyn;  // �g�`�f�[�^�͏]���ʂ�
    synthEnv [HS_PORT_1].lfoWaveNum 	= 0;
    synthEnv [HS_PORT_1].lfoWaveTbl 	= NULL;
    synthEnv [HS_PORT_1].velocityMapNum = 0;
    synthEnv [HS_PORT_1].velocityMapTbl = NULL;


    // BD0 �� SPU2 ���[�J���������ɓ]��
    if (sceHSyn_VoiceTrans (SD_CORE_0, BD0_ADDRESS, SPU_ADDR0, BD0_SIZE)
	!= sceHSynNoError) {
	printf ("sceHSyn_VoiceTrans Error\n");
	return (-1);
    }

    // HD0 �� BD0 ���u���̓|�[�g 0 �̃o���N�ԍ� HS_BANK_0 (= 0)�v�Ƃ��ēo�^
    if (sceHSyn_Load (&synthCtx, HS_PORT_0, SPU_ADDR0, HD0_ADDRESS, HS_BANK_0)
	!= sceHSynNoError) {
	printf ("sceHSyn_Load (%d)[MSIn] error\n", HS_PORT_0);
	return (-1);
    }
    // HD0 �� BD0 ���u���̓|�[�g 1 �̃o���N�ԍ� HS_BANK_0 (= 0)�v�Ƃ��ēo�^
    if (sceHSyn_Load (&synthCtx, HS_PORT_1, SPU_ADDR0, HD0_ADDRESS, HS_BANK_0)
	!= sceHSynNoError) {
	printf ("sceHSyn_Load (%d)[SEIn] error\n", HS_PORT_1);
	return (-1);
    }


    // modhsyn �̏�ԃ��j�^�o�b�t�@��ݒ�
    sceHSyn_SetVoiceStatBuffer (pVstat);

    return 0;
}

/* ----------------------------------------------------------------
 * MIDI Stream input �Z�b�g�A�b�v
 * ---------------------------------------------------------------- */
int
set_msin (void)
{
    // modmidi �S�̂̏�����
  if (sceMSIn_Init (&msinCtx) != sceMSInNoError) {
	printf ("sceMSIn_Init Error\n");
	return (-1);
	}
    //���̓o�b�t�@�P�ʂł̃{�����[���̐ݒ�
    //                 module_Ctxaddr, port ,vol
    sceHSyn_SetVolume(&msinCtx, OUT_PORT, 0x7f);
    return 0;
}
/* ----------------------------------------------------------------
 * SE Stream input �Z�b�g�A�b�v
 * ---------------------------------------------------------------- */
int
set_sein (void)
{
  // modsein �S�̂̏�����
    if (sceSEIn_Init (&seinCtx) != sceSEInNoError) {
	printf ("sceSEIn_Init Error %x\n", sceSEIn_Init (&seinCtx));
	return (-1);
    }
    return 0;
}

/* ----------------------------------------------------------------
 * 1 tick ���̏���
 * ---------------------------------------------------------------- */
static int
atick (void)
{
    unsigned int count;
    int finalize;

    count = finalize = 0;
    while (1) {
	SleepThread ();

	//           module_Ctx addr, port, channel(���݋󂢂Ă������), program NO
	sceMSIn_ProgramChange(&msinCtx, OUT_PORT, 0, 0);
	switch (count) {
	case 100:
	    printf ("[MSIn]Note on --- Comedy.\n");
	    //           module_Ctx addr, port, channel, note NO., vel
	    sceMSIn_NoteOn (&msinCtx, OUT_PORT, 0, 48, 0x7f); // ver. 1
	    break;

	case 500:
	    printf ("[SEIn]Note on --- Comedy.\n");
	    //              module_Ctx addr, port, id, bank, prog, note NO., vel, pan
	    sceSEIn_MakeNoteOn (&seinCtx, OUT_PORT, 0x11223344, HS_BANK_0, 0, 48, 0x7f, 64);
	    // ver. 1
	    break;

	case 1000:
	    printf ("[MSIn]Note on --- Cheers!\n");
	    //           module_Ctx addr, port, channel, note NO., vel
	    sceMSIn_NoteOn (&msinCtx, OUT_PORT, 0, 72, 0x7f); // ver. 1
	    break;

	case 1500:
	    printf ("[SEIn]Note on --- Cheers!\n");
	    //              module_Ctx addr, port, id, bank, prog, note NO., vel, pan
	    sceSEIn_MakeNoteOn (&seinCtx, OUT_PORT, 0x11223345, HS_BANK_0, 0, 72, 0x7f, 64);
	    break;

	case 2000:
	    printf ("[MSIn]Note on --- telephone!\n");
	    //           module_Ctx addr, port, channel, note NO., vel
	    sceMSIn_NoteOn (&msinCtx, OUT_PORT, 0, 96, 0x7f); // ver. 1
	    break;

	case 2500:
	    printf ("[SEIn]Note on --- telephone!\n");
	    //              module_Ctx addr, port, id, bank,prog, note NO., vel, pan
	    sceSEIn_NoteOn (&seinCtx, OUT_PORT, 0x11223346, HS_BANK_0, 0, 96, 0x7f, 64);
	    break;

	case 3000:
	    printf ("[MSIn]Note off.(Comedy, Cheers, telephone)\n");
	    //               Ctx addr, port, channel, note
	    sceMSIn_NoteOff (&msinCtx, OUT_PORT, 0, 48);
	    sceMSIn_NoteOff (&msinCtx, OUT_PORT, 0, 72);
	    sceMSIn_NoteOff (&msinCtx, OUT_PORT, 0, 96);
	    break;
	    
	case 3500:
	    printf ("[SEIn]Note off.(Comedy, Cheers, telephone)\n");
	    //               Ctx addr, port, channel, note
	    sceSEIn_NoteOff (&seinCtx, OUT_PORT, 0x11223344, HS_BANK_0, 0, 48);
	    sceSEIn_NoteOff (&seinCtx, OUT_PORT, 0x11223345, HS_BANK_0, 0, 72);
	    sceSEIn_NoteOff (&seinCtx, OUT_PORT, 0x11223346, HS_BANK_0, 0, 96);
	    break;
	}

	if (count > 4000) {
	    finalize ++;
	}

	// Tick ����
	sceMSIn_ATick (&msinCtx);	// modmsin
	sceSEIn_ATick (&seinCtx);	// modsein
	sceHSyn_ATick (&synthCtx);	// modhsyn

	if (finalize) {
	    // �����{�C�X�̊m�F
	    if (! (vstat.pendingVoiceCount || vstat.workVoiceCount)) {
		// ��������Ă���{�C�X����
		SignalSema (main_sema); // start() �ɏ�����Ԃ�
	    }
	}

	count ++;
    }

    return 0;
}

/* ----------------------------------------------------------------
 * SPU2 �ݒ�
 * ---------------------------------------------------------------- */
void
set_spu2 (void)
{
    int i;
    sceSdEffectAttr r_attr;

    // set digital output mode
    sceSdSetCoreAttr (SD_C_SPDIF_MODE, (SD_SPDIF_MEDIA_CD |
					SD_SPDIF_OUT_PCM  |
					SD_SPDIF_COPY_NORMAL));

    for (i = 0; i < 2; i++) {
	/*
	 * �G�t�F�N�g�ݒ�
	 */
	// effect workarea end address �� CORE0 �� CORE1 �Ƃŕʂ̗̈�ɐݒ�
	sceSdSetAddr (i | SD_A_EEA, 0x1fffff - 0x20000 * i);

	// set reverb attribute
	r_attr.depth_L  = 0;
	r_attr.depth_R  = 0;
	r_attr.mode = SD_REV_MODE_HALL | SD_REV_MODE_CLEAR_WA;
	sceSdSetEffectAttr (i, &r_attr);

	// reverb on
	sceSdSetCoreAttr (i | SD_C_EFFECT_ENABLE, 1);
	sceSdSetParam    (i | SD_P_EVOLL, 0x3fff);
	sceSdSetParam    (i | SD_P_EVOLR, 0x3fff);

	/*
	 * �}�X�^�[�{�����[���ݒ�
	 */
	sceSdSetParam (i | SD_P_MVOLL, 0x3fff);
	sceSdSetParam (i | SD_P_MVOLR, 0x3fff);
    }

    return;
}

/* ----------------------------------------------------------------
 * �����I���҂��Z�}�t�H�̍쐬
 * ---------------------------------------------------------------- */
int
make_semaphore (void)
{
    struct SemaParam sema;

    sema.initCount = 0;
    sema.maxCount  = 1;
    sema.attr      = AT_THFIFO;

    /* �Z�}�t�H�쐬 */
    return (CreateSema (&sema));
}

/* ----------------------------------------------------------------
 * tick �����p�X���b�h�̍쐬
 * ---------------------------------------------------------------- */
int
make_thread (void)
{
    struct ThreadParam param;

    param.attr         = TH_C;
    param.entry        = atick;
    param.initPriority = 32 - 3;
    param.stackSize    = 0x800;
    param.option       = 0;

    /* �X���b�h�쐬 */
    return (CreateThread (&param));
}

/* ----------------------------------------------------------------
 * ���C������
 * ---------------------------------------------------------------- */
int
start (int argc, char *argv [])
{
    int thid;
    TimerCtx timer;

    // initialize SPU2 hardware
    sceSdInit (0);
    set_spu2 ();		// Digital effect/master volume

    // initialize modules environment
    set_module_context ();	// CSL module context
    set_hsyn (&vstat);		// modhsyn
    set_msin ();		// modmsin
    set_sein ();		// modsein

    // semaphore: for finishing the play
    main_sema = make_semaphore ();

    // Thread: ATick() process
    thid = make_thread ();
    StartThread (thid, (unsigned long) NULL);

    // set timer
    timer.thread_id = thid;
    if (set_timer (&timer) < 0) {
	printf ("set_timer: something wrong ...");
    }

    // waiting for finishing the play
    WaitSema (main_sema);

    // timer disable
    if (clear_timer (&timer) < 0) {
	printf ("clear_timer: something wrong ...");
    }

    printf ("Fine ............\n");

    return 0;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
