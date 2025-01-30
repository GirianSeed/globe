/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *	Inet Setting Application Sample
 *
 *                          Version 1.4
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Inc.
 *	                 All Rights Reserved.
 *
 *                              Rpc.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.16      tetsu      Change for PPPoE
 */
#include "setapp.h"

sceNetCnf_t result, result_stat;
sceSifClientData cd, cd_stat;

u_char sdata[SSIZE] __attribute__((aligned(64)));
u_char sdata_stat[SSIZE] __attribute__((aligned(64)));

/*************************/
/* 以下 SIF DMA 関数です */
/*************************/

int
send_iop(u_int data, u_int addr, u_int size, u_int mode)
{
  sceSifDmaData fdma;
  int id;

  fdma.data = data;
  fdma.addr = addr;
  fdma.size = (size & 0xfffffff0) + ((size % 0x10) ? 0x10 : 0);
  fdma.mode = mode;
  FlushCache(0);
  id = sceSifSetDma(&fdma, 1);
  return (id);
}

int
dma_check(int id)
{
  return (sceSifDmaStat(id));
}

/*************************************/
/* 以下 RPC による呼び出し関数です。 */
/*************************************/

void
Rpc_setup(void)
{
  int i;

  /* NETCNF_RPC の使用許可 */
  while(1){
    if(sceSifBindRpc(&cd, NETCNF_INTERFACE, SIF_RPCM_NOWAIT) < 0){
      printf("Bind Error\n");
      while(1);
    }
    while(sceSifCheckStatRpc((sceSifRpcData *)&cd) != 0);
    if(cd.serve != 0) break;
    i = 0x10000;
    while(i --);
  }

  while(1){
    if(sceSifBindRpc(&cd_stat, NETCNF_STATUS, SIF_RPCM_NOWAIT) < 0){
      printf("Bind Error\n");
      while(1);
    }
    while(sceSifCheckStatRpc((sceSifRpcData *)&cd_stat) != 0);
    if(cd_stat.serve != 0) break;
    i = 0x10000;
    while(i --);
  }

  /* Rpc バッファの初期化 */
  Rpc_Init();
  Rpc_Init_Stat();
}

void
Rpc_Init(void)
{
  sceNetCnf_t *p;

  p = (sceNetCnf_t *)sdata;
  p->data = 0;
}

void
Rpc_Init_Stat(void)
{
  sceNetCnf_t *p;

  p = (sceNetCnf_t *)sdata_stat;
  p->data = -1;
}

int
Rpc_Check(void)
{
  return (sceSifCheckStatRpc((sceSifRpcData *)&cd));
}

int
Rpc_Check_Stat(void)
{
  return (sceSifCheckStatRpc((sceSifRpcData *)&cd_stat));
}

int
Rpc_Get_Result(void)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  return (p->data);
}

int
Rpc_Get_Result_Stat(void)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata_stat;

  return (p->data);
}

void
Rpc_sceNetCnfGetCount(int type)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->type        = type;
  p->device_stat = device_stat;
  if(mc_stat0 == 0 && mc_stat1 == 1){
    strcpy(p->dir_name, MC_DB_NAME1);
  }else{
    strcpy(p->dir_name, MC_DB_NAME0);
  }
  strcpy(p->dir_name2, HDD_DB_NAME);
  strcpy(p->dir_name3, HOST_DB_NAME);

  /* GET_COUNT の呼び出し */
  sceSifCallRpc(&cd, GET_COUNT, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_sceNetCnfGetList(sceNetCnfList2_t *dst_addr, int size, int type)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->dst_addr    = (u_int)dst_addr;
  p->size        = size;
  p->type        = type;
  p->no_decode   = no_decode;
  p->no_decode2  = no_decode2;
  p->no_decode3  = no_decode3;
  p->device_stat = device_stat;
  if(mc_stat0 == 0 && mc_stat1 == 1){
    strcpy(p->dir_name, MC_DB_NAME1);
  }else{
    strcpy(p->dir_name, MC_DB_NAME0);
  }
  strcpy(p->dir_name2, HDD_DB_NAME);
  strcpy(p->dir_name3, HOST_DB_NAME);

  /* GET_LIST の呼び出し */
  sceSifCallRpc(&cd, GET_LIST, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_sceNetCnfLoadEntry(sceNetCnfEnvData_t *dst_addr, int type, char *usr_name, int select_device)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->dst_addr = (int)dst_addr;
  p->type     = type;
  strcpy(p->usr_name, usr_name);
  switch(select_device){
  case SELECT_MC:
    p->no_decode = no_decode;
    if(mc_stat0 == 0 && mc_stat1 == 1){
      strcpy(p->dir_name, MC_DB_NAME1);
    }else{
      strcpy(p->dir_name, MC_DB_NAME0);
    }
    break;
  case SELECT_HDD:
    p->no_decode = no_decode2;
    strcpy(p->dir_name, HDD_DB_NAME);
    break;
  case SELECT_HOST:
    p->no_decode = no_decode3;
    strcpy(p->dir_name, HOST_DB_NAME);
    break;
  }

  /* LOAD_ENTRY の呼び出し */
  sceSifCallRpc(&cd, LOAD_ENTRY, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_sceNetCnfAddEntry(sceNetCnfEnvData_t *src_addr, int type, char *usr_name, int select_device)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->src_addr = (u_int)src_addr;
  p->type     = type;
  strcpy(p->usr_name, usr_name);
  switch(select_device){
  case SELECT_MC:
    p->no_decode = no_decode;
    if(mc_stat0 == 0 && mc_stat1 == 1){
      strcpy(p->dir_name, MC_DB_NAME1);
    }else{
      strcpy(p->dir_name, MC_DB_NAME0);
    }
    break;
  case SELECT_HDD:
    p->no_decode = no_decode2;
    strcpy(p->dir_name, HDD_DB_NAME);
    break;
  case SELECT_HOST:
    p->no_decode = no_decode3;
    strcpy(p->dir_name, HOST_DB_NAME);
    break;
  }

  /* ADD_ENTRY の呼び出し */
  sceSifCallRpc(&cd, ADD_ENTRY, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_sceNetCnfEditEntry(sceNetCnfEnvData_t *src_addr, int type, char *usr_name, char *usr_name2, int select_device)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->src_addr = (u_int)src_addr;
  p->type     = type;
  strcpy(p->usr_name, usr_name);
  strcpy(p->usr_name2, usr_name2);
  switch(select_device){
  case SELECT_MC:
    p->no_decode = no_decode;
    if(mc_stat0 == 0 && mc_stat1 == 1){
      strcpy(p->dir_name, MC_DB_NAME1);
    }else{
      strcpy(p->dir_name, MC_DB_NAME0);
    }
    break;
  case SELECT_HDD:
    p->no_decode = no_decode2;
    strcpy(p->dir_name, HDD_DB_NAME);
    break;
  case SELECT_HOST:
    p->no_decode = no_decode3;
    strcpy(p->dir_name, HOST_DB_NAME);
    break;
  }

  /* EDIT_ENTRY の呼び出し */
  sceSifCallRpc(&cd, EDIT_ENTRY, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_sceNetCnfDeleteEntry(int type, char *usr_name, int select_device)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->type = type;
  strcpy(p->usr_name, usr_name);
  switch(select_device){
  case SELECT_MC:
    if(mc_stat0 == 0 && mc_stat1 == 1){
      strcpy(p->dir_name, MC_DB_NAME1);
    }else{
      strcpy(p->dir_name, MC_DB_NAME0);
    }
    break;
  case SELECT_HDD:
    strcpy(p->dir_name, HDD_DB_NAME);
    break;
  case SELECT_HOST:
    strcpy(p->dir_name, HOST_DB_NAME);
    break;
  }

  /* DELETE_ENTRY の呼び出し */
  sceSifCallRpc(&cd, DELETE_ENTRY, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_sceNetCnfDeleteAll(int select_device)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  switch(select_device){
  case SELECT_MC:
    if(mc_stat0 == 0 && mc_stat1 == 1){
      strcpy(p->dir_name, MC_DB_NAME1);
    }else{
      strcpy(p->dir_name, MC_DB_NAME0);
    }
    break;
  case SELECT_HDD:
    strcpy(p->dir_name, HDD_DB_NAME);
    break;
  case SELECT_HOST:
    strcpy(p->dir_name, HOST_DB_NAME);
    break;
  }

  /* DELETE_ALL の呼び出し */
  sceSifCallRpc(&cd, DELETE_ALL, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_sceNetCnfSetConfiguration(int data, sceNetCnfEnvData_t *src_addr, char *usr_name, char *usr_name2, int select_device, int select_device2)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->data = data;
  if(src_addr  != NULL) p->src_addr = (int)src_addr;
  if(usr_name  != NULL) strcpy(p->usr_name, usr_name);
  if(usr_name2 != NULL) strcpy(p->usr_name2, usr_name2);
  switch(select_device){
  case NO_SELECT:
    strcpy(p->dir_name, "");
    break;
  case SELECT_MC:
    p->no_decode = no_decode;
    if(mc_stat0 == 0 && mc_stat1 == 1){
      strcpy(p->dir_name, MC_DB_NAME1);
    }else{
      strcpy(p->dir_name, MC_DB_NAME0);
    }
    break;
  case SELECT_HDD:
    p->no_decode = no_decode2;
    strcpy(p->dir_name, HDD_DB_NAME);
    break;
  case SELECT_HOST:
    p->no_decode = no_decode3;
    strcpy(p->dir_name, HOST_DB_NAME);
    break;
  }
  switch(select_device2){
  case NO_SELECT:
    strcpy(p->dir_name2, "");
    break;
  case SELECT_MC:
    p->no_decode2 = no_decode;
    if(mc_stat0 == 0 && mc_stat1 == 1){
      strcpy(p->dir_name2, MC_DB_NAME1);
    }else{
      strcpy(p->dir_name2, MC_DB_NAME0);
    }
    break;
  case SELECT_HDD:
    p->no_decode2 = no_decode2;
    strcpy(p->dir_name2, HDD_DB_NAME);
    break;
  case SELECT_HOST:
    p->no_decode2 = no_decode3;
    strcpy(p->dir_name2, HOST_DB_NAME);
    break;
  }

  /* SET_CNF の呼び出し */
  sceSifCallRpc(&cd, SET_CNF, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_sceNetCnfDownInterface(void)
{
  /* DOWN の呼び出し */
  sceSifCallRpc(&cd, DOWN, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_AllocSysMemory(int size)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->size = size;

  /* ALLOC_AREA の呼び出し */
  sceSifCallRpc(&cd, ALLOC_AREA, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_FreeSysMemory(int data)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->data = data;

  /* FREE_AREA の呼び出し */
  sceSifCallRpc(&cd, FREE_AREA, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_WaitEvent(int data)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata_stat;

  /* 引数の設定 */
  p->data   = data;

  /* WAIT_EVENT の呼び出し */
  sceSifCallRpc(&cd_stat, WAIT_EVENT, SIF_RPCM_NOWAIT, &sdata_stat[0], SSIZE, &sdata_stat[0], SSIZE, 0, 0);
}

void
Rpc_GetInterfaceCount(int mode)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->data        = mode;
  p->no_decode   = no_decode;
  p->no_decode2  = no_decode2;
  p->no_decode3  = no_decode3;
  p->device_stat = device_stat;
  if(mc_stat0 == 0 && mc_stat1 == 1){
    strcpy(p->dir_name, MC_DB_NAME1);
  }else{
    strcpy(p->dir_name, MC_DB_NAME0);
  }
  strcpy(p->dir_name2, HDD_DB_NAME);
  strcpy(p->dir_name3, HOST_DB_NAME);

  /* GET_IF_COUNT の呼び出し */
  sceSifCallRpc(&cd, GET_IF_COUNT, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_GetInterfaceList(int dst_addr, int size, int mode)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->data        = mode;
  p->dst_addr    = dst_addr;
  p->size        = size;
  p->no_decode   = no_decode;
  p->no_decode2  = no_decode2;
  p->no_decode3  = no_decode3;
  p->device_stat = device_stat;
  if(mc_stat0 == 0 && mc_stat1 == 1){
    strcpy(p->dir_name, MC_DB_NAME1);
  }else{
    strcpy(p->dir_name, MC_DB_NAME0);
  }
  strcpy(p->dir_name2, HDD_DB_NAME);
  strcpy(p->dir_name3, HOST_DB_NAME);

  /* GET_IF_LIST の呼び出し */
  sceSifCallRpc(&cd, GET_IF_LIST, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_GetHWAddr(int id)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->data = id;

  /* GET_HWADDR の呼び出し */
  sceSifCallRpc(&cd, GET_HWADDR, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}

void
Rpc_sceNetCnfCopyEntry(int type, char *usr_name, char *usr_name2, int select_device, int select_device2)
{
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  /* 引数の設定 */
  p->type = type;
  if(usr_name  != NULL) strcpy(p->usr_name, usr_name);
  if(usr_name2 != NULL) strcpy(p->usr_name2, usr_name2);
  switch(select_device){
  case NO_SELECT:
    strcpy(p->dir_name, "");
    break;
  case SELECT_MC:
    p->no_decode = no_decode;
    if(mc_stat0 == 0 && mc_stat1 == 1){
      strcpy(p->dir_name, MC_DB_NAME1);
    }else{
      strcpy(p->dir_name, MC_DB_NAME0);
    }
    break;
  case SELECT_HDD:
    p->no_decode = no_decode2;
    strcpy(p->dir_name, HDD_DB_NAME);
    break;
  case SELECT_HOST:
    p->no_decode = no_decode3;
    strcpy(p->dir_name, HOST_DB_NAME);
    break;
  }
  switch(select_device2){
  case NO_SELECT:
    strcpy(p->dir_name2, "");
    break;
  case SELECT_MC:
    p->no_decode2 = no_decode;
    if(mc_stat0 == 0 && mc_stat1 == 1){
      strcpy(p->dir_name2, MC_DB_NAME1);
    }else{
      strcpy(p->dir_name2, MC_DB_NAME0);
    }
    break;
  case SELECT_HDD:
    p->no_decode2 = no_decode2;
    strcpy(p->dir_name2, HDD_DB_NAME);
    break;
  case SELECT_HOST:
    p->no_decode2 = no_decode3;
    strcpy(p->dir_name2, HOST_DB_NAME);
    break;
  }

  /* COPY_ENTRY の呼び出し */
  sceSifCallRpc(&cd, COPY_ENTRY, SIF_RPCM_NOWAIT, &sdata[0], SSIZE, &sdata[0], SSIZE, 0, 0);
}
