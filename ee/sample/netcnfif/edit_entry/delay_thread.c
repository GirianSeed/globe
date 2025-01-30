/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         delay_thread.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.03.13      tetsu       First version
 */

#include <eekernel.h>

static volatile int alarm_id; /* �A���[�� ID */
static volatile int sema_id;  /* �Z�}�t�H ID */

/* �Z�}�t�H������ԋp����R�[���o�b�N�֐� */
static void cbfunc(int id, unsigned short time, void *arg)
{
    if(id == alarm_id) iSignalSema(sema_id);
}

/* �X���b�h���w�莞�� delay ������ */
void delay_thread(unsigned short time)
{
    struct SemaParam sema;

    /* �A���[����ݒ肷�� */
    alarm_id = SetAlarm(time, cbfunc, NULL);

    /* �Z�}�t�H���쐬 */
    sema.initCount = 0;
    sema.maxCount  = 1;
    sema.option    = 0;
    sema_id = CreateSema(&sema);

    /* �Z�}�t�H�����l�����A�w�莞�ԑ҂� */
    WaitSema(sema_id);

    /* �Z�}�t�H�̍폜 */
    DeleteSema(sema_id);
}
