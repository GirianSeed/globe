/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                      Emotion Engine Library
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        rspu2drv - filedrv.c
 *
 *     Version    Date          Design     Log
 *  --------------------------------------------------------------------
 */

#include <kernel.h>
#include <stdio.h>
#include <sys/file.h>

#include <sif.h>
#include "sifcmd.h"
#include "sifrpc.h"
#include "sifstub.h"
#include "sifdev.h"


static void sce_open(void *data, int size, void *rdata)
{
  int fd;
  _sceFsOpenData *fodp = (_sceFsOpenData *) data;

  printf("open name %s flag %x data %x\n", fodp->name, (unsigned int)fodp->flag,(unsigned int)data);

  /* ファイルをオープン */
  fd = open ( fodp->name, fodp->flag);
  printf("open fd = %d\n",fd);

  memcpy(rdata, &fd, sizeof(fd));
  printf("open end\n");
  return ;
}

static void sce_close(void *data, int size, void *rdata)
{
  int ret;
  _sceFsCloseData *fcdp = (_sceFsCloseData *) data;

  printf("close fd = %d\n", fcdp->fd);

  /* ファイルをクローズ */
  ret = close ( fcdp->fd);

  memcpy(rdata, &ret, sizeof(ret));
  
  printf("close end\n");
  return ;
}

static void sce_lseek(void *data, int size, void *rdata)
{
  int ret;
  _sceFsLseekData *fldp = (_sceFsLseekData *) data;
  
  /* ファイルをシーク */
  ret = lseek ( fldp->fd , fldp->offset, fldp->flag);
  
  memcpy(rdata, &ret, sizeof(ret));
  
  return;
}

#define RW_BSIZE 0x4000

static unsigned char rw_buffer[RW_BSIZE];
static _sceFsReadIntrData frid;

static void sce_read(void *data, int size,void *rdata)
{
  int fd,tsize,psize,bsize,ssize,ret,rs,offset,rsize;
  unsigned int taddr,paddr,baddr,saddr;
  _sceFsReadData * frdp = ( _sceFsReadData *) data;
  sceSifDmaData sdd;
  int oldisEI;


  /* パラメーターのコピー */
  fd = frdp->fd; /*ファィルディスクリプタ*/
  offset = frdp->offset; /* オフセット */
  taddr = frdp->addr; /* 転送アドレス */
  tsize = frdp->size; /* 転送サイズ */
  
  printf("read fd = %d addr %x size %x offset %x\n",fd,taddr,tsize,offset);

  /* シークしておく */
  lseek( fd, offset, SEEK_SET);
  rsize = 0;  /* total read size set 0 */

  /* 小データと端数(16バイト境界以外のデータ)の前処理 */
  if (tsize < 16) {
    /* 16 バイト以下ならすべて前端数に入れる */
    paddr = taddr; psize = tsize;
    baddr = 0 ;bsize = 0;
    saddr = 0 ;ssize = 0 ; 
  } else {
    if (taddr % 16 == 0) psize = 0;
    else psize = ((taddr /16) * 16 + 16) - taddr;
    paddr = taddr;
    baddr = taddr + psize; bsize =((taddr + tsize)/16)*16 - baddr;
    saddr = baddr + bsize; ssize = taddr + tsize - saddr;
  }
  //   printf("read paddr %x psize %x baddr %x bsize %x saddr %x ssize %x\n", paddr, psize, baddr, bsize, saddr, ssize);
  
  /* 前端数があれば読んで蓄えておく */
  if (psize > 0) {
    ret = read( fd, &(frid.pdata[0]), psize);
    if (psize != ret) {
      /* error */
      if (ret > 0) psize = ret;
      else psize = 0;
      bsize = 0;   /* 後ろ端数は,読めなかったので０とする */
      rsize += psize;
      goto READ_EXIT;      
    }
    rsize += ret;
  }

  /* 16バイト境界の中を読む */
  while(bsize > 0) {
    if (bsize > RW_BSIZE) rs = RW_BSIZE;
    else  rs = bsize;

    // Kprintf("fr:1 ");
    ret = read( fd, &(rw_buffer[0]), rs);
    //    for (i = 0; i < rs; i ++) printf("%x ", rw_buffer[i]);
    
    if (rs != ret) {
      /* error */
      if (ret > 0) {
	sdd.data = (unsigned int) &(rw_buffer[0]);
	sdd.addr = baddr;
	sdd.size = ret;
	sdd.mode = 0;
	// Kprintf("fr:2 ");
	CpuSuspendIntr(&oldisEI);
	sceSifSetDma(&sdd,1);
	CpuResumeIntr(oldisEI);
	rsize += ret;
      }
      bsize = 0; /* 後ろ端数は,読めなかったので０とする */
      goto READ_EXIT;
    }
    sdd.data = (unsigned int) &(rw_buffer[0]);
    sdd.addr = baddr;
    sdd.size = ret;
    sdd.mode = 0;
    //    Kprintf("fr:3 ");
    CpuSuspendIntr(&oldisEI);
    sceSifSetDma(&sdd,1);
    CpuResumeIntr(oldisEI);

    /* 値の再計算 */
    rsize += ret;
    bsize -= ret;
    baddr += ret;
  }
  /* 後端数があれば読んで蓄えておく */
  if (ssize > 0) {
    ret = read( fd, &(frid.sdata[0]), ssize);
    if (ssize != ret) {
      /* error */
      if (ret > 0) ssize = ret;
      else  ssize = 0;
    }
    rsize += ssize;
    goto READ_EXIT;
  }

READ_EXIT:
  frid.psize = psize;
  frid.ssize = ssize;
  frid.paddr = paddr;
  frid.saddr = saddr;

  sdd.data = (unsigned int) &frid;
  sdd.addr = frdp->intr_addr;
  sdd.size = sizeof(frid);
  sdd.mode = 0;
  //  printf("data %x addr %x size %x", sdd.data, sdd.addr, sdd.size);
  //  Kprintf("fr:4 ");
  CpuSuspendIntr(&oldisEI);
  sceSifSetDma(&sdd,1);
  CpuResumeIntr(oldisEI);
  //  printf("read end\n");
  //  Kprintf("\n");
  memcpy(rdata, &rsize, sizeof(rsize));
  
  return ;
}

static void sce_write(void *data, int dsize, void *rdata)
{
  int fd,size,psize,offset,wsize,gsize,ret;
  unsigned int gaddr;
  _sceFsWriteData *swdp =( _sceFsWriteData *)data;

  /* パラメーターのコピー */
  fd = swdp->fd; /*ファィルディスクリプタ*/
  offset = swdp->offset; /* オフセット */
  size = swdp->size; /* 転送サイズ */
  psize =swdp->psize ; /* 前端数サイズ */
  //  printf("write fd = %d size %x psize %x offset %x\n", fd,size,psize,offset);

  lseek( fd, offset, SEEK_SET);
  wsize = 0;  /* total write size set 0 */
  
  /* 前端数を書き込む */
  if (psize > 0) {
    ret = write( fd, &(swdp->pdata[0]), psize);
    if (ret != psize) {
      if (ret > 0)  wsize += ret;
      /* error */
      memcpy(rdata, &wsize, sizeof(wsize));
      return;
    }
    wsize += ret;
  }

  /* 送るアドレスの設定 */
  gaddr = swdp->addr + psize;
  size -= psize;
  //  printf("get address %x size %d\n", gaddr, size);
  while(size) {
    if (size > RW_BSIZE) gsize = RW_BSIZE;
    else gsize = size;
    /* 相手から取って来る */
#if 0
    sceSifGetOtherData(gaddr,(unsigned int) &(rw_buffer[0]), gsize, 0);
#endif    
    //    for (i = 0; i < gsize; i ++) printf("%x ", rw_buffer[i]);
    
    ret = write( fd, &(rw_buffer[0]), gsize);
    if (ret != gsize) {
      if (ret > 0)  wsize += ret;
      /* error */
      memcpy(rdata, &wsize, sizeof(wsize));
      return;
    }
    /* 書き込み数のインクリメント*/
    wsize += gsize;
    size -= gsize;
    gaddr += gsize;
  }

  memcpy(rdata, &wsize, sizeof(wsize));  
  return ;
}

static void sce_ioctl(void *data, int size, void *rdata)
{
  return;
}

extern int sceIopLoadElf(char *, unsigned int *,unsigned int *);
static sceExecData sed;

#if 0
static void * sce_loadelf(unsigned int fno, void *data, int size)
{
  int ret;
  unsigned int epc,gp;
  sceSifDmaData sdd;
  _sceLoadElfData *lep = (_sceLoadElfData *) data;
  int oldisEI;

  printf("loadelf name %s addr = %x\n", lep->name, lep->reply_addr);

  /* ファイルをオープン */
  ret = sceIopLoadElf( lep->name, &epc, &gp);
  sed.epc = epc;
  sed.gp = gp;
  sed.sp = 0;

  return (void *)&sed;
}
#endif

static unsigned int rdata[4];
static void * sce_fileio(unsigned int fno, void *data, int size)
{
  switch (fno) {
  case SCE_OPEN:
    sce_open(data, size, &rdata[0]);
    break;
  case SCE_CLOSE:
    sce_close(data, size, &rdata[0]);
    break;
  case SCE_READ:
    sce_read(data, size, &rdata[0]);
    break;
  case SCE_WRITE:
    sce_write(data, size, &rdata[0]);
    break;
  case SCE_LSEEK:
    sce_lseek(data, size, &rdata[0]);
    break;
  case SCE_IOCTL:
    sce_ioctl(data, size, &rdata[0]);
    break;
  default:
    printf("sce_fileio: unrecognized code %x\n", fno);
    break;
  }

  return (void *) &rdata[0];
}


static sceSifQueueData qd;
static sceSifServeData sd0;
static sceSifServeData sd1;
static unsigned int buffer[0x10];
static unsigned int buffer1[0x10];

int sce_fs_loop()
{
  sceSifSetRpcQueue(&qd,GetThreadId());
  
  sceSifRegisterRpc(&sd0, SCE_SIF_FILEIO,
		    sce_fileio, (void *)&buffer[0], 0, 0, &qd);
#if 0
  sceSifRegisterRpc(&sd1, SCE_SIF_LOADELF,
		    sce_loadelf, (void *)&buffer1[0], 0, 0, &qd);
#endif
  sceSifRpcLoop(&qd);
  return 0;
}
