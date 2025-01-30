/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - <ball_game> -
 *
 *                         Version <1.00>
 *                           Shift-JIS
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            <poweroff.c>
 *                     <function for dev9 power off>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           May,22,2001     komaki      first version
 */

#include <stdio.h>
#include <eekernel.h>
#include <sifdev.h>
#include <libcdvd.h>

#define STACKSZ		(512 * 16)

static u_char stack[STACKSZ] __attribute__ ((aligned(16)));

void PowerOffThread(void *arg) {
	int sid = (int)arg;
	int stat;
	while(1) {
		WaitSema(sid);
		printf("power off request has come.\n");
		// dev9 power off, need to power off PS2
		while(sceDevctl("dev9x:", DDIOC_OFF, NULL, 0, NULL, 0) < 0);
		// PS2 power off
		while(!sceCdPowerOff(&stat) || stat);
    }
}

void PowerOffHandler(void *arg) {
    int sid = (int)arg;
    iSignalSema(sid);
}

void PreparePowerOff(void) {
    struct ThreadParam tparam;
    struct SemaParam   sparam;
    int tid;
    int sid;

    sparam.initCount = 0;
    sparam.maxCount  = 1;
    sparam.option    = 0;
    sid = CreateSema(&sparam);

    ChangeThreadPriority(GetThreadId(), 2);
    tparam.stackSize = STACKSZ;
    tparam.gpReg = &_gp;
    tparam.entry = PowerOffThread;
    tparam.stack = (void *)stack;
    tparam.initPriority = 1;
    tid = CreateThread(&tparam);
    StartThread(tid, (void *)sid);

    sceCdPOffCallback(PowerOffHandler, (void *)sid);
}
