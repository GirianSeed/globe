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
  DMA�̃`���l�����ƂɃX���b�h�ŌĂяo���Ė��Ȃ��悤��libdma����������B
 */


/*
  WaitDma  ��莞��DMA�]�����I������܂ő҂�

����
  d �Ώۂ�DMA�`���l��

�Ԃ�l
  �Ȃ�
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
  WatchDma ��莞�ԓ���A�h���X��DMA�]���҂�

����
  d �Ώۂ�DMA�`���l�� 
  addr �����Ώۂ̃������A�h���X 

�Ԃ�l
  �Ȃ�
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
  SPR��̃A�h���X�̏ꍇDMA�ɐݒ肷��A�h���X��
  31bit�ڂ�1�ɂ���K�v������̂ŕϊ�����B
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


/* sceDmaGetChan �`���l���\���̂̎擾 

����
  id �`���l���ԍ� 
�Ԃ�l
  0:id���s��
  �`���l���ԍ�id�ɑ�������sceDmaChan�\���̂̃A�h���X
 */
sceDmaChan *sceDmaGetChan(int id)
{
	return ((id>=0 && id<=9) ? (sceDmaChan *)dch[id]: 0);
}



static const int isclr[10] = {1,1,1,1,1,0,0,0,1,1};
/* 
  sceDmaReset DMAC�̃��Z�b�g

����
  mode ���Z�b�g���[�h
   1:enable
   0:disable
�Ԃ�l
   ���O��mode�̒l
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
  sceDmaPutEnv �`���l�����ʃ��W�X�^�̐ݒ�

����
  env ���W�X�^�ɏ������ޒl
�Ԃ�l
  0:����I��
  ��:�ݒ�l���s��
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
  sceDmaGetEnv �`���l�����ʃ��W�X�^�̎擾 

����
  denv ���W�X�^�̒l��ǂݏo���\���̂̃A�h���X
�Ԃ�l
  denv

 */
sceDmaEnv *sceDmaGetEnv(sceDmaEnv *denv)
{	
	*denv = sceDmaCurrentEnv;
	return(denv);
}


/*
  sceDmaPutStallAddr �X�g�[���A�h���X�̐ݒ�

����
  addr �X�g�[������A�h���X

�Ԃ�l
  �ݒ�O�̃X�g�[���A�h���X
 */
void *sceDmaPutStallAddr(void *addr)
{
	u_int oaddr = *D_STADR;
	*D_STADR = (u_int)addr;
	return((void *)oaddr);
}


/*
  sceDmaSend DMA�]���̋N���iSource Chain Mode�j

����
  d �]�����s��DMA�`���l�� 
  tag �]�����X�g�̐擪�A�h���X
�Ԃ�l
  �Ȃ�
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
  sceDmaSendN DMA�]���̋N���iNormal Mode, ���������f�o�C�X�j 

����  
  d �]�����s��DMA�`���l�� 
  addr �]���J�n�A�h���X 
  size �]������f�[�^�T�C�Y�iqword�j

�Ԃ�l
  �Ȃ�
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
  sceDmaSendI DMA�]���̋N���iInterleave Mode, SPR���������j

����
  d �]�����s��DMA�`���l���itoSPR�̂݁j 
  addr �]���f�[�^�̃A�h���X 
  size �]���f�[�^�̃T�C�Y�iqword�j 

�Ԃ�l
  �Ȃ�
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
 sceDmaRecv DMA�]���̋N���iDestination Chain Mode�j

����
  d �]�����s��DMA�`���l�� 

�Ԃ�l
  �Ȃ�
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
  sceDmaRecvN MA�]���̋N���iNormal Mode, �f�o�C�X���������j

����
  d �]�����s��DMA�`���l�� 
  addr �]����̃������A�h���X 
  size �]������f�[�^�T�C�Y�iqword�j 

�Ԃ�l   
  �Ȃ�
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
  sceDmaRecvI DMA�]���̋N���iInterleave Mode, SPR���������j

����
  d �]�����s��DMA�`���l���ifromSPR�j 
  addr �]����̃������A�h���X 
  size �]������f�[�^�T�C�Y�iqword�j 

�Ԃ�l
  �Ȃ�
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
  sceDmaWatch DMA�]���̏I���҂�

����
  d �Ώۂ�DMA�`���l��
  mode �u���b�N���[�h
   0:�u���b�N
   1:�m���u���b�N 

�Ԃ�l
  1:���쒆
  0:��~��
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
  sceDmaWatch ����A�h���X�̓]���҂�

����
  d �Ώۂ�DMA�`���l��
  addr �����Ώۂ̃������A�h���X
  mode �u���b�N���[�h
   0:�u���b�N
   1:�m���u���b�N 

�Ԃ�l
  1:�]���ς�
  0:���]��
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
  sceDmaPause DMA�]���̈ꎞ��~ 

����
  d �ꎞ��~������DMA�`���l�� 

�Ԃ�l
  ��~�������Ƃ�Dn_CHCR�̒l
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

  /* DMA�]����~��ւ��߂ɂ́A2���I/O read access���K�v�Ȃ̂�
     D_CTRL,D_CHCR��ǂݍ���*/
  ctrl = *D_CTRL;
  ochcr = *(volatile u_int*) &(d->chcr);
  chcr = ochcr;
  chcr &= ~D_CHCR_STR_M  ;

  /*DMA�]����~��ADn_CHCR���X�V����B*/
  *((volatile u_int*)&(d->chcr)) = (volatile u_int)chcr;

  *D_ENABLEW = enabler;

  if(status)
    EI();
  return(ochcr);
}


/*
  sceDmaRestart �ꎞ��~����DMA�]���̍ĊJ
  
����
  d �]���ĊJ����DMA�`���l�� 
  chcr sceDmaPause�֐��̕Ԃ�l

�Ԃ�l
  �]���ĊJ����DMA�`���l���̏��
   0:��~��
   1:���łɓ��쒆
 */
int sceDmaRestart(sceDmaChan *d, u_int chcr)
{
  int str = d->chcr.STR;
  *((volatile u_int*)&(d->chcr)) = (volatile u_int)chcr ;
  return(str);
}
