/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                     I/O Processor Library 
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        thread.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.4.0
 */

#ifndef _THREAD_H_
#define _THREAD_H_

#include <sys/types.h>

#define TH_SELF		0
#define TPRI_RUN	0
 
#define	LOWEST_PRIORITY		(32*2-2)
#define	HIGHEST_PRIORITY	1

/*  ����åɴ�����ǽ */
struct ThreadParam {
    int		status;		/* ����åɤξ��� 		*/
    void	*entry;		/* ����ȥꥢ�ɥ쥹 	(c)	*/
    void	*stack;		/* �����å��ΰ����Ƭ 		*/
    int		stackSize;	/* �����å������� 	(c)	*/
    void	*gpReg;		/* gp �쥸������ 		*/
    int		initPriority;	/* ͥ���� ����� 	(c)	*/
    int		currentPriority;/* ͥ���� ������ 		*/
    u_int	attr;		/* °�� 		(c)	*/
    u_int	option;		/* �桼�����ղþ���  	(c)	*/
    int		waitType;	/* �Ԥ����֤μ���		*/
    int		waitId;		/* waittype ���Ԥ��оݤ� ID	*/
    int		wakeupCount;	/* ̤������ wakeup ���		*/
};

/* ThreadParam.status */
#define	THS_RUN		0x01
#define THS_READY	0x02
#define THS_WAIT	0x04
#define THS_SUSPEND	0x08
#define THS_WAITSUSPEND	0x0c
#define THS_DORMANT	0x10

/* ThreadParam.waitType */
#define TSW_SLEEP	1	/* SleepThread()�ˤ���Ԥ����� */
#define TSW_DELAY	2	/* DelayThread()�ˤ���Ԥ����� */
#define TSW_SEMA	3	/* ���ޥե����Ԥ����� */
#define TSW_EVENTFLAG	4	/* ���٥�ȥե饰���Ԥ����� */
#define TSW_MBX		5	/* ��å������ܥå������Ԥ����� */
#define TSW_VPL		6	/* ����Ĺ����ס���������Ԥ����� */
#define TSW_FPL		7	/* ����Ĺ����֥�å��������Ԥ����� */

/* ThreadParam.attr */
#define TH_ASM		0x01000000
#define TH_C		0x02000000
#define TH_COP1		0x20000000	/* Coprocessor 1 usable */
#define TH_COP2		0x40000000	/* Coprocessor 2 usable */
#define TH_COP3		0x80000000	/* Coprocessor 3 usable */
#define TH_UMODE	0x00000008
#define TH_FILLSTACK	0x00100000

int CreateThread( struct ThreadParam *param );
int DeleteThread( int thid );
int StartThread( int thid, u_long arg );
int StartThreadArgs( int thid, int args, void *argp );
int ExitThread();
//	int ExitDeleteThread();
//	int TerminateThread( int thid );
//	int iTerminateThread( int thid );
//	int DisableDispatchThread();
//	int EnableDispatchThread();
int ChangeThreadPriority( int thid, int priority );
int iChangeThreadPriority( int thid, int priority );
int RotateThreadReadyQueue( int priority );
int iRotateThreadReadyQueue( int priority );
//	int ReleaseWaitThread( int thid );
//	int iReleaseWaitThread( int thid );
int GetThreadId();
//	int ReferThreadStatus( int thid, struct ThreadParam *info );
//	int iReferThreadStatus( int thid, struct ThreadParam *info );

/* Ʊ�����̿���ǽ */
/*   ����å�ľ�� */
int SleepThread();
int WakeupThread( int thid );
int iWakeupThread( int thid );
int CancelWakeupThread( int thid );
int iCancelWakeupThread( int thid );
//	int SuspendThread( int thid );
//	int iSuspendThread( int thid );
//	int ResumeThread( int thid );
//	int iResumeThread( int thid );

/*  ���ޥե� */
struct SemaParam {
    int		currentCount;	/* ���ޥե��θ����� */
    int		maxCount;	/* ���ޥե��κ����� */
    int		initCount;	/* ���ޥե��ν���� */
    int		numWaitThreads;	/* ���ޥե��Ԥ�����åɿ� */
    u_int	attr;		/* ���ޥե���°�� */
    u_int	option;		/* �桼�����  */
};

#define SA_FIFO		1
#define SA_THPRI	2
#define SA_IHTHPRI	0x80

//	int CreateSema( struct SemaParam *param );
//	int DeleteSema( int semid );
//	int SignalSema( int semid );
//	int iSignalSema( int semid );
//	int WaitSema( int semid );
//	int PollSema( int semid );
//	int iPollSema( int semid );
//	int ReferSemaStatus( int semid, struct SemaParam *info );
//	int iReferSemaStatus( int semid, struct SemaParam *info );

/*  ���٥�ȥե饰 */

struct EventFlagParam {
    u_int   currentPattern;	/* ���٥�ȥե饰�θ����� */
    u_int   initPattern;	/* ���٥�ȥե饰�ν���� */
    int	    numWaitThreads;	/* ���٥�ȥե饰�Ԥ�����åɿ� */
    u_int   attr;		/* ���٥�ȥե饰��°�� */
    u_int   option;		/* �桼�����  */
};

#define EA_SINGLE	0
#define EA_MULTI	1
#define EW_AND		0
#define EW_OR		1
#define EW_CLEAR	0x10

int CreateEventFlag( struct EventFlagParam *param );
//	int DeleteEventFlag( int evfid );
int SetEventFlag( int evfid, u_long bitpattern );
int iSetEventFlag( int evfid, u_long bitpattern );
int ClearEventFlag( int evfid, u_long bitpattern );
int iClearEventFlag( int evfid, u_long bitpattern );
int WaitEventFlag( int evfid, u_long bitpattern, int waitmode,
			   u_long *resultpat );
//	int PollEventFlag( int evfid, u_long bitpattern, int waitmode,
//			   u_long *resultpat );
//	int iPollEventFlag( int evfid, u_long bitpattern, int waitmode,
//			    u_long *resultpat );
//	int ReferEventFlagStatus( int evfid, struct EventFlagParam *info );
//	int iReferEventFlagStatus( int evfid, struct EventFlagParam *info );

/*   ��å������ܥå��� */
struct MbxParam {
};

struct MsgPacket {
};

//	int CreateMbx( struct MbxParam *param );
//	int DeleteMbx( int mbxid );
//	int SendMbx( int mbxid, struct MsgPacket *sendmsg );
//	int iSendMbx( int mbxid, struct MsgPacket *sendmsg );
//	int ReceiveMbx( struct MsgPacket **recvmsg, int mbxid );
//	int PollMbx( struct MsgPacket **recvmsg, int mbxid );
//	int iPollMbx( struct MsgPacket **recvmsg, int mbxid );
//	int ReferMbxStatus( int mbxid, struct MbxParam *info );
//	int iReferMbxStatus( int mbxid, struct MbxParam *info );

/* ����ס��������ǽ */
struct VplParam {
};

//	int CreateVpl( struct VplParam *param );
//	int DeleteVpl( int vplid );
//	int AllocateVpl( int vplid, int size, void **block );
//	int pAllocateVpl( int vplid, int size, void **block );
//	int ipAllocateVpl( int vplid, int size, void **block );
//	int FreeVpl( int vplid, void *block );
//	int iFreeVpl( int vplid, void *block );
//	int ReferVpl( int vplid, struct VplParam *info );

struct FplParam {
};

//	int CreateFpl( struct FplParam *param );
//	int DeleteFpl( int fplid );
//	int GetFpl( int fplid , void **block );
//	int pGetFpl( int fplid, void **block );
//	int ipGetFpl( int fplid, void **block );
//	int ReleaseFpl( int fplid, void *block );
//	int iReleaseFpl( int fplid, void *block );
//	int ReferFpl( int fplid, struct FplParam *info );

/* ����/�����޴�����ǽ */
#if 1
//	int SetTime(?);
//	int GetTime(?);
int DelayThread(int usec);
//	int DefTimerHandler(?);
//	int RemoveTimerHandler(??);
#endif

/* �����ƥ������ǽ */
#if 0
//	int GetKenelVersion(?);
//	int ReferSystemStatus(?);
#endif

#endif /* _THREAD_H_ */
