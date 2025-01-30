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
 *                              main.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.16      tetsu      Change for PPPoE
 */
#include "setapp.h"

#define OFFX		(((4096 - SCREEN_WIDTH) / 2) << 4)
#define OFFY		(((4096 - SCREEN_HEIGHT) / 2) << 4)

#define BuffSizeRw	1024

#ifndef NO_HDD
#define STACKSZ		(512 * 16)
static u_char stack[STACKSZ] __attribute__ ((aligned(16)));
#endif /* NO_HDD */

//#define MC_DEBUG		/* メモリーカードの状態遷移を表示 */

unsigned char rwbuff[BuffSizeRw];

int mc_flag;
int mc_port;
int mc_stat0, mc_stat1;

int device_stat;		/* デバイスの状態 */

int no_decode;			/* no_decode フラグ(MC) */
int no_decode2;			/* no_decode フラグ(HDD) */
int no_decode3;			/* no_decode フラグ(HOST) */

char db_name[MAX_LEN];		/* 現在の DB_NAME */

int viewer_flag;		/* 現在の viewer_flag */
int viewer_type;		/* 現在の viewer_type */
int viewer_mode;		/* 現在の viewer_mode */
int viewer_init;		/* 画面の初期化情報 */
int add_flag;			/* Add Flag */
int add_type;			/* Add Type */
int delete_flag;		/* Delete Flag */
int hdd_flag;			/* HDD Flag */

static int r_count;		/* 画面の履歴 */
static int r_flag[MAX_LEN];	/* 画面の履歴(viewer_flag) */
static int r_type[MAX_LEN];	/* 画面の履歴(viewer_type) */
static int r_mode[MAX_LEN];	/* 画面の履歴(viewer_mode) */

int ct;				/* 時間(0 <= ct && cd <= 59) */
int console;			/* 描画環境 */

/* Inet Information */
sceNetCnfEnvData_t env_data __attribute__((aligned(64)));
sceNetCnfEnvData_t last_data __attribute__((aligned(64)));

/* Pad 情報 */
static int port, slot;
static u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));

static void pad_setup(void);
#ifndef NO_HDD
static void PowerOffThread(void *);
static void PowerOffHandler(void *);
static void PreparePowerOff(void);
#endif /* NO_HDD */

int
main(void)
{
  int result, cmd, type, free, format, frame;
#ifndef NO_HDD
  int r;
#endif /* NO_HDD */

#ifdef DEBUG
#ifndef CD_ROM
  static char inetlog_arg[]  = "host0:./inet.log";
  static char ppp_arg[]      = "debug=7f";
#endif /* CD_ROM */
  static char inetctl_arg[]  = "-no_auto\0-verbose";
  static char usbmload_arg[] = "conffile="USBMLOAD_CNF"\0debug=1";
  static char netcnf_arg[] = "icon="ICON_FILE"\0iconsys="ICON_SYS_FILE;
#ifndef NO_HDD
  static char hddarg[] = "-o" "\0" "8" "\0" "-n" "\0" "32";
  static char pfsarg[] = "-m" "\0" "8" "\0" "-o" "\0" "10" "\0" "-n" "\0" "64";
#endif /* NO_HDD */
#else /* DEBUG */
  static char inetctl_arg[]  = "-no_auto";
  static char usbmload_arg[] = "conffile="USBMLOAD_CNF;
  static char netcnf_arg[] = "icon="ICON_FILE"\0iconsys="ICON_SYS_FILE;
#endif /* DEBUG */

  /* 初期化 */
  bzero((void *)&env_data, sizeof(env_data));
  bzero((void *)&last_data, sizeof(last_data));
  init_data(&env_data);
  init_data(&last_data);
  status	      = -1;
  if_list	      = NULL;
  env_list	      = NULL;
  ifc_list	      = NULL;
  dev_list	      = NULL;
  viewer_flag         = 0;
  viewer_init	      = 1;
  add_flag	      = 0;
  delete_flag	      = 0;
  list_update_arg     = 10;
  ct                  = 0;
  mc_stat0            = 0;
  mc_stat1            = 0;
  mc_flag             = 0;
  mc_port             = 0;

#ifdef MC_NO_DECODE
  no_decode           = 1; 				/* 0: mc   で encode, decode をしない */
#else /* MC_NO_DECODE */
  no_decode           = 0; 				/* 0: mc   で encode, decode をする */
#endif /* MC_NO_DECODE */

#ifdef HDD_NO_DECODE
  no_decode2          = 1; 				/* 0: hdd  で encode, decode をしない */
#else /* HDD_NO_DECODE */
  no_decode2          = 0; 				/* 0: hdd  で encode, decode をする */
#endif /* HDD_NO_DECODE */

#ifdef HOST_NO_DECODE
  no_decode3          = 1; 				/* 1: host で encode, decode をしない */
#else /* HOST_NO_DECODE */
  no_decode3          = 0; 				/* 0: host で encode, decode をする */
#endif /* HOST_NO_DECODE */

#ifdef NO_HOST
  device_stat         = 0;
#else /* NO_HOST */
  device_stat         = DEVICE_STAT_HOST_ON;
#endif /* NO_HOST */

  hdd_flag            = 3;
  select_error_device = NO_SELECT;			/* エラーを起こしているデバイス */
  device_error_type   = 0;				/* デバイスエラータイプ */
  mc_error_type       = 0;				/* メモリーカードエラータイプ */

  /* Initialize */
#ifdef REPLACE
  sceSifInitRpc(0);
#ifdef CD_ROM
  sceCdInit(SCECdINIT);
#endif /* CD_ROM */
#ifdef MEM2MB_MODE
  while(sceSifLoadModule(MEM2MB, 0, NULL) < 0){
    printf("Can't load module mem2MB\n");
  }
#endif /* MEM2MB_MODE */
  while (!sceSifRebootIop (IOPRP));			/* IOPリブート・モジュール置き換え */
  while( !sceSifSyncIop() );				/* 終了を待つ */
#endif /* REPLACE */
  sceSifInitRpc(0);
  sceSifLoadFileReset();
  sceFsReset();
#ifdef CD_ROM
  sceCdInit(SCECdINIT);
  sceCdMmode(SCECdCD);					/* Media: CD-ROM */
#endif /* CD_ROM */

  /* load sio2man.irx */
  while(sceSifLoadModule(SIO2MAN, 0, NULL) < 0){
    printf("Can't load module sio2man\n");
  }

  /* load padman.irx */
  while(sceSifLoadModule(PADMAN, 0, NULL) < 0){
    printf("Can't load module padman\n");
  }

  /* load mcman.irx */
  while(sceSifLoadModule(MCMAN, 0, NULL) < 0){
    printf("Can't load module mcman\n");
  }

  /* load mcserv.irx */
  while(sceSifLoadModule(MCSERV, 0, NULL) < 0){
    printf("Can't load module mcserv\n");
  }

  /* load netcnf.irx */
  while(sceSifLoadModule(NETCNF, sizeof(netcnf_arg), netcnf_arg) < 0){
    printf("Can't load module netcnf\n");
  }

  /* load inet.irx */
  while(sceSifLoadModule(INET, 0, NULL) < 0){
    printf("Can't load module inet\n");
  }

#ifdef DEBUG
#ifndef CD_ROM
  /* load inetlog.irx */
  while(sceSifLoadModule(INETLOG, sizeof(inetlog_arg), inetlog_arg) < 0){
    printf("Can't load module inetlog\n");
  }
#endif /* CD_ROM */
#endif /* DEBUG */

  /* load inetctl.irx */
  while(sceSifLoadModule(INETCTL, sizeof(inetctl_arg), inetctl_arg) < 0){
    printf("Can't load module inetctl\n");
  }

#ifdef DEBUG
#ifndef CD_ROM
  /* load ppp.irx */
  while(sceSifLoadModule(PPP, sizeof(ppp_arg), ppp_arg) < 0){
    printf("Can't load module ppp\n");
  }
#else /* CD_ROM */
  /* load ppp.irx */
  while(sceSifLoadModule(PPP, 0, NULL) < 0){
    printf("Can't load module ppp\n");
  }
#endif /* CD_ROM */
#else /* DEBUG */
  /* load ppp.irx */
  while(sceSifLoadModule(PPP, 0, NULL) < 0){
    printf("Can't load module ppp\n");
  }
#endif /* DEBUG */

  /* load pppoe.irx */
  while(sceSifLoadModule(PPPOE, 0, NULL) < 0){
    printf("Can't load module pppoe\n");
  }

  /* load usbd.irx */
  while(sceSifLoadModule(USBD, 0, NULL) < 0){
    printf("Can't load module usbd\n");
  }

  /* load usbmload.irx */
  while(sceSifLoadModule(USBMLOAD, sizeof(usbmload_arg), usbmload_arg) < 0){
    printf("Can't load module usbdmload\n");
  }

  /* load ifnetcnf.irx */
  while(sceSifLoadModule(IFNETCNF, 0, NULL) < 0){
    printf("Can't load module ifnetcnf\n");
  }

#ifndef NO_HDD
  /* load dev9.irx */
  if((r = sceSifLoadModule(DEV9, 0, NULL)) < 0){
    printf("Can't load module dev9(r = %d)\n", r);
    hdd_flag = 3;
  }else{
    hdd_flag = 0;
    PreparePowerOff();
  }

  /* load atad.irx */
  if((r = sceSifLoadModule(ATAD, 0, NULL)) < 0){
    printf("Can't load module atad(r = %d)\n", r);
    hdd_flag = 3;
  }else{
    hdd_flag = 0;
  }

  /* load hdd.irx */
#ifdef DEBUG
  if((r = sceSifLoadModule(HDD, 10, hddarg)) < 0){
    printf("Can't load module hdd(r = %d)\n", r);
    hdd_flag = 3;
  }else{
    hdd_flag = 0;
  }
#else /* DEBUG */
  if((r = sceSifLoadModule(HDD, 0, NULL)) < 0){
    printf("Can't load module hdd(r = %d)\n", r);
    hdd_flag = 3;
  }else{
    hdd_flag = 0;
  }
#endif /* DEBUG */

  /* load pfs.irx */
#ifdef DEBUG
  if((r = sceSifLoadModule(PFS, 18, pfsarg)) < 0){
    printf("Can't load module pfs(r = %d)\n", r);
    hdd_flag = 3;
  }else{
    hdd_flag = 0;
  }
#else /* DEBUG */
  if((r = sceSifLoadModule(PFS, 0, NULL)) < 0){
    printf("Can't load module pfs(r = %d)\n", r);
    hdd_flag = 3;
  }else{
    hdd_flag = 0;
  }
#endif /* DEBUG */

  /* load smap.irx */
  if((r = sceSifLoadModule(SMAP, 0, NULL)) < 0){
    printf("Can't load module smap(r = %d)\n", r);
    hdd_flag = 3;
  }else{
    hdd_flag = 0;
  }

  if(hdd_flag == 0){
    /* check format */
    hdd_flag = sceDevctl("hdd0:", HDIOC_STATUS, NULL, 0, NULL, 0);

    /* Mount pfs. */
    if(hdd_flag == 0){
      if((r = sceMount("pfs0:", "hdd0:__sysconf", SCE_MT_RDWR, NULL, 0)) < 0){
	printf("Can't mount hdd0:__sysconf.(r = %d)\n", r);
	hdd_flag = 1;
      }
    }
  }
#endif /* NO_HDD */

  /* HDD デバイスの使用判定 */
  if(hdd_flag == 0) device_stat |= DEVICE_STAT_HDD_ON;

  /* Rpc 初期化 */
  Rpc_setup();

  /* GS 初期化 */
  Gs_Initialize();

  /* フォントシステム初期化 */
  sceDevConsInit();
  console = sceDevConsOpen(OFFX + (16 << 4), OFFY + (24 << 4), 75, 30);
  sceDevConsClear(console);

  /* Pad 初期化 */
  pad_setup();

  frame = 0;
  sceGsSyncPath(0,0);

  /* メモリーカード環境の初期化 */
  sceMcInit();
  sceMcGetInfo(mc_port, 0, &type, &free, &format);
  sceMcSync(0, &cmd, &result);

  while(1){
    sceGsSwapDBuff(&db, frame);
    ++frame;

    /* ファイルの中身を表示する */
    switch(viewer_flag){
    case 0:
      connect_viewer();
      break;
    case 1:
      menu_viewer();
      break;
    case 2:
      list_viewer();
      break;
    case 3:
      dev_viewer();
      break;
    case 4:
      ifc_viewer();
      break;
    case 5:
      dev_more_viewer();
      break;
    case 6:
      ifc_more_viewer();
      break;
    }

    /* interrace half pixcel adjust */
    if(frame & 0x01){
      sceGsSetHalfOffset(&db.draw1, 2048, 2048, sceGsSyncV(0) ^ 0x01);
    }else{
      sceGsSetHalfOffset(&db.draw0, 2048, 2048, sceGsSyncV(0) ^ 0x01);
    }

    /* timer check */
    ct++;

    /******************************************************************/
    /* メモリーカードの状態チェック                                   */
    /*     mc_flag :  1 … メモリーカードチェック不可。               */
    /*                0 … メモリーカードチェック可。                 */
    /*     mc_port :  1 … メモリーカード1 チェック中。               */
    /*                0 … メモリーカード0 チェック中。               */
    /*     mc_stat0:  1 … メモリーカード0 が存在する。               */
    /*                0 … メモリーカード0 が存在しない。             */
    /*     mc_stat1:  1 … メモリーカード1 が存在する。               */
    /*                0 … メモリーカード1 が存在しない。             */
    /******************************************************************/
    if(viewer_init == 1 || add_flag == 1 || delete_flag == 1){
      sceMcSync(0, &cmd, &result);
      mc_flag = 1;
    }
    if(0 < mc_flag && viewer_init == 0 && add_flag == 0 && delete_flag == 0){
      mc_flag = 0;
    }
    if(mc_flag <= 0){
      /* 1/2 秒毎に mc0, mc1 を交互に検出 */
      if(ct % 30 == 0){
	if(Rpc_Check() == 0 && viewer_init == 0 && add_flag == 0 && delete_flag == 0){
	  if(sceMcSync(1, &cmd, &result)){
	    switch(result){
	    case  0: /* 既検出 */
	      mc_flag = 0;
	      switch(mc_port){
	      case 0:
		if(mc_stat0 == 0){
		  if(select_error_device == SELECT_MC &&
		     device_error_type != sceNETCNF_MAGIC_ERROR) device_stat |= DEVICE_STAT_MC_ON;
		  status       = IEV_Conf;
		  env_select   = 0;
		  ifc_select   = 0;
		  dev_select   = 0;
		}
		mc_stat0 = 1;
		break;
	      case 1:
		if(mc_stat0 == 0 && mc_stat1 == 0){
		  if(select_error_device == SELECT_MC &&
		     device_error_type != sceNETCNF_MAGIC_ERROR) device_stat |= DEVICE_STAT_MC_ON;
		  status       = IEV_Conf;
		  env_select   = 0;
		  ifc_select   = 0;
		  dev_select   = 0;
		}
		mc_stat1 = 1;
		break;
	      }
	      break;
	    case -1: /* フォーマット済メモリーカード検出 */
	      mc_flag = 0;
	      switch(mc_port){
	      case 0:
		if(mc_stat0 == 0){
		  device_stat |= DEVICE_STAT_MC_ON;
		  if(select_error_device == SELECT_MC &&
		     device_error_type == sceNETCNF_MAGIC_ERROR) device_stat &= DEVICE_STAT_MC_OFF;
		  status       = IEV_Conf;
		  env_select   = 0;
		  ifc_select   = 0;
		  dev_select   = 0;
		}
		mc_stat0 = 1;
		break;
	      case 1:
		if(mc_stat0 == 0 && mc_stat1 == 0){
		  device_stat |= DEVICE_STAT_MC_ON;
		  if(select_error_device == SELECT_MC &&
		     device_error_type == sceNETCNF_MAGIC_ERROR) device_stat &= DEVICE_STAT_MC_OFF;
		  status       = IEV_Conf;
		  env_select   = 0;
		  ifc_select   = 0;
		  dev_select   = 0;
		}
		mc_stat1 = 1;
		break;
	      }
	      break;
	    case -2: /* 未フォーマットメモリーカード検出 */
	      mc_flag = 0;
	      switch(mc_port){
	      case 0:
		if(mc_stat0 == 0){
		  device_stat |= DEVICE_STAT_MC_ON;
		  if(select_error_device == SELECT_MC &&
		     device_error_type == sceNETCNF_MAGIC_ERROR) device_stat &= DEVICE_STAT_MC_OFF;
		  status       = IEV_NoConf;
		  env_select   = 0;
		  ifc_select   = 0;
		  dev_select   = 0;
		}
		mc_stat0 = 1;
		break;
	      case 1:
		if(mc_stat0 == 0 && mc_stat1 == 0){
		  device_stat |= DEVICE_STAT_MC_ON;
		  if(select_error_device == SELECT_MC &&
		     device_error_type == sceNETCNF_MAGIC_ERROR) device_stat &= DEVICE_STAT_MC_OFF;
		  status       = IEV_NoConf;
		  env_select   = 0;
		  ifc_select   = 0;
		  dev_select   = 0;
		}
		mc_stat1 = 1;
		break;
	      }
	      break;
	    }
	    if(result < -10){
	      /* メモリーカード未検出 */
	      mc_flag = 0;
	      switch(mc_port){
	      case 0:
		if(mc_stat0 == 1){
		  switch(viewer_flag){
		  case 0: /* connect_viewer */
		  case 2: /* list_viewer */
		    if(select_error_device == SELECT_MC) device_error_type = 0;
		    break;
		  }
		  device_stat  &= DEVICE_STAT_MC_OFF;
		  status        = IEV_NoConf;
		  env_select    = 0;
		  ifc_select    = 0;
		  dev_select    = 0;
		}
		mc_stat0 = 0;
		break;
	      case 1:
		if(mc_stat0 == 0 && mc_stat1 == 1){
		  switch(viewer_flag){
		  case 0: /* connect_viewer */
		  case 2: /* list_viewer */
		    if(select_error_device == SELECT_MC) device_error_type = 0;
		    break;
		  }
		  device_stat  &= DEVICE_STAT_MC_OFF;
		  status        = IEV_NoConf;
		  env_select    = 0;
		  ifc_select    = 0;
		  dev_select    = 0;
		}
		mc_stat1 = 0;
		break;
	      }
	    }
	    type = 0;
	    free = 0;
	    format = 0;
	    mc_port = mc_port ? 0 : 1;
	    sceMcGetInfo(mc_port, 0, &type, &free, &format);
	  }
	}
      }
    }
#ifdef MC_DEBUG
    if(ct % 30 == 0) printf("mc_flag:%d, mc_port:%d, mc_stat0:%d, mc_stat1:%d, status:%d\n"
			    , mc_flag, mc_port, mc_stat0, mc_stat1, status);
#endif /* MC_DEBUG */

    /* timer reset */
    if(ct == 60) ct = 0;

    /* status check */
    if(Rpc_Check_Stat() == 0 && Rpc_Get_Result_Stat() == -1){
      Rpc_WaitEvent(-1);
    }else{
      if(Rpc_Get_Result_Stat() != -1){
	status = Rpc_Get_Result_Stat();
	Rpc_Init_Stat();
      }
    }

    FlushCache(0);
    sceGsSyncPath(0,0);
  }

  return (0);
}

/**************************/
/* pad の情報を初期化する */
/**************************/
static void
pad_setup(void)
{
  int ret;
  int term_id;
  int rterm_id;

  /* コントローラライブラリ初期化 */
  scePadInit(0);
  port = 0;
  slot = 0;
  term_id = 0;
  rterm_id = 0;

  scePadSetWarningLevel(0);

  /* ポートオープン */
  ret = scePadPortOpen( port, slot, pad_dma_buf );
  if(ret == 0){
    printf("ERROR: scePadPortOpen\n");
  }
}

/************************/
/* pad の情報を取得する */
/************************/
u_short
pad_read(void)
{
  unsigned char rdata[32];
  unsigned short paddata, tpad;
  static unsigned short rpad = 0;
  static unsigned short rept = 0;

  if(scePadRead(port, slot, rdata) == 0){
    return (0);
  }

  if(rdata[0] == 0){
    paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);

    if(paddata == rpad){
      rept++;
    }else{
      rept = 0;
    }

    tpad = paddata & ~rpad;
    rpad = paddata;

    if(rept >= 20){
      tpad = paddata;
      rept -= 7;
    }
    return (tpad); 
  }else {
    return (0);
  }
}

/**************************/
/* 戻る画面情報を設定する */
/**************************/
void
set_return(int rf, int rt, int rm, int f, int t, int m)
{
  int i;

  if(m == NO_RETURN){
    /* 履歴を初期化 */
    r_count = 0;
  }else{
    /* 履歴は最大 MAX_LEN まで。
       それを超えたら古いものから順に削除される。 */
    if(r_count == MAX_LEN){
      for(i = 0; i < MAX_LEN - 1; i++){
	r_flag[i] = r_flag[i + 1];
      }
      r_count--;
    }
    r_flag[r_count] = rf;
    r_type[r_count] = rt;
    r_mode[r_count] = rm;
    r_count++;
    viewer_flag = f;
    viewer_type = t;
    viewer_mode = m;
  }
}

/**************************/
/* 戻る画面情報を取得する */
/**************************/
int
get_return(void)
{
  int flag, r;

  if(0 < r_count){
    r_count -= 1;
    flag     = 1;
  }else{
    flag     = 0;
  }

  if(r_mode[r_count] == NO_RETURN){
    r                 = 0;
    if(flag) r_count += 1;
  }else{
    r            = (viewer_flag == r_flag[r_count]) ? 2 : 1;
    viewer_flag  = r_flag[r_count];
    viewer_type  = r_type[r_count];
    viewer_mode  = r_mode[r_count];
  }

  return (r);
}

/******************************/
/* sceNetCnfEnvData の初期化 */
/******************************/
void
init_data(sceNetCnfEnvData_t *data)
{
  strcpy(data->attach_ifc, "");
  strcpy(data->attach_dev, "");
  strcpy(data->address, "");
  strcpy(data->netmask, "");
  strcpy(data->gateway, "");
  strcpy(data->dns1_address, "");
  strcpy(data->dns2_address, "");
  strcpy(data->phone_numbers1, "");
  strcpy(data->phone_numbers2, "");
  strcpy(data->phone_numbers3, "");
  strcpy(data->auth_name, "");
  strcpy(data->auth_key, "");
  strcpy(data->vendor, "");
  strcpy(data->product, "");
  strcpy(data->chat_additional, "");
  strcpy(data->outside_set, "");
  strcpy(data->dhcp_host_name, "");
  data->dialing_type = 0;
  data->idle_timeout = 10;
  data->type = 0;
  data->phy_config = 0;
  data->dhcp = 0;
  data->pppoe = 0;
}

/****************************/
/* 有効なデータをセーブする */
/****************************/
void
save_data(void)
{
  if(env_data.attach_ifc[0]      != '\0') strcpy(last_data.attach_ifc     , env_data.attach_ifc);
  if(env_data.attach_dev[0]      != '\0') strcpy(last_data.attach_dev     , env_data.attach_dev);
  if(env_data.address[0]         != '\0') strcpy(last_data.address        , env_data.address);
  if(env_data.netmask[0]         != '\0') strcpy(last_data.netmask        , env_data.netmask);
  if(env_data.gateway[0]         != '\0') strcpy(last_data.gateway        , env_data.gateway);
  if(env_data.dns1_address[0]    != '\0') strcpy(last_data.dns1_address   , env_data.dns1_address);
  if(env_data.dns2_address[0]    != '\0') strcpy(last_data.dns2_address   , env_data.dns2_address);
  if(env_data.phone_numbers1[0]  != '\0') strcpy(last_data.phone_numbers1 , env_data.phone_numbers1);
  if(env_data.phone_numbers2[0]  != '\0') strcpy(last_data.phone_numbers2 , env_data.phone_numbers2);
  if(env_data.phone_numbers3[0]  != '\0') strcpy(last_data.phone_numbers3 , env_data.phone_numbers3);
  if(env_data.auth_name[0]       != '\0') strcpy(last_data.auth_name      , env_data.auth_name);
  if(env_data.auth_key[0]        != '\0') strcpy(last_data.auth_key       , env_data.auth_key);
  if(env_data.vendor[0]          != '\0') strcpy(last_data.vendor         , env_data.vendor);
  if(env_data.product[0]         != '\0') strcpy(last_data.product        , env_data.product);
  if(env_data.chat_additional[0] != '\0') strcpy(last_data.chat_additional, env_data.chat_additional);
  if(env_data.outside_set[0]     != '\0') strcpy(last_data.outside_set    , env_data.outside_set);
  if(env_data.dhcp_host_name[0]  != '\0') strcpy(last_data.dhcp_host_name , env_data.dhcp_host_name);

  if(env_data.dialing_type       != 0) last_data.dialing_type = env_data.dialing_type;
  if(env_data.idle_timeout       != 0) last_data.idle_timeout = env_data.idle_timeout;
  if(env_data.type               != 0) last_data.type         = env_data.type;
  if(env_data.phy_config         != 0) last_data.phy_config   = env_data.phy_config;
  if(env_data.dhcp               != 0) last_data.dhcp         = env_data.dhcp;
  if(env_data.pppoe              != 0) last_data.pppoe        = env_data.pppoe;
}

#ifdef DEBUG
/**********************/
/* データをダンプする */
/**********************/
void
dump_data(sceNetCnfEnvData_t *data)
{
  printf("attach_ifc      : %s\n", data->attach_ifc);
  printf("attach_dev      : %s\n", data->attach_dev);
  printf("address         : %s\n", data->address);
  printf("netmask         : %s\n", data->netmask);
  printf("gateway         : %s\n", data->gateway);
  printf("dns1_address    : %s\n", data->dns1_address);
  printf("dns2_address    : %s\n", data->dns2_address);
  printf("phone_numbers1  : %s\n", data->phone_numbers1);
  printf("phone_numbers2  : %s\n", data->phone_numbers2);
  printf("phone_numbers3  : %s\n", data->phone_numbers3);
  printf("auth_name       : %s\n", data->auth_name);
  printf("auth_key        : %s\n", data->auth_key);
  printf("vendor          : %s\n", data->vendor);
  printf("product         : %s\n", data->product);
  printf("chat_additional : %s\n", data->chat_additional);
  printf("outside_set     : %s\n", data->outside_set);
  printf("dhcp_host_name  : %s\n", data->dhcp_host_name);
  printf("dialing_type    : %d\n", data->dialing_type);
  printf("idle_timeout    : %d\n", data->idle_timeout);
  printf("type            : %d\n", data->type);
  printf("phy_config      : %d\n", data->phy_config);
  printf("dhcp            : %d\n", data->dhcp);
  printf("pppoe           : %d\n", data->pppoe);
}
#endif /* DEBUG */

#ifndef NO_HDD
static void
PowerOffThread(void *arg)
{
  int sid = (int)arg;
  int stat = 0;
  WaitSema(sid);
  printf("power off request has come.\n");
  /* close all files */
  sceDevctl("pfs:", PDIOC_CLOSEALL, NULL, 0, NULL, 0);
  /* dev9 power off, need to power off PS2 */
  while(sceDevctl("hdd:", HDIOC_DEV9OFF, NULL, 0, NULL, 0)<0);
  /* PS2 power off */
  while(!sceCdPowerOff(&stat)||stat);
}

static void
PowerOffHandler(void *arg)
{
  int sid = (int)arg;
  iSignalSema(sid);
}

static void
PreparePowerOff(void)
{
  struct ThreadParam tparam;
  struct SemaParam   sparam;
  int tid;
  int sid;

  sparam.initCount = 0;
  sparam.maxCount  = 1;
  sparam.option    = 0;
  sid = CreateSema(&sparam);

  ChangeThreadPriority(GetThreadId(), 2);
  tparam.stackSize = STACKSZ;
  tparam.gpReg = &_gp;
  tparam.entry = PowerOffThread;
  tparam.stack = (void *)stack;
  tparam.initPriority = 1;
  tid = CreateThread(&tparam);
  StartThread(tid, (void *)sid);

  sceCdPOffCallback(PowerOffHandler, (void *)sid);
}
#endif /* NO_HDD */
