/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Emotion Engine Library Sample Program
 *
 *                           - <rotate> -
 *
 *                          Version <1.00>
 *                               SJIS
 *
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                            <rotate.c>
 *                 <rotation of thread ready queue>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Jul,31,2000     kumagae     first version
 */

#include <stdio.h>
#include <stdlib.h>
#include <libgraph.h>
#include <sifdev.h>
#include <assert.h>
#include <sifrpc.h>
#include <sifcmd.h>

#define STACK_SIZE 4096

#define MAIN_THREAD_PRIO 10
#define PRINT_THREAD_PRIO 1

//�J��Ԃ���
#define COUNT 4

//�X���b�h�����̂��߂̃O���[�o���ϐ�
int main_thread_id;
int print_thread1_id;
int print_thread2_id;
int print_thread3_id;
char print_thread1_stack[STACK_SIZE] __attribute__((aligned(16)));
char print_thread2_stack[STACK_SIZE] __attribute__((aligned(16)));
char print_thread3_stack[STACK_SIZE] __attribute__((aligned(16)));

//�v���g�^�C�v�錾
int CreateStartThread( void (*entry)( void* ), void *stack, int stacksize, int prio,  void *arg );
void PrintThread(void *p);

int main(void){

  sceSifInitRpc(0);

  main_thread_id = GetThreadId();

  print_thread1_id = CreateStartThread(PrintThread, print_thread1_stack, sizeof(print_thread1_stack), PRINT_THREAD_PRIO, NULL);
  //PrintThread1��READY��Ԃֈڍs
  print_thread2_id = CreateStartThread(PrintThread, print_thread2_stack, sizeof(print_thread2_stack), PRINT_THREAD_PRIO, NULL);
  //PrintThread2��READY��Ԃֈڍs
  print_thread3_id = CreateStartThread(PrintThread, print_thread3_stack, sizeof(print_thread3_stack), PRINT_THREAD_PRIO, NULL);
  //PrintThread3��READY��Ԃֈڍs


  //main�ɍŌ�ɐ����߂����߂ɗD��x�𗎂��B
  //���̒����PrintThread1��RUN��ԂɂȂ�Bmain��READY��ԂɂȂ�B
  ChangeThreadPriority(main_thread_id, MAIN_THREAD_PRIO);


  //PrintThread1,2,3���I���������ƁA���䂪�߂��Ă���B
  sceSifExitCmd();
  return 0;
}

/* CreateStartThread
 * �X���b�h���쐬��,�N�����܂��B
 * 
 * void (*entry)(void*) ���s���J�n����֐��̃A�h���X
 * void *stack �X���b�h���g�p����X�^�b�N�̈�
 * int stacksize �X�^�b�N�T�C�Y
 * int prio �D��x
 * void *arg ����
 *
 * �߂�l �X���b�hID
 */
int CreateStartThread( void (*entry)( void* ), void *stack, int stacksize, int prio,  void *arg )
{
  struct ThreadParam tp;
  int id;
  tp.entry = entry;
  tp.stack = stack;
  tp.stackSize = stacksize;
  tp.initPriority = prio;
  tp.gpReg = &_gp;
  id = CreateThread(&tp);
  StartThread(id,arg);
  return id;
}

/* PrintThread
 * �f�o�b�O�R���\�[���ɕ�������o�͂���֐�
 * 
 *
 */
void PrintThread(void *p){
  int id = GetThreadId();
  int cnt = COUNT;
  while(cnt--){
    printf("Print Thread Running ID=%d\n",id);
    //PRINT_THREAD_PRIO�̗D��x�̃X���b�h���f�B�L���[����]������B
    //PrintThread1 -> PrintThread2 -> PrintThread3 -> PrintThread1 ...
    RotateThreadReadyQueue(PRINT_THREAD_PRIO);
  }
  ExitDeleteThread();
}
