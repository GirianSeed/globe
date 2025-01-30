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

/* IOP のスレッドベースのプログラムは crt0 を必要としません。
 * main関数ではなく start関数を用意します。
 */

int start(int argc, char *argv[])
{
    struct ThreadParam param;
    int	thid;

    printf("\nCrate thread test ! \n");
    /* スレッドとしてメモリに常駐するプログラムでは
     * start 関数は出来るだけ速やかに戻ることが期待されているので
     * 時間のかからない初期化等を済ませたあとは、自分のつくった
     * スレッドに制御を渡して終了します。
     */
    param.attr         = TH_C;
    param.entry        = firstthread;
    param.initPriority = BASE_priority;
    param.stackSize    = 0x800;
    param.option       = 0;
    thid = CreateThread(&param);
    if( thid > 0 ) {
	StartThread(thid,0);
	return 0;	/* 0: プログラムをメモリに常駐させたまま終了 */
    } else {
	return 1;	/* 1: プログラムをメモリから消去して終了 */
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
    /* 同じ関数を実行するスレッドを 6個作成 */
    for( i = 0; i < 6 ; i ++ ) {
	thid[i] = CreateThread(&param);	
    }
    /* スレッドを起動 */
    for( i = 0; i < 6 ; i ++ ) {
	StartThread(thid[i],i);
    }
    /* 自分のプライオリティを下げて,他のスレッドに実行権を渡す。*/
    ChangeThreadPriority( TH_SELF, BASE_priority+2 );
    printf(" all thread end #1\n");

    ChangeThreadPriority( TH_SELF, BASE_priority );
    /* スレッドを再度起動 */
    for( i = 0; i < 6 ; i += 2 ) {
	StartThread(thid[i],i);
    }
    for( i = 1; i < 6 ; i += 2 ) {
	StartThread(thid[i],i);
    }

    ChangeThreadPriority( TH_SELF, BASE_priority+2 );
    printf(" all thread end #2\n");

    /* スレッドを削除 */
    for( i = 0; i < 6 ; i ++ ) {
	DeleteThread(thid[i]);
    }
    printf("fin\n");
    /* いまは、まだ終了したスレッドプログラムをメモリから除去する機能は
     *  未実装
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
    /* スレッドのトップの関数から抜けることと ExitThread()を呼ぶことは同等 */
}
