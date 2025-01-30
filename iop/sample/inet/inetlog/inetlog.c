/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                    I/O Processor Library
 *
 *                        - inetlog -
 *
 *                        Version 1.03
 *                          Shift-JIS
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 * inetlog.c - display/transfer INET logging informations 
 *
 * $Id: inetlog.c,v 1.8 2002/12/12 00:32:49 ksh Exp $
 */

#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/file.h>

#if defined(REMOVABLE_RESIDENT_END) && !defined(DISABLE_MODULE_UNLOAD)
#define ENABLE_MODULE_UNLOAD
#endif

#if defined(USE_LOCAL_INET_HEADERS)
#include "inet.h"
#else	/* USE_LOCAL_INET_HEADERS */
#include <inet/inet.h>
#endif	/* USE_LOCAL_INET_HEADERS */

ModuleInfo Module = { "INET_log", MODVER };

static char buf[1024];

static int thread_priority = USER_LOWEST_PRIORITY;
static int thread_stack = 0x1000;      /* 4KB */
static int tid;
#if defined(ENABLE_MODULE_UNLOAD)
static int eid;
#endif

static int usage(void){
	printf("Usage: inetlog [thpri=<prio>] [thstack=<stack>] [<fname>]\n");
	return(-1);
}

static void inetlog_thread(u_long arg){
	int fd = (int)arg, r, n;

	while(0 < (n = sceInetGetLog(buf, sizeof(buf), -1)))
		if(n != (r = write(fd, buf, n))){
			printf("inetlog_thread: write %d -> %d\n", n, r);
			goto err;
		}
	if(n != sceINETE_ABORT)
		printf("inetlog_thread: sceInetGetLog -> %d\n", n);
err:	close(fd);
#if defined(ENABLE_MODULE_UNLOAD)
	SetEventFlag(eid, 0x1);
#endif
	while(1)
		SleepThread();
}

int start_inetlog(int ac, char *av[]){
        struct ThreadParam tparam;
#if defined(ENABLE_MODULE_UNLOAD)
	struct EventFlagParam eparam;
#endif
	int fd, r;
	char *fname = "tty0:", *bp;

	for(--ac, ++av; 0 < ac && *av[0] == '-'; --ac, ++av)
		if(!strncmp("thpri=", av[0], 6)){
			bp = av[0] + 6;
			if(!isdigit(*bp)){
invalid_option:			printf("inetlog: invalid option (%s)\n",
					av[0]);
				return(-6);
			}
			thread_priority = atoi(bp);
			if(thread_priority < USER_HIGHEST_PRIORITY
				    || USER_LOWEST_PRIORITY < thread_priority)
				goto invalid_option;
			for( ; *bp != '\0' && isdigit(*bp); bp++)
				;
			if(*bp != '\0')
				goto invalid_option;
		}else if(!strncmp("thstack=", av[0], 8)){
			bp = av[0] + 8;
			if(!isdigit(*bp))
				goto invalid_option;
			thread_stack = atoi(bp);
			for( ; *bp != '\0' && isdigit(*bp); bp++)
				;
			if(!strcmp(bp, "KB"))
				thread_stack <<= 10;
			else if(*bp != '\0')
				goto invalid_option;
		}else if(!strcmp("-help", av[0]))
			return(usage());
		else
			return(usage());
	if(1 < ac)
		return(usage());
	if(0 < ac)
		fname = av[0];
	if(0 > (fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0666))){
		printf("inetlog: can not open %s\n", fname);
		return(-2);
	}

#if defined(ENABLE_MODULE_UNLOAD)
	eparam.attr = EA_SINGLE;
	eparam.initPattern = 0;
	eparam.option = 0;
	if(0 >= (eid = CreateEventFlag(&eparam))){
		printf("inetlog: CreateEventFlag (%d)\n", eid);
		close(fd);
		return(-5);
	}
#endif
        tparam.attr = TH_C;
        tparam.entry = inetlog_thread;
        tparam.initPriority = thread_priority;
        tparam.stackSize = thread_stack;
        tparam.option = 0;
        if(0 >= (tid = CreateThread(&tparam))){
                printf("inetlog: CreateThread (%d)\n", tid);
		close(fd);
#if defined(ENABLE_MODULE_UNLOAD)
		DeleteEventFlag(eid);
#endif
		return(-3);
        }
        if(KE_OK != (r = StartThread(tid, (u_long)fd))){
                printf("inetlog: StartThread (%d)\n", r);
                DeleteThread(tid);
		close(fd);
#if defined(ENABLE_MODULE_UNLOAD)
		DeleteEventFlag(eid);
#endif
		return(-4);
        }
	return(0);
}

#if defined(ENABLE_MODULE_UNLOAD)
static int unloadable(void){
	int dummy, mcount = -1;

	GetModuleIdList(&dummy, 1, &mcount);
	return((mcount < 0)? 0: 1);
}
#endif

int start(int ac, char *av[]){
	int r;

#if defined(ENABLE_MODULE_UNLOAD)
	if(ac < 0){
		u_long result;

		sceInetAbortLog();
		WaitEventFlag(eid, 0x1, EW_OR | EW_CLEAR, &result);
		DeleteEventFlag(eid);
		TerminateThread(tid);
		DeleteThread(tid);
		return(NO_RESIDENT_END);
	}
#endif
	r = start_inetlog(ac, av);
	if(0 != r)
		return((r << 4) | NO_RESIDENT_END);
#if defined(ENABLE_MODULE_UNLOAD)
	if(unloadable())
		return(REMOVABLE_RESIDENT_END);
#endif
	return(RESIDENT_END);
}
