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
 *                              dev.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.11      tetsu      Change for PPPoE
 */
#include "setapp.h"

#ifdef HOST_READ_ONLY
#define SAVE_TYPE_MAX		(2)
#else /* HOST_READ_ONLY */
#define SAVE_TYPE_MAX		(3)
#endif /* HOST_READ_ONLY */

#define DIALING_TYPE_MAX	(2)

int select_dev_device;
static int save_type;

#ifdef HOST_READ_ONLY
static char *save_type_msg[] = {"Save Memroy Card",
				"Save HDD"};
#else /* HOST_READ_ONLY */
static char *save_type_msg[] = {"Save Memroy Card",
				"Save HDD",
				"Save HOST"};
#endif /* HOST_READ_ONLY */

static char *dialing_type_msg[] = {"Tone", "Pulse"};

static u_char hwaddr[6];
static int hwaddr_flag;

static int dev_pad(void);
static void initialize(void);
static void delete_dev(void);
static void add_dev(void);

/************************************/
/* ネットワーク設定画面コントロール */
/************************************/
static int
dev_pad(void)
{
  u_short tpad;
  int i;

  if((tpad = pad_read()) == 0) return (0);

  if(tpad & SCE_PADLup){
    switch(viewer_type){
    case 2:
      env_data.dialing_type--;
      env_data.dialing_type = LIMIT(env_data.dialing_type, 0, DIALING_TYPE_MAX - 1);
      break;
    case 4:
      if(save_type != -1 && add_flag == 0){
	if(0 <= save_type - 1){
	  for(i = save_type - 1; 0 <= i; i--){
	    switch(i){
	    case 0:
	      if(device_stat & DEVICE_STAT_MC_ON){
		if(viewer_mode == COPY_MODE){
		  if((dev_list + dev_menu)->select_device != SELECT_MC) save_type = i;
		}else{
		  save_type = i;
		}
	      }
	      break;
	    case 1:
	      if(device_stat & DEVICE_STAT_HDD_ON){
		if(viewer_mode == COPY_MODE){
		  if((dev_list + dev_menu)->select_device != SELECT_HDD) save_type = i;
		}else{
		  save_type = i;
		}
	      }
	      break;
#ifndef HOST_READ_ONLY
	    case 2:
	      if(device_stat & DEVICE_STAT_HOST_ON){
		if(viewer_mode == COPY_MODE){
		  if((dev_list + dev_menu)->select_device != SELECT_HOST) save_type = i;
		}else{
		  save_type = i;
		}
	      }
	      break;
#endif /* HOST_READ_ONLY */
	    }
	    if(i == save_type) break;
	  }
	}
      }
      break;
    }
  }
  if(tpad & SCE_PADLdown){
    switch(viewer_type){
    case 2:
      env_data.dialing_type++;
      env_data.dialing_type = LIMIT(env_data.dialing_type, 0, DIALING_TYPE_MAX - 1);
      break;
    case 4:
      if(save_type != -1 && add_flag == 0){
	if(save_type + 1 < SAVE_TYPE_MAX){
	  for(i = save_type + 1; i < SAVE_TYPE_MAX; i++){
	    switch(i){
	    case 0:
	      if(device_stat & DEVICE_STAT_MC_ON){
		if(viewer_mode == COPY_MODE){
		  if((dev_list + dev_menu)->select_device != SELECT_MC) save_type = i;
		}else{
		  save_type = i;
		}
	      }
	      break;
	    case 1:
	      if(device_stat & DEVICE_STAT_HDD_ON){
		if(viewer_mode == COPY_MODE){
		  if((dev_list + dev_menu)->select_device != SELECT_HDD) save_type = i;
		}else{
		  save_type = i;
		}
	      }
	      break;
#ifndef HOST_READ_ONLY
	    case 2:
	      if(device_stat & DEVICE_STAT_HOST_ON){
		if(viewer_mode == COPY_MODE){
		  if((dev_list + dev_menu)->select_device != SELECT_HOST) save_type = i;
		}else{
		  save_type = i;
		}
	      }
	      break;
#endif /* HOST_READ_ONLY */
	    }
	    if(i == save_type) break;
	  }
	}
      }
      break;
    }
  }
  if(tpad & SCE_PADLleft){
  }
  if(tpad & SCE_PADLright){
  }
  if(tpad & SCE_PADRup){
  }
  if(tpad & SCE_PADRdown){
    switch(add_flag){
    case -4:
      add_flag = 0;
      break;
    default:
      if(get_return() == 1){
	viewer_init  = 1;
      }
      break;
    }
  }
  if(tpad & SCE_PADRleft){
    switch(viewer_type){
    case 2:
      /* 詳細画面に移動する */
      set_return(viewer_flag, viewer_type, viewer_mode, 5, 0, viewer_mode);
      viewer_init = 1;
      break;
    case 3:
      /* 詳細画面に移動する */
      if(env_data.type != 2){
	set_return(viewer_flag, viewer_type, viewer_mode, 5, 1, viewer_mode);
	viewer_init = 1;
      }
      break;
    case 4:
      if(add_flag == 0){
	if(viewer_mode == ALL_ADD){
	  set_return(viewer_flag, viewer_type, viewer_mode, 4, -1, viewer_mode);
	  viewer_init = 1;
	}else{
	  set_return(0, -1, DEFAULT, 1, 0, DEFAULT);
	  viewer_init = 1;
	}
	dev_select = 3;
	save_data();
      }
      break;
    }
  }
  if(tpad & SCE_PADRright){
    switch(viewer_type){
    case 0:
      set_return(viewer_flag, viewer_type, NO_RETURN, viewer_flag, 1, viewer_mode);
      delete_flag = 1;
      break;
    case 1:
      if(delete_flag < 0){
	set_return(0, -1, DEFAULT, 1, 0, DEFAULT);
	viewer_init = 1;
	delete_flag = 0;
      }
      break;
    case 2:
      if(viewer_mode == ALL_ADD){
	set_return(viewer_flag, viewer_type, viewer_mode, 4, -1, viewer_mode);
	viewer_init = 1;
      }else{
	set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 3, viewer_mode);
      }
      break;
    case 3:
      if(viewer_mode == ALL_ADD){
	set_return(viewer_flag, viewer_type, viewer_mode, 4, -1, viewer_mode);
	viewer_init = 1;
      }else{
	set_return(viewer_flag, viewer_type, NO_RETURN, viewer_flag, 4, viewer_mode);
      }
      break;
    case 4:
      switch(add_flag){
      case -1:
	mc_error_type = 0;
	if(viewer_mode != COPY_MODE) dev_select = 3;
      case -2:
	add_flag = -3;
	break;
      case -4:
	add_flag = -5;
	break;
      case  0:
	if(save_type != -1){
	  switch(save_type){
	  case 0:
	    select_dev_device = SELECT_MC;
	    break;
	  case 1:
	    select_dev_device = SELECT_HDD;
	    break;
#ifndef HOST_READ_ONLY
	  case 2:
	    select_dev_device = SELECT_HOST;
	    break;
#endif /* HOST_READ_ONLY */
	  }

	  switch(viewer_mode){
	  case COPY_MODE:
	    add_type   = 2; /* COPY_MODE */
	    add_flag   = 1;
	    break;
	  case ADD_MODE:
	    add_type   = 1; /* ADD_MODE */
	    add_flag   = 1;
	    dev_select = 2;
	    break;
	  default:
	    add_type   = 0; /* EDIT_MODE */
	    add_flag   = 1;
	    dev_select = 2;
	    break;
	  }
	}
	break;
      }
      break;
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
  }
  if(tpad & SCE_PADselect){
  }

  return (1);
}

/************************/
/* ネットワーク設定画面 */
/************************/
void
dev_viewer(void)
{
  int i;

  /* 状態チェック */
  switch(status){
  case IEV_Conf:
  case IEV_NoConf:
    switch(select_dev_device){
    case NO_SELECT:
      switch(change_device(SELECT_HOST)){
      case SELECT_MC:
	save_type = 0;
	break;
      case SELECT_HDD:
	save_type = 1;
	break;
#ifdef HOST_READ_ONLY
      case SELECT_HOST:
	save_type = -1;
	break;
#else /* HOST_READ_ONLY */
      case SELECT_HOST:
	save_type = 2;
	break;
#endif /* HOST_READ_ONLY */
      }
      break;
    case SELECT_MC:
      if(save_type == -1){
	if((device_stat & DEVICE_STAT_MC_ON) == DEVICE_STAT_MC_ON) save_type = 0;
      }else{
	if((device_stat & DEVICE_STAT_MC_ON) != DEVICE_STAT_MC_ON) save_type = -1;
      }
      break;
    case SELECT_HDD:
      if(save_type == -1){
	if((device_stat & DEVICE_STAT_HDD_ON) == DEVICE_STAT_HDD_ON) save_type = 1;
      }else{
	if((device_stat & DEVICE_STAT_HDD_ON) != DEVICE_STAT_HDD_ON) save_type = -1;
      }
      break;
#ifndef HOST_READ_ONLY
    case SELECT_HOST:
      if(save_type == -1){
	if((device_stat & DEVICE_STAT_HOST_ON) == DEVICE_STAT_HOST_ON) save_type = 2;
      }else{
	if((device_stat & DEVICE_STAT_HOST_ON) != DEVICE_STAT_HOST_ON) save_type = -1;
      }
      break;
#endif /* HOST_READ_ONLY */
    }
  }
  status = -1;

  if(Rpc_Check() == 0 && 0 != viewer_init) initialize();		/* 画面初期化 */
  if(Rpc_Check() == 0 && 0 != add_flag) add_dev();			/* Dev ファイル作成 */
  if(Rpc_Check() == 0 && 0 != delete_flag) delete_dev();		/* Dev ファイル削除 */

  if(viewer_init == 0 && add_flag <= 0 && delete_flag <= 0) dev_pad();	/* コントローラ操作 */

  /* タイトル表示 */
  sceDevConsClear(console);
  sceDevConsLocate(console, 0, 0);
  sceDevConsPrintf(console, "Network Configuration(dev)\n");
  sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
  sceDevConsPrintf(console, "\n");

  if(viewer_init){
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "Now Loading...\n");
    sceDevConsAttribute(console, WHITE);
  }else{
    switch(viewer_type){
      /**************************/
      /* Dev ファイルを削除する */
      /**************************/
    case 0:
      sceDevConsAttribute(console, YELLOW);
      sceDevConsPrintf(console, "  You will delete %s.\n", (dev_list + dev_menu)->usr_name);
      sceDevConsAttribute(console, WHITE);
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "  If you want to delete, push Circle button, please\n");
      sceDevConsPrintf(console, "  If you want to cancel, push Eks button, please\n");
      sceDevConsLocate(console, 0, 18);
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;

    case 1:
      switch(delete_flag){
      case 1:
      case 2:
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Please wait ...\n");
	sceDevConsAttribute(console, WHITE);
	break;
      case -1:
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Error !\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsPrintf(console, "\n");
	sceDevConsPrintf(console, "  Please push Circle button.\n");
	break;
      case -2:
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Complete !\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsPrintf(console, "\n");
	sceDevConsPrintf(console, "  Please push Circle button.\n");
	break;
      }
      break;

      /*****************************/
      /* トーン・パルス を設定する */
      /*****************************/
    case 2:
      for(i = 0; i < DIALING_TYPE_MAX; i++){
	if(env_data.dialing_type == i){
	  sceDevConsPrintf(console,"* %s\n", dialing_type_msg[i]);
	}else{
	  sceDevConsPrintf(console,"  %s\n", dialing_type_msg[i]);
	}
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "  Please select dialing type.\n");
      sceDevConsLocate(console, 0, 18);
      sceDevConsPrintf(console, "  Square button : Advanced Settings\n");
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;

      /******************************/
      /* 詳細への移行画面(eth, nic) */
      /******************************/
    case 3:
      sceDevConsPrintf(console, "  Please push Circle button, \"");
      sceDevConsAttribute(console, YELLOW);
      sceDevConsPrintf(console, "%s", dev_usr_name);
      sceDevConsAttribute(console, WHITE);
      sceDevConsPrintf(console, "\" is done.\n");
      if(hwaddr_flag){
	sceDevConsPrintf(console, "\n  Hardware Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
			 hwaddr[0], hwaddr[1], hwaddr[2],
			 hwaddr[3], hwaddr[4], hwaddr[5]);
      }
      sceDevConsLocate(console, 0, 18);
      if(env_data.type != 2) sceDevConsPrintf(console, "  Square button : Advanced Settings\n");
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;

      /************************/
      /* 設定の保存を確認する */
      /************************/
    case 4:
      if(add_flag){
	switch(add_flag){
	case -1:
	  sceDevConsAttribute(console, YELLOW);
	  switch(mc_error_type){
	  case -1:
	    sceDevConsPrintf(console, "  Not PlayStation 2 Memory Card !\n");
	    break;
	  case -2:
	    switch(select_dev_device){
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
	  case -5:
	    sceDevConsPrintf(console, "  Format Error !\n");
	    break;
	  }
	  sceDevConsAttribute(console, WHITE);
	  sceDevConsLocate(console, 0, 18);
	  sceDevConsPrintf(console, "  Circle button : Done\n");
	  break;
	case -2:
	  sceDevConsAttribute(console, YELLOW);
	  sceDevConsPrintf(console, "  Save Done.\n");
	  sceDevConsAttribute(console, WHITE);
	  sceDevConsLocate(console, 0, 18);
	  sceDevConsPrintf(console, "  Circle button : Done\n");
	  break;
	case -4:
	  sceDevConsAttribute(console, YELLOW);
	  sceDevConsPrintf(console, "  Format Memory Card?\n");
	  sceDevConsAttribute(console, WHITE);
	  sceDevConsLocate(console, 0, 18);
	  sceDevConsPrintf(console, "  Circle button : Format\n");
	  sceDevConsPrintf(console, "  Eks button    : Cancel\n");
	  break;
	default:
	  sceDevConsAttribute(console, YELLOW);
	  sceDevConsPrintf(console, "  Please wait, now save...\n");
	  sceDevConsAttribute(console, WHITE);
	  break;
	}
      }else{
	if(save_type == -1){
	  sceDevConsPrintf(console,"  (no save device)\n");
	  sceDevConsLocate(console, 0, 18);
	  sceDevConsPrintf(console, "  Square button : No Save\n");
	}else{
	  switch(select_dev_device){
	  case NO_SELECT:
	    for(i = 0; i < SAVE_TYPE_MAX; i++){
	      switch(i){
	      case 0:
		if((device_stat & DEVICE_STAT_MC_ON) != DEVICE_STAT_MC_ON) continue;
		if(viewer_mode == COPY_MODE){
		  if((dev_list + dev_menu)->select_device == SELECT_MC) continue;
		}
		break;
	      case 1:
		if((device_stat & DEVICE_STAT_HDD_ON) != DEVICE_STAT_HDD_ON) continue;
		if(viewer_mode == COPY_MODE){
		  if((dev_list + dev_menu)->select_device == SELECT_HDD) continue;
		}
		break;
#ifndef HOST_READ_ONLY
	      case 2:
		if((device_stat & DEVICE_STAT_HOST_ON) != DEVICE_STAT_HOST_ON) continue;
		if(viewer_mode == COPY_MODE){
		  if((dev_list + dev_menu)->select_device == SELECT_HOST) continue;
		}
		break;
#endif /* HOST_READ_ONLY */
	      }

	      if(i == save_type){
		if(i == 0){
		  sceDevConsPrintf(console,"* %s", save_type_msg[i]);
		  if(mc_stat0 == 0 && mc_stat1 == 1){
		    sceDevConsPrintf(console,"1\n");
		  }else{
		    sceDevConsPrintf(console,"0\n");
		  }
		}else{
		  sceDevConsPrintf(console,"* %s\n", save_type_msg[i]);
		}
	      }else{
		if(i == 0){
		  sceDevConsPrintf(console,"  %s", save_type_msg[i]);
		  if(mc_stat0 == 0 && mc_stat1 == 1){
		    sceDevConsPrintf(console,"1\n");
		  }else{
		    sceDevConsPrintf(console,"0\n");
		  }
		}else{
		  sceDevConsPrintf(console,"  %s\n", save_type_msg[i]);
		}
	      }
	    }
	    sceDevConsPrintf(console, "\n");
	    sceDevConsPrintf(console, "  Please select save type.\n");
	    break;

	  default:
	    if(save_type == 0){
	      sceDevConsPrintf(console,"* %s", save_type_msg[save_type]);
	      if(mc_stat0 == 0 && mc_stat1 == 1){
		sceDevConsPrintf(console,"1\n");
	      }else{
		sceDevConsPrintf(console,"0\n");
	      }
	    }else{
	      sceDevConsPrintf(console,"* %s", save_type_msg[save_type]);
	    }
	    break;
	  }
	  sceDevConsLocate(console, 0, 18);
	  sceDevConsPrintf(console, "  Circle button : Save\n");
	  sceDevConsPrintf(console, "  Square button : No Save\n");
	}
      }
      break;
    }
  }

  /* 描画 */
  sceDevConsDraw(console);
}

/************************/
/* Dev 画面を初期化する */
/************************/
static void
initialize(void)
{
  int i, no, cmp_no;
  char dev_usr_name_tmp[MAX_LEN];
  char dev_usr_name_org[MAX_LEN];
  sceNetCnf_t *p;

  switch(viewer_init){
    /**************************/
    /* モードにより動作を変更 */
    /**************************/
  case 1:
    /* all mode common initialize */
    hwaddr_flag = 0;
    delete_flag = 0;
    add_flag    = 0;
    add_type    = 0;
    switch(select_dev_device){
    case NO_SELECT:
      switch(change_device(SELECT_HOST)){
      case SELECT_MC:
	save_type = 0;
	break;
      case SELECT_HDD:
	save_type = 1;
	break;
#ifdef HOST_READ_ONLY
      case SELECT_HOST:
	save_type = -1;
	break;
#else /* HOST_READ_ONLY */
      case SELECT_HOST:
	save_type = 2;
	break;
#endif /* HOST_READ_ONLY */
      }
      break;
    case SELECT_MC:
      save_type = 0;
      break;
    case SELECT_HDD:
      save_type = 1;
      break;
#ifndef HOST_READ_ONLY
    case SELECT_HOST:
      save_type = 2;
      break;
#endif /* HOST_READ_ONLY */
    }

    /* select type */
    switch(viewer_mode){
    case COPY_MODE:
      switch(save_type){
      case 0:
	if((dev_list + dev_menu)->select_device == SELECT_MC){
	  switch(change_device(SELECT_MC)){
	  case SELECT_MC:
	    save_type = -1;
	    break;
	  case SELECT_HDD:
	    save_type = 1;
	    break;
#ifdef HOST_READ_ONLY
	  case SELECT_HOST:
	    save_type = -1;
	    break;
#else /* HOST_READ_ONLY */
	  case SELECT_HOST:
	    save_type = 2;
	    break;
#endif /* HOST_READ_ONLY */
	  }
	}
	break;
      case 1:
	if((dev_list + dev_menu)->select_device == SELECT_HDD){
	  switch(change_device(SELECT_HDD)){
	  case SELECT_MC:
	    save_type = 0;
	    break;
	  case SELECT_HDD:
	    save_type = -1;
	    break;
#ifdef HOST_READ_ONLY
	  case SELECT_HOST:
	    save_type = -1;
	    break;
#else /* HOST_READ_ONLY */
	  case SELECT_HOST:
	    save_type = 2;
	    break;
#endif /* HOST_READ_ONLY */
	  }
	}
	break;
#ifndef HOST_READ_ONLY
      case 2:
	if((dev_list + dev_menu)->select_device == SELECT_HOST){
	  switch(change_device(SELECT_HOST)){
	  case SELECT_MC:
	    save_type = 0;
	    break;
	  case SELECT_HDD:
	    save_type = 1;
	    break;
	  case SELECT_HOST:
	    save_type = -1;
	    break;
	  }
	}
	break;
#endif /* HOST_READ_ONLY */
      }
      viewer_type = 4;
      viewer_init = 0;
      break;
    case DELETE_MODE:
      viewer_type = 0;
      viewer_init = 0;
      break;
    case ADD_MODE:
    case ALL_ADD:
      if((if_list + if_menu)->type & sceInetDevF_ARP){
	if((if_list + if_menu)->type & sceInetDevF_NIC){
	  env_data.type = 3; /* nic */
	}else{
	  env_data.type = 1; /* eth */
	}
      }else{
	env_data.type   = 2; /* ppp */
      }
      strcpy(env_data.vendor, (if_list + if_menu)->vendor);
      strcpy(env_data.product, (if_list + if_menu)->product);
      viewer_init = 2;
      break;
    default:
      Rpc_Init();
      Rpc_sceNetCnfLoadEntry(&env_data, 2, (dev_list + dev_menu)->usr_name, (dev_list + dev_menu)->select_device);
      viewer_init = 2;
      break;
    }
    break;

  case 2:
    switch(env_data.type){
    case 1: /* eth */
      if(viewer_mode == ADD_MODE || viewer_mode == ALL_ADD){
	env_data.phy_config = 1;
      }
      env_data.dialing_type = -1; /* none */
      env_data.idle_timeout = -1;
      strcpy(env_data.chat_additional, "");
      strcpy(env_data.outside_set, "");
      viewer_type = 3;
      viewer_init = 3;
      break;
    case 2: /* ppp */
      if(viewer_mode == ADD_MODE || viewer_mode == ALL_ADD){
	env_data.dialing_type =  0; /* Tone */
	env_data.idle_timeout = 10;
	strcpy(env_data.chat_additional, "");
	strcpy(env_data.outside_set, "");
      }
      env_data.phy_config = -1;
      viewer_type = 2;
      viewer_init = 3;
      break;
    case 3: /* nic */
      if(viewer_mode == ADD_MODE || viewer_mode == ALL_ADD){
	env_data.phy_config = 1;
      }
      env_data.dialing_type = -1; /* none */
      env_data.idle_timeout = -1;
      strcpy(env_data.chat_additional, "");
      strcpy(env_data.outside_set, "");
      viewer_type = 3;
      viewer_init = 3;
      break;
    }
    break;

    /**************************/
    /* デフォルト設定名の設定 */
    /**************************/
  case 3:
    if(viewer_mode == ADD_MODE || viewer_mode == ALL_ADD){
      /* デフォルト名の設定 */
      strcpy(dev_usr_name, env_data.vendor);
      strcat(dev_usr_name, "/");
      strcat(dev_usr_name, env_data.product);

      /* デフォルト名の重複チェック(vendor, product が同じで type が違う場合) */
      no = 1;
      strcpy(dev_usr_name_org, dev_usr_name);
    DEV_NAME_RE_CHECK:
      cmp_no = no;
      for(i = 0; i < dev_menu_max; i++){
	if(strcmp((dev_list + i)->usr_name, dev_usr_name) == 0) no++;
      }
      if(cmp_no != no){
	strcpy(dev_usr_name_tmp, dev_usr_name_org);
	sprintf(dev_usr_name, "%s%d", dev_usr_name_tmp, no);
	goto DEV_NAME_RE_CHECK;
      }
      if(env_data.type != 2){
	viewer_init = 4;
      }else{
	viewer_init = 0;
      }
    }else{
      strcpy(dev_usr_name, (dev_list + dev_menu)->usr_name);
      strcpy(dev_usr_name_bak, dev_usr_name);
      viewer_init = 0;
    }
    break;

    /**********************************/
    /* ハードウェアアドレスを取得する */
    /**********************************/
  case 4:
    Rpc_GetHWAddr((if_list + if_menu)->id);
    viewer_init = 5;
    break;

  case 5:
    if(Rpc_Get_Result() < 0){
      hwaddr_flag = 0;
    }else{
      p = (sceNetCnf_t *)sdata;
      for(i = 0; i < 6; i++){
	hwaddr[i] = p->hwaddr[i];
      }
      hwaddr_flag = 1;
    }
    viewer_init = 0;
    break;
  }
}

/**************************/
/* Dev ファイルを削除する */
/**************************/
static void
delete_dev(void)
{
  switch(delete_flag){
  case  1:
    Rpc_Init();
    Rpc_sceNetCnfDeleteEntry(2, (dev_list + dev_menu)->usr_name, (dev_list + dev_menu)->select_device);
    delete_flag = 2;
    break;

  case  2:
    if(Rpc_Get_Result() < 0){
      delete_flag = -1;
    }else{
      delete_flag = -2;
    }
    break;
  }
}

/***********************************/
/* Dev ファイルを Add or Edit する */
/***********************************/
static void
add_dev(void)
{
  static int addr = 0;
  static int id = 0;
  int type, free, format, cmd, result, r;

  switch(add_flag){
  case 1:
    switch(select_dev_device){
    case SELECT_MC:
      type = 1;
      free = 0;
      format = 1;
      if(mc_stat0 == 0 && mc_stat1 == 1){
	sceMcGetInfo(1, 0, &type, &free, &format);
      }else{
	sceMcGetInfo(0, 0, &type, &free, &format);
      }
      sceMcSync(0, &cmd, &result);
      /* Memory Card タイプのチェック */
      if(type != 2){
	add_flag      = -1;
	mc_error_type = -1; /* PlayStation 2 用メモリーカードじゃない */
      }else{
	/* Memory Card フォーマットのチェック */
	if(format == 0){
	  /* もう一度フォーマットされているかどうか確認する(TRC) */
	  if(mc_stat0 == 0 && mc_stat1 == 1){
	    sceMcGetInfo(1, 0, &type, &free, &format);
	  }else{
	    sceMcGetInfo(0, 0, &type, &free, &format);
	  }
	  sceMcSync(0, &cmd, &result);
	}
	if(format == 0){
	  add_flag = -4;
	}else{
	  Rpc_Init();
	  Rpc_AllocSysMemory(GET_SIZE(sizeof(sceNetCnfEnvData_t)));
	  add_flag = 2;
	}
      }
      break;
    case SELECT_HDD:
      Rpc_Init();
      Rpc_AllocSysMemory(GET_SIZE(sizeof(sceNetCnfEnvData_t)));
      add_flag = 2;
      break;
#ifndef HOST_READ_ONLY
    case SELECT_HOST:
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
      mc_error_type = -3; /* 設定ファイルが作成できない */
    }else{
      id = send_iop((int)&env_data, addr, sizeof(sceNetCnfEnvData_t), 0);
      add_flag = 3;
    }
    break;

  case 3:
    if(dma_check(id) < 0){
      switch(add_type){
      case 2:
	Rpc_sceNetCnfCopyEntry(2, (dev_list + dev_menu)->usr_name, (dev_list + dev_menu)->usr_name,
			       (dev_list + dev_menu)->select_device, select_dev_device);
	break;
      case 1:
	Rpc_sceNetCnfAddEntry((sceNetCnfEnvData_t *)addr, 2, dev_usr_name, select_dev_device);
	break;
      case 0:
	Rpc_sceNetCnfEditEntry((sceNetCnfEnvData_t *)addr, 2, dev_usr_name_bak, dev_usr_name, select_dev_device);
	break;
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

  case -2:
    /* Save 完了 */
    break;

  case -3:
    set_return(0, -1, DEFAULT, 1, 0, DEFAULT);
    viewer_init = 1;
    add_flag = 0;
    break;
  case -5:
    if(mc_stat0 == 0 && mc_stat1 == 1){
      sceMcFormat(1, 0);
    }else{
      sceMcFormat(0, 0);
    }
    add_flag = -6;
    break;
  case -6:
    if(sceMcSync(1, &cmd, &result)){
      if(result == 0){
	add_flag = 1;
      }else{
	add_flag      = -1;
	mc_error_type = -5; /* フォーマット失敗 */
      }
    }
    break;
  }
}
