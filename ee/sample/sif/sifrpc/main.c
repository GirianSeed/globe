/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library  Release 2.0
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
 *                            <sample.c>
 *                     <main function of file read>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10            Mar,26,1999     hakamatani  
 *      0.90            Aug,23,1999     hakamatani  first version
 */


#include <eekernel.h>
#include <eetypes.h>
#include <eeregs.h>
#include <stdio.h>
#include <sifrpc.h>

/* ================================================================
 *
 *      Program
 *
 * ================================================================ */

#define SERVER

#define TEST_CALL0 0x11
#define TEST_CALL1 0x12

#define SSIZE 0x80

#ifdef SERVER
static u_long128 buffer [SSIZE];
#else
static u_int sdata[SSIZE / 4] __attribute__((aligned(64)));
static u_long128 rdata[SSIZE / 16];
#endif

#ifdef SERVER
void * rpc_server(unsigned int fno, void *data, int size);
#else
void intr_func(void *);
#endif



int main()
{
#ifdef SERVER
  sceSifQueueData qd,qd0;
  sceSifServeData sd0,sd1;
#else
  sceSifClientData cd0,cd1;
  unsigned int *p,i;
#endif


  printf("RPC Initialize\n");
  sceSifInitRpc(0);

#ifdef SERVER

  printf("set rpc queue\n");
  sceSifSetRpcQueue(&qd, GetThreadId());
  sceSifSetRpcQueue(&qd0, GetThreadId());

  printf("register function\n");
  sceSifRegisterRpc(&sd0, TEST_CALL0,rpc_server,(void *) &buffer[0],0,0,&qd);
  sceSifRegisterRpc(&sd1, TEST_CALL1,rpc_server,0,0,0,&qd);

  printf("goto wait loop\n");
  sceSifRpcLoop(&qd);
#else
  printf("bind funcion 1(nowait mode)\n");
  while(1){
    if (sceSifBindRpc( &cd1, TEST_CALL1, SIF_RPCM_NOWAIT) < 0) {
      printf("bind errr\n");
      while(1);
    }
    while(sceSifCheckStatRpc((sceSifRpcData *)&cd1) != 0){i ++;};
    if (cd1.serve != 0) break;
    i = 0x10000;
    while(i --);
  }
  printf("bind funcion 2(wait mode = sleep thread)\n");
  while(1){
    if (sceSifBindRpc( &cd0, TEST_CALL0, 0) < 0) {
      printf("bind errr\n");
      while(1);
    }
    if (cd0.serve != 0) break;
    i = 0x10000;
    while(i --);
  }

  printf("data initialize\n");  
  p = (unsigned int *) &sdata[0];
  for (i = 0; i < SSIZE / 4; i ++) *(p + i ) = i;
  FlushCache(0);

  printf("call fucniton: normal\n"); 
  sceSifCallRpc(&cd0, 0, 0, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
  p = (unsigned int *) &sdata[0];
  for (i = 0; i < SSIZE / 4; i ++) printf("%x %x\n",i ,*(p + i ));
  
  printf("call fucniton: with end funcion\n"); 
  p = (unsigned int *) &sdata[0];
  for (i = 0; i < SSIZE / 4; i ++) *(p + i ) = i;
  sceSifCallRpc(&cd0, 1, 0,
	     &sdata[0],SSIZE, &sdata[0], SSIZE, intr_func, &sdata[0]);
  for (i = 0; i < SSIZE / 4; i ++) printf("%x %x\n",i ,*(p + i ));

  printf("call fucniton: with no wait & end funcion\n");
  p = (unsigned int *) &sdata[0];
  for (i = 0; i < SSIZE / 4; i ++) *(p + i ) = i|0xbb0;
  sceSifCallRpc(&cd0, 2, SIF_RPCM_NOWAIT,
  	     &sdata[0],SSIZE, &rdata[0], SSIZE, intr_func, &rdata[0]);
  i = 0;
  while(sceSifCheckStatRpc((sceSifRpcData *) &cd0) != 0){i ++;};
  printf("loop cnt %x \n",i );
  p = (unsigned int *) &rdata[0];
  for (i = 0; i < SSIZE / 4; i ++) printf("%x %x\n",i ,*(p + i ));

  printf("end\n");
  while(1);

#endif

}

#ifdef SERVER
void * rpc_server(unsigned int fno, void *data, int size)
{
  unsigned int *p,i;
  
  printf("rpc_test no %x size %x\n", fno, size);

  p = (unsigned int *) data;
  for (i = 0; i < size / 4; i ++) {
    printf("%x  %x\n", i , *(p + i));
  }

  if (size > 0) {
    p = (unsigned int *) data;
    for (i = 0; i < size / 4; i ++) *(p + i) |= 0xff000000;
    return data;
  } else {
    return (void *) 0;
  }
}
#else
void intr_func(void *data)
{
  unsigned int *p,i;
  
  p = (unsigned int *) data;
  for (i = 0; i < SSIZE / 4; i ++) *(p + i) |= 0xaa00;
  
}
#endif



/* end of file.*/
