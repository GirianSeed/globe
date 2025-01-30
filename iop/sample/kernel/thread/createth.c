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
 *                            createth.c
 * 
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.4.0   
 */

#include <stdio.h>
#include <kernel.h>

#define BASE_priority  32

void firstthread(int arg);
void thread(int arg);

/* IOP �̃X���b�h�x�[�X�̃v���O������ crt0 ��K�v�Ƃ��܂���B
 * main�֐��ł͂Ȃ� start�֐���p�ӂ��܂��B
 */

int start(int argc, char *argv[])
{
    struct ThreadParam param;
    int	thid;

    printf("\nCrate thread test ! \n");
    /* �X���b�h�Ƃ��ă������ɏ풓����v���O�����ł�
     * start �֐��͏o���邾�����₩�ɖ߂邱�Ƃ����҂���Ă���̂�
     * ���Ԃ̂�����Ȃ������������ς܂������Ƃ́A�����̂�����
     * �X���b�h�ɐ����n���ďI�����܂��B
     */
    param.attr         = TH_C;
    param.entry        = firstthread;
    param.initPriority = BASE_priority;
    param.stackSize    = 0x800;
    param.option       = 0;
    thid = CreateThread(&param);
    if( thid > 0 ) {
	StartThread(thid,0);
	return 0;	/* 0: �v���O�������������ɏ풓�������܂܏I�� */
    } else {
	return 1;	/* 1: �v���O����������������������ďI�� */
    }
}

void firstthread(int arg)
{
    struct ThreadParam param;
    int	thid[6], i;

    param.attr         = TH_C;
    param.entry        = thread;
    param.initPriority = BASE_priority+1;
    param.stackSize    = 0x800;
    param.option = 0;
    /* �����֐������s����X���b�h�� 6�쐬 */
    for( i = 0; i < 6 ; i ++ ) {
	thid[i] = CreateThread(&param);	
    }
    /* �X���b�h���N�� */
    for( i = 0; i < 6 ; i ++ ) {
	StartThread(thid[i],i);
    }
    /* �����̃v���C�I���e�B��������,���̃X���b�h�Ɏ��s����n���B*/
    ChangeThreadPriority( TH_SELF, BASE_priority+2 );
    printf(" all thread end #1\n");

    ChangeThreadPriority( TH_SELF, BASE_priority );
    /* �X���b�h���ēx�N�� */
    for( i = 0; i < 6 ; i += 2 ) {
	StartThread(thid[i],i);
    }
    for( i = 1; i < 6 ; i += 2 ) {
	StartThread(thid[i],i);
    }

    ChangeThreadPriority( TH_SELF, BASE_priority+2 );
    printf(" all thread end #2\n");

    /* �X���b�h���폜 */
    for( i = 0; i < 6 ; i ++ ) {
	DeleteThread(thid[i]);
    }
    printf("fin\n");
    /* ���܂́A�܂��I�������X���b�h�v���O���������������珜������@�\��
     *  ������
     */
}

void thread(int arg)
{
    int i;
    printf("thread no. %d start\n",arg);
    for( i = 0; i < 3 ; i ++ ) {
	printf("\t thread no. %d   %d time Rotate Ready Queue\n",arg, i);
	RotateThreadReadyQueue(TPRI_RUN);
    }
    printf("thread no. %d end\n",arg);
    /* �X���b�h�̃g�b�v�̊֐����甲���邱�Ƃ� ExitThread()���ĂԂ��Ƃ͓��� */
}
