/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 */
/*
 *                      Emotion Engine Library
 *                          Version 0.01
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        libpkt - devvif0.c
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,29,1999     shibuya
 */

#include <eekernel.h>
#include <eeregs.h>
#include "devvif0.h"

static u_int sceDevVif0Sync(void);

void sceDevVif0Reset(void){
  DPUT_VIF0_FBRST(0x00000001);
  DPUT_VIF0_ERR(0x00000006);
}

int sceDevVif0Pause(int mode){
  u_int stat = sceDevVif0Sync();
  if(stat == 0){ return 0; }					/*	idle	*/
  if(stat == 1){ return 0; }					/*	stall	*/

  if(mode == 0){
    DPUT_VIF0_FBRST(0x00000002);				/*	force break	*/

  }else if(mode == 1){
    DPUT_VIF0_FBRST(0x00000004);				/*	stop		*/
  }

  return 1;
}

int sceDevVif0Continue(void){
  u_int stat = sceDevVif0Sync();
  if(stat != 1){ return 0; }					/*	not stall?	*/
  DPUT_VIF0_FBRST(0x00000008);					/*	stall cancel */
  return 1;
}

u_int sceDevVif0PutErr(int interrupt, int miss1, int miss2){
  u_int err = interrupt | (miss1 << 1) | (miss2 << 2);
  DPUT_VIF0_ERR(err);
  return err;
}

u_int sceDevVif0GetErr(void){
  return DGET_VIF0_ERR();
}

static u_int sceDevVif0Sync(void){
  u_int stat = DGET_VIF0_STAT();
  if(0 == (stat & 0x00000003)){ return 0; }		/*	idle	*/
  if(stat & 0x00000700){ return 1; }			/*	stall	*/
  if(stat & 0x0000000c){ return 2; }			/*	wait	*/
  if(stat & 0x00003000){ return 3; }			/*	error	*/
  return 4;										/*	busy	*/
}

int sceDevVif0GetCnd(sceDevVif0Cnd *cnd){
  u_int cycle = DGET_VIF0_CYCLE();
  u_int stat = sceDevVif0Sync();
  if(1 < stat){ return 0; }

  cnd->row[0] = DGET_VIF0_R0();
  cnd->row[1] = DGET_VIF0_R1();
  cnd->row[2] = DGET_VIF0_R2();
  cnd->row[3] = DGET_VIF0_R3();
  cnd->col[0] = DGET_VIF0_C0();
  cnd->col[1] = DGET_VIF0_C1();
  cnd->col[2] = DGET_VIF0_C2();
  cnd->col[3] = DGET_VIF0_C3();
  cnd->mask = DGET_VIF0_MASK();
  cnd->code = DGET_VIF0_CODE();
  cnd->stat = DGET_VIF0_STAT();
  cnd->itop = (u_short)DGET_VIF0_ITOP();
  cnd->itops = (u_short)DGET_VIF0_ITOPS();
  cnd->mark = (u_short)DGET_VIF0_MARK();
  cnd->num = (u_short)DGET_VIF0_NUM();
  cnd->error = (u_char)DGET_VIF0_ERR();
  cnd->cl = (u_char)(cycle);
  cnd->wl = (u_char)(cycle >> 8);
  cnd->cmod = (u_char)DGET_VIF0_MODE();

  return 1;
}

int sceDevVif0PutFifo(u_long128 *addr, int n){
  u_int count = 0;

  while(n--){
    if(DGET_VIF0_STAT() & 0x10000000){ return count; }	/*	fifo full? */
    count++;
    DPUT_VIF0_FIFO(*addr++);
  }

  return count;
}


/* [eof] */
