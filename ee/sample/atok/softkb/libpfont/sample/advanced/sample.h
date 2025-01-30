/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#ifndef __sample__
#define __sample__

#include <eetypes.h>
#include <eekernel.h>
#include <eeregs.h>
#include <eestruct.h>

#include <libvu0.h>

#define __INLINE__
#include "xvifpk.h"
#undef __INLINE__
#include "misc.h"

#define	__MFIFO__
//#define	__PKTDBG__


#define MAIN_THREAD_PRIO	(64)

#define IDLE_THREAD_PRIO	(127)
#define IDLE_STACK_SIZE		(1024)

#define SCREEN_WIDTH	(640)
#define SCREEN_HEIGHT	(224)
#define SCREEN_CENTERX	(2048)
#define SCREEN_CENTERY	(2048)
#define SCREEN_TOP		(SCREEN_CENTERY - SCREEN_HEIGHT / 2)
#define SCREEN_LEFT		(SCREEN_CENTERX - SCREEN_WIDTH / 2)
#define SCREEN_RIGHT	(SCREEN_LEFT + SCREEN_WIDTH)
#define SCREEN_BOTTOM	(SCREEN_TOP + SCREEN_HEIGHT)

#define FBS_FRAME		((SCREEN_WIDTH * SCREEN_HEIGHT + 2047) / 2048)
#define FBP_FRAME0		(0)
#define FBP_FRAME1		(FBP_FRAME0 + FBS_FRAME)
#define FBP_ZBUF		(FBP_FRAME1 + FBS_FRAME)
#define FBP_TEXTOP		(FBP_ZBUF + FBS_FRAME)
#define TBP_TEX0		(FBP_TEXTOP * 2048 / 64)

#define PKTBUF_SIZE		(256)

#define CLUT_GSMEM		(TBP_TEX0 + (FBS_FRAME * 2048 / 64))
#define CLUT_GSMEMSIZE	(2048 / 64 * 1)
#define FONT_GSMEM		(CLUT_GSMEM + CLUT_GSMEMSIZE)
#define FONT_GSMEMSIZE	(2048 / 64 * 1)

#define FONT_DATASIZE	(3 * 1024 * 1024)


typedef struct _SampleStr{
	u_long128 pktbuf[2][PKTBUF_SIZE];

	sceVif1Packet packet;

	int volatile exit_flag;

	u_long128 font_cache[64];	// 1k bytes

}SampleStr;

int appInit(void);
int appRun(int argc, char const* const argv[]);
void appExit(void);

SampleStr* GetApp(void);
sceVif1Packet* GetPkt(void);


void sampleDrawSync(void);
void sampleDrawKick(void);
void samplePuts(char const* str, int x, int y);
void sampleCalcChrPos(char const* str, int x, int y, sceVu0FVECTOR* pPosList);
void sampleDrawSolidRect(int left, int right, int top, int bottom, u_int rgba);
void sampleClearZBuffer(u_int z);
void sampleSetTexture(u_int tbp, u_int tbw, u_int tpsm, u_int tw, u_int th, u_int cbp, u_int cpsm);
void sampleDrawSprite(int u, int v, int w, int h, int x, int y);
void sampleDrawSpriteEx(int u, int v, int uw, int uh, int x, int y, int w, int h);

void jokeKPuts(char const* str, int x, int y);


///////////////////////////////////////

static SampleStr* const __theApp__ = (SampleStr*)0x70000000U;

extern __inline__ SampleStr* GetApp(void){
	return __theApp__;
}

extern __inline__ sceVif1Packet* GetPkt(void){
	return &(GetApp()->packet);
}


#endif /* __sample__ */
