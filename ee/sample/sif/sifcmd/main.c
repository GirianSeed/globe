/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - <file> -
 *
 *                         Version <0.10>
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            <main.c>
 *                     <sif cmd test program>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10            Mar,26,1999     hakamatani  first version
 *      0.90            Aug,23,1999     hakamatani  first version
 */

/* ================================================================
 *
 *      Program
 *
 * ================================================================ */

#include <eekernel.h>
#include <stdio.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>

#define _SIF_DI  DI()
#define _SIF_EI  EI()
sceSifCmdHdr	sch __attribute__((aligned(64)));
sceSifCmdSRData srd __attribute__((aligned(64)));

/* SIF CMD関数および変数の宣言 */
static void  test_f0(void *,void *);
static volatile int   test0;
static void  test_f1(void *,void *);
static volatile int   test1;

/* SIFCMD関数登録バッファ */
#define BSIZE 0x10
static sceSifCmdData cmdbuffer[BSIZE];

int main()
{
  unsigned int id;
  
  /* init */
  /* 本来なら InitCmd だが、IOP 側が動かないために InitRpc */
  sceSifInitRpc(0);

  /* set local buffer & functions */
  /* 登録の間割り込み禁止*/
  _SIF_DI;
  
  /* SIFCMD 用の関数バッファの設定*/
  sceSifSetCmdBuffer( &cmdbuffer[0], BSIZE);

  /* 関数バッファの0/1番へ,関数の登録 */
  sceSifAddCmdHandler( 0, (void *) test_f0, (void *) &test0);
  sceSifAddCmdHandler( 1, (void *) test_f1, (void *) &test1);
  _SIF_EI;

  /* 変数の内容を０へ */
  test0 = 0;
  test1 = 0;

  /* sync other cpu */
  /* 標準登録のSIFCMD関数で,相手側のCPUと待ち合わせ */
  srd.rno = 31;
  srd.value = 1;
  sceSifSendCmd(SIF_CMDC_SET_SREG, &srd, sizeof(srd), 0 , 0, 0);
  while(sceSifGetSreg(31) == 0);

  /* call test_f0 */
  /* 相手側の０番目の登録関数を呼ぶ。*/
  sch.opt = (unsigned int) 10;
  id = sceSifSendCmd(0, &sch, sizeof(sch), 0 , 0, 0);
  while(sceSifDmaStat(id) >= 0);
  
  /* call test_f1 */
  /* 相手側の１番目の登録関数を呼ぶ */
  sch.opt = (unsigned int) 20;
  id = sceSifSendCmd(1, &sch, sizeof(sch), 0 , 0, 0);
  while(sceSifDmaStat(id) >= 0);

  /* loop */
  /* 相手側から登録関数がよばれて、変数の内容が変わるまで待つ */
  while((test0 == 0) || (test1 == 0));
  
  /* 変更された変数の内容を表示 */
  printf("test0 = %d test1 = %d\n", test0, test1);

  while(1);

}

/* ０番目の登録関数 */
/* 関数登録時のデータアドレを,int のアドレスとして,その値を */
/* コマンドパケットのopt フィールドに設定*/
void test_f0(void *p,void *q)
{
  sceSifCmdHdr *h = (sceSifCmdHdr *) p;

  *(int *)q = (int)h->opt;
  
  return;
}

/* １番目の登録関数 */
/* 関数登録時のデータアドレを,int のアドレスとして,その値を */
/* コマンドパケットのopt フィールドに設定*/
void test_f1(void *p,void *q)
{
  sceSifCmdHdr *h = (sceSifCmdHdr *) p;

  *(int *)q = (int)h->opt;
  
  return;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */
