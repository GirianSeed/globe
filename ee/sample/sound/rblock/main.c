/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                           - rblock -
 *
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999, 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                             main.c
 *
 *     Version   Date          Design     Log
 *  --------------------------------------------------------------------
 *     1.50      Mar.30.2000   kaol       For new DMA/SPU2 interrupt handler
 *     0.60      Oct.14.1999   morita     first checked in.
 */

#include <eekernel.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libgraph.h>
#include <libsdr.h>
#include <sdrcmd.h>

/* ���[�v����ꍇ 1 �� */
#define IS_LOOP		0

/* L/R 512 �o�C�g�ŃC���^���[�u���ꂽ�X�g���[�g PCM �g�`�f�[�^�t�@�C�� */
#define PCM_FILENAME	"host0:/usr/local/sce/data/sound/wave/knot.int"
#define PCM_SIZE	1604888
#define ALLOC_IOP_HEAP_SIZE   1604888

// �f�o�b�O�v�����g
//#define PRINTF(x)
#define PRINTF(x) scePrintf x
#define TRANS_CH  0

int gCallbackThreadID;		/* libsdr �̃R�[���o�b�N�X���b�h�̃X���b�h ID */
int gMainThreadID;		/* ���̃v���O�������g�̃X���b�h ID */
static int gDataTopAddr;	/* IOP ���Ńf�[�^���u���ꂽ�̈�̐擪�A�h���X */

volatile int trans_flag = 0;	/* DMA �]�����荞�݂��J�E���g�A�b�v */
volatile int intr_flag = 0;	/* SPU2 ���荞�݂��J�E���g�A�b�v */

/*
 * �t�@�C���̓��e�� IOP ���������ɓ]��
 */
void dataset(void)
{
    /* IOP ���̃q�[�v�̈�𑀍삷�邽�߂̏����� */
    sceSifInitIopHeap();

    /* IOP ���������̗̈���m�� */
    PRINTF(("allocate IOP heap memory - " ));
    while ((gDataTopAddr = (int)(int *)sceSifAllocSysMemory (0, ALLOC_IOP_HEAP_SIZE, NULL)) == (int)NULL) {
        scePrintf( "\nCan't alloc heap \n");
    }
    /* �t�@�C���̓��e�� IOP ���������ɓ]�� */
    PRINTF(("alloced 0x%x  ", gDataTopAddr));
    while (sceSifLoadIopHeap (PCM_FILENAME, (void*)gDataTopAddr) < 0) {
        scePrintf( "\nCan't load PCM file to iop heap \n"); 
    }
    PRINTF(("- data loaded 0x%x \n", (int)gDataTopAddr ));
    
    return;
}

/* �]���I�����荞�݃n���h�� */
int
IntTrans (int core, void *common)
{
    (* ((int *) common)) ++;
    PRINTF (("///// AutoDMA interrupt detected (%d). /////\n",
	     (* ((int *) common))));
    return 0;
}

/* SPU2 ���荞�݃n���h�� */
int
IntSPU2 (int core_bit, void *common)
{
    /* SPU2 ���荞�݂𖳌��� */
    sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_IRQ_ENABLE, 0);
    (* ((int *) common)) ++;
    PRINTF (("##### SPU2 interrupt detected (%d). ######\n",
	     (* ((int *) common))));
    return 0;
}

/*
 * �u���b�N�]���̏���
 */
int rAutoDma( void )
{
    int do_intr;

    // �჌�x���T�E���h���C�u�����̏�����
    sceSdRemote( 1, rSdInit , SD_INIT_COLD );

    // SPU2 �̑����̐ݒ�
    sceSdRemote( 1, rSdSetParam, SD_CORE_0 | SD_P_MVOLL , 0x3fff );
    sceSdRemote( 1, rSdSetParam, SD_CORE_0 | SD_P_MVOLR , 0x3fff );
    sceSdRemote( 1, rSdSetParam, SD_CORE_1 | SD_P_MVOLL , 0x3fff );
    sceSdRemote( 1, rSdSetParam, SD_CORE_1 | SD_P_MVOLR , 0x3fff );
    // �{�C�X�~�L�V���O: �T�E���h�f�[�^���͂̓G�t�F�N�g���ɓ���Ȃ�
    sceSdRemote( 1, rSdSetParam, SD_CORE_0 | SD_P_MMIX ,
		 ((~(SD_MMIX_MINEL | SD_MMIX_MINER)) & 0x0ff0));
    sceSdRemote( 1, rSdSetParam, SD_CORE_1 | SD_P_MMIX ,
		 ((~(SD_MMIX_MINEL | SD_MMIX_MINER)) & 0x0fff));

    // ���荞�݉񐔕ϐ��̏�����
    trans_flag = intr_flag = 0;
    // SPU2 ���荞�݃A�h���X�̐ݒ�
    sceSdRemote (1, rSdSetAddr, SD_CORE_0 | SD_A_IRQA, 0x4100);
    // �]���ASPU2 �̊e���荞�݃n���h���̐ݒ�
    sceSdRemote (1, rSdSetTransIntrHandler, TRANS_CH, IntTrans, &trans_flag); 
    sceSdRemote (1, rSdSetSpu2IntrHandler,            IntSPU2,  &intr_flag);

#if IS_LOOP
#define TRANS_MODE (SD_TRANS_MODE_WRITE | SD_BLOCK_LOOP)
#define TRANS_SIZE (PCM_SIZE/512)*512
#else
#define TRANS_MODE (SD_TRANS_MODE_WRITE | SD_BLOCK_ONESHOT)
#define TRANS_SIZE PCM_SIZE
#endif
    // �u���b�N�]���̊J�n
    sceSdRemote (1, rSdBlockTrans, TRANS_CH, TRANS_MODE, gDataTopAddr, TRANS_SIZE);
    // �T�E���h�f�[�^���͂̃{�����[�����グ��
    sceSdRemote (1, rSdSetParam, TRANS_CH|SD_P_BVOLL , 0x3fff);
    sceSdRemote (1, rSdSetParam, TRANS_CH|SD_P_BVOLR , 0x3fff);

    do_intr = 0;
    while (1) {
#if !IS_LOOP
	// 0 ��Ԃ�����u���b�N�]�����I��
	if (sceSdRemote (1, rSdBlockTransStatus, TRANS_CH) == 0) {
	    break;
	}
#endif
	if ((trans_flag % 2) == 1) {
	    if (do_intr == 0) {
		// SPU2 ���荞�݂�L����
		sceSdRemote (1, rSdSetCoreAttr, SD_CORE_0 | SD_C_IRQ_ENABLE, 1);
		do_intr = 1;
	    }
	} else {
	    if (do_intr == 1) do_intr = 0;
	}
	sceGsSyncV(0);
    }
#if !IS_LOOP
    scePrintf ("waiting .");
    while (trans_flag == 0) {
	scePrintf (".");
    }
    scePrintf ("\n");
#endif
    // �I������
    sceSdRemote(1, rSdBlockTrans, TRANS_CH, SD_TRANS_MODE_STOP, NULL, 0 );
    if (TerminateThread( gCallbackThreadID ) == gCallbackThreadID) {
	DeleteThread( gCallbackThreadID );
    }
    PRINTF(("Quit...\n"));

    return 0;
}

int
main(void)
{
    // SIF RPC API �̏����� 
    sceSifInitRpc(0);

    // IOP ���W���[���̃��[�h
    while (sceSifLoadModule("host0:../../../../iop/modules/libsd.irx",
                        0, NULL) < 0){
        scePrintf("Can't load module libsd\n");
    }
    while (sceSifLoadModule("host0:../../../../iop/modules/sdrdrv.irx",
                        0, NULL) < 0){
        scePrintf("Can't load module sdrdrv\n");
    }

    // Callback Thread�𓮂������߂ɁAmain���ō��v���C�I���e�B���牺����
    gMainThreadID = GetThreadId();
    ChangeThreadPriority(gMainThreadID , 10 );

    // libsdr �̏�����
    sceSdRemoteInit();

    // Callback Thread��main���������v���C�I���e�B�ō��
    gCallbackThreadID = sceSdRemoteCallbackInit(5);
    dataset();
    
    rAutoDma();

    return 0;
}
