/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                       Timer Library
 *                        Version 1.0
 *                         Shift-JIS
 *
 *      Copyright (C) 2001-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         alarm.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0            2003/05/15      kono        First version
 */


#include <sys/types.h>
#include <eekernel.h>
#include <sceerrno.h>
#include "libtimer.h"


typedef struct ALARM {
	struct ALARM *pNext;						// �P���������N���X�g
	volatile int counterid;						// �J�E���^ID
	u_long (*cbHandler)(int, u_long, u_long, void *, void *);	// �A���[���n���h���̊֐��|�C���^
	void *arg;									// �A���[���n���h���ɓn������
} ALARM __attribute__((aligned(16)));

#define ALARM2ID(_p, _u)	(int)(((((u_int)(_p))<<4) | ((_u) & 0xFE) | 1))
#define ID2ALARM(_p)		(ALARM *)((((u_int)(_p))>>8)<<4)
#define IS_VALIDID(_id)		(((int)(_id)>=0) && (((((u_int)(_id)) ^ ((ID2ALARM(_id))->counterid)) & 0xFF)==0))


// �v���g�^�C�v�錾
int _sceTimerAlarmInit(void);
static __inline__ ALARM *AllocAlarm(void);
static __inline__ void FreeAlarm(ALARM *pAlarm);
static u_long AlarmHandler(int counterid, u_long ulSchedule, u_long ulActual, void *arg, void *addr);

static ALARM g_AlarmBuf[SCE_TIMER_MAX_SOFTALARM] __attribute__((aligned(16)));
static ALARM *g_pFreeAlarm;


// �A���[���p���[�N�G���A�̃����N���X�g��������
int _sceTimerAlarmInit(void)
{
	int i;
	g_pFreeAlarm = &g_AlarmBuf[0];
	for (i=0; i<SCE_TIMER_MAX_SOFTALARM; i++) {
		g_AlarmBuf[i].pNext = &g_AlarmBuf[i + 1];
	}
	g_AlarmBuf[SCE_TIMER_MAX_SOFTALARM - 1].pNext = NULL;
	return (0);
}


// �A���[���p���[�N���m��
static __inline__ ALARM *AllocAlarm(void)
{
	ALARM *pAlarm;
	pAlarm = g_pFreeAlarm;
	if (pAlarm!=NULL) {
		g_pFreeAlarm = pAlarm->pNext;
	}
	return (pAlarm);
}


// �A���[���p���[�N�����
static __inline__ void FreeAlarm(ALARM *pAlarm)
{
	pAlarm->pNext = g_pFreeAlarm;
	pAlarm->counterid = 0;					// �J�E���^ID�𖳌���Ԃɂ���
	g_pFreeAlarm  = pAlarm;
	return;
}


// �A���[���̂��߂̃J�E���^��v�n���h��
// ��������{���̃A���[���n���h�����Ăяo��
static u_long AlarmHandler(int counterid, u_long ulSchedule, u_long ulActual, void *arg, void *addr)
{
	ALARM *pAlarm = (ALARM *)arg;
	u_long ulNext = pAlarm->cbHandler(ALARM2ID(pAlarm, counterid), ulSchedule, ulActual, pAlarm->arg, addr);

	if (ulNext==0) {
		// �A���[���I�����A�J�E���^���폜����
		FreeAlarm(pAlarm);
		return ((u_long)-1);
	}
	return (ulNext);
}


// �A���[����ݒ�
int sceTimerSetAlarm(u_long ulClock, u_long (*cbHandler)(int, u_long, u_long, void *, void *), void *arg)
{
	int intr;
	ALARM *pAlarm;
	int counterid;

	if (cbHandler==NULL) {
		// �n���h���֐��Ƃ���NULL�|�C���^���n���ꂽ
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINVAL));
	}

	intr = DI();							// ���荞�݋֎~

	// �A���[���p���[�N�G���A���m��
	pAlarm = AllocAlarm();
	if (pAlarm==NULL) {
		// �A���[�����[�N�̃A���P�[�g�Ɏ��s
		if (intr) EI();
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_ETIMER));
	}

	// �J�E���^���m��
	counterid = sceTimerAllocCounter();
	if (counterid<0) {
		// �J�E���^�̃A���P�[�g�Ɏ��s
		FreeAlarm(pAlarm);					// �A���[���̃��[�N�����
		if (intr) EI();
		return (counterid);
	}

	// �J�E���g�l��v�n���h����o�^
	pAlarm->counterid = counterid;
	pAlarm->cbHandler = cbHandler;
	pAlarm->arg       = arg;
	sceTimerSetHandler(counterid, ulClock, AlarmHandler, pAlarm);

	sceTimerStartCounter(counterid);		// �J�E���^���N��

	if (intr) EI();							// ���荞�݋���Ԃ̕��A
	return (ALARM2ID(pAlarm, counterid));
}


// �A���[����ݒ�
int isceTimerSetAlarm(u_long ulClock, u_long (*cbHandler)(int, u_long, u_long, void *, void *), void *arg)
{
	ALARM *pAlarm;
	int counterid;

	if (cbHandler==NULL) {
		// �n���h���֐��Ƃ���NULL�|�C���^���n���ꂽ
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINVAL));
	}

	// �A���[���p���[�N�G���A���m��
	pAlarm = AllocAlarm();
	if (pAlarm==NULL) {
		// �A���[�����[�N�̃A���P�[�g�Ɏ��s
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_ETIMER));
	}

	// �J�E���^���m��
	counterid = isceTimerAllocCounter();
	if (counterid<0) {
		// �J�E���^�̃A���P�[�g�Ɏ��s
		FreeAlarm(pAlarm);					// �A���[���̃��[�N�����
		return (counterid);
	}

	// �J�E���g�l��v�n���h����o�^
	pAlarm->counterid = counterid;
	pAlarm->cbHandler = cbHandler;
	pAlarm->arg       = arg;
	isceTimerSetHandler(counterid, ulClock, AlarmHandler, pAlarm);

	isceTimerStartCounter(counterid);		// �J�E���^���N��
	return (ALARM2ID(pAlarm, counterid));
}


// �A���[�������
int sceTimerReleaseAlarm(int id)
{
	int intr;
	ALARM *pAlarm = ID2ALARM(id);

	intr = DI();							// ���荞�݋֎~
	if (!IS_VALIDID(id)) {
		// �������Ȃ��A���[��ID�������ꍇ
		if (intr) EI();						// ���荞�݋���Ԃ̕��A
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	}

	sceTimerFreeCounter(pAlarm->counterid);	// �J�E���^���J��
	FreeAlarm(pAlarm);						// �A���[���p���[�N�G���A���J��
	if (intr) EI();							// ���荞�݋���Ԃ̕��A
	return (SCE_OK);
}


// �A���[�������
int isceTimerReleaseAlarm(int id)
{
	ALARM *pAlarm = ID2ALARM(id);
	int res;
	if (!IS_VALIDID(id)) {
		// �������Ȃ��A���[��ID�������ꍇ
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	}

	res = isceTimerFreeCounter(pAlarm->counterid);	// �J�E���^���J��
	if (res==SCE_OK) {
		FreeAlarm(pAlarm);					// �A���[���p���[�N�G���A���J��
	}
	return (res);
}

