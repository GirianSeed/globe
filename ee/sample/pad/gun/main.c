/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *	Controller Liblary Sample for GUNCON (NAMCO)
 *
 *			   Version 1.0
 *			     Shift-JIS
 *
 *	Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *			  All Rights Reserved.
 *
 *			      main.c
 *
 *       Version         Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0             Apr,21,2000     iwano       initial
 */

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpkt.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libdev.h>
#include <libpad.h>
#include "sprt.h"

#define ERR_STOP	while(1)

/* �`����}�N����` */
#define SUNIT		8
#define PACKETSIZE	(8*SUNIT)
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	224
#define OFFX	(((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY	(((4096-SCREEN_HEIGHT)/2)<<4)

/* �`����p */
static int sindex = 0;
static sceGsDBuff db;
static sceDmaChan *p1;
static u_long giftag[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0x000000000000000eL };
static sceVif1Packet packet[2];
static SPRTX sprt[32];
static int frame = 0;

/* �`��p�p�P�b�g�o�b�t�@ */
static u_long128 packet_buff[2][PACKETSIZE];

/* IOP�C���^�t�F�[�X�pDMA�o�b�t�@ */
static u_long128 pad_dma_buf[2][scePadDmaBufferMax] __attribute__((aligned (64))); 

/* �R���g���[����� */
static unsigned short paddata = 0;

/* �t�H���g�\���p */
static int console;
extern void sceDevConsLocate(int cd, u_int lx, u_int ly);

/* GS������ */
static void Gs_Initialize(void)
{
	sceDmaEnv env;
	u_long giftagAD[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0x000000000000000eL };

	sceDevVif0Reset();
	sceDevVu0Reset();
	sceGsResetPath();
	sceDmaReset(1);

	sceVif1PkInit(&packet[0], (u_long128 *)packet_buff[0]);
	sceVif1PkInit(&packet[1], (u_long128 *)packet_buff[1]);

	sceDmaGetEnv(&env);
	env.notify = 1<<SCE_DMA_VIF1; /* notify channel */
	sceDmaPutEnv(&env);

	p1 = sceDmaGetChan(SCE_DMA_VIF1);
	p1->chcr.TTE = 1;

	//sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
	sceGsResetGraph(0, 0, SCE_GS_NTSC, SCE_GS_FRAME);
	sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);

	db.clear0.rgbaq.R = db.clear1.rgbaq.R = 44;
	db.clear0.rgbaq.G = db.clear1.rgbaq.G = 82;
	db.clear0.rgbaq.B = db.clear1.rgbaq.B = 124;

	FlushCache(0);

	sindex = 0;
	sceVif1PkReset(&packet[sindex]);
	sceVif1PkCnt(&packet[sindex], 0);
	sceVif1PkOpenDirectCode(&packet[sindex], 0);
	sceVif1PkOpenGifTag(&packet[sindex], *(u_long128*)&giftagAD[0]);

	sceVif1PkReserve(&packet[sindex],
		sceGsSetDefAlphaEnv((sceGsAlphaEnv *)packet[sindex].pCurrent,0)
		* 4);

	sceVif1PkCloseGifTag(&packet[sindex]);
	sceVif1PkCloseDirectCode(&packet[sindex]);
	sceVif1PkEnd(&packet[sindex], 0);
	sceVif1PkTerminate(&packet[sindex]);

	/* kick Gs initialize packet */
	sceDmaSend(p1,(u_int *)(((u_int)packet[sindex].pBase)));

	sceGsSyncPath(0,0);
}

void scePacketOpen(void)
{
	/* �`��p�P�b�g�\���̏����� */
	sceVif1PkReset(&packet[sindex]);
	sceVif1PkCnt(&packet[sindex], 0);
	sceVif1PkOpenDirectCode(&packet[sindex], 0);
	sceVif1PkOpenGifTag(&packet[sindex], *(u_long128*)&giftag[0]);
}

void scePacketClose(void)
{
	/* �`��p�P�b�g�̏I�[���� */
	sceVif1PkReserve(&packet[sindex], sceGsSetDefAlphaEnv((sceGsAlphaEnv *)packet[sindex].pCurrent,0)* 4);
	sceVif1PkCloseGifTag(&packet[sindex]);
	sceVif1PkCloseDirectCode(&packet[sindex]);
	sceVif1PkEnd(&packet[sindex], 0);
	sceVif1PkTerminate(&packet[sindex]);
}

/* �{�^�����擾 */
static int attach_controller(int port, int slot, int phase)
{
	int i;
	int id;
	int state;
	SPRTX* p;
	unsigned char rdata[32];
	char mes_stat[80];
	char mes_data[80];

	strcpy(mes_stat, "");
	strcpy(mes_data, "");

	/* �R���g���[���̏�ԃ`�F�b�N */
	state = scePadGetState(port,slot);
	scePadStateIntToStr(state, mes_stat);

	/* �R���g���[���������ꂽ��Phase 0�� */
	if (state==scePadStateDiscon)
	{
		phase = 0;
	}

	/* 1P = �� 2P = �� ��\�� */
	p = &sprt[2+port];
	if (port == 0)
	{
		SuperxMakeSprt(p, 4*8+32, 32, 16, 16);
		setRGB0(p, 32, 255, 32);
	}
	else
	{
		SuperxMakeSprt(p, 13*8 + 32, 32, 16, 16);
		setRGB0(p, 255, 32, 32);
	}
	SuperxVif1AddSprite(&packet[sindex], p, 1);


	/* �R���g���[�����擾�A�A�N�`���G�[�^�ݒ� */
	switch(phase)
	{
		case 0:
		/* �R���g���[�����ڑ�����Ă邩�H */
		if (state != scePadStateStable &&
		  state != scePadStateFindCTP1 ) break;

		/* �R���g���[��ID�擾 */
		id = scePadInfoMode(port, slot, InfoModeCurID, 0 );
		if (id==0) break;

		/* �K���R�����ڑ�����Ă��邩���m�F */
		if (id==6)
		{
			phase = 60;
			break;
		}
		phase = 100;
		break;

		/* �K���R�����W���擾 */
		case 60:
		if (state==scePadStateFindCTP1)
		{
			int gx, gy;

			/* �{�^�����A���W�����擾 */
			scePadRead(port, slot, rdata);

			for( i = 0; i <16; i++ )
			{
				sprintf( strchr(mes_data, 0), "%02X ", rdata[ i ] );
			}

			/* �{�^������_�����]���� [1= ����, 0= �����[�X]�ɂ��� */
			paddata = 0xffff^((rdata[2]<<8)|rdata[3]);

			/* ���W���(�s�N�Z���f�[�^)���X�N���[�����W�ɕϊ� */
			gx = (int)rdata[4] + (int)rdata[5]*256 - 0x40;
			gy = (int)rdata[6] + (int)rdata[7]*256 - 0x28;

			gx = gx*SCREEN_WIDTH / 384;
			gy = gy*SCREEN_HEIGHT / 240;

			/* �^�[�Q�b�g�J�[�\����\�� */
			p = &sprt[port];
			SuperxMakeSprt(p, gx - 16, gy*2 - 16, 32, 32);
			if (port==0)
			{
				setRGB0(p, 32, 255, 32);
			}
			else
			{
				setRGB0(p, 255, 32, 32);
			}
			SuperxVif1AddSprite(&packet[sindex], p, 1);
			p++;
		}
		break;

		/* �K���R���ȊO�̃R���g���[�� */
		case 100:
		break;
	}

	/* �t�H���g�\�� */
	sceDevConsPrintf(console, "[PORT %d]\n", port );
	sceDevConsPrintf(console, "STATUS : %s\n", mes_stat );
	sceDevConsPrintf(console, "  DATA : %s\n", mes_data );

	return (phase);
}

int main(void)
{
	int ret;
	int phase1, phase2;
	int port;
	int slot;

	/* Sif���C�u���������� */
	sceSifInitRpc(0);

	/* sio2mam.irx �����[�h */
	while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx", 0, NULL) < 0)
	{
		printf("Can't load module sio2man\n");
	}

	/* padman.irx �����[�h */
	while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx", 0, NULL) < 0)
	{
		printf("Can't load module padman\n");
	}

	/* GS������ */
	Gs_Initialize();

	/* �t�H���g�V�X�e�������� */
	sceDevConsInit();
	console = sceDevConsOpen(OFFX + (24<<4), OFFY + (16<<4), 75, 20);

	/* �R���g���[�����C�u���������� */
	scePadInit(0);

	/* �|�[�g�I�[�v�� */
	port = 0;		/* 1P */
	slot = 0;

	ret = scePadPortOpen( port, slot, pad_dma_buf[port] );
	if (ret==0)
	{
		printf( "ERROR: scePadPortOpen\n" );
		ERR_STOP;
	}

	port = 1;		/* 2P */
	slot = 0;
	ret = scePadPortOpen( port, slot, pad_dma_buf[port] );
	if (ret==0)
	{
		printf( "ERROR: scePadPortOpen\n" );
		ERR_STOP;
	}

	phase1 = 0;
	phase2 = 0;
	frame = 0;

	/* display next in odd field when interlace */
	while(!sceGsSyncV(0));

	/* ���C�����[�v */
	while(1)
	{
		/* �`��p�P�b�g�o�b�t�@������ */
		scePacketOpen();

		/* �t�H���g�t���[�������� */
		sceDevConsClear(console);
		sceDevConsLocate(console, 0, 0);
		sceDevConsPrintf(console, "1P =     2P = \n");

		/* 1P ���� */
		sceDevConsLocate(console, 0, 2);
		phase1 = attach_controller(0, slot, phase1);

		/* 2P ���� */
		sceDevConsLocate(console, 0, 13);
		phase2 = attach_controller(1, slot, phase2);

		/* �`��p�P�b�g�I�[���� */
		scePacketClose();

		/* �t�H���g�\�� */
		sceDevConsDraw(console);

		if(frame&0x01)
		{	/* interrace half pixcel adjust */
			sceGsSetHalfOffset(&db.draw1,2048,2048, sceGsSyncV(0)^0x01);
		}
		else
		{
			sceGsSetHalfOffset(&db.draw0,2048,2048, sceGsSyncV(0)^0x01);
		}

		sceGsSwapDBuff(&db, frame);

		FlushCache(0);
		sceDmaSend(p1,(u_int *)(((u_int)packet[sindex].pBase)));
		sceGsSyncPath(0,0);

		sindex ^= 0x01;
		frame++;
	}
}
