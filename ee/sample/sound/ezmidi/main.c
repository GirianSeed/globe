/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *              Emotion Engine Library Sample Program
 *                          Version 1.30
 *                           Shift-JIS
 *
 *      Copyright (C) 1999, 2000-2002 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                        EzMIDI - main.c
 *                         main routine
 *
 *     Version    Date          Design     Log
 *  --------------------------------------------------------------------
 *     1.30       Dec.12.1999   morita     first checked in.
 */

#include <eekernel.h>
#include <string.h>		/* for strcpy() */
#include <sifdev.h>
#include <libgraph.h>		/* for sceGsSyncV() */
#include <libpad.h>
#include <libsdr.h>
#include <sdrcmd.h>
#include <csl.h>
#include <cslmidi.h>
#include <modhsyn.h>
#include <libmsin.h>
#include "ezmidi_i.h"

/* �g�p���� IOP ���W���[�� */
#define  SQ_FILENAME	"host0:/usr/local/sce/data/sound/seq/sakana.sq"
#define  HD_FILENAME1	"host0:/usr/local/sce/data/sound/wave/sakana.hd"
#define  BD_FILENAME1	"host0:/usr/local/sce/data/sound/wave/sakana.bd"
#define  HD_FILENAME2	"host0:/usr/local/sce/data/sound/wave/eff.hd"
#define  BD_FILENAME2	"host0:/usr/local/sce/data/sound/wave/eff.bd"

/* ��� */
#define PLAYSTAT_STOP    0
#define PLAYSTAT_RUN     1
#define PLAYSTAT_PREPARE 2
#define PLAYSTAT_PAUSE   3

/* �擪�A�h���X (SPU2 ���[�J����������) */
#define SPU_TOP_ADDR  0x5010

/* CSL �֘A�ϐ� */
static sceCslCtx msinCtx;		/* CSL context */
static sceCslBuffGrp msinBfGrp[2];	/* CSL buffer group */
static sceCslBuffCtx msinBfCtx[1];	/* CSL buffer context */

static int msinBf[MSIN_TRANS_SIZE/4]  __attribute__((aligned (64))); //-- ���ӁI���̂܂�IOP��DMA�]������܂�;

static EZMIDI_BANK  gBank __attribute__((aligned (64))); //-- ���ӁI���̂܂�IOP��DMA�]������܂�

static char gFilename[64] __attribute__((aligned (64))); //-- ���ӁI���̂܂�IOP��DMA�]������܂�

/* �O���֐� (midi_rpc.c) */
extern int ezMidiInit( void );
extern int ezMidi( int command,  int status );
extern int ezTransToIOP( void *iopAddr, void *eeAddr, int size );

/* �R���g���[����Ɨp�o�b�t�@ */
u_long128 pad_dma_buf[scePadDmaBufferMax]  __attribute__((aligned (64)));

/*
 * load_modules()	���W���[���̃��[�h
 *	����:   �Ȃ�
 *	�Ԃ�l: �Ȃ�
 */
void load_modules( void )
{
	/* SIF RPC �̏����� */
	sceSifInitRpc(0);

	/* �V���A���ʐM��{���W���[�� */
	while(sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx",0, NULL) < 0){
		scePrintf("loading sio2man.irx failed\n");
	}
	/* �R���g���[�����䃂�W���[�� */
	while(sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx",0, NULL) < 0){
		scePrintf("loading padman.irx failed\n");
	}

	/* �჌�x���T�E���h���䃂�W���[�� */
	while(sceSifLoadModule("host0:../../../../iop/modules/libsd.irx",0, NULL) < 0){
		scePrintf("loading libsd.irx failed\n");
	}
	
	/* �჌�x���T�E���h���䃂�W���[�� (EE libsdr �p) */
	while(sceSifLoadModule("host0:../../../../iop/modules/sdrdrv.irx",0, NULL) < 0){
		scePrintf("loading sdrdrv.irx failed\n");
	}
	
	/* CSL: MIDI �V�[�P���T */
	while(sceSifLoadModule("host0:../../../../iop/modules/modmidi.irx",0, NULL) < 0){
		scePrintf("loading modmidi.irx failed\n");
	}
	/* CSL: �n�[�h�E�F�A�E�V���Z�T�C�U */
	while(sceSifLoadModule("host0:../../../../iop/modules/modhsyn.irx",0, NULL) < 0){
		scePrintf("loading modhsyn.irx failed\n");
	}

	/* EzMIDI�p�h���C�o���W���[�� */
	while(sceSifLoadModule("host0:../../../../iop/sample/sound/ezmidi/ezmidi.irx",0, NULL) < 0){
		scePrintf("loading ezmidi.irx failed\n");
	}

	return;
}



/*
 * dataset()	���W���[���̃��[�h
 *	����:
 *		sqaddr:  ���ʃf�[�^ (SQ) �� IOP ���������̐擪�A�h���X
 *		hd1addr: ���ʃf�[�^�p�g�`�f�[�^�w�b�_ (HD) �� IOP ���������̐擪�A�h���X
 *		hd2addr: ���ʉ��p�g�`�f�[�^�w�b�_ (HD) �� IOP ���������̐擪�A�h���X
 *	�Ԃ�l:
 *		0 ... ����I��
 *	       -1 ... �G���[
 */
/*------------------------------------------
  sq, hd �t�@�C����IOP���������Ƀ��[�h
  ------------------------------------------*/
int dataset( int* sqaddr, int* hd1addr, int* hd2addr )
{
	int i, sqlen, hd1len, hd2len;

	/* SIF RPC �ڑ��̊m�� */
	sceSifInitIopHeap();

	/* �e�f�[�^�̃T�C�Y���擾 */
	strcpy( gFilename, SQ_FILENAME );
	sqlen = ezMidi( EzMIDI_GETFILELENGTH, (int)gFilename ) ;
	strcpy( gFilename, HD_FILENAME1 );
	hd1len = ezMidi( EzMIDI_GETFILELENGTH, (int)gFilename ) ;
	strcpy( gFilename, HD_FILENAME2 );
	hd2len = ezMidi( EzMIDI_GETFILELENGTH, (int)gFilename ) ;

	/* IOP ���ł̃f�[�^�̈�̊m�� */
	PRINTF(("allocate IOP heap memory - " ));
	*sqaddr = (int)(int *)sceSifAllocSysMemory (0, sqlen + hd1len + hd2len, NULL);
	if( sqaddr == NULL ) { scePrintf( "\nCan't alloc heap \n"); return -1; }
	PRINTF(("alloced 0x%x  ", *sqaddr));

	*hd1addr = (int)(*sqaddr) + sqlen  ;
	*hd2addr = (int)(*sqaddr) + sqlen + hd1len  ;

	i = sceSifLoadIopHeap( SQ_FILENAME, (void*)*sqaddr);
	if( i < 0 ) { scePrintf( "\nCan't load SQ_FILENAME to iop heap  %d\n", i );return -1; }

	i = sceSifLoadIopHeap( HD_FILENAME1, (void*)(*hd1addr) );
	if( i < 0 ) { scePrintf( "\nCan't load HD_FILENAME1 to iop heap %d\n", i ); return -1; }

	i = sceSifLoadIopHeap( HD_FILENAME2, (void*)(*hd2addr) );
	if( i < 0 ) { scePrintf( "\nCan't load HD_FILENAME2 to iop heap %d\n", i ); return -1; }

	PRINTF(("- data loaded 0x%x \n", (int)*sqaddr ));

	return 0;
}


/*-------------------
  SPU2�ݒ�
  -------------------*/
void set_spu( void )
{
	int i;
	sceSdEffectAttr r_attr;

	//-- initialize SPU
	sceSdRemoteInit();
	sceSdRemote( 1, rSdInit , SD_INIT_COLD );

	for( i = 0; i < 2; i++ )
	{
		//--- �G�t�F�N�g�ݒ�
		//--- reverb end address�� core0��core1�Ƃŕʂ̗̈�ɐݒ肷��
		sceSdRemote(1, rSdSetAddr, i|SD_A_EEA, 0x1fffff - 0x20000*i );
		// --- set reverb attribute
		r_attr.depth_L  = 0;
		r_attr.depth_R  = 0;
		r_attr.mode = SD_REV_MODE_HALL | SD_REV_MODE_CLEAR_WA;
		sceSdRemote(1, rSdSetEffectAttr, i, &r_attr );
		// --- reverb on
		sceSdRemote(1, rSdSetCoreAttr, i|SD_C_EFFECT_ENABLE, 1 );
		sceSdRemote(1, rSdSetParam, i|SD_P_EVOLL , 0x3fff);
		sceSdRemote(1, rSdSetParam, i|SD_P_EVOLR , 0x3fff);

		//--- �}�X�^�[�{�����[���ݒ�
		sceSdRemote(1, rSdSetParam, i|SD_P_MVOLL , 0x3fff);
		sceSdRemote(1, rSdSetParam, i|SD_P_MVOLR , 0x3fff);
	}

	return;
}


/*----------------------------------------
  ���ʉ��̃Z�b�g�A�b�v
  ----------------------------------------*/
int init_se( int hd2addr )
{
	int bd2size;

	gBank.hdAddr  = (int)hd2addr;
	gBank.spuAddr = SPU_TOP_ADDR;
	strcpy( gBank.bdName ,  BD_FILENAME2 );
	scePrintf("bdname = %s\n", gBank.bdName );
	FlushCache(WRITEBACK_DCACHE); 
	//--- bd�t�@�C����ǂ݂��݁E�Z�b�g
	bd2size = gBank.bdSize = ezMidi( EzMIDI_TRANSBDPACKET, (int)(&gBank) );
	//--- hd�t�@�C�����Z�b�g
	ezMidi( EzMIDI_SETHD_P1, (int)(&gBank) );
	//--- �|�[�g�̃v���C�I���e�B�ƍő�m�[�g�����Z�b�g
	ezMidi( EzMIDI_SETATTR_P1, 0x0840 ); //pri 0x40, max 0x08
	//--- �|�[�g�̃{�����[�����Z�b�g
	ezMidi( EzMIDI_SETVOL_P1, sceHSyn_Volume_0db );

	msinCtx.extmod = NULL;
	msinCtx.callBack = NULL;
	msinCtx.conf = NULL;
	msinCtx.buffGrpNum = 2;
	msinCtx.buffGrp = msinBfGrp;
	msinBfGrp[0].buffNum = 0;
	msinBfGrp[0].buffCtx = NULL;
	msinBfGrp[1].buffNum = 1;
	msinBfGrp[1].buffCtx = msinBfCtx;
		msinBfCtx[0].sema = 0;
		msinBfCtx[0].buff = msinBf;
	((sceCslMidiStream*)msinBf)->buffsize = MSIN_BUF_SIZE;
	((sceCslMidiStream*)msinBf)->validsize = 0;
	((MSInBuf *)msinBf)->flag = 1; /* always `1' as the transfer flag */

	if (sceMSIn_Init(&msinCtx ) != sceMSInNoError) {
		scePrintf("sceMSIn_Init Error\n"); return 1;
	}

	//--- �v���O�����`�F���W�𔭍s
	sceMSIn_ProgramChange(&msinCtx, 0, 0, 0);

	return bd2size;
}


/*----------------------------------------
  ���y�̃Z�b�g�A�b�v���牉�t�X�^�[�g�܂�
  ----------------------------------------*/
int init_music( int sqaddr, int hd1addr, int bd2size )
{
	gBank.hdAddr  = hd1addr ;
	gBank.spuAddr = SPU_TOP_ADDR + bd2size +16;
	strcpy( gBank.bdName ,  BD_FILENAME1 );
	scePrintf("bdname = %s\n", gBank.bdName );
	FlushCache(WRITEBACK_DCACHE); 
	//--- bd�t�@�C���̓ǂ݂��݁E�Z�b�g
	gBank.bdSize = ezMidi( EzMIDI_TRANSBDPACKET, (int)(&gBank));
	//--- hd�t�@�C���̃Z�b�g
	ezMidi( EzMIDI_SETHD_P0, (int)(&gBank) );
	//--- �|�[�g�̃v���C�I���e�B�ƍő�m�[�g�����Z�b�g
	ezMidi( EzMIDI_SETATTR_P0, 0x3010 ); //pri 0x10, max 0x30
	//--- �|�[�g�̃{�����[�����Z�b�g
	ezMidi( EzMIDI_SETVOL_P0, sceHSyn_Volume_0db*0.8 );

	//--- sq�t�@�C���̃Z�b�g
	ezMidi( EzMIDI_SETSQ, sqaddr );

	//--- MIDI���t�X�^�[�g
	ezMidi( EzMIDI_START, 0);
	scePrintf("EzMIDI start!\n");

	return 0;
}


/*----------------------
  ���C�����[�v
  ----------------------*/
int main_loop( int iopMSINBuffAddr, int sqaddr )
{
	int curStatus;
	u_int paddata, oldpad;
	u_char rdata[32];
	sceMSInHsMsg hs_msg;
	int playStat = PLAYSTAT_PREPARE;

	scePadInit(0);
	scePadPortOpen(0, 0, pad_dma_buf);

	oldpad = paddata = 0;
	while( 1 )
	{
	    oldpad = paddata;
	    if(scePadRead(0, 0, rdata) > 0){
		paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
	    }
	    else{
		paddata = 0;
	    }

	    if( paddata != oldpad )
	    {
		//-- STOP
		if ( paddata & SCE_PADRleft ){  
			ezMidi( EzMIDI_STOP, 0);
			playStat = PLAYSTAT_STOP;
		}
		//-- PLAY
		else if( paddata & SCE_PADRdown  ){  
			ezMidi( EzMIDI_SEEK, 0);
			ezMidi( EzMIDI_START, 0);
			playStat = PLAYSTAT_PREPARE;
		}
		//-- PAUSE
		else if (paddata & SCE_PADstart ){  
			if( playStat == PLAYSTAT_RUN ){
			    //�|�[�Y����
			    ezMidi( EzMIDI_STOP, 0);
			    playStat = PLAYSTAT_PAUSE;
			}
			else if ( playStat == PLAYSTAT_PAUSE ){
			    //�ĊJ����
			    ezMidi( EzMIDI_START, 0);
			    playStat = PLAYSTAT_PREPARE;
			}
		}

		//-- SE
		else if( paddata & SCE_PADLright  ){
			//-- �����HS���b�Z�[�W�ɂ̂ݗL����Pan�ݒ�
			sceMSIn_MakeHsPrePanpot( &hs_msg, 0, 127 );
			sceMSIn_PutHsMsg(&msinCtx,0,&hs_msg);
			//-- ID�t��Note On. ����Key�ł��ʂ�On/Off�\�B
			sceMSIn_MakeHsNoteOn( &hs_msg, 0, 48, 0, 0x60 );//ID=0
			sceMSIn_PutHsMsg(&msinCtx,0,&hs_msg);
		}
		else if( paddata & SCE_PADLleft  ){
			//-- �����HS���b�Z�[�W�ɂ̂ݗL����Pan�ݒ�
			sceMSIn_MakeHsPrePanpot( &hs_msg, 0, 0 );
			sceMSIn_PutHsMsg(&msinCtx,0,&hs_msg);
			//-- ID�t��Note On. ����Key�ł��ʂ�On/Off�\�B
			sceMSIn_MakeHsNoteOn( &hs_msg, 0, 48, 1, 0x60 );//ID=1
			sceMSIn_PutHsMsg(&msinCtx,0,&hs_msg);
		}
		else if( paddata & SCE_PADLdown  ){
			//-- �ʏ��(MIDI�����́jNote On
			sceMSIn_NoteOn(&msinCtx, 0, 0, 72, 0x60);
		}
		else if( paddata & SCE_PADLup  ){
			//-- �ʏ��(MIDI�����́jNote On
			sceMSIn_NoteOn(&msinCtx, 0, 0, 96, 0x7f);
		}

		//-- QUIT
		else if (paddata & SCE_PADselect ){  
			ezMidi( EzMIDI_STOP, 0);
			return 0;
		}
	    }


	    if( ((sceCslMidiStream*)msinBf)->validsize != 0 )
	    {
		ezTransToIOP( (void*)iopMSINBuffAddr, (void*)msinBf, MSIN_TRANS_SIZE );
		((sceCslMidiStream*)msinBf)->validsize = 0;
	    }

	    curStatus = ezMidi( EzMIDI_GETSTATUS, 0);
	    if( (playStat == PLAYSTAT_RUN) && (curStatus == 0) ){
		ezMidi( EzMIDI_SETSQ, sqaddr );
		ezMidi( EzMIDI_START, 0);
		playStat = PLAYSTAT_PREPARE;
	    } 
	    if( (playStat == PLAYSTAT_PREPARE) && (curStatus != 0) ){
		playStat = PLAYSTAT_RUN;
	    }

	    sceGsSyncV(0);
	}
	return 0;
}


/*----------------------
  �G���g���[
  ----------------------*/
int main( int argc, char *argv[]  )
{
	int iopMSINBuffAddr;
	int sqaddr, hd1addr, hd2addr, bd2size;

	//--- ���W���[���̃��[�h
	load_modules();
	scePrintf("EzMIDI initialize...\n");

	//--- EzMIDI RPC���̏�����
	ezMidiInit();

	//--- SPU2�̏�����
	set_spu();

	//--- EzMIDI�h���C�o�̏�����
        //    ������bd�̕������[�h�p�o�b�t�@�T�C�Y
        //    �Ԓl�͌��ʉ��o�b�t�@(msinBf�j�̓]����A�h���X
	iopMSINBuffAddr = ezMidi( EzMIDI_INIT, 16*1024 );

	//--- sq, hd�t�@�C����ǂ݂���
	dataset( &sqaddr, &hd1addr, &hd2addr );

	//--- ���ʉ��̃Z�b�g�A�b�v
	bd2size = init_se(hd2addr);

	//--- ���y�̃Z�b�g�A�b�v�Ɖ��t�J�n
	init_music( sqaddr, hd1addr , bd2size);

	//--- ���t�̃R���g���[��
	main_loop(iopMSINBuffAddr, sqaddr);

	scePrintf("EzMIDI finished...\n");
	return 0;
}


