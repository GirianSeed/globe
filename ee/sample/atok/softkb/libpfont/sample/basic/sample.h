/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
 */
/* 
 *                libpfont Library Sample Program
 *
 *                           - basic -
 *
 *                          Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                             sample.h
 *                      main function of basic.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Jan,14,2002      gens
 */

#ifndef __sample__
#define __sample__

#include <eetypes.h>

#include <libvu0.h>
#include <libvifpk.h>

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

#define CLUT_GSMEM		(TBP_TEX0 + (FBS_FRAME * 2048 / 64))
#define CLUT_GSMEMSIZE	(2048 / 64 * 1)
#define FONT_GSMEM		(CLUT_GSMEM + CLUT_GSMEMSIZE)
#define FONT_GSMEMSIZE	(2048 / 64 * 1)

#define FONT_DATASIZE	(3 * 1024 * 1024)

#define SCRATCH_PAD_RAM		(0x70000000)
#define PKTBUF_SIZE			(256)

int  appInit(void);
int  appRun(int argc, char* argv[]);
void appExit(void);

#endif /* __sample__ */
