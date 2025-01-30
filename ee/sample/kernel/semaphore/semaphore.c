/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Emotion Engine Library Sample Program
 *
 *                           - <semaphore> -
 *
 *                          Version <1.00>
 *                               SJIS
 *
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                       <libconf - semaphore.c>
 *                    <the way of using semaphore>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Jul,31,2000     kumagae     first version
 *      1.01            Oct,16,2000     kumagae     change sema.initCount from sema.currentCount
 *      1.02            Nov,13,2000     kumagae     Add RemoveIntcHandler
 *      1.03            Dec,29,2000     kumagae     change ExitDeleteThread from ExitThread
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

//繰り返し回数
#define COUNT 4

//スレッド生成のためのグローバル変数
int main_thread_id;
int print_thread1_id;
int print_thread2_id;
int print_thread3_id;
char print_thread1_stack[STACK_SIZE] __attribute__((aligned(16)));
char print_thread2_stack[STACK_SIZE] __attribute__((aligned(16)));
char print_thread3_stack[STACK_SIZE] __attribute__((aligned(16)));
//セマフォのためのグローバル変数
int semamain_id;
int sema1_id;
int sema2_id;
int sema3_id;


//プロトタイプ宣言
int CreateStartThread( void (*entry)( void* ), void *stack, int stacksize, int prio,  void *arg );
void PrintThread(int sema_id);
void PrintThread1(void *p);
void PrintThread2(void *p);
void PrintThread3(void *p);
int VBlankStartHandler(int ch);

int main(void){
  struct SemaParam sema;
  int VBlankStartHandler_id;

  sceSifInitRpc(0);

  sema.initCount = 0;
  sema.maxCount = 1;
  semamain_id = CreateSema(&sema);
  sema1_id = CreateSema(&sema);
  sema2_id = CreateSema(&sema);
  sema3_id = CreateSema(&sema);

  main_thread_id = GetThreadId();

  //優先度を落す。
  ChangeThreadPriority(main_thread_id, MAIN_THREAD_PRIO);
  print_thread1_id = CreateStartThread(PrintThread1, print_thread1_stack, sizeof(print_thread1_stack), PRINT_THREAD_PRIO, NULL);
  //mainはREADY状態になり,PrintThread1はRUN状態に移る。
  //その後、WaitSemaでWAIT状態になり,mainがRUN状態に移る。
  print_thread2_id = CreateStartThread(PrintThread2, print_thread2_stack, sizeof(print_thread2_stack), PRINT_THREAD_PRIO, NULL);
  //mainはREADY状態になり,PrintThread2はRUN状態に移る。
  //その後、WaitSemaでWAIT状態になり,mainがRUN状態に移る。
  print_thread3_id = CreateStartThread(PrintThread3, print_thread3_stack, sizeof(print_thread3_stack), PRINT_THREAD_PRIO, NULL);
  //mainはREADY状態になり,PrintThread3はRUN状態に移る。
  //その後、WaitSemaでWAIT状態になり、mainがRUN状態に移る。

  
  VBlankStartHandler_id = AddIntcHandler(INTC_VBLANK_S, VBlankStartHandler, 0);
  
  //最初のVSyncStart割り込みの前にWaitSema(semamain_id)を
  //実行できるようにsceGsSyncV(0)を置く。
  sceGsSyncV(0);
  EnableIntc(INTC_VBLANK_S);
  WaitSema(semamain_id);
  //mainがWAIT状態の時にVSyncStart割り込みが発生する。
 

  //PrintThread1,2,3が終了した後に制御が戻ってくる。
  DisableIntc(INTC_VBLANK_S);

  DeleteSema(semamain_id);
  DeleteSema(sema1_id);
  DeleteSema(sema2_id);
  DeleteSema(sema3_id);
  
  RemoveIntcHandler(INTC_VBLANK_S, VBlankStartHandler_id);
  
  sceSifExitCmd();
  return 0;
}

/* VBlankStartHandler
 * 割り込みハンドラから呼ばれる関数
 *
 */
int VBlankStartHandler(int ch)
{
  scePrintf("--VBlankStartHander--\n");
  iSignalSema(sema1_id);
  iSignalSema(sema2_id);
  iSignalSema(sema3_id);
  ExitHandler();
  //sema1,sema2,sema3セマフォを解放して,READY状態になった順番から
  //PrintThread1がRUN状態になって,
  //PrintThread2,3がREADY状態になる。
  return 0;
}

/* CreateStartThread
 * スレッドを作成し,起動します。
 * 
 * void (*entry)(void*) 実行を開始する関数のアドレス
 * void *stack スレッドが使用するスタック領域
 * int stacksize スタックサイズ
 * int prio 優先度
 * void *arg 引数
 *
 * 戻り値 スレッドID
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
 * デバッグコンソールに文字列を出力する関数
 * 
 *
 */
void PrintThread(int sema_id){
  int  id = GetThreadId();
  int cnt = COUNT;
  while(cnt--){
    //WAIT状態になるたびに制御が別のスレッドに移行する。
    WaitSema(sema_id);
    printf("Print Thread Running ID=%d\n",id);
  }
}

void PrintThread1(void *p){
  PrintThread(sema1_id);
  ExitDeleteThread();
}

void PrintThread2(void *p){
  PrintThread(sema2_id);
  ExitDeleteThread();
}

void PrintThread3(void *p){
  PrintThread(sema3_id);
  SignalSema(semamain_id);
  ExitDeleteThread();
}
