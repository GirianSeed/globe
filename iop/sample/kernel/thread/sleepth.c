/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/* 
 *              I/O Processor Library Sample Program
 * 
 *                         - thread -
 * 
 * 
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 * 
 *                            sleepth.c
 * 
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.4.0
 */

#include <stdio.h>
#include <kernel.h>

#define BASE_priority  32

int firstthread(int arg);
int thread(int arg);

int start(int argc, char *argv[])
{
    struct ThreadParam param;
    int	thid;

    printf("\nSleep thread test ! \n");
    param.attr         = TH_C;
    param.entry        = firstthread;
    param.initPriority = BASE_priority;
    param.stackSize    = 0x800;
    param.option       = 0;
    thid = CreateThread(&param);
    if( thid > 0 ) {
	StartThread(thid,0);
	return 0;
    } else {
	return 1;
    }
}

int firstthread(int arg)
{
    struct ThreadParam param;
    int	thid[6], i;
    char cmd[80];

    param.attr         = TH_C;
    param.entry        = thread;
    param.initPriority = BASE_priority+1;
    param.stackSize    = 0x800;
    param.option = 0;
    for( i = 0; i < 6 ; i ++ ) {
	thid[i] = CreateThread(&param);	
	StartThread(thid[i],i);
    }
    for(;;) {
	printf("0..5,up,down > ");
	gets(cmd);
	switch(cmd[0]) {
	case '0': 	case '1':
	case '2':	case '3':
	case '4':	case '5':
	    WakeupThread( thid[cmd[0]-'0'] );   break;
	case 'u': ChangeThreadPriority( TH_SELF, BASE_priority); break;
	case 'd': ChangeThreadPriority( TH_SELF, BASE_priority+2); break;
	}
    }
}

int thread(int arg)
{
    int i;
    printf("thread no. %d start\n",arg);
    for(i = 0;;i++) {
	printf("\t thread no. %d   %d time SleepThread\n",arg, i);
	SleepThread();
	printf("\t thread no. %d   wakeup count = %d\n",arg,
	       CancelWakeupThread( TH_SELF ));
    }
}
