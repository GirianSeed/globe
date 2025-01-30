/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 */
/* 
 *                      Emotion Engine Library
 *                          Version 0.12
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       libdma - libdma.c
 *                      main portion of libdma
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.09            October.07.1998 suzu        initial
 *      0.10            March.25.1999   suzu        use inline function
 *      0.11            May,17,1999     suzu        delete warnings
 *      0.12            January,23,2001 akiyuki     delete warnings
 *      0.13            Feb,6,2001      kumagae     add static declarartion
 */

#include <eekernel.h>
#include <eeregs.h>
#include "dmac.h"
#include <libdma.h>


#define setADR(v,x)	{if ((v)!=(void *)-1) (v)=(sceDmaTag *)(x);}
#define setQWC(v,x)	(v)=(x)
#define setCHCR(v,m,d)	(v).MOD=(m),(v).DIR=(d),(v).STR=1
#define settag(t,i,a,q)	(t)->id=(i),(t)->next=(sceDmaTag *)(a),(t)->qwc=(q)


#define MAXWAIT		0x1000000

/*
  DMAのチャネルごとにスレッドで呼び出して問題ないようにlibdmaを実装する。
 */


/*
  WaitDma  一定時間DMA転送が終了するまで待つ

引数
  d 対象のDMAチャネル

返り値
  なし
 */ 
static void WaitDma(sceDmaChan *d){
  int status;
  int chcr;
  int timeout = MAXWAIT;

  while((*(volatile u_int*) &((d)->chcr)) & D_CHCR_STR_M){
    if(--timeout < 0){
      scePrintf("libdma: sync timeout\n");
      sceDmaPause(d);
    }
  }
}

/*
  WatchDma 一定時間特定アドレスのDMA転送待ち

引数
  d 対象のDMAチャネル 
  addr 検査対象のメモリアドレス 

返り値
  なし
 */
static void WatchDma(sceDmaChan *d, void *addr, int timeout){
  if(timeout==0)
    timeout = MAXWAIT;
  while (*(volatile u_int *)&(d->madr) < (u_int)addr) {
    if(--timeout < 0){
      scePrintf("libdma: sync timeout\n");
      sceDmaPause(d);
    }
  }
}


/*
  SPR上のアドレスの場合DMAに設定するアドレスは
  31bit目を1にする必要があるので変換する。
 */
#define SPR_ADDR 0x70000000
static void * CheckAddress(void * paddr){
  u_int addr = (u_int)paddr;
  if((addr >> 28 ) == 7 ){
    addr &= 0x0fffffff;
    addr |= D_MADR_SPR_M;
  }
  return (void*)addr;
}

//
// channel registers
//
DCH *dch[] = {
	(DCH *)(D0_CHCR),	// VIF0
	(DCH *)(D1_CHCR),	// VIF1
	(DCH *)(D2_CHCR),	// GIF	
	(DCH *)(D3_CHCR),	// fromIPU
	(DCH *)(D4_CHCR),	// toIPU
	(DCH *)(D5_CHCR),	// SIF0
	(DCH *)(D6_CHCR),	// SIF1
	(DCH *)(D7_CHCR),	// SIF2
	(DCH *)(D8_CHCR),	// fromSPR
	(DCH *)(D9_CHCR)	// toSPR
};

//
// state
//
static sceDmaEnv	sceDmaCurrentEnv;	// current denv

// clear: this function should be faster
static void memclr(u_char *ptr, int size)
{
	while (size--)
		*ptr++ = 0;
}


/* sceDmaGetChan チャネル構造体の取得 

引数
  id チャネル番号 
返り値
  0:idが不正
  チャネル番号idに相当するsceDmaChan構造体のアドレス
 */
sceDmaChan *sceDmaGetChan(int id)
{
	return ((id>=0 && id<=9) ? (sceDmaChan *)dch[id]: 0);
}



static const int isclr[10] = {1,1,1,1,1,0,0,0,1,1};
/* 
  sceDmaReset DMACのリセット

引数
  mode リセットモード
   1:enable
   0:disable
返り値
   直前のmodeの値
 */
int sceDmaReset(int mode)
{
	int		ret = getDMAE(*D_CTRL);
	int		i;
	sceDmaEnv	env;

	// clear channel registers	
	for (i = 0; i < 10; i++) {
		if (isclr[i]) {
			dch[i]->chcr = 0;
			dch[i]->madr = dch[i]->tadr = 0;
			dch[i]->asr0 = dch[i]->asr1 = 0;
			dch[i]->sadr = 0;
		}
	}

	// clear interrupt
	*D_STAT  = 0x0000ff1f;
	*D_STAT  = (*D_STAT & 0xff1f0000);

	// clear common registers
	memclr((u_char *)&env, sizeof(sceDmaEnv));
	sceDmaPutEnv(&env);

	// set enable
	if (mode == 1)
		setDMAE(*D_CTRL);	

	return(ret);
}


/*
  sceDmaPutEnv チャネル共通レジスタの設定

引数
  env レジスタに書き込む値
返り値
  0:正常終了
  負:設定値が不正
 */
//                         0 1 2 3 4 5 6 7 8 9    			
static const u_char ststbl[] = { 0,0,0,3,0,1,0,0,2,0,};
static const u_char stdtbl[] = { 0,1,2,0,0,0,3,0,0,0,};
static const u_char mfdtbl[] = { 0,2,3,0,0,0,0,0,0,0,};
int sceDmaPutEnv(sceDmaEnv *env)
{
	u_int	ctrl = *D_CTRL;
	u_int	pcr  = *D_PCR;
	u_int	sqwc = *D_SQWC;
	u_int	rbor = *D_RBOR;
	u_int	rbsr = *D_RBSR;

	if (env->sts    > 9) return(-1);	// check Stall Source
	if (env->std    > 9) return(-2);	// check Stall Drain
	if (env->mfd    > 9) return(-3);	// check MFIFO Drain	
	if (env->rcycle > 6) return(-4);	// check RCYCLE	

	setSTS(ctrl, ststbl[env->sts]);		// set stall source
	setSTD(ctrl, stdtbl[env->std]);		// set stall drain	
	setMFD(ctrl, mfdtbl[env->mfd]);		// set MFIFO drain

	if (env->rcycle) {			// set RCYCLE
		setRELE(ctrl);
		setRCYC(ctrl, env->rcycle-1);
	}
	else
		clearRELE(ctrl);

	pcr  = (env->express<<16) | env->notify;// set PCR
	sqwc = (env->tqwc   <<16) | env->sqwc;	// set SQWC
	rbor = (u_int)env->rbadr;		// set RBOR
	rbsr = (u_int)env->rbmsk;		// set RBSR

	*D_CTRL = ctrl;				
	*D_PCR  = pcr;
	*D_SQWC = sqwc;
	*D_RBOR = rbor;
	*D_RBSR = rbsr;

	sceDmaCurrentEnv = *env;
	return(0);
}


/*
  sceDmaGetEnv チャネル共通レジスタの取得 

引数
  denv レジスタの値を読み出す構造体のアドレス
返り値
  denv

 */
sceDmaEnv *sceDmaGetEnv(sceDmaEnv *denv)
{	
	*denv = sceDmaCurrentEnv;
	return(denv);
}


/*
  sceDmaPutStallAddr ストールアドレスの設定

引数
  addr ストールするアドレス

返り値
  設定前のストールアドレス
 */
void *sceDmaPutStallAddr(void *addr)
{
	u_int oaddr = *D_STADR;
	*D_STADR = (u_int)addr;
	return((void *)oaddr);
}


/*
  sceDmaSend DMA転送の起動（Source Chain Mode）

引数
  d 転送を行うDMAチャネル 
  tag 転送リストの先頭アドレス
返り値
  なし
*/
void sceDmaSend(sceDmaChan *d, void *tag)
{
	tD_CHCR	chcr;  
	tag  = CheckAddress(tag);
	WaitDma(d);
	setADR(d->tadr, tag);
	setQWC(d->qwc,  0);

	chcr = d->chcr;
	setCHCR(chcr, 1, 1);
	d->chcr = chcr;
}


/*
  sceDmaSendN DMA転送の起動（Normal Mode, メモリ→デバイス） 

引数  
  d 転送を行うDMAチャネル 
  addr 転送開始アドレス 
  size 転送するデータサイズ（qword）

返り値
  なし
 */
void sceDmaSendN(sceDmaChan *d, void *addr, int size)
{
	tD_CHCR	chcr;
	addr = CheckAddress(addr);
	
	WaitDma(d);
	setADR(d->madr, addr);
	setQWC(d->qwc,  size);

	chcr = d->chcr;
	setCHCR(chcr, 0, 1);
	d->chcr = chcr;
	
}


/*
  sceDmaSendI DMA転送の起動（Interleave Mode, SPR→メモリ）

引数
  d 転送を行うDMAチャネル（toSPRのみ） 
  addr 転送データのアドレス 
  size 転送データのサイズ（qword） 

返り値
  なし
 */
void sceDmaSendI(sceDmaChan *d, void *addr, int size)
{
	tD_CHCR	chcr;
	addr = CheckAddress(addr);

	WaitDma(d);
	setADR(d->madr, addr);
	setQWC(d->qwc,  size);

	chcr = d->chcr;
	setCHCR(chcr, 2, 1);
	d->chcr = chcr;
}


/*
 sceDmaRecv DMA転送の起動（Destination Chain Mode）

引数
  d 転送を行うDMAチャネル 

返り値
  なし
*/
void sceDmaRecv(sceDmaChan *d)
{
	tD_CHCR	chcr;
	WaitDma(d);
	setQWC(d->qwc,  0);

	chcr = d->chcr;
	setCHCR(chcr, 1, 0);
	d->chcr = chcr;
}

// Recieve by Normal 

/*
  sceDmaRecvN MA転送の起動（Normal Mode, デバイス→メモリ）

引数
  d 転送を行うDMAチャネル 
  addr 転送先のメモリアドレス 
  size 転送するデータサイズ（qword） 

返り値   
  なし
 */
void sceDmaRecvN(sceDmaChan *d, void *addr, int size)
{
  tD_CHCR	chcr;
  addr = CheckAddress(addr);
  
  WaitDma(d);
  setADR(d->madr, addr);
  setQWC(d->qwc,  size);
  
  chcr = d->chcr;
  setCHCR(chcr, 0, 0);
  d->chcr = chcr;
}


/*
  sceDmaRecvI DMA転送の起動（Interleave Mode, SPR→メモリ）

引数
  d 転送を行うDMAチャネル（fromSPR） 
  addr 転送先のメモリアドレス 
  size 転送するデータサイズ（qword） 

返り値
  なし
 */
void sceDmaRecvI(sceDmaChan *d, void *addr, int size)
{
  tD_CHCR	chcr;

  addr = CheckAddress(addr);
  WaitDma(d);
  setADR(d->madr, addr);
  setQWC(d->qwc,  size);
  
  chcr = d->chcr;
  setCHCR(chcr, 2, 0);
  d->chcr = chcr;
  
}


/*
  sceDmaWatch DMA転送の終了待ち

引数
  d 対象のDMAチャネル
  mode ブロックモード
   0:ブロック
   1:ノンブロック 

返り値
  1:動作中
  0:停止中
 */
int sceDmaSync(sceDmaChan *d, int mode, int timeout)
{
  if (mode == 1) 
    return(d->chcr.STR);
  else {
    WaitDma(d);
    return(0);
  }
}


/*
  sceDmaWatch 特定アドレスの転送待ち

引数
  d 対象のDMAチャネル
  addr 検査対象のメモリアドレス
  mode ブロックモード
   0:ブロック
   1:ノンブロック 

返り値
  1:転送済み
  0:未転送
 */
int sceDmaWatch(sceDmaChan *d, void *addr, int mode, int timeout)
{
  if (mode == 1)
    return(d->madr < addr);
  else {
    WatchDma(d, addr, timeout);
    return(0);
  }
}


/*
  sceDmaPause DMA転送の一時停止 

引数
  d 一時停止させるDMAチャネル 

返り値
  停止したあとのDn_CHCRの値
 */
u_int sceDmaPause(sceDmaChan *d)
{
  u_int chcr;
  u_int  ochcr;
  int status = DI();
  int enabler = *D_ENABLER;
  int ctrl;

  if(!(enabler & D_ENABLER_CPND_M))
    *D_ENABLEW = enabler | D_ENABLER_CPND_M ;

  /* DMA転送停止を俟つためには、2回のI/O read accessが必要なので
     D_CTRL,D_CHCRを読み込む*/
  ctrl = *D_CTRL;
  ochcr = *(volatile u_int*) &(d->chcr);
  chcr = ochcr;
  chcr &= ~D_CHCR_STR_M  ;

  /*DMA転送停止後、Dn_CHCRを更新する。*/
  *((volatile u_int*)&(d->chcr)) = (volatile u_int)chcr;

  *D_ENABLEW = enabler;

  if(status)
    EI();
  return(ochcr);
}


/*
  sceDmaRestart 一時停止したDMA転送の再開
  
引数
  d 転送再開するDMAチャネル 
  chcr sceDmaPause関数の返り値

返り値
  転送再開するDMAチャネルの状態
   0:停止中
   1:すでに動作中
 */
int sceDmaRestart(sceDmaChan *d, u_int chcr)
{
  int str = d->chcr.STR;
  *((volatile u_int*)&(d->chcr)) = (volatile u_int)chcr ;
  return(str);
}
