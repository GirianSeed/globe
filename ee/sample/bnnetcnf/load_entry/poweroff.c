/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: poweroff.c,v 1.2 2002/10/20 13:51:31 tetsu Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * bnnetcnf/load_entry - poweroff.c
 *	"PlayStation BB Navigator" Network Configuration Read Library Sample Program
 *	"PlayStation BB Unit" poweroff program
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-12      tetsu       first version
 *      2002-10-20      tetsu       correspond to code-review
 */

#include <eekernel.h>
#include <sifdev.h>
#include <libcdvd.h>

#define STACK_SIZE (512 * 16)

static u_char stack[STACK_SIZE] __attribute__ ((aligned(16)));
static int    tid;
static int    sid;

/* �v���g�^�C�v�錾 */
static void poweroff_thread(void *arg);
static void poweroff_handler(void *arg);

/*
 * �p���[�I�t�����s����X���b�h
 */
static void poweroff_thread(void *arg)
{
    static int stat = 0;
    int        sema_id = (int)arg;

    /* �Z�}�t�H�����l�����AiSignalSema() �����܂ő҂� */
    WaitSema(sema_id);

    /* pfs ���}�E���g����Ă��鎞�A�t�@�C���N���[�Y���� */
    sceDevctl("pfs:", PDIOC_CLOSEALL, NULL, 0, NULL, 0);

    /* Dev9 being powered off directry */
    while (sceDevctl("dev9x:", DDIOC_OFF, NULL, 0, NULL, 0) < 0);

    /* Power off */
    while (!sceCdPowerOff(&stat) || stat);

    while (1);
}

/*
 * �Z�}�t�H������ԋp����R�[���o�b�N�֐�
 */
static void poweroff_handler(void *arg)
{
    int sema_id = (int)arg;

    iSignalSema(sema_id);
}

/*
 * DTL-H30000( BAY ���f��)���g�p���āA(�I��/�X�^���o�C/) RESET �{�^����
 * �y�������ꂽ�ۂɁA�d�� OFF ����������邽�߂̏���
 */
void prepare_poweroff(void)
{
    struct ThreadParam tparam;
    struct SemaParam   sparam;

    sparam.initCount = 0;
    sparam.maxCount  = 1;
    sparam.option    = 0;
    sid = CreateSema(&sparam);

    ChangeThreadPriority(GetThreadId(), 2);

    tparam.stackSize    = STACK_SIZE;
    tparam.gpReg        = &_gp;
    tparam.entry        = poweroff_thread;
    tparam.stack        = (void *)stack;
    tparam.initPriority = 1;
    tid = CreateThread(&tparam);
    StartThread(tid, (void *)sid);

    sceCdPOffCallback(poweroff_handler, (void *)sid);
}

/*
 * �d�� OFF �������폜����
 */
void destory_prepare_poweroff(void)
{
    /* �d�� OFF �R�[���o�b�N�֐����N���A���� */
    sceCdPOffCallback(NULL, NULL);

    /* poweroff_thread ���폜���� */
    TerminateThread(tid);
    DeleteThread(tid);

    /* �Z�}�t�H���폜���� */
    DeleteSema(sid);
}
