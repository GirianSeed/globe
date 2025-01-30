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
 *                         libtimer.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0            2003/05/15      kono        First version
 */


#include <sys/types.h>
#include <string.h>
#include <eeregs.h>
#include <eekernel.h>
#include <sceerrno.h>
#include "libtimer.h"


// EE��BUSCLOCK���x���`
#define BUSCLOCK		147456000UL
// 147.456MHz = 48kHz * 768 * 4
//              |        |    |
//              |        |    +--- EE(bus)/IOP�̕�����
//              |        +-------- IOP/SPU2�̕�����
//              +----------------- SPU2���g��


// �o�X�N���b�N���Z�Ŏ��Ԃ�\������
#define SEC2BUSCLK(_s)	(BUSCLOCK * (_s))
#define MSEC2BUSCLK(_m)	(BUSCLOCK * (_m) / (1000))
#define USEC2BUSCLK(_u)	(BUSCLOCK * (_u) / (1000*1000))

// �g�p����^�C�}��؂�ւ�
#if !defined(USE_TIMER0) && !defined(USE_TIMER1)
// �f�t�H���g�̓^�C�}#1���g�p
#if !defined(USE_TIMER0)
#define USE_TIMER0		0
#endif	/* !defined(USE_TIMER0) */
#if !defined(USE_TIMER1)
#define USE_TIMER1		1
#endif	/* !defined(USE_TIMER1) */
#endif	/* !defined(USE_TIMER0) && defined(USE_TIMER1) */

#if USE_TIMER0
// �^�C�}#0���g�p����Ƃ�
#define INTC_TIMER		INTC_TIM0
#define TIMER_BASE		T0_COUNT
#define DPUT_T_MODE		DPUT_T0_MODE
#define DGET_T_MODE		DGET_T0_MODE
#define DPUT_T_COUNT	DPUT_T0_COUNT
#define DGET_T_COUNT	DGET_T0_COUNT
#define DPUT_T_COMP		DPUT_T0_COMP
#define DGET_T_COMP		DGET_T0_COMP
#elif USE_TIMER1
// �^�C�}#1���g�p����Ƃ�
#define INTC_TIMER		INTC_TIM1
#define TIMER_BASE		T1_COUNT
#define DPUT_T_MODE		DPUT_T1_MODE
#define DGET_T_MODE		DGET_T1_MODE
#define DPUT_T_COUNT	DPUT_T1_COUNT
#define DGET_T_COUNT	DGET_T1_COUNT
#define DPUT_T_COMP		DPUT_T1_COMP
#define DGET_T_COMP		DGET_T1_COMP
#else
#error	"define using counter which timer0 or timer1"
#endif


#define PRESCALE_L(_c, _m)	((_c)<<(((_m) & T_MODE_CLKS_M)<<2))
#define PRESCALE_R(_c, _m)	((_c)>>(((_m) & T_MODE_CLKS_M)<<2))


#undef	EI
#define	EI()			__asm__ volatile("sync.l; ei")


#define MODE_START			0x00000001			// �J�E���^���쒆
#define MODE_HANDLER		0x00000002			// �n���h���o�^�ς�

typedef struct COUNTER {
	struct COUNTER *pNext, *pPrev;				// �o���������N
	volatile u_int  uiKey;						// �L�[�o�����[
	u_int  uiMode;								// �J�E���^�̓��샂�[�h�ݒ�
	u_long ulBaseTime;							// �x�[�X����
	u_long ulBaseCount;							// �x�[�X�����ł̃J�E���g�l
	u_long ulSchedule;							// �J�E���^�n���h���Ăяo���\�莞��
	u_long (*cbHandler)(int, u_long, u_long, void *, void *);
												// �J�E���^�n���h���֐�
	void  *gp_value;							// �n���h���Ăяo������$gp���W�X�^�l
	void  *arg;									// �n���h���Ăяo�����̈���
	u_int pad[3];								// 64�o�C�g�ɂ��邽�߂̃p�f�B���O
} COUNTER __attribute__((aligned(64)));

#define COUNTER2ID(_p)		(int)((((u_int)(_p))<<4) | ((_p)->uiKey))
#define ID2COUNTER(_id)		(COUNTER *)((((u_int)(_id))>>10)<<6)
#define IS_VALIDID(_id)		(((int)(_id)>=0) && (((u_int)(_id) & 0x3FF)==((ID2COUNTER(_id))->uiKey)))


// �v���g�^�C�v�錾
extern int _sceTimerAlarmInit(void);

static __inline__ u_long ReferHardCounter(void);
static __inline__ u_long ApplyOverflow(void);
static void InsertAlarm(COUNTER *pCounter);
static COUNTER *UnlinkAlarm(COUNTER *pCounter);
static void SetNextComp(u_long ulNowTime);
static int  cbTimerHandler(int ca, void *arg, void *addr);


static COUNTER g_CounterBuf[SCE_TIMER_MAX_SOFTCOUNTER] __attribute__((aligned(64)));

// �^�C�}�p�̃��[�N
static struct {
	volatile u_long ulHighCount;			// �I�[�o�[�t���[��
	int hid;								// �^�C�}���荞�݃n���h���̃n���h��ID
	volatile u_int uniqkey;					// �J�E���^ID�����̂��߂̃��j�[�N�l
	volatile u_int uiUsedCounters;			// �g�p���̃J�E���^��
	COUNTER *pFree;							// �󂫃J�E���^�̃����N���X�g(�P����)
	COUNTER *pAlarm;						// �A���[�����X�g�ɐڑ�����Ă���J�E���^�̃����N���X�g
	volatile int cbcounterid;				// �R�[���o�b�N�����Ăяo�����̃J�E���^ID
} g_Timer = {
	0UL,
	-1,
	1,
	0,
	NULL,
	NULL,
	-1
};


// �^�C�}�[���z�����C�u����������
int sceTimerInit(u_int uiMode)
{
	int hid;
	u_int mode;
	int intr;
	int i;

	if (g_Timer.hid>=0) {
		// �^�C�}���荞�݃n���h�����o�^����Ă����Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINIT));
	}

	// ���[�N��������
	g_Timer.ulHighCount = 0;
	g_Timer.uiUsedCounters = 0;
	memset(&g_CounterBuf, 0x00, sizeof(g_CounterBuf));

	// �����N���X�g��������
	g_Timer.pFree = &g_CounterBuf[0];
	for (i=0; i<SCE_TIMER_MAX_SOFTCOUNTER; i++) {
		g_CounterBuf[i].pNext = &g_CounterBuf[i + 1];
	}
	g_CounterBuf[SCE_TIMER_MAX_SOFTCOUNTER - 1].pNext = NULL;

	// �A���[���p���[�N�G���A��������
	_sceTimerAlarmInit();

	// INTC���荞�݃n���h���̓o�^
	hid = AddIntcHandler2(INTC_TIMER, cbTimerHandler, 0, NULL);
	if (hid<0) {
		// �G���[
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINT_HANDLER));
	}
	g_Timer.hid = hid;

	// �n�[�h�E�F�A�^�C�}�̏�����
	intr = DI();									// ���荞�݋֎~
	// ���[�h���W�X�^�̒l���v�Z
	mode = DGET_T_MODE();							// ���݂̃��[�h���W�X�^�̒l�𓾂�
	mode = (mode & (u_int)~T_MODE_CLKS_M) | uiMode;	// �v���X�P�[���l��ݒ�
	mode |= (T_MODE_OVFE_M | T_MODE_CMPE_M);		// �I�[�o�[�t���[���荞��,��r�l��v�t���O������
	if (!(mode & T_MODE_CUE_M)) {
		// ���荞�ݒ�~���������Ƃ�
		mode |= T_MODE_CUE_M;						// �^�C�}���N��
		mode |= T_MODE_OVFF_M | T_MODE_EQUF_M;		// �I�[�o�[�t���[�t���O�A��r�l��v�t���O�����Z�b�g
		DPUT_T_COUNT(0);							// �J�E���^�l�����Z�b�g
		DPUT_T_COMP(0xFFFF);						// �R���y�A�l�����Z�b�g
	}
	DPUT_T_MODE(mode);								// �^�C�}���[�h�ݒ�

	EnableIntc(INTC_TIMER);							// �^�C�}���荞�݋���
	if (intr) EI();									// ���荞�݋���Ԃ̕��A
	return (SCE_OK);
}


// �^�C�}�[���z�����C�u�����I��
int sceTimerEnd(void)
{
	int res;
	int intr;

	if (g_Timer.hid<0) {
		// �^�C�}���荞�݃n���h���͓o�^����Ă��Ȃ��Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINIT));
	} else if (g_Timer.uiUsedCounters>0) {
		// �J�E���^��1�ł��g�p���������Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EBUSY));
	}

	intr = DI();									// ���荞�݋֎~
	res = RemoveIntcHandler(INTC_TIMER, g_Timer.hid);
	if (res==0) {
		// �Ō�̊��荞�݂������ꍇ
		DisableIntc(INTC_TIMER);					// �^�C�}���荞�݂��֎~

		// �n�[�h�E�F�A�^�C�}��~
		DPUT_T_MODE(T_MODE_OVFF_M | T_MODE_EQUF_M);
		DPUT_T_COUNT(0);
	}
	g_Timer.ulHighCount = 0;						// �I�[�o�[�t���[�񐔂����Z�b�g
	g_Timer.hid = -1;								// ���荞�݃n���h��ID�����Z�b�g
	if (intr) EI();									// ���荞�݋���Ԃ̕��A
	return (SCE_OK);
}



// �v���X�P�[���l�̎擾
int sceTimerGetPreScaleFactor(void)
{
	if (g_Timer.hid<0) {
		// �^�C�}���荞�݃n���h���͓o�^����Ă��Ȃ��Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINIT));
	}
	return (DGET_T_MODE() & T_MODE_CLKS_M);			// �v���X�P�[���l��ݒ�
}


// �V�X�e���^�C���̋N��
int sceTimerStartSystemTime(void)
{
	u_int mode;
	int intr;
	intr = DI();							// ���荞�݋֎~
	mode = DGET_T_MODE();
	if (mode & T_MODE_CUE_M) {
		// ���łɋN�����Ă����Ƃ�
		if (intr) EI();						// ���荞�݋���Ԃ̕��A
		return (1);
	}
	// �n�[�h�E�F�A�^�C�}�N��
	mode = (mode & (u_int)~(T_MODE_OVFF_M | T_MODE_EQUF_M)) | T_MODE_CUE_M;
	DPUT_T_MODE(mode);

	SetNextComp(isceTimerGetSystemTime());	// ���񊄂荞�ݎ�����ݒ�

	if (intr) EI();							// ���荞�݋���Ԃ̕��A
	return (0);
}


// �V�X�e���^�C���̒�~
int sceTimerStopSystemTime(void)
{
	u_int mode;
	int intr;
	intr = DI();							// ���荞�݋֎~
	mode = DGET_T_MODE();
	if ((mode & T_MODE_CUE_M)==0) {
		// ���łɒ�~���Ă����Ƃ�
		if (intr) EI();						// ���荞�݋���Ԃ̕��A
		return (0);
	}
	// �n�[�h�E�F�A�^�C�}��~
	mode = mode & (u_int)~(T_MODE_OVFF_M | T_MODE_EQUF_M | T_MODE_CUE_M);
	DPUT_T_MODE(mode);
	if (intr) EI();							// ���荞�݋���Ԃ̕��A
	return (1);
}



/*
����! ���̊֐���libtimer�S�̂���Q�Ƃ����V�X�e��������ύX���܂��B
libtimer�͕����̃v���O������1�{�̃n�[�h�E�F�A�^�C�}�����S�ɋ��L����Ƃ���
�ړI�̃��C�u�����ł����A�ǂ���1�̃v���O�������ނ�݂�libtimer�̑S�̂���
�Q�Ƃ����V�X�e��������ύX���Ă��܂��ƁA���̖ړI�𐬏A���邱�Ƃ��ł��Ȃ�
�\��������܂��̂ŁA�R�����g�A�E�g���Ă���܂��B

�R�����g�A�E�g���O���Ďg�p����ꍇ�́A�V�X�e�������̕ύX�ɂ����libtimer��
�񋟂����J�E���^,�A���[��,�X���b�h�x���ɋy�ڂ����e���ɂ��Ă悭������
����Ă��炲�g�p���������B���Ƃ��΁A
sceTimerSetSystemTime(sceTimerGetSystemTime() - 147456000);
�Ƃ���ƃV�X�e���������1�b�߂����Ƃ��ł��܂����A���̂Ƃ��S�ẴJ�E���^��
�n���h���̌Ăяo��������1�b�x��邱�ƂɂȂ�܂��B�����V���b�g�^�C�}��1�b
���x�����A�C���^�[�o���^�C�}��1��������1�b�ԉ��т����T�C�N������������
���B

�ȏ�̂悤�ɁAlibtimer�S�̂ɓ��삪�y��ł��܂��܂��̂ŁA�g�p�ɂ͂����ӂ�
�����������A�R�����g�A�E�g�����R�[�h�ł̒񋟂Ƃ����Ă��������܂����B

// �V�X�e�������ݒ�
void sceTimerSetSystemTime(u_long ulTime)
{
	u_int mode;
	int intr;
	u_long ulPrescale;
	intr = DI();							// ���荞�݋֎~

	mode = DGET_T_MODE();					// ���݂̃��[�h�l�𓾂�
	DPUT_T_MODE(mode);						// �I�[�o�[�t���[�t���O,��r�t���O�����Z�b�g

	ulPrescale = PRESCALE_R(ulTime, mode);	// �������[�h�ɉ����ĉE�V�t�g
	DPUT_T_COUNT(ulPrescale & 0xFFFF);		// �J�E���^�l�̉���16�r�b�g�w��
	g_Timer.ulHighCount = ulPrescale>>16;	// �J�E���^�l�̏�ʃr�b�g�w��
	SetNextComp(ulTime);					// ���񊄂荞�ݎ�����ݒ�
	if (intr) EI();							// ���荞�݋���Ԃ̕��A
	return;
}
*/



/*
���x���グ�邽�߂ɂ͂��̊֐��͍����Ɏ��s�����K�v������̂ŁA�C�����C���A�Z���u���֐��ɂ��܂����B
�����R�[�h�͎c���Ă���܂��B
// �n�[�h�E�F�A�^�C�}�̃J�E���g�l�擾
static __inline__ u_long ReferHardCounter(void)
{
	u_long ulHighCount, ulSystemTime;
	u_int  low, mode;

	low  = DGET_T_COUNT();					// COUNT���W�X�^��ǂݍ���
	mode = DGET_T_MODE();					// MODE���W�X�^��ǂݍ���
	ulHighCount = g_Timer.ulHighCount;
	if (mode & T_MODE_OVFF_M) {
		// �I�[�o�[�t���[���������Ă����Ƃ�
		ulHighCount++;
		low = DGET_T_COUNT();						// COUNT���W�X�^�ēǂݍ���
	}
	ulSystemTime = (ulHighCount<<16) | low;
	ulSystemTime = PRESCALE_L(ulSystemTime, mode);	// ������ɉ����č��V�t�g
	return (ulSystemTime);
}


// �I�[�o�[�t���[�t���O�����Z�b�g���A���ݎ����𐶐�
static __inline__ u_long ApplyOverflow(void)
{
	u_long ulSystemTime;
	u_int  mode;
	u_int  low;

	low  = DGET_T_COUNT();					// COUNT���W�X�^��ǂݍ���
	mode = DGET_T_MODE();					// MODE���W�X�^��ǂݍ���
	if (mode & T_MODE_OVFF_M) {
		// �I�[�o�[�t���[���������Ă����Ƃ�
		g_Timer.ulHighCount++;
		DPUT_T_MODE(mode & (u_int)~T_MODE_EQUF_M);	// �I�[�o�[�t���[�t���O���~�낷
		low = DGET_T_COUNT();						// COUNT���W�X�^�ēǂݍ���
	}
	ulSystemTime = (g_Timer.ulHighCount<<16) | low;
	ulSystemTime = PRESCALE_L(ulSystemTime, mode);	// ������ɉ����č��V�t�g
	return (ulSystemTime);
}
*/


// �n�[�h�E�F�A�^�C�}�̃J�E���g�l�擾
static __inline__ u_long ReferHardCounter(void)
{
	u_long ulSystemTime;
	__asm__ volatile (
		".set			push\n"
		".set			noreorder\n"
		"lwu			%0,  0(%1)\n"		// %0 = Tx_COUNT
		"lw				$8,  0x10(%1)\n"	// t0 = Tx_MODE
		"ld				$10, %2\n"			// t2 = g_Timer.ulHighCount
		"andi			$9,  $8, 0x0800\n"	// t1 = t0 & T_MODE_OVFF_M
		"beq			$9,  $0, 0f\n"		// t1==0�̂Ƃ�(�I�[�o�[�t���[���Ă��Ȃ��Ƃ�)
		"addiu			$9,  $0,  0x0002\n"	//t1 = 2
		"lwu			%0,  0(%1)\n"		// %0 = Tx_COUNT
		"daddiu			$10, $10, 1\n"		// t2 = t2 + 1
	"0:\n"
		"andi			$8,  $8,  0x0003\n"	// t0 = t0 & 3 = (Tx_MODE & T_MODE_CLKS_M)
		"dsll			$10, $10, 16\n"		// t2 = t2 << 16
		"movz			$9,  $0,  $8\n"		// t1 = (t0==0) ? 0 : t1 = ((Tx_MODE & T_MODE_CLKS_M)==0) ? 0 : 2
		"or				%0,  %0,  $10\n"	// %0 = %0 | t2
		"dsllv			$9,  $9,  $8\n"		// t1 = t1 << t0
		"dsll			%0,  %0,  $9\n"		// %0 = %0 << t1
		".set			pop\n"
		: "=&r"(ulSystemTime)
		: "r"(TIMER_BASE), "m"(g_Timer.ulHighCount)
		: "$8", "$9", "$10", "memory"
	);
	return (ulSystemTime);
}


// �I�[�o�[�t���[�t���O�����Z�b�g���A���ݎ����𐶐�
static __inline__ u_long ApplyOverflow(void)
{
	u_long ulSystemTime;
	__asm__ volatile (
		".set			push\n"
		".set			noreorder\n"
		"lwu			%0,  0(%1)\n"		// %0 = Tx_COUNT
		"lw				$8,  0x10(%1)\n"	// t0 = Tx_MODE
		"ld				$10, %2\n"			// t2 = g_Timer.ulHighCount
		"andi			$9,  $8, 0x0800\n"	// t1 = t0 & T_MODE_OVFF_M
		"beq			$9,  $0, 0f\n"		// t1==0�̂Ƃ�(�I�[�o�[�t���[���Ă��Ȃ��Ƃ�)
		"addiu			$9,  $0,  0x0002\n"	//t1 = 2
		"lwu			%0,  0(%1)\n"		// %0 = Tx_COUNT
		"addiu			$9,  $0,  0xFBFF\n"	// t1 = 0xFFFFFBFF = ~T_MODE_EQUF_M
		"daddiu			$10, $10, 1\n"		// t2 = t2 + 1
		"and			$8,  $8,  $9\n"		// t0 = t0 & t1 = t1 & ~T_MODE_EQUF_M
		"sd				$10, %2\n"			// g_Timer.ulHighCount = t2
		"sw				$8,  0x10(%1)\n"	// Tx_MODE = t0
		"addiu			$9,  $0,  0x0002\n"	// t1 = 2
	"0:\n"
		"andi			$8,  $8,  0x0003\n"	// t0 = t0 & 3 = (Tx_MODE & T_MODE_CLKS_M)
		"dsll			$10, $10, 16\n"		// t2 = t2 << 16
		"movz			$9,  $0,  $8\n"		// t1 = (t0==0) ? 0 : t1 = ((Tx_MODE & T_MODE_CLKS_M)==0) ? 0 : 2
		"or				%0,  %0,  $10\n"	// %0 = %0 | t2
		"dsllv			$9,  $9,  $8\n"		// t1 = t1 << t0
		"dsll			%0,  %0,  $9\n"		// %0 = %0 << t1
		".set			pop\n"
		: "=&r"(ulSystemTime)
		: "r"(TIMER_BASE), "m"(g_Timer.ulHighCount)
		: "$8", "$9", "$10", "memory"
	);
	return (ulSystemTime);
}


// ���񊄂荞�ݎ�����COMP���W�X�^�ɐݒ�ƁA��v�t���O�̃��Z�b�g
static void SetNextComp(u_long ulNowTime)
{
	u_long a0, a1;
	long lDiff;
	u_int mode;
	COUNTER *pCounter = g_Timer.pAlarm;

	if (g_Timer.cbcounterid>=0) {
		// �^�C�}���荞�ݎ��s���������ꍇ�ACOMP���W�X�^�̍Đݒ�̓L�����Z�����܂��B
		return;
	}
	mode = DGET_T_MODE();
	if (pCounter==NULL) {
		// �A���[�������݂��Ȃ��Ƃ��A�I�[�o�[�t���[���荞�݂Ɠ��^�C�~���O�ɂ��Ă���
		DPUT_T_COMP(0);
		DPUT_T_MODE(mode & (u_int)~T_MODE_OVFF_M);		// ��v�t���O�����Z�b�g����
		return;
	}

	// �L���[�̐擪�ƁA����200usec�̃G���g�����ߐڃG���g����T��
	a0 = pCounter->ulSchedule + pCounter->ulBaseTime - pCounter->ulBaseCount;
	pCounter = pCounter->pNext;
	while (pCounter) {
		a1 = pCounter->ulSchedule + pCounter->ulBaseTime - pCounter->ulBaseCount;
		if (a1 < (a0 + USEC2BUSCLK(200))) {
			a0 = a1;
		} else {
			break;
		}
		pCounter = pCounter->pNext;
	}
	lDiff = a0 - ulNowTime;
	if (lDiff < (long)USEC2BUSCLK(200)) {
		// ���łɗ\�莞�����I�[�o�[���Ă����Ƃ��Ƃ��A
		// ����������200usecs����w���Ă���
		DPUT_T_COMP(DGET_T_COUNT() + PRESCALE_R(USEC2BUSCLK(200), mode));
		DPUT_T_MODE(mode & (u_int)~T_MODE_OVFF_M);		// ��v�t���O�����Z�b�g����
	} else {
		//FIXME: �͂邩������������ADPUT_T_COMP(0)�����ق������������B
		DPUT_T_MODE(mode & (u_int)~T_MODE_OVFF_M);		// ��v�t���O�����Z�b�g����
		DPUT_T_COMP(PRESCALE_R(a0, mode));
	}
	return;
}


// �J�E���^���A���[�����X�g�ɒǉ�
static void InsertAlarm(COUNTER *pCounter)
{
	COUNTER *pp = NULL;
	COUNTER *pn = g_Timer.pAlarm;
	u_long ulAbsCompare = pCounter->ulSchedule + pCounter->ulBaseTime - pCounter->ulBaseCount;
	while (pn) {
		u_long ulTarget = pn->ulSchedule + pn->ulBaseTime - pn->ulBaseCount;
		if (ulAbsCompare < ulTarget) {
			// �ǉ����ׂ��ʒu����������
			break;
		}
		pp = pn;
		pn = pn->pNext;
	}

	pCounter->pPrev = pp;
	pCounter->pNext = pn;
	if (pn) {
		// �r���̃A���[���ɒǉ�
		pn->pPrev      = pCounter;
	}
	if (pp) {
		// �r���̃A���[���ɒǉ�
		pp->pNext      = pCounter;
	} else {
		// �擪�̃A���[���Ƃ��Ēǉ�
		g_Timer.pAlarm = pCounter;
	}
	return;
}


// �o�^����Ă����J�E���^���A�A���[�����X�g����O��
static COUNTER *UnlinkAlarm(COUNTER *pCounter)
{
	COUNTER *pNext = pCounter->pNext;
	if (pCounter->pPrev) {
		pCounter->pPrev->pNext = pNext;
	} else {
		g_Timer.pAlarm  = pNext;
	}
	if (pNext) {
		pNext->pPrev = pCounter->pPrev;
	}
	pCounter->pPrev     = NULL;
	return (pNext);
}


// �^�C�}���荞�݃n���h��
static int cbTimerHandler(int ca, void *arg, void *addr)
{
	u_long ulAbsTime;
	u_int mode = DGET_T_MODE();

	(void)ca;
	(void)arg;

	if (mode & T_MODE_EQUF_M) {
		// ��r�l��v���荞�݂������ꍇ
		COUNTER *pCounter = g_Timer.pAlarm;

		while (pCounter) {
			u_long ulTargetCompare, ulTargetTime;
			u_long ulNext;
			COUNTER *pNext;

			ulTargetCompare = pCounter->ulSchedule + pCounter->ulBaseTime - pCounter->ulBaseCount;

			// ���ݎ������擾, OVFF�t���O�����Z�b�g
			ulAbsTime = ApplyOverflow();				// ulAbsClock���X�V

			if (ulAbsTime < ulTargetCompare) {
				// �����A���[���𔭐�������ׂ��J�E���^���̂����Ă��Ȃ�
				break;
			}
			// ��������A���[�����X�g����폜����
			pNext = UnlinkAlarm(pCounter);

/*
			// �����ōēxulAbsTime���X�V�����ulTargetTime�������Ɍv�Z�ł��邪�A
			// ���ׂȂ����R�X�g���^�_�ł͂Ȃ��̂ŁA�����̓R�����g�A�E�g���Ă����B
			ulAbsTime = ReferHardCounter();				// ulAbsClock���X�V
*/
			// ���ݎ������J�E���^�̊�����ɒ���
			ulTargetTime = ulAbsTime + pCounter->ulBaseCount - pCounter->ulBaseTime;

			// �A���[���n���h���̌Ăяo�����s��
			g_Timer.cbcounterid = COUNTER2ID(pCounter);
			__asm__ volatile ( "lw   $28, %0\n" : : "m"(pCounter->gp_value) : "memory" );
			ulNext = pCounter->cbHandler(COUNTER2ID(pCounter), pCounter->ulSchedule, ulTargetTime, pCounter->arg, addr);
			if (ulNext==0) {
				// ���̃J�E���^�̃R�[���o�b�N�͏I��
				pCounter->uiMode &= ~MODE_HANDLER;		// �n���h�����폜
			} else if (ulNext==(u_long)-1) {
				// ���̃J�E���^���ƍ폜
				pCounter->uiKey  = 0;					// �J�E���^ID�𖳌���Ԃɂ���
				pCounter->uiMode = 0;					// �J�E���^���[�h�𖢎g�p��Ԃ�

				// �󂫃����N���X�g�̐擪�ɓo�^
				pCounter->pNext  = g_Timer.pFree;
				g_Timer.pFree    = pCounter;

				g_Timer.uiUsedCounters--;				// �A���P�[�g�ς݃J�E���^�����f�N�������g
			} else {
				// ulNext�����܂�ɂ�����������l�������ꍇ�ɂ́A�؂�グ���s���B
				if (ulNext < USEC2BUSCLK(100)) {
					ulNext = USEC2BUSCLK(100);
				}
				pCounter->ulSchedule += ulNext;			// ���̗\�莞����ݒ�
				InsertAlarm(pCounter);					// �A���[�����X�g�ɒǉ�
			}

			// ���̃A���[�����X�g�����𒲂ׂ�
			pCounter = pNext;
		}
	}
	g_Timer.cbcounterid = -1;							// �R�[���o�b�N�����Ăяo�����̃J�E���^ID�����Z�b�g

	ulAbsTime = ApplyOverflow();						// �J�E���^�I�[�o�[�t���[����
	SetNextComp(ulAbsTime);								// ���񊄂荞�ݎ�����ݒ�
	ApplyOverflow();									// �J�E���^�I�[�o�[�t���[����
	ExitHandler();										// �^�C�}���荞�݃n���h���̏I��
	return (0);
}



// �V�X�e�������Q��
u_long isceTimerGetSystemTime(void)
{
	return (ReferHardCounter());
}


// �V�X�e�������Q��
u_long sceTimerGetSystemTime(void)
{
	int intr;
	u_long ulSystemTime;
	intr = DI();							// ���荞�݋֎~
	ulSystemTime = ReferHardCounter();
	if (intr) EI();							// ���荞�݋���Ԃ̕��A
	return (ulSystemTime);
}


// �J�E���^���A���P�[�g
int isceTimerAllocCounter(void)
{
	COUNTER *pCounter;

	pCounter = g_Timer.pFree;
	if (pCounter==NULL) {
		// �J�E���^���A���P�[�g�ł��Ȃ������Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_ETIMER));
	}
	g_Timer.pFree   = pCounter->pNext;

	g_Timer.uiUsedCounters++;							// �A���P�[�g�ς݃J�E���^�����C���N�������g
	pCounter->uiMode      = 0;
	pCounter->cbHandler   = NULL;
	pCounter->ulBaseCount = 0;
	g_Timer.uniqkey++;
	pCounter->uiKey       = ((g_Timer.uniqkey<<1) & 0x3FE) | 1;
	return (COUNTER2ID(pCounter));
}


// �J�E���^���A���P�[�g
int sceTimerAllocCounter(void)
{
	int intr;
	int id;

	intr = DI();										// ���荞�݋֎~
	id = isceTimerAllocCounter();						// �J�E���^���A���P�[�g
	if (intr) EI();										// ���荞�݋���Ԃ̕��A
	return (id);
}


// �J�E���^���J��
int isceTimerFreeCounter(int id)
{
	COUNTER *pCounter = ID2COUNTER(id);					// �J�E���^ID���COUNTER*�\���̂̃A�h���X���擾
	if (!IS_VALIDID(id)) {
		// �������Ȃ��J�E���^ID���w�肳��Ă����Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	} else if (g_Timer.cbcounterid==id) {
		// �n���h�����s���J�E���^ID���w�肳��Ă����Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EBUSY));
	}

	// �A���[���n���h�����ݒ肳��Ă����Ƃ�
	if (pCounter->uiMode & MODE_HANDLER) {
		// �n���h�����폜
		UnlinkAlarm(pCounter);
	}
	pCounter->uiKey       = 0;							// �J�E���^ID�𖳌���Ԃɂ���
	pCounter->uiMode      = 0;							// �J�E���^���[�h�𖢎g�p��Ԃ�

	// �󂫃����N���X�g�̐擪�ɓo�^
	pCounter->pNext       = g_Timer.pFree;
	g_Timer.pFree         = pCounter;

	g_Timer.uiUsedCounters--;							// �A���P�[�g�ς݃J�E���^�����f�N�������g
	return (SCE_OK);
}


// �J�E���^���J��
int sceTimerFreeCounter(int id)
{
	int res;
	int intr;
	intr = DI();										// ���荞�݋֎~
	res = isceTimerFreeCounter(id);						// �J�E���^���J��
	if (intr) EI();										// ���荞�݋���Ԃ̕��A
	return (res);
}


// �g�p�ς�,���g�p�̃J�E���^����Ԃ�
int isceTimerGetUsedUnusedCounters(int *pnUsed, int *pnUnused)
{
	if (g_Timer.hid<0) {
		// �^�C�}���荞�݃n���h���͓o�^����Ă��Ȃ��Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINIT));
	}

	if (pnUsed!=NULL) {
		// �g�p���̃J�E���^��
		*pnUsed = g_Timer.uiUsedCounters;
	}
	if (pnUnused!=NULL) {
		// ���g�p�̃J�E���^��
		*pnUnused = SCE_TIMER_MAX_SOFTCOUNTER - g_Timer.uiUsedCounters;
	}
	return (SCE_OK);
}


// �g�p�ς�,���g�p�̃J�E���^����Ԃ�
int sceTimerGetUsedUnusedCounters(int *pnUsed, int *pnUnused)
{
	int res;
	int intr;
	intr = DI();										// ���荞�݋֎~
	res = isceTimerGetUsedUnusedCounters(pnUsed, pnUnused);	// �g�p�ς�,���g�p�̃J�E���^����Ԃ�
	if (intr) EI();										// ���荞�݋���Ԃ̕��A
	return (res);
}


// �J�E���^���N��
int isceTimerStartCounter(int id)
{
	u_long ulNowTime;
	COUNTER *pCounter = ID2COUNTER(id);					// �J�E���^ID���COUNTER*�\���̂̃A�h���X���擾

	if (!IS_VALIDID(id)) {
		// �������Ȃ��J�E���^ID���w�肳��Ă����Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	} else if (g_Timer.cbcounterid==id) {
		// �n���h�����s���J�E���^ID���w�肳��Ă����Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EBUSY));
	}
	if (pCounter->uiMode & MODE_START) {
		// ���łɋN����
		return (1);
	}

	ulNowTime = isceTimerGetSystemTime();				// ���ݎ����𓾂�
	pCounter->ulBaseTime = ulNowTime;					// ���ݎ������x�[�X�^�C���Ƃ��Đݒ�
	pCounter->uiMode |= MODE_START;						// �J�E���^�N��
	if (pCounter->uiMode & MODE_HANDLER) {
		// �n���h�����o�^����Ă���J�E���^�������ꍇ
		InsertAlarm(pCounter);							// ���̃J�E���^�����荞�ݗ\�胊�X�g�ɒǉ�
		SetNextComp(ulNowTime);							// ���񊄂荞�ݎ����̃X�P�W���[�����O
	}
	return (SCE_OK);
}


// �J�E���^���N��
int sceTimerStartCounter(int id)
{
	int res;
	int intr;
	intr = DI();										// ���荞�݋֎~
	res = isceTimerStartCounter(id);					// �J�E���^���N��
	if (intr) EI();										// ���荞�݋���Ԃ̕��A
	return (res);
}


// �J�E���^���~
int isceTimerStopCounter(int id)
{
	u_long ulNowTime;
	COUNTER *pCounter = ID2COUNTER(id);					// �J�E���^ID���COUNTER*�\���̂̃A�h���X���擾

	if (!IS_VALIDID(id)) {
		// �������Ȃ��J�E���^ID���w�肳��Ă����Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	} else if (g_Timer.cbcounterid==id) {
		// �n���h�����s���J�E���^ID���w�肳��Ă����Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EBUSY));
	}
	if ((pCounter->uiMode & MODE_START)==0) {
		// ���łɒ�~��
		return (0);
	}
	ulNowTime = isceTimerGetSystemTime();				// ���݂̃V�X�e���������擾
	pCounter->ulBaseCount += (ulNowTime - pCounter->ulBaseTime);
	pCounter->uiMode &= ~MODE_START;					// �J�E���^��~
	if (pCounter->uiMode & MODE_HANDLER) {
		UnlinkAlarm(pCounter);							// �A���[�����X�g������폜
		SetNextComp(ulNowTime);							// ���񊄂荞�ݎ�����ݒ�
	}
	return (1);
}


// �J�E���^���~
int sceTimerStopCounter(int id)
{
	int res;
	int intr;
	intr = DI();										// ���荞�݋֎~
	res = isceTimerStopCounter(id);						// �J�E���^���~
	if (intr) EI();										// ���荞�݋���Ԃ̕��A
	return (res);
}


// �J�E���^�ɃJ�E���g�l�ݒ�
u_long sceTimerSetCount(int id, u_long ulNewCount)
{
	int intr;
	u_long ulOldCount;
	COUNTER *pCounter = ID2COUNTER(id);					// �J�E���^ID���COUNTER*�\���̂̃A�h���X���擾

	intr = DI();										// ���荞�݋֎~
	if (!IS_VALIDID(id) || g_Timer.cbcounterid==id) {
		// �������Ȃ��J�E���^ID���w�肳��Ă����Ƃ�
		if (intr) EI();									// ���荞�݋���Ԃ̕��A
		return ((u_long)-1);
	}

	ulOldCount  = pCounter->ulBaseCount;				// ���݂̃J�E���g�l�𓾂�
	if (pCounter->uiMode & MODE_START) {
		// �J�E���^���쒆�������Ƃ��A�x�[�X�������ƐV�����J�E���^�l�ɕ␳
		u_long ulNowTime = isceTimerGetSystemTime();	// ���ݎ����𓾂�
		ulOldCount += (ulNowTime - pCounter->ulBaseTime);	// �x�[�X�^�C������̌o�ߎ��Ԃ����Z
		pCounter->ulBaseTime  = ulNowTime;				// �V�����x�[�X������ݒ�
		pCounter->ulBaseCount = ulNewCount;				// �V�����J�E���g�l��ݒ�
	} else {
		pCounter->ulBaseCount = ulNewCount;				// �V�����J�E���g�l��ݒ�
	}
	if (intr) EI();										// ���荞�݋���Ԃ̕��A
	return (ulOldCount);
}


// �J�E���^�̃x�[�X�������擾
u_long isceTimerGetBaseTime(int id)
{
	u_long ulBaseTime;
	COUNTER *pCounter = ID2COUNTER(id);					// �J�E���^ID���COUNTER*�\���̂̃A�h���X���擾
	if (!IS_VALIDID(id)) {
		// �������Ȃ��J�E���^ID���w�肳��Ă����Ƃ�
		return ((u_long)-1);
	}
	if (pCounter->uiMode & MODE_START) {
		// �J�E���^���쒆�������Ƃ�
		ulBaseTime = pCounter->ulBaseTime - pCounter->ulBaseCount;
	} else {
		// �J�E���^����~���������Ƃ�
		ulBaseTime = 0;
	}
	return (ulBaseTime);
}


// �J�E���^�̃x�[�X�������擾
u_long sceTimerGetBaseTime(int id)
{
	int intr;
	u_long ulBaseTime;
	intr = DI();										// ���荞�݋֎~
	ulBaseTime = isceTimerGetBaseTime(id);				// �J�E���^�̃x�[�X�^�C���l�𓾂�
	if (intr) EI();										// ���荞�݋���Ԃ̕��A
	return (ulBaseTime);
}


// �J�E���^�̌��ݒl���擾
u_long isceTimerGetCount(int id)
{
	u_long ulCount;
	COUNTER *pCounter = ID2COUNTER(id);					// �J�E���^ID���COUNTER*�\���̂̃A�h���X���擾

	if (!IS_VALIDID(id)) {
		// �������Ȃ��J�E���^ID���w�肳��Ă����Ƃ�
		return ((u_long)-1);
	}
	ulCount = pCounter->ulBaseCount;					// ���݂̃J�E���g�l�𓾂�
	if (pCounter->uiMode & MODE_START) {
		// �J�E���^���쒆�������Ƃ�
		u_long ulNowTime = isceTimerGetSystemTime();	// ���ݎ����𓾂�
		ulCount += (ulNowTime - pCounter->ulBaseTime);	// �x�[�X��������̌o�ߎ��Ԃ����Z
	}
	return (ulCount);
}


// �J�E���^�̌��ݒl���擾
u_long sceTimerGetCount(int id)
{
	int intr;
	u_long ulCount;
	intr = DI();										// ���荞�݋֎~
	ulCount = isceTimerGetCount(id);					// ���݂̃J�E���g�l�𓾂�
	if (intr) EI();										// ���荞�݋���Ԃ̕��A
	return (ulCount);
}


// �J�E���^�Ƀn���h����ݒ�
int isceTimerSetHandler(int id, u_long ulSchedule, u_long (*cbHandler)(int, u_long, u_long, void *, void *), void *arg)
{
	u_long ulNowTime;
	COUNTER *pCounter = ID2COUNTER(id);					// �J�E���^ID���COUNTER*�\���̂̃A�h���X���擾

	if (!IS_VALIDID(id)) {
		// �������Ȃ��J�E���^ID���w�肳��Ă����Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	} else if (g_Timer.cbcounterid==id) {
		// �n���h�����s���J�E���^ID���w�肳��Ă����Ƃ�
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EBUSY));
	}

	if (pCounter->uiMode & MODE_HANDLER) {
		// ���łɓo�^����Ă�����A��������A���[�����X�g����폜����
		UnlinkAlarm(pCounter);
	}
	pCounter->cbHandler = cbHandler;					// �n���h���֐���ݒ�
	if (cbHandler==NULL) {
		pCounter->uiMode   &= ~MODE_HANDLER;			// �n���h���o�^�ς݃t���O���~�낷
	} else {
		// �n���h���֐��|�C���^����NULL�������Ƃ�
		pCounter->uiMode   |= MODE_HANDLER;				// �n���h���o�^�ς݃t���O�𗧂Ă�
		pCounter->ulSchedule = ulSchedule;
		__asm__ volatile ( "sw   $28, %0\n" : : "m"(pCounter->gp_value) : "memory" );
		pCounter->arg       = arg;
		if (pCounter->uiMode & MODE_START) {
			// ���쒆�̃J�E���^�Ƀn���h����ݒ肵���Ƃ��A�A���[�����X�g�ɒǉ�
			InsertAlarm(pCounter);
		}
	}
	ulNowTime = isceTimerGetSystemTime();				// ���ݎ����𓾂�
	SetNextComp(ulNowTime);								// ���񊄂荞�ݎ�����ݒ�
	return (SCE_OK);
}


// �J�E���^�Ƀn���h����ݒ�
int sceTimerSetHandler(int id, u_long ulSchedule, u_long (*cbHandler)(int, u_long, u_long, void *, void *), void *arg)
{
	int res;
	int intr;
	intr = DI();										// ���荞�݋֎~
	res = isceTimerSetHandler(id, ulSchedule, cbHandler, arg);
														// �J�E���^�Ƀn���h����ݒ�
	if (intr) EI();										// ���荞�݋���Ԃ̕��A
	return (res);
}


// BUSCLOCK�l(147.456MHz)�������Ԃɕϊ�
void sceTimerBusClock2USec(u_long ulClock, u_int *puiSec, u_int *puiUsec)
{
	u_int uiSec = ulClock / BUSCLOCK;
	if (puiSec) {
		*puiSec = uiSec;
	}
	if (puiUsec) {
		*puiUsec = (ulClock - uiSec*BUSCLOCK) * 1000 * 1000 / BUSCLOCK;
	}
	return;
}


// �����Ԃ�BUSCLOCK�l(147.456MHz)�ɕϊ�
u_long sceTimerUSec2BusClock(u_int uiSec, u_int uiUsec)
{
	u_long ulClock = (uiSec * BUSCLOCK) + (uiUsec * (u_long)BUSCLOCK / (1000 * 1000));
	return (ulClock);
}


// BUSCLOCK�l(147.456MHz)�����g���ɕϊ�
float sceTimerBusClock2Freq(u_long ulClock)
{
	float fFreq = (float)BUSCLOCK / (float)ulClock;
	return (fFreq);
}


// ���g����BUSCLOCK�l(147.456MHz)�ɕϊ�
u_long sceTimerFreq2BusClock(float fFreq)
{
	u_long ulClock = (float)BUSCLOCK / fFreq;
	return (ulClock);
}

