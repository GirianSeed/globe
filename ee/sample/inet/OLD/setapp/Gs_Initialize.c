/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *	Inet Setting Application Sample
 *
 *                          Version 1.4
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Inc.
 *	                 All Rights Reserved.
 *
 *                         Gs_Initialize.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.16      tetsu      Change for PPPoE
 */
#include "setapp.h"

/* for GS Initialize */
#define SUNIT				(0x01)
#define PACKETSIZE			(0x100 * SUNIT)
#define WORKSIZE			(0x80)
#define WORKBASE			(0x70000000)
#define SUPPER				(WORKBASE + WORKSIZE)
#define SLOWER				(WORKBASE + WORKSIZE + PACKETSIZE)

sceGsDBuff db;

void
Gs_Initialize(void)
{
  int sindex;
  sceVif1Packet packet[2];
  sceDmaEnv env;
  sceDmaChan *p1;
  u_long giftagAD[2] = {SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0x000000000000000eL};
  sceDevVif0Reset();
  sceDevVu0Reset();
  sceGsResetPath();
  sceDmaReset(1);

  sceVif1PkInit(&packet[0], (u_long128 *)SUPPER);
  sceVif1PkInit(&packet[1], (u_long128 *)SLOWER);

  sceDmaGetEnv(&env);
  env.notify = 1 << SCE_DMA_VIF1;
  sceDmaPutEnv(&env);

  p1 = sceDmaGetChan(SCE_DMA_VIF1);
  p1->chcr.TTE = 1;

  sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
  sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		   SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);

  db.clear0.rgbaq.R = 0x40;
  db.clear0.rgbaq.G = 0x40;
  db.clear0.rgbaq.B = 0x80;

  db.clear1.rgbaq.R = 0x40;
  db.clear1.rgbaq.G = 0x40;
  db.clear1.rgbaq.B = 0x80;

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

  sceDmaSend(p1,(u_int *)(((u_int)packet[sindex].pBase&0x0fffffff)|0x80000000));

  while(!sceGsSyncV(0));
}
