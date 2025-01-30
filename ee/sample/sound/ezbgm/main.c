/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *              Emotion Engine Library Sample Program
 *                          Version 1.20
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        EzBGM - main.c
 *                        main routine
 *
 *     Version    Date          Design     Logf
 *  --------------------------------------------------------------------
 *     1.20       Nov.23.1999   morita     first checked in.
 *                Sep.   2000   kaol       separated status transition
 */

#include <eekernel.h>
#include <string.h>	/* for strcpy() */
#include <sifdev.h>	/* for sceSif*() */
#include <libsdr.h>
#include <sdrcmd.h>
#include "bgm_i.h"
#include <libdev.h>
#include <libpad.h>

/* �p�P�b�g�T�C�Y:

   �V�[�N���Ԃɍ���Ȃ��Ƃ���PACKET_SIZE�𑝂₹�Ή��P���܂����A���[�e���V��
   �������܂��B
   �l�b�g���[�N�z���Ƀz�X�g�̃f�B�X�N���A�N�Z�X����ꍇ�A�l�b�g���[�N�̒x��
   �ɂ���Đ���ɉ��t�ł��Ȃ����Ƃ�����܂��B

   2�t�@�C���̓������t�̂��߂ɁA���Ȃ�傫���o�b�t�@������Ă��܂����A1�t�@�C
   ���݂̂ł���΁A12kByte���x�ł����t�ł��܂��B

   �o�b�t�@�̃T�C�Y��2�t�@�C���Ԃŕς��Ă���̂́A�t�@�C�����[�h�̃^�C�~���O
   ���Ȃ�ׂ��Ԃ���Ȃ��悤�ɂ��邽�߂ł��B
*/

#define PACKET_SIZE1      (512*150)    //-- 512�̔{���ł��邱��
#define PACKET_SIZE2      (512*170)   //-- 512�̔{���ł��邱��


#define CNTL_RUN  0
#define CNTL_STOP 1
#define CNTL_SEEK 2
#define CNTL_PRELOAD 3
#define CNTL_START   4

char gFilename[2][64] __attribute__((aligned (64))); //-- ���ӁI���̂܂�IOP��DMA�]������܂�

/* �O���֐� (bgm_rpc.c) */
int ezBgmInit( void );
int ezBgm( int command,  int status );

/* �R���g���[����Ɨp�o�b�t�@ */
u_long128 pad_dma_buf [scePadDmaBufferMax] __attribute__((aligned (64)));

/*
 * load_modules()	���W���[���̃��[�h
 *	����:   �Ȃ�
 *	�Ԃ�l: �Ȃ�
 */
void load_modules( void )
{
	/* SIF RPC �̏����� */
	sceSifInitRpc(0);

	/*
	 *	�K�v�ƂȂ�e���W���[���̃��[�h
	 */
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
	
	/* EzBGM�p�h���C�o���W���[�� */
	while(sceSifLoadModule("host0:../../../../iop/sample/sound/ezbgm/ezbgm.irx",0, NULL) < 0){
		scePrintf("loading ezbgm.irx failed\n");
	}

	return;
}

/*
 * init_bgm()		IOP ���̃Z�b�g�A�b�v:
 *			�t�@�C���I�[�v���A�{�C�X�w��A�{�����[���ݒ肩��
 *			���t�X�^�[�g�܂�
 *	����:
 *		wav_count: �g�`�f�[�^�̐�
 *	�Ԃ�l:
 *		�K�� 0
 */
int init_bgm( int wav_count )
{
	int i, info;

	/* SIF RPC �ڑ��̊m�� */
	ezBgmInit();

	/* �ȉ��R�s�̐ݒ��libsdr���g���Ă��悢 */
	/* IOP ���჌�x���T�E���h���W���[���̏����� */
	ezBgm( EzBGM_SDINIT, SD_INIT_COLD );   //-- sceSdInit()���s�Ȃ�
	/* �}�X�^�[�{�����[���̐ݒ� */
	ezBgm( 0|EzBGM_SETMASTERVOL, 0x3fff3fff ); //-- master vol core0
	ezBgm( 1|EzBGM_SETMASTERVOL, 0x3fff3fff ); //-- master vol core1

	for( i = 0; i < wav_count ; i++ ){
		/* IOP ���Ŕg�`�f�[�^�t�@�C�����I�[�v�� */
		info = ezBgm( i|EzBGM_OPEN, (int)gFilename[i] );
		scePrintf("  wav:%d %s  %d byte(body)  ", i,gFilename[i],info&(~WAV_STEREO_BIT) );

		if( (info&WAV_STEREO_BIT) == 0 ){  //-- ���m����
			scePrintf("mono \n");
			ezBgm( i|EzBGM_INIT, PACKET_SIZE2*3 );
			ezBgm( i|EzBGM_SETMODE, BGM_MODE_REPEAT_FORCED|BGM_MODE_MONO );
		}
		else{ //-- �X�e���I
			scePrintf("stereo \n");
			ezBgm( i|EzBGM_INIT, PACKET_SIZE1*4 );
			ezBgm( i|EzBGM_SETMODE, BGM_MODE_REPEAT_FORCED|BGM_MODE_STEREO );
		}
		ezBgm( i|EzBGM_PRELOAD, 0 );		/* �g�`�f�[�^���v�����[�h */
		ezBgm( i|EzBGM_SETVOL, 0x3fff3fff );	/* �{�����[���ݒ� */
		ezBgm( i|EzBGM_START, 0 );		/* BGM ���t�J�n */
	}
	//--- �ȏ��BGM���t���X�^�[�g ---------------------
	scePrintf("EzBGM start!\n");

	return 0;
}


/*
 * main_loop()		�����̃��C�����[�v:
 *			�R���g���[�����͂��� IOP �� RPC
 *	����:   �Ȃ�
 *	�Ԃ�l: �Ȃ�
 */
void main_loop( int wav_count )
{
	int run_status[2], cntl_status[2], i, tPause = 0;
	u_int paddata, oldpad, ch;
	u_char rdata[32];

	/* �R���g���[���֘A�̏����� */
	scePadInit(0);
	scePadPortOpen(0, 0, pad_dma_buf);

	/* ��Ԃ̏����� */
	cntl_status [0] = cntl_status [1] = CNTL_RUN;
	oldpad = paddata = 0;
	while( 1 )
	{
	    oldpad = paddata;
	    FlushCache(0);
	    /* �R���g���[�����̓ǂݍ��� */ 
	    if(scePadRead(0, 0, rdata) > 0){
		paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
		if( wav_count == 1 ) paddata &= ~(SCE_PADRleft|SCE_PADRdown);
	    }
	    else{
		paddata = 0;
	    }

	    if( paddata != oldpad )
	    {
		/* IOP ���̃X�g���[�~���O�ғ���Ԃ̎擾 */
		run_status[0] = ezBgm( 0|EzBGM_GETMODE, 0)&~BGM_MASK_STATUS;
		run_status[1] = ezBgm( 1|EzBGM_GETMODE, 0)&~BGM_MASK_STATUS;
		ch = 0;
		if ( (paddata & SCE_PADRup) || (paddata & SCE_PADRright)  ) ch = 0;
		if ( (paddata & SCE_PADRleft) || (paddata & SCE_PADRdown) ) ch = 1;

		/* �X�g���[�~���O���~ */
		if ( (paddata & SCE_PADRup) || (paddata & SCE_PADRleft) ){  
			if( (run_status[ch] & BGM_MODE_RUNNING) != 0 ){
				ezBgm( ch|EzBGM_STOP, 0 );
				cntl_status[ch] = CNTL_STOP;
			}
		}
		/* �X�g���[�~���O���J�n ... ���ۂ̏����͉��� for() �ɂčs�� */
		else if( (paddata & SCE_PADRright ) || (paddata & SCE_PADRdown ) ){  
			if( run_status[ch] != 0 ){
				continue;
			}
			cntl_status[ch] = CNTL_SEEK;
		}
		/* �X�g���[�~���O���ꎞ��~/�ꎞ��~���� */
		else if (paddata & SCE_PADstart ){  
			if( tPause == 0 ){
			    for( i = 0; i < wav_count; i++ ){
				if( (run_status[i] & BGM_MODE_RUNNING) != 0 ){
					/* �X�g���[�~���O���ꎞ��~ */
					ezBgm( i|EzBGM_STOP, 0 );
				}
			    }
			    tPause = 1;
			}
			else if ( tPause == 1 ){
			    for( i = 0; i < wav_count; i++ ){
				if( (run_status[i] == 0) && (cntl_status[i] == CNTL_RUN) ){
				    /* �X�g���[�~���O���ĊJ: �ȑO�~�߂��ʒu���� */
				    cntl_status [i] = CNTL_PRELOAD;
				}
			    }
			    tPause = 0;
			}
		}
		/* �X�g���[�~���O���I�� �� �v���O�����I�� */
		else if (paddata & SCE_PADselect ){  
			for( i = 0; i < wav_count; i++ ){
				ezBgm( i|EzBGM_SETVOL, 0x0 );
				ezBgm( i|EzBGM_STOP, 0 );
				ezBgm( i|EzBGM_CLOSE, 0 );
				ezBgm( i|EzBGM_QUIT, 0 );
			}
			ezBgm( 0|EzBGM_SETMASTERVOL, 0x0 ); //-- master vol core0
			ezBgm( 1|EzBGM_SETMASTERVOL, 0x0 ); //-- master vol core1
			return;
		}
	    }

	    /* cntl_status[] �̒l�ɂ���ăX�g���[�~���O������*/
	    for (i = 0; i < wav_count; i ++) {
		int ret;
		switch (cntl_status [i]) {
		case CNTL_SEEK:
		    ret = ezBgm (i | EzBGM_SEEK, 0);	/* �ǂ݂����ʒu��擪�� */
		    if (ret != -1) {
			cntl_status [i] ++;		/* PRELOAD �ɏ�ԑJ�� */
		    }
		    break;
		case CNTL_PRELOAD:
		    ret = ezBgm (i | EzBGM_PRELOAD, 0);
		    if (ret == -1) {
			cntl_status [i] = CNTL_SEEK;	/* SEEK �ɖ߂��čĊJ */
		    } else {
			cntl_status [i] ++;		/* START �ɏ�ԑJ�� */
		    }
		    break;
		case CNTL_START:
		    ret = ezBgm (i | EzBGM_START, 0);
		    if (ret != -1)
			cntl_status [i] = CNTL_RUN;	/* �u�ғ����v�ɏ�ԑJ�� */
		    break;
		case CNTL_RUN:				/* ���̏�Ԃł͏�ԑJ�ځA�y�� */
		case CNTL_STOP:				/* IOP �Ɏw��������e�͖��� */
		default:
		    break;
		}
	    }

	    sceGsSyncV(0);
	}
	return;
}


/*----------------------
  �G���g��
  ----------------------*/
int main( int argc, char *argv[]  )
{
	int i, wav_count=0;

	//--- ���W���[���̃��[�h
	load_modules();

	scePrintf("EzBGM initialize...\n");

	//--- args�̃p�[�X
	if( (argc < 2) || argc > 3 ){
		scePrintf("\n\nusage: %s <wav file> [<wav file>]\n\n", argv[0]);
		return 0;
	}

	for( i = 0; i < argc-1 ; i++ ){
		strcpy( gFilename[i], argv[i+1]);
		wav_count++;
	}

	//--- BGM���X�^�[�g������
	init_bgm(wav_count);


	//--- BGM���R���g���[������
	main_loop(wav_count);
	

	scePrintf("EzBGM finished...\n");
	return 0;
}


