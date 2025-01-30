/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      MemoryCard Library Sample
 *
 *                          Version 1.00
 *                          Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *  main.c
 *
 *      Version    Date          Design     Log
 *  --------------------------------------------------------------------
 *      1.00       Sep,14,2002   iwano      Initial
 *      1.01       Oct,22,2002   iwano      remake
 *
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
#include <libdev.h>
#include <sifrpc.h>
#include <libdbc.h>
#include <libpad2.h>
#include <libmc2.h>

#define SIO2MAN	"host0:/usr/local/sce/iop/modules/sio2man.irx"
#define DBCMAN	"host0:/usr/local/sce/iop/modules/dbcman.irx"
#define SIO2D	"host0:/usr/local/sce/iop/modules/sio2d.irx"
#define DS2MAN	"host0:/usr/local/sce/iop/modules/pad2/ds2u_s1.irx"
#define MC2MAN	"host0:/usr/local/sce/iop/modules/mc2/mc2_d.irx"	

#define MAIN_TH_PRIO	(64)
#define MC2_TH_PRIO	(62)

#define SCREEN_WIDTH	(640)
#define SCREEN_HEIGHT	(448)
#define OFFX		(((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY		(((4096-SCREEN_HEIGHT)/2)<<4)

#define TEST_FILE	"TEST.SYS"

#define PHASE_START		(0)
#define PHASE_GETINFO		(1)
#define PHASE_GETDIR		(10)
#define PHASE_CREAT		(20)
#define PHASE_WRITE		(30)
#define PHASE_READ		(40)
#define PHASE_DELETE		(50)
#define PHASE_FORMAT_EXEC	(60)
#define PHASE_STANDBY		(100)
#define PHASE_FORMAT		(120)
#define PHASE_FORMAT_IDLE	(121)
#define PHASE_FINISH		(130)
#define PHASE_FINISH_IDLE 	(131)

#ifndef SCE_PADstart
#define SCE_PADstart   (1<<11)
#endif /* SCE_PADstart */


/* �`��� */
#define SUNIT		0x01
#define PACKETSIZE	(0x100*SUNIT)
static sceVif1Packet packet[2];
static u_long128 packet_buff[2][PACKETSIZE];
static sceGsDBuff db;
static int sindex = 0;

/* �t�H���g�\���p */
static int scrn;
static int console;
extern void sceDevConsLocate(int cd, u_int lx, u_int ly);
static int hcount;
static char mes_hcount[128];
static char mes_state[128];
static char mes_cmds[128];

/* PAD2 */
static int socket_pad;
static scePad2SocketParam padSocket;
static u_long128 pad_buf[SCE_PAD2_DMA_BUFFER_MAX] __attribute__((aligned (64)));
static unsigned char pad_rdata[256];
static unsigned short pad_state;

/* MC2 */
static int socket_mc2;
static SceMc2DirParam dir;
static SceMc2InfoParam info;
static SceMc2SocketParam mcSocket;
static u_long128 mc2_dma_buf[SCE_MC2_DMA_BUFFER_MAX] __attribute__((aligned (64)));
static unsigned char bufs[512*1024];
/***************************************************************************


***************************************************************************/
/* GS������ */
static void Gs_Initialize(void)
{
	sceDmaEnv env;
	sceDmaChan *p1;
	u_long giftagAD[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1),
			 0x000000000000000eL };

	sceDmaReset(1);
	sceGsResetPath();

	sceVif1PkInit(&packet[0], (u_long128 *)packet_buff[0]);
	sceVif1PkInit(&packet[1], (u_long128 *)packet_buff[1]);

	sceDmaGetEnv(&env);
	env.notify = 1<<SCE_DMA_VIF1; /* notify channel */
	sceDmaPutEnv(&env);

	p1 = sceDmaGetChan(SCE_DMA_VIF1);
	p1->chcr.TTE = 1;

	sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FIELD);
	sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);

	db.clear0.rgbaq.R = db.clear1.rgbaq.R = 64;
	db.clear0.rgbaq.G = db.clear1.rgbaq.G = 64;
	db.clear0.rgbaq.B = db.clear1.rgbaq.B = 128;

	/* Gs initialize packet */
	sindex = 0;
	sceVif1PkReset(&packet[sindex]);
	sceVif1PkCnt(&packet[sindex], 0);
	sceVif1PkOpenDirectCode(&packet[sindex], 0);
	sceVif1PkOpenGifTag(&packet[sindex], *(u_long128*)&giftagAD[0]);

	sceVif1PkReserve(&packet[sindex],
		sceGsSetDefAlphaEnv((sceGsAlphaEnv *)packet[sindex].pCurrent,0) * 4);

	sceVif1PkCloseGifTag(&packet[sindex]);
	sceVif1PkCloseDirectCode(&packet[sindex]);
	sceVif1PkEnd(&packet[sindex], 0);
	sceVif1PkTerminate(&packet[sindex]);

	sceGsSyncV(0);
	FlushCache(0);
	sceDmaSend(p1,(u_int *)(((u_int)packet[sindex].pBase)));
	sceGsSyncPath(0,0);

	while(!sceGsSyncV(0));

	*T0_MODE = T_MODE_CLKS_M | T_MODE_CUE_M; /* Hcount Timer Setting */
}
/***************************************************************************


***************************************************************************/
static int pad_attach( unsigned char* rdata, unsigned short* state )
{
	int pad_state;

	pad_state = scePad2GetState( socket_pad );

	*state = pad_state;
	if( pad_state == scePad2StateStable )
	{
		scePad2Read( socket_pad, rdata );
		return 1;
	}
	return 0;
}

static int pad_is_start_button( unsigned char* rdata, unsigned short state )
{
	int ret;

	if (state!=scePad2StateStable) return 0;

	ret = scePad2GetButtonInfo(socket_pad, rdata, SCE_PAD2_START );
	if (ret<0) return 0;

	return ret;
}

static int pad_is_any_button( unsigned char* rdata, unsigned short state )
{
	unsigned short button;

	if (state!=scePad2StateStable) return 0;

	memcpy(&button, rdata, 2);
	button ^= 0xffff;

	return (button!=0 ? 1:0);
}

/***************************************************************************


***************************************************************************/
int main(void)
{
	int i;
	int cmds;
	int rslt;
	int func;
	int files;
	int frame;
	int count;

	/* ���C���X���b�h�D��x�������� */
	ChangeThreadPriority(GetThreadId(), MAIN_TH_PRIO );

	/* SIF������ */
	sceSifInitRpc(0);

	/* sio2man.irx�����[�h���� */
	while( sceSifLoadModule( SIO2MAN, 0, NULL ) < 0 )
	{
		scePrintf( "Can't load module sio2man\n" );
	}

	/* dbcman.irx�����[�h���� */
	while( sceSifLoadModule( DBCMAN, 0, NULL) < 0 )
	{
		scePrintf("Can't load module dbcman\n");
	}

	/* sio2d.irx�����[�h���� */
	while( sceSifLoadModule( SIO2D, 0, NULL) < 0 )
	{
		scePrintf("Can't load module sio2d\n");
	}

	/* �R���g���[���h���C�o�[���W���[�������[�h���� */
	while( sceSifLoadModule( DS2MAN, 0, NULL ) < 0 )
	{
		scePrintf("Can't load module ds2man\n");
	}

	/* �������J�[�h�h���C�o�[���W���[�������[�h���� */
	if (sceSifLoadModule(MC2MAN, 0, NULL ) < 0)
	{
		scePrintf("Can't load module mc2man\n");
	}

	scePrintf ("*** SAMPLE START ***\n");

	/* GS������ */
	Gs_Initialize();

	/* �t�H���g�V�X�e�������� */
	sceDevConsInit();
	console = sceDevConsOpen(OFFX + (16<<4), OFFY + (24<<4), 75, 6);
	sceDevConsClear(console);
	scrn = sceDevConsOpen(OFFX + (16<<4), OFFY + (80<<4), 75, 40);
	sceDevConsClear(console);

	/* DBC���C�u���������� */
	while( sceDbcInit()!=1 )
	{
		scePrintf ("DBC Init faild\n");
	}

	/* PAD2���C�u���������� */
	while( scePad2Init(0)!=1 )
	{
		scePrintf ("PAD2 Init faild\n");
	}

	/* PAD�o�[�`�����\�P�b�g�̍쐬 */
	padSocket.option = SCE_PAD2_SPECIFIC_PORT;
	padSocket.port  = SCE_PAD2_PORT_1C;
	padSocket.slot  = 0;
	socket_pad = scePad2CreateSocket( &padSocket, pad_buf );
	if ( socket_pad < 0 )
	{
		scePrintf ("CreateSocket Faild\n");
		return 0;
	}
	scePrintf ("PAD SOCKET = %d\n", socket_pad);

	/* MC2���C�u���������� */
	while( sceMc2Init(0)<0 )
	{
		scePrintf ("MC2 Init faild\n");
		return 0;
	}

	/* MC2�T�u�X���b�h�D��x�̐ݒ� */
	sceMc2ChangeThreadPriority( MC2_TH_PRIO );

	/* MC2�o�[�`�����\�P�b�g�̍쐬 */
	mcSocket.option = SCE_MC2_SPECIFIC_PORT;
	mcSocket.port  = SCE_MC2_PORT_1;
	mcSocket.slot  = 0;
	socket_mc2 = sceMc2CreateSocket(&mcSocket, mc2_dma_buf);
	scePrintf ("MC2 SOCKET = %d\n", socket_mc2);

	cmds = 0;
	rslt = 0;
	count = 0;
	frame = 0;
	sceGsSyncPath(0,0);

	func = PHASE_START;

	while (1)
	{
		sceGsSwapDBuff(&db, frame);
		++frame;
		pad_attach( pad_rdata, &pad_state );

		*T0_COUNT = 0;		/* Hcounter reset */

		/* �t�H�[�}�b�g�m�F�t�F�[�Y */
		if (func == PHASE_FORMAT_IDLE)
		{
			if (frame% 48>=24)
				sceDevConsPrintf(scrn, "-- PRESS START BUTTON TO FORMAT --  \r" );
			else	sceDevConsPrintf(scrn, "-- PRESS START BUTTON TO FORMAT -- _\r" );
			if ( pad_is_start_button( pad_rdata, pad_state ))
			{
				sceDevConsPrintf(scrn, "                                    \n" );
				sceMc2Sync(NULL,NULL);
				func = PHASE_FORMAT_EXEC;
			}
		}

		/* �����J��Ԃ��m�F�t�F�[�Y */
		if (func == PHASE_FINISH_IDLE)
		{
			if (frame% 48>=24)
				sceDevConsPrintf(scrn, "-- PRESS ANY BUTTON TO CONTINUE --  \r" );
			else	sceDevConsPrintf(scrn, "-- PRESS ANY BUTTON TO CONTINUE -- _\r" );
			if ( pad_is_any_button( pad_rdata, pad_state ))
			{
				sceDevConsPrintf(scrn, "                                    \n" );
				sceMc2Sync(NULL,NULL);
				func = PHASE_GETINFO;
			}
		}

		switch ( sceMc2CheckAsync(&cmds, &rslt))
		{
			/***********************************

			  �o�^�����Ȃ�
			***********************************/
			case SCE_MC2_STAT_EXEC_IDLE:
			switch (func)
			{
				/* �������J�[�h�̏�ԃ`�F�b�N */
				case PHASE_START:
				sceMc2GetInfoAsync(socket_mc2, NULL);
				strcpy (mes_cmds, "GETINFO");
				break;

				/* �J�[�h���擾 */
				case PHASE_GETINFO:
				sceMc2GetInfoAsync(socket_mc2, &info);
				strcpy (mes_cmds, "GETINFO");
				break;

				/* �t�@�C�����݊m�F */
				case PHASE_GETDIR:
				sceMc2GetDirAsync(socket_mc2, TEST_FILE, 0, 1, &dir, &files );
				strcpy (mes_cmds, "GETDIR");
				break;

				/* �t�@�C���쐬 */
				case PHASE_CREAT:
				sceMc2CreateFileAsync(socket_mc2, TEST_FILE );
				strcpy(mes_cmds, "CREATE_FILE");
				break;

				/* �t�@�C���������� */
				case PHASE_WRITE:
				for (i=0; i<sizeof(bufs); i++)
				{
					bufs[i] = (i%256);
				}
				sceMc2WriteFileAsync(socket_mc2, TEST_FILE, bufs, 0, sizeof(bufs));
				strcpy(mes_cmds, "WRITE_FILE");
				break;

				/* �t�@�C���ǂ݂��� */
				case PHASE_READ:
				memset(bufs, 0, sizeof(bufs));
				sceMc2ReadFileAsync(socket_mc2, TEST_FILE, bufs, 0, sizeof(bufs));
				strcpy(mes_cmds, "READ_FILE");
				break;

				/* �t�@�C������ */
				case PHASE_DELETE:
				sceMc2DeleteAsync(socket_mc2, TEST_FILE );
				strcpy(mes_cmds,"DELETE");
				break;

				/* �I�� */
				case PHASE_FINISH:
				sceDevConsPrintf(scrn, "\n" );
				strcpy (mes_cmds, "");
				func = PHASE_FINISH_IDLE;

				case PHASE_FINISH_IDLE:
				sceMc2GetInfoAsync(socket_mc2, NULL);
				break;

				/* �t�H�[�}�b�g */
				case PHASE_FORMAT_EXEC:
				sceMc2FormatAsync(socket_mc2);
				strcpy(mes_cmds,"FORMAT");
				break;

				/* �������J�[�h�̍����ւ��܂� */
				case PHASE_STANDBY:
				sceMc2GetInfoAsync(socket_mc2, NULL);
				strcpy (mes_cmds, "");
				break;

				/* �t�H�[�}�b�g�m�F�I�� */
				case PHASE_FORMAT:
				strcpy (mes_cmds, "FORMAT");
				func = PHASE_FORMAT_IDLE;

				case PHASE_FORMAT_IDLE:
				sceMc2GetInfoAsync(socket_mc2, NULL);
				break;

			}
			count = frame;
			break;
			/***********************************

			  ���s��
			***********************************/
			case SCE_MC2_STAT_EXEC_RUN:
			/* ���b�Z�[�W�\�� */
			switch (func)
			{
				case PHASE_START:
				case PHASE_STANDBY:
				case PHASE_FORMAT_IDLE:
				case PHASE_FINISH_IDLE:
				break;

				default:
				if ((frame-count)% 48>=24)
					sceDevConsPrintf(scrn, "%-12s --  \r", mes_cmds );
				else	sceDevConsPrintf(scrn, "%-12s -- _\r", mes_cmds );
				break;
			}
			break;
			/***********************************

			  �����I��
			***********************************/
			case SCE_MC2_STAT_EXEC_FINISH:

			/* ���b�Z�[�W�\�� */
			switch (func)
			{
				case PHASE_START:
				case PHASE_STANDBY:
				case PHASE_FINISH_IDLE:
				case PHASE_FORMAT_IDLE:
				break;
	
				default:
				if (rslt>=SCE_OK)
					sceDevConsPrintf(scrn, "%-12s -- OK\n", mes_cmds );
				else	sceDevConsPrintf(scrn, "%-12s -- NG [%x(%d)]\n", mes_cmds, rslt, SCE_ERROR_ERRNO(rslt));
				break;
			}

			/* �e�֐����Ƃ̏I������ */
			switch (func)
			{
				/* �������J�[�h�̏�ԃ`�F�b�N */
				case PHASE_START:
				if (rslt<SCE_OK)
				{
					strcpy (mes_state, "UNKNOWN");
					if (SCE_ERROR_ERRNO(rslt)==SCE_ENODEV)
					{
						strcpy (mes_state, "NO CARD");
					}
					if (SCE_ERROR_ERRNO(rslt)==SCE_EFORMAT)
					{
						strcpy (mes_state, "NOT FORMAT");
						func = PHASE_FORMAT; /* �t�H�[�}�b�g���� */
					}
					if (SCE_ERROR_ERRNO(rslt)==SCE_ENEW_DEVICE)
					{
						strcpy (mes_state, "CARD DETECT");
						func = PHASE_GETINFO;
						sceDevConsClear(scrn);
					}
					break;
				}
				func = PHASE_GETINFO;
				break;

				/* �J�[�h���擾 */
				case PHASE_GETINFO:
				if (SCE_ERROR_ERRNO(rslt)!=SCE_OK)
				{
					func = PHASE_START;
					break;
				}
				sprintf (mes_state, "FORMATED   [%d CLUST FREE]", info.freeClust);
				func = PHASE_GETDIR;
				break;

				/* �t�@�C�����݊m�F */
				case PHASE_GETDIR:
				if (rslt!=SCE_OK)
				{
					func = PHASE_START;
					break;
				}
				if (files==0)
				{
					func = PHASE_CREAT;
					break;
				}
				func = PHASE_WRITE;
				break;

				/* �t�@�C���쐬 */
				case PHASE_CREAT:
				if (SCE_ERROR_ERRNO(rslt)==SCE_ENOSPC)
				{
					/* �e�ʕs�� */
					func = PHASE_FINISH; /* �J�[�h�����܂� */
					strcpy (mes_state, "NO FREE SPACE");
					break;
				}
				if (rslt!=SCE_OK)
				{
					func = PHASE_START;
					break;
				}
				func = PHASE_WRITE;
				break;

				/* �t�@�C���������� */
				case PHASE_WRITE:
				if (rslt < SCE_OK)
				{
					if (SCE_ERROR_ERRNO(rslt)==SCE_ENOSPC)
					{
						/* �e�ʕs�� */
						func = PHASE_FINISH; /* �J�[�h�����܂� */
						strcpy (mes_state, "NO FREE SPACE");
						break;
					}
					func = PHASE_START;
					break;
				}
				func = PHASE_READ;
				break;

				/* �t�@�C���ǂݍ��� */
				case PHASE_READ:
				if (rslt < SCE_OK)
				{
					func = PHASE_GETINFO;
					break;
				}
				func = PHASE_DELETE;
				break;

				/* �t�@�C������ */
				case PHASE_DELETE:
				if (rslt!=SCE_OK)
				{
					func = PHASE_START;
					break;
				}
				func = PHASE_FINISH;
				break;

				/* �t�H�[�}�b�g */
				case PHASE_FORMAT_EXEC:
				if (rslt==SCE_OK)
				{
					func = PHASE_START;
					break;
				}
				func = PHASE_GETINFO;
				break;

				/* �������J�[�h�̍����ւ��܂� */
				case PHASE_FORMAT_IDLE:
				case PHASE_FINISH_IDLE:
				case PHASE_STANDBY:
				if (SCE_ERROR_ERRNO(rslt)==SCE_ENEW_DEVICE || SCE_ERROR_ERRNO(rslt)==SCE_ENODEV)
				{
					func = PHASE_START;
					break;
				}
				break;
			}
		}

		hcount = *T0_COUNT;
		for (i=0; i< (hcount < 50 ? hcount:50) ; i++) mes_hcount[i] = '#';
		mes_hcount[i] = '\0';

		/* �t�H���g�\�� */
		sceDevConsClear(console);
		sceDevConsLocate(console, 0, 0);
		sceDevConsPrintf(console, "[ LIBMC2 SAMPLE %3d ] %s\n", hcount, mes_hcount);
		sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
		sceDevConsPrintf(console, "\n" );
		sceDevConsPrintf(console, "STATE : %s\n", mes_state );
		sceDevConsPrintf(console, "CMDS  : %-10s [%4d]\n", mes_cmds, frame-count);
		sceDevConsDraw(console);
		sceDevConsDraw(scrn);

		sceGsSyncV(0);
		FlushCache(0);
		sceGsSyncPath(0,0);
	}

	return 1;
}
