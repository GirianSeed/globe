/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *              I/O Processor Library Sample Program
 * 
 *                         - thread -
 * 
 * 
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 * 
 *                            profiling.c
 * 
 *   Date            Design      Log
 *  --------------------------------------------------------------------
 *  2002-5-15
 */

#include <stdio.h>
#include <loadcore.h>
#include <modload.h>
#include <thread.h>
#include <kerror.h>

#define BASE_priority  32

void printThStat(int thid, struct ThreadInfo *info);
void printThRunStat(int thid, ThreadRunStatus *stat);
void printSysStat(SystemStatus *stat);
void printModuleName(void *addr);

#define MAXID 100
int idlist[MAXID];
int subidlist[MAXID];

int dummy(){ return KE_ERROR ;}

int start(int argc, char *argv[])
{
    int prio, i, j, ircnt, jrcnt, imax, jmax;
    struct ThreadInfo info;
    ThreadRunStatus stat;
    SystemStatus sysstat;
    struct SemaInfo seminfo;
    struct EventFlagInfo evfinfo;

    dummy();
    prio = GetThreadCurrentPriority();
    if( prio < KE_OK ) {
	printf("\n\nNeed new IOP kernel\n\n");
	return NO_RESIDENT_END;
    }
    printf("\n");
    if( ReferSystemStatus(&sysstat, sizeof(SystemStatus)) == KE_OK )
	printSysStat(&sysstat);

    printf("\nstart() fucntion running thread Priority = %d\n", prio);
    printf("  this thread stack now left %d byte\n",CheckThreadStack());
    printf("  this thread stack free %d byte\n",
	   GetThreadStackFreeSize(TH_SELF));

    printf("this thread status ===============================\n");
    ChangeThreadPriority(TH_SELF, BASE_priority+10);
    if( ReferThreadStatus( TH_SELF, &info ) == KE_OK )
	printThStat(GetThreadId(), &info);
    if( ReferThreadRunStatus(TH_SELF, &stat, sizeof(ThreadRunStatus)) == KE_OK)
	printThRunStat(GetThreadId(), &stat);

    printf("\nAll thread list =====================\n");
    ircnt =  GetThreadmanIdList(TMID_Thread, idlist, MAXID, &imax);
    for( i = 0; i < ircnt; i++) printf("%x ",idlist[i]);
    printf("\n");

    printf("\nAll semaphore list =====================\n");
    ircnt =  GetThreadmanIdList(TMID_Semaphore, idlist, MAXID, &imax);
    for( i = 0; i < ircnt; i++) printf("%x ",idlist[i]);
    printf("\n");

    printf("\nAll eventflag list =====================\n");
    ircnt =  GetThreadmanIdList(TMID_EventFlag, idlist, MAXID, &imax);
    for( i = 0; i < ircnt; i++) printf("%x ",idlist[i]);
    printf("\n");

    printf("\nAll thread status =====================\n");
    ircnt =  GetThreadmanIdList(TMID_Thread, idlist, MAXID, &imax);
    for( i = 0; i < ircnt; i++) {
	if( ReferThreadStatus( idlist[i], &info ) == KE_OK ) {
	    printModuleName(info.entry);
	    printThStat(idlist[i], &info);
	}
	if( ReferThreadRunStatus( idlist[i], &stat,
				  sizeof(ThreadRunStatus)) == KE_OK)
	    printThRunStat(idlist[i], &stat);
	printf("\n");
    }

    printf("\nsemaphore wait list =====================\n");
    ircnt =  GetThreadmanIdList(TMID_Semaphore, idlist, MAXID, &imax);
    for( i = 0; i < ircnt; i++) {
	if( ReferSemaStatus(idlist[i], &seminfo) == KE_OK
	    && seminfo.numWaitThreads > 0 ) {
	    printf(" semaphore %x has %d wait threads =====================\n",idlist[i],
		   seminfo.numWaitThreads );
	    jrcnt =  GetThreadmanIdList(idlist[i], subidlist, MAXID, &jmax);
	    for( j = 0; j < jrcnt; j++) {
		if( ReferThreadRunStatus( subidlist[j], &stat,
					  sizeof(ThreadRunStatus)) == KE_OK)
		    printThRunStat(subidlist[j], &stat);
		printf("\n");
	    }
	}
    }
    printf("\n");

    printf("\neventflag wait list =====================\n");
    ircnt =  GetThreadmanIdList(TMID_EventFlag, idlist, MAXID, &imax);
    for( i = 0; i < ircnt; i++) {
	if( ReferEventFlagStatus(idlist[i], &evfinfo) == KE_OK
	    && evfinfo.numWaitThreads > 0 ) {
	    printf(" eventflag %x has %d wait threads =====================\n",idlist[i],
		   evfinfo.numWaitThreads );
	    jrcnt =  GetThreadmanIdList(idlist[i], subidlist, MAXID, &jmax);
	    for( j = 0; j < jrcnt; j++) {
		if( ReferThreadRunStatus( subidlist[j], &stat,
					  sizeof(ThreadRunStatus)) == KE_OK)
		    printThRunStat(subidlist[j], &stat);
		printf("\n");
	    }
	}
    }
    printf("\n");

    ChangeThreadPriority(TH_SELF, prio);
    return NO_RESIDENT_END;
}

void printThStat(int thid, struct ThreadInfo *info)
{
    printf("Thrad 0x%x\n", thid);
    printf(" attr= 0x%x,", info->attr);
    printf(" option= 0x0x%x,", info->option);
    printf(" entry= 0x0x%p\n", info->entry);
    printf(" stack/stackSize= 0x%p/0x%x,", info->stack, info->stackSize);
    printf(" gpReg= 0x%p, ", info->gpReg);
    printf(" initPriority = 0x%x\n", info->initPriority);
    printf(" status= 0x%x,", info->status);
    printf(" currentPriority= 0x%x\n", info->currentPriority);
    printf(" waitType,waitId,wakeupCount= 0x%x,0x%x,0x%x\n",
	   info->waitType,info->waitId,info->wakeupCount);
    printf(" regContext= 0x%p\n", info->regContext);

}

void printThRunStat(int thid, ThreadRunStatus *stat)
{
    printf("Thrad 0x%x\n", thid);
    printf(" status= 0x%x,", stat->status);
    printf(" currentPriority= 0x%x\n", stat->currentPriority);
    printf(" waitType,waitId,wakeupCount= 0x%x,0x%x,0x%x\n",
	   stat->waitType,stat->waitId,stat->wakeupCount);
    printf(" regContext= 0x%p,", stat->regContext);
    printf(" runClocks= 0x%08x_%08x\n",
	   stat->runClocks.hi, stat->runClocks.low);
    printf(" intrPreemptCount,threadPreemptCount,releaseCount= 0x%x,0x%x,0x%x\n",
	   stat->intrPreemptCount, stat->threadPreemptCount, 
	   stat->releaseCount);
}


void printSysStat(SystemStatus *stat)
{
    printf("System status=0x%x, systemLowTimerWidth=0x%x,",
	  stat->status, stat->systemLowTimerWidth);
    printf("  runClocks= 0x%08x_%08x\n",
	   stat->idleClocks.hi, stat->idleClocks.low);
    printf("  comesOutOfIdleCount,threadSwitchCount = %d,%d\n",
	   stat->comesOutOfIdleCount,stat->threadSwitchCount);
}

void printModuleName(void *addr)
{
    int modid = SearchModuleByAddress(addr);
    ModuleStatus mst;
    if( modid >= KE_OK ) {
	if( ReferModuleStatus(modid, &mst) == KE_OK ) {
	    printf("Module Name = %s\n", mst.name);
	}
    }
}
