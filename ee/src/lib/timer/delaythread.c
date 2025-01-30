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
 *                         delaythread.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0            2003/05/15      kono        First version
 */


#include <sys/types.h>
#include <eekernel.h>
#include <sceerrno.h>
#include "libtimer.h"


// �v���g�^�C�v�錾
static u_long _sceTimerThreadWakeup(int id, u_long ulSchedule, u_long ulActual, void *arg, void *addr);


// �X���b�h�x�����s��
int sceTimerDelayThread(u_int uiUsec)
{
	struct SemaParam sp;
	int sid;
	int aid;
	u_long ulClock;
	int status;

	// ���荞�݋֎~��Ԃ��ǂ����`�F�b�N
	__asm__ volatile (
		"mfc0     %0, $12"
		: "=r"(status)
	);
	if (!(status & 0x00010000)) {
		// ���荞�݋֎~�������Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_ECPUDI));
	}

	// �e���|�����ɃC�x���g�ʒm�p�Z�}�t�H���쐬����
	sp.initCount = 0;
	sp.maxCount  = 1;
	sp.option    = (u_int)"SceTimerDelayThread";	// �Z�}�t�H�ɖ��O������
	sid = CreateSema(&sp);
	if (sid < 0) {
		// �Z�}�t�H���쐬�ł��Ȃ������Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_ESEMAPHORE));
	}

	// usec�P�ʂ���BUSCLOCK�P�ʂɕϊ�
	ulClock = sceTimerUSec2BusClock(0, uiUsec);

	// �A���[����o�^
	aid = sceTimerSetAlarm(ulClock, _sceTimerThreadWakeup, (void *)sid);
	if (aid < 0) {
		DeleteSema(sid);
		return (aid);
	}

	// �A���[������Z�}�t�H�����΂����̂�ҋ@
	WaitSema(sid);

	// �e���|�����ɍ쐬�����Z�}�t�H���폜
	DeleteSema(sid);
	return (SCE_OK);
}


// �X���b�h�x���p�̃A���[���n���h��
static u_long _sceTimerThreadWakeup(int id, u_long ulSchedule, u_long ulActual, void *arg, void *addr)
{
	(void)id;
	(void)ulSchedule;
	(void)ulActual;
	(void)addr;

	// arg�o�R�œn�����Z�}�t�H�𔭉�
	iSignalSema((int)arg);
	ExitHandler();

	// �߂�l�Ƃ���0��Ԃ����ƂŁA�A���[�����폜
	return (0);
}

