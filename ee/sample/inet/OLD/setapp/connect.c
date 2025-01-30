/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *	Inet Setting Application Sample
 *
 *                          Version 1.5
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Inc.
 *	                 All Rights Reserved.
 *
 *                          connect.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.16      tetsu      Change for PPPoE
 *        1.5           2001.10.02      tetsu      Add redial message
 */
#include "setapp.h"

/**********************************/
/* メモリーカード設定ファイル情報 */
/**********************************/
#define MAX_MC_COMBINATION	6

static char *mc_combination_name[] = {"Combination1", "Combination2", "Combination3", "Combination4", "Combination5", "Combination6"};
static int mc_combination_flag[MAX_MC_COMBINATION];
static int mc_combination_stat[MAX_MC_COMBINATION];
static int mc_combination_menu;

/**************************/
/* HDD 設定ファイル情報 */
/**************************/
#define MAX_HDD_COMBINATION	10

static char *hdd_combination_name[] = {"Combination1", "Combination2", "Combination3", "Combination4", "Combination5",
				       "Combination6", "Combination7", "Combination8", "Combination9", "Combination10"};
static int hdd_combination_flag[MAX_HDD_COMBINATION];
static int hdd_combination_stat[MAX_HDD_COMBINATION];
static int hdd_combination_menu;

/**************************/
/* ホスト設定ファイル情報 */
/**************************/
#define MAX_HOST_COMBINATION	10

static char *host_combination_name[] = {"Combination1", "Combination2", "Combination3", "Combination4", "Combination5",
					"Combination6", "Combination7", "Combination8", "Combination9", "Combination10"};
static int host_combination_flag[MAX_HOST_COMBINATION];
static int host_combination_stat[MAX_HOST_COMBINATION];
static int host_combination_menu;

/************************/
/* 共通設定ファイル情報 */
/************************/
static int set_combination_device = NO_SELECT;	/* 組み合わせのデバイスを直接指定するフラグ */
static int select_combination_device;		/* 現在選択されているデバイス */
static char **combination_name;			/* 現在のデバイス上における組み合わせ名 */
static int set_combination_menu = -1;		/* 組み合わせの番号を直接指定するフラグ */
static int *combination_menu;			/* 現在選択している組み合わせの番号 */
static int combination_menu_max;		/* 現在のデバイス上における最大組み合わせ数 */
static int *combination_flag;			/* 現在のデバイス上における組み合わせの使用可・不可フラグ */
static int *combination_stat;			/* 現在のデバイス上における組み合わせの登録済・未登録フラグ */
static int combination_num;			/* 組み合わせ(Combination)を何回表示したかを示す数 */

sceNetCnfList2_t *env_list;			/* Env List */
sceNetCnfList2_t *ifc_list;			/* Ifc List */
sceNetCnfList2_t *dev_list;			/* Dev List */

int env_menu_max;				/* Env Menu の最大数 */
int ifc_menu;					/* 現在選択している Ifc Menu の menu 番号 */
int ifc_menu_max;				/* Ifc Menu の最大数 */
int dev_menu;					/* 現在選択している Dev Menu の menu 番号 */
int dev_menu_max;				/* Dev Menu の最大数 */

int env_select;					/* Env Select Flag */
int ifc_select;					/* Ifc Select Flag */
int dev_select;					/* Dev Select Flag */

char env_usr_name[MAX_LEN];			/* Env ファイル名(user name) */
char ifc_usr_name[MAX_LEN];			/* Ifc ファイル名(user name) */
char dev_usr_name[MAX_LEN];			/* Dev ファイル名(user name) */
char ifc_usr_name_disp[MAX_LEN];		/* Ifc ファイル名(user name)表示用 */
char dev_usr_name_disp[MAX_LEN];		/* Dev ファイル名(user name)表示用 */
char ifc_usr_name_bak[MAX_LEN];			/* Ifc ファイル名(user name)バックアップ */
char dev_usr_name_bak[MAX_LEN];			/* Dev ファイル名(user name)バックアップ */

int list_update_arg;				/* list_update() 引数 */

/**********************/
/* その他、接続情報等 */
/**********************/
int select_error_device;			/* エラーを起こしているデバイス */
int device_error_type;				/* デバイスエラータイプ */
int mc_error_type;				/* メモリーカードエラータイプ */
int status;					/* 状態表示 */
static int cy, cy_min, cy_max;			/* カーソル情報 */
static int connect_phase = 0;			/* Connect Phase(接続処理番号) */
static int connect_stat  = 0;			/* Connect Status(0<:エラー, 0:未接続, 1:接続処理中, 2:接続) */
static int redial_stat   = 0;			/* Redial Status(0: リダイアル中でない, 1: リダイアル中である) */
static int connect_type  = 0;			/* Connect Type(0:all memory, 1:dev memory & ifc file, 2:ifc memory & dev file
						   3: ifc file & dev file, 4: combination file) */
/********************/
/* プロトタイプ宣言 */
/********************/
static int connect_pad(void);
static void initialize(void);
static void get_env_usr_name(int);
static void get_ifc_usr_name(int);
static void get_dev_usr_name(int);
static int get_another_name(char *, char *, sceNetCnfList2_t *, int, int);
static void add_env(void);
static void delete_env(void);
static void delete_all(void);
static void connect(void);
static void init_combination_device(int);

/************************************/
/* ネットワーク接続画面コントロール */
/************************************/
static int
connect_pad(void)
{
  u_short tpad;

  if((tpad = pad_read()) == 0) return (0);

  if(tpad && 0 < hdd_flag){
    hdd_flag = 0;
    return (0);
  }

  if(tpad && add_flag < 0){
    mc_error_type          = 0;
    add_flag               = 0;
    env_select             = 0;
    dev_select             = 0;
    ifc_select             = 0;
    viewer_init            = 1;
    list_update_arg        = 10;
    set_combination_device = select_combination_device;
    set_combination_menu   = (*combination_menu);
    return (0);
  }

  if(tpad && delete_flag < 0 && device_error_type == 0){
    delete_flag            = 0;
    env_select             = 0;
    dev_select             = 0;
    ifc_select             = 0;
    viewer_init            = 1;
    list_update_arg        = 10;
    set_combination_device = select_combination_device;
    set_combination_menu   = (*combination_menu);
    return (0);
  }

  if(tpad && connect_stat < 0){
    connect_stat = 0;
    return (0);
  }

  if(tpad & SCE_PADLup){
    if(device_error_type == 0){
      cy--;
      cy = LIMIT(cy, cy_min, cy_max);
    }
  }
  if(tpad & SCE_PADLdown){
    if(device_error_type == 0){
      cy++;
      cy = LIMIT(cy, cy_min, cy_max);
    }
  }
  if(tpad & SCE_PADLleft){
  }
  if(tpad & SCE_PADLright){
  }
  if(tpad & SCE_PADRup){
    if(device_error_type == 0){
      switch(cy){
      case 3:
	/* (条件１) dev, ifc 共にリストが存在しなければならない */
	if(dev_list != NULL && ifc_list != NULL){
	  /*(条件２)  dev, ifc 共にファイルでなければならない */
	  if((dev_select == 1 || dev_select == 2) && (ifc_select == 1 || ifc_select == 2)){
	    /* (条件３) dev, ifc 共に同一デバイス上でなければならない */
	    if(select_combination_device == (dev_list + dev_menu)->select_device &&
	       select_combination_device == (ifc_list + ifc_menu)->select_device){
#ifdef HOST_READ_ONLY
	      /* (条件４) ホストであってはならない */
	      if(select_combination_device != SELECT_HOST) add_flag = 1;
#else /* HOST_READ_ONLY */
	      add_flag = 1;
#endif /* HOST_READ_ONLY */
	    }
	  }
	}
	break;
      }
    }
  }
  if(tpad & SCE_PADRdown){
    if(device_error_type){
      switch(delete_flag){
      case -3:
	/* 本当に消していいですか？ */
	delete_flag = -4;
	break;

      case 0:
	/* 削除しますか？ */
	delete_flag = -4;
	break;
      }
    }
  }
  if(tpad & SCE_PADRleft){
    if(device_error_type == 0){
      switch(cy){
      case 3:
	/* (条件１) 組み合わせが存在しなければならない */
	if((*(combination_stat + (*combination_menu))) == 1){
#ifdef HOST_READ_ONLY
	  /* (条件２) ホストであってはならない */
	  if(select_combination_device != SELECT_HOST) delete_flag = 1;
#else /* HOST_READ_ONLY */
	  delete_flag = 1;
#endif /* HOST_READ_ONLY */
	}
	break;
      }
    }
  }
  if(tpad & SCE_PADRright){
    if(device_error_type == 0){
      switch(cy){
      case 3:
	(*combination_menu)++;
	combination_num++;
	if(combination_num == combination_menu_max){
	  combination_num = 0;

	  /* 組み合わせを一巡したら次のデバイスを検索する */
	  select_combination_device = change_device(select_combination_device);

	  /* 組み合わせのデバイスを初期化 */
	  init_combination_device(select_combination_device);

	  /* 組み合わせ数の最大値をチェック */
	  if((*combination_menu) == combination_menu_max) (*combination_menu) = 0;

	  /* 組み合わせ(dev_usr_name & ifc_usr_name)を取得するフラグを立てる */
	  viewer_init     = 10;
	}else{
	  /* 組み合わせ数の最大値をチェック */
	  if((*combination_menu) == combination_menu_max) (*combination_menu) = 0;

	  /* 組み合わせ(dev_usr_name & ifc_usr_name)を取得するフラグを立てる */
	  viewer_init = 10;
	}
	break;

      case 4:
	/* (条件１) dev_list が存在しなければならない */
	if(dev_list){
	  dev_menu++;
	  if(dev_menu == dev_menu_max){
	    /* 一巡したら (no select) 状態にする */
	    strcpy(dev_usr_name, "(no select)");
	    dev_menu   = -1;
	    dev_select = 0;
	  }else{
	    /* 接続機器設定が単独で選択されている状態 */
	    strcpy(dev_usr_name, (dev_list + dev_menu)->usr_name);
	    dev_select = 2;
	  }
	}
	break;

      case 5:
	/* (条件１) ifc_list が存在しなければならない */
	if(ifc_list){
	  ifc_menu++;
	  if(ifc_menu == ifc_menu_max){
	    /* 一巡したら (no select) 状態にする */
	    strcpy(ifc_usr_name, "(no select)");
	    ifc_menu   = -1;
	    ifc_select = 0;
	  }else{
	    /* 接続プロバイダ設定が単独で選択されている状態 */
	    strcpy(ifc_usr_name, (ifc_list + ifc_menu)->usr_name);
	    ifc_select = 2;
	  }
	}
	break;
      }
    }else{
      switch(delete_flag){
      case -4:
	/* そのデバイスは見ることができません */
	delete_flag = 0;
	device_error_type = 0;
	env_select = 0;
	dev_select = 0;
	ifc_select = 0;
	viewer_init = 1;
	list_update_arg = 10;
	break;

      case -3:
	/* 本当に消していいですか？ */
	delete_flag = 1;
	break;

      case -2:
	/* 削除完了 */
	delete_flag = 0;
	switch(select_error_device){
	case SELECT_MC:
	  device_stat |= DEVICE_STAT_MC_ON;
	  break;
	case SELECT_HDD:
	  device_stat |= DEVICE_STAT_HDD_ON;
	  break;
	}
	device_error_type = 0;
	env_select = 0;
	dev_select = 0;
	ifc_select = 0;
	viewer_init = 1;
	list_update_arg = 10;
	break;

      case -1:
	/* 削除に失敗しました */
	delete_flag = -4;
	break;

      case 0:
	if(select_error_device == SELECT_HOST){
	  /* そのデバイスは見ることができません */
	  delete_flag = 0;
	  device_error_type = 0;
	  env_select = 0;
	  dev_select = 0;
	  ifc_select = 0;
	  viewer_init = 1;
	  list_update_arg = 10;
	}else{
	  /* 削除しますか？ */
	  delete_flag = -3;
	}
	break;
      }
    }
  }
  if(tpad & SCE_PADL1){
  }
  if(tpad & SCE_PADL2){
  }
  if(tpad & SCE_PADR1){
  }
  if(tpad & SCE_PADR2){
  }
  if(tpad & SCE_PADstart){
    if(device_error_type == 0){
      if(0 < connect_stat){
#ifdef DEBUG
	printf("[connect_pad] Connection End   - (type:%d, phase:%d, stat:%d -> ", connect_type, connect_phase, connect_stat);
#endif /* DEBUG */
	connect_phase = 6;
	connect_stat  = 1;
#ifdef DEBUG
	printf("%d ...)\n", connect_stat);
#endif /* DEBUG */
      }else{
	if(connect_stat == 0){
	  if(env_select && ifc_select == 1 && dev_select == 1){
	    if((*(combination_flag + (*combination_menu)))){
	      connect_type  = 4;
	      connect_phase = 3;
	      connect_stat  = 1;
	    }
	  }else{
	    if(ifc_select == 2 && dev_select == 2){
	      if((dev_list + dev_menu)->flag){
		connect_type  = 3;
		connect_phase = 3;
		connect_stat  = 1;
	      }
	    }else if(ifc_select == 3 && dev_select == 2){
	      if((dev_list + dev_menu)->flag){
		connect_type  = 2;
		connect_phase = 1;
		connect_stat  = 1;
	      }
	    }else if(ifc_select == 2 && dev_select == 3){
	      connect_type  = 1;
	      connect_phase = 1;
	      connect_stat  = 1;
	    }else if(ifc_select == 3 && dev_select == 3){
	      connect_type  = 0;
	      connect_phase = 1;
	      connect_stat  = 1;
	    }else if(ifc_select == 2 && dev_select == 1){
	      if((dev_list + dev_menu)->flag){
		connect_type  = 3;
		connect_phase = 3;
		connect_stat  = 1;
	      }
	    }else if(ifc_select == 1 && dev_select == 2){
	      if((dev_list + dev_menu)->flag){
		connect_type  = 3;
		connect_phase = 3;
		connect_stat  = 1;
	      }
	    }else if(ifc_select == 3 && dev_select == 1){
	      if((dev_list + dev_menu)->flag){
		connect_type  = 1;
		connect_phase = 1;
		connect_stat  = 1;
	      }
	    }else if(ifc_select == 1 && dev_select == 3){
	      connect_type  = 1;
	      connect_phase = 1;
	      connect_stat  = 1;
	    }
	  }
#ifdef DEBUG
	  if(connect_stat){
	    printf("[connect_pad] Connection Start - (type:%d, phase:%d, stat:0 -> %d ...)\n", connect_type, connect_phase, connect_stat);
	  }
#endif /* DEBUG */
	}
      }
    }
  }
  if(tpad & SCE_PADselect){
    if(device_error_type == 0){
      set_return(viewer_flag, -1, DEFAULT, 1, 0, DEFAULT);
      dev_select  = 0;
      ifc_select  = 0;
      viewer_init = 1;
    }
  }

  return (1);
}

/************************/
/* ネットワーク接続画面 */
/************************/
void
connect_viewer(void)
{
  if(Rpc_Check() == 0 && viewer_init) initialize();	/* 画面初期化 */

  /* 状態チェック */
  switch(status){
  case IEV_Conf:
  case IEV_NoConf:
    if(connect_stat == 0){
      viewer_init     = 1;
      list_update_arg = 10;
    }
    break;
  case IEV_Attach:
  case IEV_Detach:
    if(connect_stat == 0){
      if(viewer_init == 0){
	/* 現在表示している内容は変えない */
	set_combination_device = select_combination_device;
	set_combination_menu   = (*combination_menu);
      }
      viewer_init     = 1;
      list_update_arg = 10;
    }
    break;
  case IEV_Start:
    connect_stat = 2;
    redial_stat  = 0;
    break;
  case IEV_Stop:
    connect_stat = 0;
    redial_stat  = 0;
    break;
  case IEV_Error:
    if(connect_stat){
      connect_stat = -1;
      redial_stat  = 0;
    }
    break;
  case IEV_Redial:
    redial_stat = 1;
    break;
  }
  status = -1;

  if(Rpc_Check() == 0 && 0 < add_flag) add_env();					/* Env ファイル作成 */
  if(Rpc_Check() == 0 && 0 < delete_flag && device_error_type == 0) delete_env();	/* Env ファイル削除 */
  if(Rpc_Check() == 0 && 0 < delete_flag && device_error_type != 0) delete_all();	/* 設定ファイル全削除 */
  if(Rpc_Check() == 0 && connect_stat == 1) connect();					/* 接続・接続解除 */

  if(viewer_init == 0 && add_flag <= 0 && delete_flag <= 0) connect_pad();	/* コントローラ操作 */

  /* タイトル表示 */
  sceDevConsClear(console);
  sceDevConsLocate(console, 0, 0);
  sceDevConsPrintf(console, "Network Connection (Version %s)\n", SETAPP_VER);
  sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
  sceDevConsPrintf(console, "\n" );

  if(viewer_init){
    /* 初期化中表示 */
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "Now Loading...\n");
    sceDevConsAttribute(console, WHITE);
  }else{
    /* 他の PlayStation2 の設定の場合と設定が壊れてる場合全ての設定を削除するかどうか尋ねる */
    if(device_error_type != 0){
      switch(delete_flag){
      case -4:
	/* そのデバイスは見ることができません */
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  ");
	print_device(console, select_error_device);
	sceDevConsPrintf(console, "device can't view\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsLocate(console, 0, 18);
	sceDevConsPrintf(console, "  Circle button   : Done\n");
	break;

      case -3:
	/* 本当に消していいですか？ */
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  ");
	print_device(console, select_error_device);
	sceDevConsPrintf(console, "setting all delete. really ?\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsLocate(console, 0, 18);
	sceDevConsPrintf(console, "  Circle button   : Yes\n");
	sceDevConsPrintf(console, "  Eks button      : No\n");
	break;

      case -2:
	/* 削除完了 */
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  ");
	print_device(console, select_error_device);
	sceDevConsPrintf(console, "setting all delete complete.\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsLocate(console, 0, 18);
	sceDevConsPrintf(console, "  Circle button   : Done\n");
	break;

      case -1:
	/* 削除に失敗しました */
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  ");
	print_device(console, select_error_device);
	sceDevConsPrintf(console, "setting all delete error !\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsLocate(console, 0, 18);
	sceDevConsPrintf(console, "  Circle button   : Done\n");
	break;

      case 0:
	if(select_error_device == SELECT_HOST){
	  /* そのデバイスは見ることができません */
	  sceDevConsAttribute(console, YELLOW);
	  sceDevConsPrintf(console, "  ");
	  print_device(console, select_error_device);
	  switch(device_error_type){
	  case sceNETCNF_SYNTAX_ERROR:
	    sceDevConsPrintf(console, "setting is broken.\n");
	    break;
	  case sceNETCNF_MAGIC_ERROR:
	    sceDevConsPrintf(console, "setting is other \"PlayStation 2\" data.\n");
	    break;
	  }
	  sceDevConsPrintf(console, "  ");
	  print_device(console, select_error_device);
	  sceDevConsPrintf(console, "device can't view\n");
	  sceDevConsAttribute(console, WHITE);
	  sceDevConsLocate(console, 0, 18);
	  sceDevConsPrintf(console, "  Circle button   : Done\n");
	}else{
	  /* 削除しますか？ */
	  sceDevConsAttribute(console, YELLOW);
	  sceDevConsPrintf(console, "  ");
	  print_device(console, select_error_device);
	  switch(device_error_type){
	  case sceNETCNF_SYNTAX_ERROR:
	    sceDevConsPrintf(console, "setting is broken.\n");
	    break;
	  case sceNETCNF_MAGIC_ERROR:
	    sceDevConsPrintf(console, "setting is other \"PlayStation 2\" data.\n");
	    break;
	  }
	  sceDevConsPrintf(console, "  ");
	  print_device(console, select_error_device);
	  sceDevConsPrintf(console, "setting all delete ?\n");
	  sceDevConsAttribute(console, WHITE);
	  sceDevConsLocate(console, 0, 18);
	  sceDevConsPrintf(console, "  Circle button   : Yes\n");
	  sceDevConsPrintf(console, "  Eks button      : No\n");
	}
	break;

      default:
	/* お待ち下さい */
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Please wait ...\n");
	switch(select_error_device){
	case SELECT_MC:
	  sceDevConsPrintf(console, "  Please don't pull out Memory Card.\n");
	case SELECT_HDD:
	  sceDevConsPrintf(console, "  Please don't power off PlayStation 2.\n");
	  break;
	}
	sceDevConsAttribute(console, WHITE);
	break;
      }
    }else{
      /* 組み合わせの表示 */
      if(cy == 3){
	sceDevConsPrintf(console, "* Combination              : ");
	if(env_select) sceDevConsAttribute(console, (*(combination_flag + (*combination_menu))) ? WHITE : BLUE);
      }else{
	sceDevConsPrintf(console, "  Combination              : ");
	if(env_select) sceDevConsAttribute(console, (*(combination_flag + (*combination_menu))) ? WHITE : BLUE);
      }
      if((*combination_menu) != -1){
	print_device(console, select_combination_device);
      }
      sceDevConsPrintf(console, "%s\n", env_usr_name);
      sceDevConsAttribute(console, WHITE);

      /* 接続機器の表示 */
      if(cy == 4){
	sceDevConsPrintf(console, "* Hardware                 : ");
	if((0 <= dev_menu) && dev_list && dev_select) sceDevConsAttribute(console, (dev_list + dev_menu)->flag ? WHITE : BLUE);
      }else{
	sceDevConsPrintf(console, "  Hardware                 : ");
	if((0 <= dev_menu) && dev_list && dev_select) sceDevConsAttribute(console, (dev_list + dev_menu)->flag ? WHITE : BLUE);
      }
      if(dev_list && dev_select){
	if(dev_menu == -1){
	  sceDevConsPrintf(console, "mem-");
	}else{
	  print_device(console, (dev_list + dev_menu)->select_device);
	}
      }
      sceDevConsPrintf(console, "%s\n", dev_usr_name);
      sceDevConsAttribute(console, WHITE);

      /* 接続プロバイダの表示 */
      if(cy == 5){
	sceDevConsPrintf(console, "* Network Service Provider : ");
	if((0 <= ifc_menu) && ifc_list && ifc_select) sceDevConsAttribute(console, (ifc_list + ifc_menu)->flag ? WHITE : BLUE);
      }else{
	sceDevConsPrintf(console, "  Network Service Provider : ");
	if((0 <= ifc_menu) && ifc_list && ifc_select) sceDevConsAttribute(console, (ifc_list + ifc_menu)->flag ? WHITE : BLUE);
      }
      if(ifc_list && ifc_select){
	if(ifc_menu == -1){
	  sceDevConsPrintf(console, "mem-");
	}else{
	  print_device(console, (ifc_list + ifc_menu)->select_device);
	}
      }
      sceDevConsPrintf(console, "%s\n", ifc_usr_name);
      sceDevConsAttribute(console, WHITE);

      /* 選択しているファイル番号を表示 */
      switch(cy){
      case 3:
	if((*combination_menu) == -1){
	  sceDevConsPrintf(console, "\n  Your select number = --- /%3d (---)\n", combination_menu_max);
	}else{
	  sceDevConsPrintf(console, "\n  Your select number = %3d /%3d\n", (*combination_menu) + 1, combination_menu_max);
	  sceDevConsPrintf(console, "                    (H: ");
	  print_device(console, select_combination_device);
	  sceDevConsPrintf(console, "%s)\n", dev_usr_name_disp);
	  sceDevConsPrintf(console, "                    (N: ");
	  print_device(console, select_combination_device);
	  sceDevConsPrintf(console, "%s)\n", ifc_usr_name_disp);
	}
	break;

      case 4:
	if(dev_menu == -1){
	  sceDevConsPrintf(console, "\n  Your select number = --- /%3d\n", dev_menu_max);
	}else{
	  sceDevConsPrintf(console, "\n  Your select number = %3d /%3d\n", dev_menu + 1, dev_menu_max);
	}
	break;

      case 5:
	if(ifc_menu == -1){
	  sceDevConsPrintf(console, "\n  Your select number = --- /%3d\n", ifc_menu_max);
	}else{
	  sceDevConsPrintf(console, "\n  Your select number = %3d /%3d\n", ifc_menu + 1, ifc_menu_max);
	}
	break;
      }

      /* メッセージの表示 */
      switch(hdd_flag){
      case 1:
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "\n  HDD is unformat.\n");
	sceDevConsAttribute(console, WHITE);
	break;
      case 2:
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "\n  HDD is lock.\n");
	sceDevConsAttribute(console, WHITE);
	break;
      }
      if(0 < add_flag){
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "\n  Please wait ...\n");
	sceDevConsAttribute(console, WHITE);
      }else if(add_flag == -1){
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "\n");
	switch(mc_error_type){
	case -1:
	  sceDevConsPrintf(console, "  Not PlayStation 2 Memory Card !\n");
	  break;
	case -2:
	  switch(select_combination_device){
	  case SELECT_MC:
	    sceDevConsPrintf(console, "  No Capacity of  Memory Card !\n");
	    break;
	  case SELECT_HDD:
	    sceDevConsPrintf(console, "  No Capacity of  HDD !\n");
	    break;
	  }
	  break;
	case -3:
	  sceDevConsPrintf(console, "  Save Error !\n");
	  break;
	case -4:
	  sceDevConsPrintf(console, "  Setting Data Max !\n");
	  break;
	}
	sceDevConsAttribute(console, WHITE);
      }else if(add_flag == -2){
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "\n  Complete !\n");
	sceDevConsAttribute(console, WHITE);
      }else if(delete_flag == -1){
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "\n  Delete Error !\n");
	sceDevConsAttribute(console, WHITE);
      }else if(delete_flag == -2){
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "\n  Complete !\n");
	sceDevConsAttribute(console, WHITE);
      }else if(connect_stat == -1){
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "\n  Error !\n");
	sceDevConsAttribute(console, WHITE);
      }else if(connect_stat == 1){
	sceDevConsAttribute(console, YELLOW);
	if(redial_stat){
	  sceDevConsPrintf(console, "\n  Redialing, Please wait ...\n");
	}else{
	  sceDevConsPrintf(console, "\n  Please wait ...\n");
	}
	sceDevConsAttribute(console, WHITE);
      }else if(connect_stat == 2){
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "\n  Connect !\n");
	sceDevConsAttribute(console, WHITE);
	goto DEFAULT_MSG;
      }else{
    DEFAULT_MSG:
	sceDevConsLocate(console, 0, 18);
	if(connect_stat){
	  sceDevConsPrintf(console, "  START button    : Disconnect\n");
	}else{
	  sceDevConsPrintf(console, "  START button    : Connect\n");
	}
	sceDevConsPrintf(console, "  SELECT button   : Connection Settings\n");
#ifdef HOST_READ_ONLY
	if(select_combination_device != SELECT_HOST){
	  if(cy == 3)  sceDevConsPrintf(console, "  Triangle button : Add\n");
	  if(cy == 3)  sceDevConsPrintf(console, "  Square button   : Delete\n");
	}
#else /* HOST_READ_ONLY */
	if(cy == 3)  sceDevConsPrintf(console, "  Triangle button : Add\n");
	if(cy == 3)  sceDevConsPrintf(console, "  Square button   : Delete\n");
#endif /* HOST_READ_ONLY */
      }
    }
  }

  /* 描画 */
  sceDevConsDraw(console);
}

/********************************/
/* ネットワーク接続画面の初期化 */
/********************************/
static void
initialize(void)
{
  int i;

  if(viewer_init == 1){
    /* initialize pointer */
    cy     = 3;
    cy_min = 3;
    cy_max = 5;

    /* initialize add_flag */
    if(0 < add_flag) add_flag = 0;

    /* initialize delete_flag */
    if(0 < delete_flag) delete_flag = 0;

    /* initialize viewer history */
    set_return(0, 0, 0, 0, 0, NO_RETURN);
  }

  /* env_list, ifc_list, dev_list update */
  list_update(list_update_arg);

  if(viewer_init == 4){
    if(env_list){
      /* select combination device */
      if(set_combination_device == NO_SELECT){
	init_combination_device(env_list->select_device);
      }else{
	init_combination_device(set_combination_device);
	set_combination_device = NO_SELECT;
      }

      /* set combination_menu & combination_num */
      if(set_combination_menu == -1){
	for(i = 0, (*combination_menu) = -1; i < combination_menu_max; i++){
	  if(strcmp(env_list->usr_name, (*(combination_name + i))) == 0){
	    (*combination_menu) = i;
	  }
	}
	if((*combination_menu) == -1){
	  combination_num = -1;
	}else{
	  combination_num = 0;
	}
      }else{
	(*combination_menu)  = set_combination_menu;
	combination_num      = 0;
	set_combination_menu = -1;
      }
    }else{
      /* select combination device */
      if(set_combination_device == NO_SELECT){
#ifdef NO_HOST
	init_combination_device(change_device(SELECT_MC));
#else /* NO_HOST */
	init_combination_device(change_device(SELECT_HOST));
#endif /* NO_HOST */
      }else{
	init_combination_device(set_combination_device);
	set_combination_device = NO_SELECT;
      }

      /* set combination_menu & combination_num */
      if(set_combination_menu == -1){
	(*combination_menu) = -1;
	combination_num     = -1;
      }else{
	(*combination_menu)  = set_combination_menu;
	combination_num      = 0;
	set_combination_menu = -1;
      }
    }
  }

  switch(viewer_init){
    /***********************************/
    /* 表示する Env ファイルを読み込む */
    /***********************************/
  case 10:
    Rpc_Init();
    if((*combination_menu) != -1){
      /* 選択した組み合わせ番号の状態をチェック */
      for(i = 0; i < env_menu_max; i++){
	if(strcmp((env_list + i)->usr_name, (*(combination_name + (*combination_menu)))) == 0
	   && (env_list + i)->select_device == select_combination_device){
	  (*(combination_flag + (*combination_menu))) = (env_list + i)->flag; /* 使用可・不可 */
	  (*(combination_stat + (*combination_menu))) = 1;                    /* 登録済・未登録 */
	}
      }
      /* 組み合わせファイルを読み込む */
      if((*(combination_stat + (*combination_menu)))){
	Rpc_sceNetCnfLoadEntry(&env_data, 0, (*(combination_name + (*combination_menu))), select_combination_device);
      }
      viewer_init = 11;
    }else{
      viewer_init = 11;
    }
    break;

  case 11:
    get_env_usr_name((*(combination_stat + (*combination_menu)))); /* Env User Name を取得する */
    get_ifc_usr_name((*(combination_stat + (*combination_menu)))); /* Ifc User Name を取得する */
    get_dev_usr_name((*(combination_stat + (*combination_menu)))); /* Dev User Name を取得する */
    viewer_init = 0;
    break;

    /************************************/
    /* 追加・編集した設定を直接指定する */
    /************************************/
  case 12:
    /* set dev_usr_name */
    switch(dev_select){
    case 0:
      strcpy(dev_usr_name, "(no select)");
      dev_menu = -1;
      break;
    case 2:
      for(i = 0; i < dev_menu_max; i++){
	if((dev_list + i)->select_device == select_dev_device){
	  /* 保存したら必ず各デバイスの先頭にくる */
	  dev_menu = i;
	  break;
	}
      }
      break;
    case 3:
      dev_menu = -1;
      break;
    }

    /* set ifc_usr_name */
    switch(ifc_select){
    case 0:
      strcpy(ifc_usr_name, "(no select)");
      ifc_menu = -1;
      break;
    case 2:
      for(i = 0; i < ifc_menu_max; i++){
	if((ifc_list + i)->select_device == select_ifc_device){
	  /* 保存したら必ず各デバイスの先頭にくる */
	  ifc_menu = i;
	  break;
	}
      }
      break;
    case 3:
      ifc_menu = -1;
      break;
    }

    /* set_env_usr_name */
    strcpy(env_usr_name, "(no select)");
    (*combination_menu) = -1;
    env_select          = 0;
    viewer_init         = 0;
    break;
  }
}

/****************************/
/* Env User Name を取得する */
/****************************/
static void
get_env_usr_name(int stat)
{
  env_select = stat ? 1 : 0;
  if((*combination_menu) == -1){
    strcpy(env_usr_name, "(no select)");
  }else{
    strcpy(env_usr_name, (*(combination_name + (*combination_menu))));
  }
}

/****************************/
/* Ifc User Name を取得する */
/****************************/
static void
get_ifc_usr_name(int stat)
{
  strcpy(ifc_usr_name_disp, "");
  if(dev_select < 2 && ifc_select < 2){
    strcpy(ifc_usr_name, "");
    if(stat){
      ifc_select = 1;
      ifc_menu = get_another_name(env_data.attach_ifc, ifc_usr_name, ifc_list, ifc_menu_max, select_combination_device);
      if(ifc_menu < 0) goto IFC_NO_SELECT;
    }else{
    IFC_NO_SELECT:
      ifc_select = 0;
      ifc_menu = -1;
      strcpy(ifc_usr_name, "(no select)");
    }
    strcpy(ifc_usr_name_disp, ifc_usr_name);
  }else{
    if(stat){
      if(get_another_name(env_data.attach_ifc, ifc_usr_name_disp, ifc_list, ifc_menu_max, select_combination_device) < 0) goto IFC_NO_SELECT2;
    }else{
    IFC_NO_SELECT2:
      strcpy(ifc_usr_name_disp, "(no select)");
    }
  }
}

/****************************/
/* Dev User Name を取得する */
/****************************/
static void
get_dev_usr_name(int stat)
{
  strcpy(dev_usr_name_disp, "");
  if(dev_select < 2 && ifc_select < 2){
    strcpy(dev_usr_name, "");
    if(stat){
      dev_select = 1;
      dev_menu = get_another_name(env_data.attach_dev, dev_usr_name, dev_list, dev_menu_max, select_combination_device);
      if(dev_menu < 0) goto DEV_NO_SELECT;
    }else{
    DEV_NO_SELECT:
      dev_select = 0;
      dev_menu = -1;
      strcpy(dev_usr_name, "(no select)");
    }
    strcpy(dev_usr_name_disp, dev_usr_name);
  }else{
    if(stat){
      if(get_another_name(env_data.attach_dev, dev_usr_name_disp, dev_list, dev_menu_max, select_combination_device) < 0) goto DEV_NO_SELECT2;
    }else{
    DEV_NO_SELECT2:
      strcpy(dev_usr_name_disp, "(no select)");
    }
  }
}

/*********************************/
/* Env, Ifc, Dev List を更新する */
/*********************************/
void
list_update(int next_no)
{
  int size;
  sceNetCnf_t *p;

  /* Initialize */
  p = (sceNetCnf_t *)sdata;

  switch(viewer_init){
  case 1:
    /* env_list の数を取得 */
    Rpc_Init();
    Rpc_sceNetCnfGetCount(0);
    viewer_init = 2;
    break;

  case 2:
    /* 領域を確保して、env_list を取得 */
    env_menu_max = Rpc_Get_Result();
    if(env_menu_max <= 0){
      env_list = NULL;
      viewer_init  = 4;
    }else{
      if(env_list != NULL){
	free((void *)env_list);
	env_list = NULL;
      }

      size = sizeof(sceNetCnfList2_t) * env_menu_max;
      env_list = (sceNetCnfList2_t *)memalign(64, GET_SIZE(size));
      if(env_list == NULL){
	viewer_init  = 4;
	break;
      }

      Rpc_sceNetCnfGetList(env_list, env_menu_max, 0);
      viewer_init = 3;
    }
    break;

  case 3:
    /* env_list 取得完了 */
    env_menu_max = Rpc_Get_Result();
    if(env_menu_max <= 0){
      free((void *)env_list);
      env_list            = NULL;
      select_error_device = p->device_stat;
      switch(env_menu_max){
      case sceNETCNF_MAGIC_ERROR:
	switch(select_error_device){
	case SELECT_MC:
	  device_stat &= DEVICE_STAT_MC_OFF;
	  break;
	case SELECT_HDD:
	  device_stat &= DEVICE_STAT_HDD_OFF;
	  break;
	case SELECT_HOST:
	  device_stat &= DEVICE_STAT_HOST_OFF;
	  break;
	}
	device_error_type = sceNETCNF_MAGIC_ERROR;
	viewer_init       = 1;
	break;
      case sceNETCNF_SYNTAX_ERROR:
	switch(select_error_device){
	case SELECT_MC:
	  device_stat &= DEVICE_STAT_MC_OFF;
	  break;
	case SELECT_HDD:
	  device_stat &= DEVICE_STAT_HDD_OFF;
	  break;
	case SELECT_HOST:
	  device_stat &= DEVICE_STAT_HOST_OFF;
	  break;
	}
	device_error_type = sceNETCNF_SYNTAX_ERROR;
	viewer_init       = 1;
	break;
      default:
	viewer_init = 4;
	break;
      }
    }else{
      viewer_init  = 4;
    }
    break;

  case 4:
    /* ifc_list の数を取得 */
    Rpc_Init();
    Rpc_sceNetCnfGetCount(1);
    viewer_init = 5;
    break;

  case 5:
    /* 領域を確保して、ifc_list を取得 */
    ifc_menu_max = Rpc_Get_Result();
    if(ifc_menu_max <= 0){
      ifc_list = NULL;
      viewer_init  = 7;
    }else{
      if(ifc_list != NULL){
	free((void *)ifc_list);
	ifc_list = NULL;
      }

      size = sizeof(sceNetCnfList2_t) * ifc_menu_max;
      ifc_list = (sceNetCnfList2_t *)memalign(64, GET_SIZE(size));
      if(ifc_list == NULL){
	viewer_init  = 7;
	break;
      }

      Rpc_sceNetCnfGetList(ifc_list, ifc_menu_max, 1);
      viewer_init = 6;
    }
    break;

  case 6:
    /* ifc_list 取得完了 */
    ifc_menu_max = Rpc_Get_Result();
    if(ifc_menu_max <= 0){
      free((void *)ifc_list);
      ifc_list            = NULL;
      select_error_device = p->device_stat;
      switch(ifc_menu_max){
      case sceNETCNF_MAGIC_ERROR:
	switch(select_error_device){
	case SELECT_MC:
	  device_stat &= DEVICE_STAT_MC_OFF;
	  break;
	case SELECT_HDD:
	  device_stat &= DEVICE_STAT_HDD_OFF;
	  break;
	case SELECT_HOST:
	  device_stat &= DEVICE_STAT_HOST_OFF;
	  break;
	}
	device_error_type = sceNETCNF_MAGIC_ERROR;
	viewer_init       = 1;
	break;
      case sceNETCNF_SYNTAX_ERROR:
	switch(select_error_device){
	case SELECT_MC:
	  device_stat &= DEVICE_STAT_MC_OFF;
	  break;
	case SELECT_HDD:
	  device_stat &= DEVICE_STAT_HDD_OFF;
	  break;
	case SELECT_HOST:
	  device_stat &= DEVICE_STAT_HOST_OFF;
	  break;
	}
	device_error_type = sceNETCNF_SYNTAX_ERROR;
	viewer_init       = 1;
	break;
      default:
	viewer_init = 7;
	break;
      }
    }else{
      viewer_init  = 7;
    }
    break;

  case 7:
    /* dev_list の数を取得 */
    Rpc_Init();
    Rpc_sceNetCnfGetCount(2);
    viewer_init = 8;
    break;

  case 8:
    /* 領域を確保して、dev_list を取得 */
    dev_menu_max = Rpc_Get_Result();
    if(dev_menu_max <= 0){
      dev_list = NULL;
      viewer_init  = next_no;
    }else{
      if(dev_list != NULL){
	free((void *)dev_list);
	dev_list = NULL;
      }

      size = sizeof(sceNetCnfList2_t) * dev_menu_max;
      dev_list = (sceNetCnfList2_t *)memalign(64, GET_SIZE(size));
      if(dev_list == NULL){
	viewer_init  = next_no;
	break;
      }

      Rpc_sceNetCnfGetList(dev_list, dev_menu_max, 2);
      viewer_init = 9;
    }
    break;

  case 9:
    /* dev_list 取得完了 */
    dev_menu_max = Rpc_Get_Result();
    if(dev_menu_max <= 0){
      free((void *)dev_list);
      dev_list            = NULL;
      select_error_device = p->device_stat;
      switch(dev_menu_max){
      case sceNETCNF_MAGIC_ERROR:
	switch(select_error_device){
	case SELECT_MC:
	  device_stat &= DEVICE_STAT_MC_OFF;
	  break;
	case SELECT_HDD:
	  device_stat &= DEVICE_STAT_HDD_OFF;
	  break;
	case SELECT_HOST:
	  device_stat &= DEVICE_STAT_HOST_OFF;
	  break;
	}
	device_error_type = sceNETCNF_MAGIC_ERROR;
	viewer_init       = 1;
	break;
      case sceNETCNF_SYNTAX_ERROR:
	switch(select_error_device){
	case SELECT_MC:
	  device_stat &= DEVICE_STAT_MC_OFF;
	  break;
	case SELECT_HDD:
	  device_stat &= DEVICE_STAT_HDD_OFF;
	  break;
	case SELECT_HOST:
	  device_stat &= DEVICE_STAT_HOST_OFF;
	  break;
	}
	device_error_type = sceNETCNF_SYNTAX_ERROR;
	viewer_init       = 1;
	break;
      default:
	viewer_init = next_no;
	break;
      }
    }else{
      viewer_init  = next_no;
    }
    break;
  }
}

/*****************************************************/
/* usr_name(sys_name) から sys_name(usr_name) を取得 */
/*****************************************************/
static int
get_another_name(char *sys_name, char *usr_name, sceNetCnfList2_t *list, int menu_max, int select_device)
{
  int i = 0, flag;

  /* flag init */
  flag = 1;

  /* usr_name(sys_name) から sys_name(usr_name) を取得 */
  if(list != NULL){
    for(i = 0; i < menu_max; i++){
      if(strcmp(usr_name, (list + i)->usr_name) == 0 && select_device == (list + i)->select_device){
	strcpy(sys_name, (list + i)->sys_name);
	flag = 0;
	break;
      }
      if(strcmp(sys_name, (list + i)->sys_name) == 0 && select_device == (list + i)->select_device){
	strcpy(usr_name, (list + i)->usr_name);
	flag = 0;
	break;
      }
    }
  }

  if(flag == 0){
    return (i);
  }else{
    return (-1);
  }
}

/***************************/
/* Env ファイルを Add する */
/***************************/
static void
add_env(void)
{
  static int addr = 0;
  static int id = 0;
  int type, free, format, cmd, result, r;

  switch(add_flag){
  case 1:
    switch(select_combination_device){
    case SELECT_MC:
      type = 1;
      free = 0;
      format = 0;
      if(mc_stat0 == 0 && mc_stat1 == 1){
	sceMcGetInfo(1, 0, &type, &free, &format);
      }else{
	sceMcGetInfo(0, 0, &type, &free, &format);
      }
      sceMcSync(0, &cmd, &result);
      /* Memory Card タイプのチェック */
      if(type != 2){
	add_flag      = -1;
	mc_error_type = -1;
      }else{
	strcpy(env_data.attach_ifc, "");
	strcpy(env_data.attach_dev, "");
	if(ifc_list) get_another_name(env_data.attach_ifc, ifc_usr_name, ifc_list, ifc_menu_max, SELECT_MC);
	if(dev_list) get_another_name(env_data.attach_dev, dev_usr_name, dev_list, dev_menu_max, SELECT_MC);
	Rpc_Init();
	Rpc_AllocSysMemory(GET_SIZE(sizeof(sceNetCnfEnvData_t)));
	add_flag = 2;
      }
      break;
    case SELECT_HDD:
      strcpy(env_data.attach_ifc, "");
      strcpy(env_data.attach_dev, "");
      if(ifc_list) get_another_name(env_data.attach_ifc, ifc_usr_name, ifc_list, ifc_menu_max, SELECT_HDD);
      if(dev_list) get_another_name(env_data.attach_dev, dev_usr_name, dev_list, dev_menu_max, SELECT_HDD);
      Rpc_Init();
      Rpc_AllocSysMemory(GET_SIZE(sizeof(sceNetCnfEnvData_t)));
      add_flag = 2;
      break;
#ifndef HOST_READ_ONLY
    case SELECT_HOST:
      strcpy(env_data.attach_ifc, "");
      strcpy(env_data.attach_dev, "");
      if(ifc_list) get_another_name(env_data.attach_ifc, ifc_usr_name, ifc_list, ifc_menu_max, SELECT_HOST);
      if(dev_list) get_another_name(env_data.attach_dev, dev_usr_name, dev_list, dev_menu_max, SELECT_HOST);
      Rpc_Init();
      Rpc_AllocSysMemory(GET_SIZE(sizeof(sceNetCnfEnvData_t)));
      add_flag = 2;
      break;
#endif /* HOST_READ_ONLY */
    }
    break;

  case 2:
    addr = Rpc_Get_Result();
    if(addr == NULL){
      add_flag      = -1;
      mc_error_type = -3;
    }else{
      save_data();
      id = send_iop((int)&last_data, addr, sizeof(sceNetCnfEnvData_t), 0);
      add_flag = 3;
    }
    break;

  case 3:
    if(dma_check(id) < 0){
      if((*(combination_stat + (*combination_menu)))){
	Rpc_sceNetCnfEditEntry((sceNetCnfEnvData_t *)addr, 0, env_usr_name, env_usr_name, select_combination_device);
      }else{
	Rpc_sceNetCnfAddEntry((sceNetCnfEnvData_t *)addr, 0, env_usr_name, select_combination_device);
      }
      add_flag = 4;
    }
    break;

  case 4:
    if((r = Rpc_Get_Result()) < 0){
      Rpc_FreeSysMemory(addr);
      add_flag      = -1;
      switch(r){
      case sceNETCNF_TOO_MANY_ENTRIES:
	mc_error_type = -4; /* 設定ファイル数が最大値 */
	break;
      case sceNETCNF_CAPACITY_ERROR:
	mc_error_type = -2; /* 容量が足りない */
	break;
      default:
	mc_error_type = -3; /* 設定ファイルが作成できない */
	break;
      }
    }else{
      Rpc_FreeSysMemory(addr);
      add_flag = -2;
    }
    break;
  }
}

/**************************/
/* Env ファイルを削除する */
/**************************/
static void
delete_env(void)
{
  switch(delete_flag){
  case 1:
    Rpc_Init();
    Rpc_sceNetCnfDeleteEntry(0, env_usr_name, select_combination_device);
    delete_flag = 2;
    break;

  case 2:
    if(Rpc_Get_Result() < 0){
      delete_flag = -1; /* error */
    }else{
      delete_flag = -2; /* complete */
    }
    break;
  }
}

/******************************/
/* 設定ファイルを全て削除する */
/******************************/
static void
delete_all(void)
{
  switch(delete_flag){
  case 1:
    Rpc_Init();
    Rpc_sceNetCnfDeleteAll(select_error_device);
    delete_flag = 2;
    break;

  case 2:
    if(Rpc_Get_Result() < 0){
      delete_flag = -1; /* error */
    }else{
      delete_flag = -2; /* complete */
    }
    break;
  }
}

/**********************/
/* 接続・接続解除する */
/**********************/
static void
connect(void)
{
  static int addr = 0;
  static int id = 0;

  switch(connect_phase){
    /************************************/
    /* EE 上の設定データを IOP 上に転送 */
    /************************************/
  case 1:
    Rpc_Init();
    Rpc_AllocSysMemory(GET_SIZE(sizeof(sceNetCnfEnvData_t)));
    connect_phase = 2;
    break;

  case 2:
    addr = Rpc_Get_Result();
    if(addr == NULL){
      connect_phase = 0;
      connect_stat  = -1;
    }else{
      id = send_iop((int)&last_data, addr, sizeof(sceNetCnfEnvData_t), 0);
      connect_phase = 3;
    }
#ifdef DEBUG
    dump_data(&last_data);
#endif /* DEBUG */
    break;

    /*******************************************/
    /* netcnf.irx に設定を反映させて、接続する */
    /*******************************************/
  case 3:
    if(connect_type < 3){
      if(dma_check(id) < 0){
	switch(connect_type){
	case 0:
	  Rpc_Init();
	  Rpc_sceNetCnfSetConfiguration(connect_type, (sceNetCnfEnvData_t *)addr, NULL, NULL,
					NO_SELECT, NO_SELECT);
	  break;
	case 1:
	  Rpc_Init();
	  Rpc_sceNetCnfSetConfiguration(connect_type, (sceNetCnfEnvData_t *)addr, ifc_usr_name, NULL,
					(ifc_list + ifc_menu)->select_device, NO_SELECT);
	  break;
	case 2:
	  Rpc_Init();
	  Rpc_sceNetCnfSetConfiguration(connect_type, (sceNetCnfEnvData_t *)addr, dev_usr_name, NULL,
					(dev_list + dev_menu)->select_device, NO_SELECT);
	  break;
	}
	connect_phase = 4;
      }
    }else{
      switch(connect_type){
      case 3:
	Rpc_Init();
	Rpc_sceNetCnfSetConfiguration(connect_type, NULL, ifc_usr_name, dev_usr_name,
				      (ifc_list + ifc_menu)->select_device, (dev_list + dev_menu)->select_device);
	break;
      case 4:
	Rpc_Init();
	Rpc_sceNetCnfSetConfiguration(connect_type, NULL, env_usr_name, NULL,
				      select_combination_device, NO_SELECT);
	break;
      }
      connect_phase = 4;
    }
    break;

  case 4:
    if(Rpc_Get_Result() < 0){
      connect_phase = 0;
      connect_stat  = -1;
    }else{
      connect_phase = 0;
    }
    break;

    /****************/
    /* 接続解除する */
    /****************/
  case 6:
    Rpc_Init();
    Rpc_sceNetCnfDownInterface();
    connect_phase = 7;
    break;

  case 7:
    if(Rpc_Get_Result() < 0){
      connect_phase = 0;
      connect_stat  = -1;
    }else{
      connect_phase = 0;
      connect_stat  = 0;
    }
    break;
  }
}

/**************************/
/* 次のデバイスを検索する */
/**************************/
int
change_device(int now_device)
{
  int next_device;

  /* Initialize */
  next_device = 0;

  /* MC -> HDD -> HOST の順でトグルする */
  switch(now_device){
  case SELECT_MC:
    /* デバイスをチェックし、存在しなければ次のデバイスへトグルする */
    if(device_stat & DEVICE_STAT_HDD_ON){
      next_device = SELECT_HDD;
    }else{
      if(device_stat & DEVICE_STAT_HOST_ON){
	next_device = SELECT_HOST;
      }else{
	next_device = SELECT_MC;
      }
    }
    break;

  case SELECT_HDD:
    /* デバイスをチェックし、存在しなければ次のデバイスへトグルする */
    if(device_stat & DEVICE_STAT_HOST_ON){
      next_device = SELECT_HOST;
    }else{
      if(device_stat & DEVICE_STAT_MC_ON){
	next_device = SELECT_MC;
      }else{
	next_device = SELECT_HDD;
      }
    }
    break;

  case SELECT_HOST:
    /* デバイスをチェックし、存在しなければ次のデバイスへトグルする */
    if(device_stat & DEVICE_STAT_MC_ON){
      next_device = SELECT_MC;
    }else{
      if(device_stat & DEVICE_STAT_HDD_ON){
	next_device = SELECT_HDD;
      }else{
	next_device = SELECT_HOST;
      }
    }
    break;
  }

  return (next_device);
}

/********************************/
/* デバイスポインタを初期化する */
/********************************/
static void
init_combination_device(int now_device)
{
  int i;

  /* initialize device */
  select_combination_device = now_device;

  /* initialize other pointer */
  switch(now_device){
  case SELECT_MC:
    combination_menu_max = MAX_MC_COMBINATION;
    combination_name     = mc_combination_name;
    combination_flag     = mc_combination_flag;
    combination_stat     = mc_combination_stat;
    combination_menu     = &mc_combination_menu;
    break;

  case SELECT_HDD:
    combination_menu_max = MAX_HDD_COMBINATION;
    combination_name     = hdd_combination_name;
    combination_flag     = hdd_combination_flag;
    combination_stat     = hdd_combination_stat;
    combination_menu     = &hdd_combination_menu;
    break;

  case SELECT_HOST:
    combination_menu_max = MAX_HOST_COMBINATION;
    combination_name     = host_combination_name;
    combination_flag     = host_combination_flag;
    combination_stat     = host_combination_stat;
    combination_menu     = &host_combination_menu;
    break;
  }

  /* initialize combination value */
  (*combination_menu) = 0;
  for(i = 0; i < combination_menu_max; i++){
    (*(combination_flag + i)) = 0;
    (*(combination_stat + i)) = 0;
  }
}

/**********************/
/* デバイスを出力する */
/**********************/
void
print_device(int cons, int select_device)
{
  switch(select_device){
  case NO_SELECT:
    sceDevConsPrintf(cons, "new-");
    break;
  case SELECT_MC:
    sceDevConsPrintf(cons, "mc");
    if(mc_stat0 == 0 && mc_stat1 == 1){
      sceDevConsPrintf(cons, "1-");
    }else{
      sceDevConsPrintf(cons, "0-");
    }
    break;
  case SELECT_HDD:
    sceDevConsPrintf(cons, "hdd-");
    break;
  case SELECT_HOST:
    sceDevConsPrintf(cons, "host-");
    break;
  }
}
