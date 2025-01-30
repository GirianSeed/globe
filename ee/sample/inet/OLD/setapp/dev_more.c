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
 *                          dev_more .c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.16      tetsu      Change for PPPoE
 */
#include "setapp.h"

int dev_more_edit;	/* Dev More Edit Flag */
int dev_more_edit2;	/* Dev More Edit Flag2 */

int idle_timeout_flag;	/* 回線切断設定フラグ */

/* カーソル情報 */
static int cur_init_flag;
static int cx, cx_min, cx_max;
static int cy, cy_min, cy_max;

static int dev_more_pad(void);
static void initialize(void);
static void cur_init(int, int, int, int);

/************************************/
/* ネットワーク設定画面コントロール */
/************************************/
static int
dev_more_pad(void)
{
  u_short tpad;

  if((tpad = pad_read()) == 0) return (0);

  if(tpad & SCE_PADLup){
    if(dev_more_edit){
      dev_more_edit2 = 1;
      disp_char_no++;
      if(CMAX <= disp_char_no) disp_char_no = 0;
      if(disp_char_no < 0) disp_char_no = CMAX - 1;
    }else{
      switch(viewer_type){
      case 0:
	if(idle_timeout_flag){
	  env_data.idle_timeout++;
	  env_data.idle_timeout = LIMIT(env_data.idle_timeout, 0, 90);
	  break;
	}
      case 1:
	cy--;
	cy = LIMIT(cy, cy_min, cy_max);
	cur_init_flag = 2;
	break;
      }
    }
  }
  if(tpad & SCE_PADLdown){
    if(dev_more_edit){
      dev_more_edit2 = 1;
      disp_char_no--;
      if(CMAX <= disp_char_no) disp_char_no = 0;
      if(disp_char_no < 0) disp_char_no = CMAX - 1;
    }else{
      switch(viewer_type){
      case 0:
	if(idle_timeout_flag){
	  env_data.idle_timeout--;
	  env_data.idle_timeout = LIMIT(env_data.idle_timeout, 0, 90);
	  break;
	}
      case 1:
	cy++;
	cy = LIMIT(cy, cy_min, cy_max);
	cur_init_flag = 2;
	break;
      }
    }
  }
  if(tpad & SCE_PADLleft){
    if(dev_more_edit){
      dev_more_edit2 = 0;
      switch(viewer_type){
      case 0:
	switch(cy){
	case 3:
	  write_char(env_data.chat_additional, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 4:
	  write_char(env_data.outside_set, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      }
      cx--;
      cx = LIMIT(cx, cx_min, cx_max);
    }
  }
  if(tpad & SCE_PADLright){
    if(dev_more_edit){
      dev_more_edit2 = 0;
      switch(viewer_type){
      case 0:
	switch(cy){
	case 3:
	  write_char(env_data.chat_additional, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 4:
	  write_char(env_data.outside_set, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      }
      cx++;
      cx = LIMIT(cx, cx_min, cx_max);
    }
  }
  if(tpad & SCE_PADRup){
    if(dev_more_edit){
      dev_more_edit2 = 0;
      switch(viewer_type){
      case 0:
	switch(cy){
	case 3:
	  insert_char(env_data.chat_additional, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 4:
	  insert_char(env_data.outside_set, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	}
	break;
      }
    }else{
      switch(viewer_type){
      case 0:
	switch(cy){
	case 3:
	case 4:
	  dev_more_edit = 1;
	  dev_more_edit2 = 0;
	  break;
	case 5:
	  idle_timeout_flag = 1;
	  break;
	}
	break;
      }
    }
  }
  if(tpad & SCE_PADRdown){
    if(dev_more_edit || idle_timeout_flag){
      dev_more_edit = 0;
      idle_timeout_flag = 0;
    }else{
      get_return();
      cur_init_flag = 1;
    }
  }
  if(tpad & SCE_PADRleft){
    if(dev_more_edit){
      dev_more_edit2 = 0;
      switch(viewer_type){
      case 0:
	switch(cy){
	case 3:
	  delete_char(env_data.chat_additional, cx - cx_min);
	  break;
	case 4:
	  delete_char(env_data.outside_set, cx - cx_min);
	  break;
	}
	break;
      }
    }else{
      switch(viewer_type){
      case 0:
	switch(cy){
	case 3:
	case 4:
	case 5:
	  viewer_init = 2;
	  break;
	}
	break;
      }
    }
  }
  if(tpad & SCE_PADRright){
    if(dev_more_edit){
      dev_more_edit2 = 0;
      switch(viewer_type){
      case 0:
	switch(cy){
	case 3:
	  write_char(env_data.chat_additional, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 4:
	  write_char(env_data.outside_set, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      }
      cx++;
      cx = LIMIT(cx, cx_min, cx_max);
    }else{
      switch(viewer_type){
      case 0:
	get_return();
	cur_init_flag = 1;
	break;
      case 1:
	switch(cy){
	case 3:
	  env_data.phy_config = sceNetCnf_PHYCONFIG_AUTO;
	  break;
	case 4:
	  env_data.phy_config = sceNetCnf_PHYCONFIG_10;
	  break;
	case 5:
	  env_data.phy_config = sceNetCnf_PHYCONFIG_10_FD;
	  break;
	case 6:
	  env_data.phy_config = sceNetCnf_PHYCONFIG_TX;
	  break;
	case 7:
	  env_data.phy_config = sceNetCnf_PHYCONFIG_TX_FD;
	  break;
	}
	get_return();
	cur_init_flag = 1;
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
  }
  if(tpad & SCE_PADselect){
  }

  return (1);
}

/************************/
/* ネットワーク設定画面 */
/************************/
void
dev_more_viewer(void)
{
  static int default_cy;

  if(Rpc_Check() == 0 && 0 != viewer_init) initialize();	/* 画面初期化 */

  if(viewer_init == 0) dev_more_pad();				/* コントローラ操作 */

  /* タイトル表示 */
  sceDevConsClear(console);
  sceDevConsLocate(console, 0, 0);
  sceDevConsPrintf(console, "Network Configuration(dev_more)\n");
  sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
  sceDevConsPrintf(console, "\n" );

  if(viewer_init){
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "Now Loading...\n");
    sceDevConsAttribute(console, WHITE);
  }else{
    switch(viewer_type){
      /******************************************************/
      /* 追加 AT コマンド、外線発信番号、回線切断を設定する */
      /******************************************************/
    case 0:
      if(cur_init_flag) cur_init(26, 71, 3, 5);
      /* 追加 AT コマンド */
      if(cy == 3){
	if(dev_more_edit == 0 && env_data.chat_additional[0] == '\0'){
	  sceDevConsPrintf(console, "* Additional AT Command : (no setting)\n");
	}else{
	  sceDevConsPrintf(console, "* Additional AT Command : %s\n", env_data.chat_additional);
	}
      }else{
	if(env_data.chat_additional[0] == '\0'){
	  sceDevConsPrintf(console, "  Additional AT Command : (no setting)\n");
	}else{
	  sceDevConsPrintf(console, "  Additional AT Command : %s\n", env_data.chat_additional);
	}
      }
      /* 外線発信番号 */
      if(cy == 4){
	if(dev_more_edit == 0 && env_data.outside_set[0] == '\0'){
	  sceDevConsPrintf(console, "* Outside Number        : (no setting)\n");
	}else{
	  sceDevConsPrintf(console, "* Outside Number        : %s\n", env_data.outside_set);
	}
      }else{
	if(env_data.outside_set[0] == '\0'){
	  sceDevConsPrintf(console, "  Outside Number        : (no setting)\n");
	}else{
	  sceDevConsPrintf(console, "  Outside Number        : %s\n", env_data.outside_set);
	}
      }
      /* 回線切断 */
      if(cy == 5){
	sceDevConsPrintf(console, "* Line Timeout          : ");
	sceDevConsAttribute(console, (idle_timeout_flag ? YELLOW : WHITE));
	sceDevConsPrintf(console, "%2d", env_data.idle_timeout);
	sceDevConsAttribute(console, WHITE);
	sceDevConsPrintf(console, " (min) ... Max 90 min, 0 is no disconnect.\n");
      }else{
	sceDevConsPrintf(console, "  Line Timeout          : %2d (min) ... Max 90 min, 0 is no disconnect.\n", env_data.idle_timeout);
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "\n");
      sceDevConsAttribute(console, YELLOW);
      sceDevConsPrintf(console, "  Please set detailed information.\n");
      sceDevConsAttribute(console, WHITE);
      sceDevConsLocate(console, 0, 18);
      sceDevConsPrintf(console, "  Square button : Restore Defaults\n");
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;

      /************************************/
      /* Ether,Nic の動作モードを設定する */
      /************************************/
    case 1:
      if(cur_init_flag){
	default_cy = 0;
	if(cur_init_flag == 1 && viewer_mode == EDIT_MODE){
	  default_cy = env_data.phy_config + 2;
	}
	cur_init(0, 71, 3, 7);
	if(default_cy){
	  cy = default_cy;
	  default_cy = 0;
	}
      }
      switch(cy){
      case 3:
	sceDevConsPrintf(console, "* Auto\n");
	sceDevConsPrintf(console, "  10BaseT Half-Duplex\n");
	sceDevConsPrintf(console, "  10BaseT Full-Duplex\n");
	sceDevConsPrintf(console, "  100BaseTX Half-Duplex\n");
	sceDevConsPrintf(console, "  100BaseTX Full-Duplex\n");
	break;
      case 4:
	sceDevConsPrintf(console, "  Auto\n");
	sceDevConsPrintf(console, "* 10BaseT Half-Duplex\n");
	sceDevConsPrintf(console, "  10BaseT Full-Duplex\n");
	sceDevConsPrintf(console, "  100BaseTX Half-Duplex\n");
	sceDevConsPrintf(console, "  100BaseTX Full-Duplex\n");
	break;
      case 5:
	sceDevConsPrintf(console, "  Auto\n");
	sceDevConsPrintf(console, "  10BaseT Half-Duplex\n");
	sceDevConsPrintf(console, "* 10BaseT Full-Duplex\n");
	sceDevConsPrintf(console, "  100BaseTX Half-Duplex\n");
	sceDevConsPrintf(console, "  100BaseTX Full-Duplex\n");
	break;
      case 6:
	sceDevConsPrintf(console, "  Auto\n");
	sceDevConsPrintf(console, "  10BaseT Half-Duplex\n");
	sceDevConsPrintf(console, "  10BaseT Full-Duplex\n");
	sceDevConsPrintf(console, "* 100BaseTX Half-Duplex\n");
	sceDevConsPrintf(console, "  100BaseTX Full-Duplex\n");
	break;
      case 7:
	sceDevConsPrintf(console, "  Auto\n");
	sceDevConsPrintf(console, "  10BaseT Half-Duplex\n");
	sceDevConsPrintf(console, "  10BaseT Full-Duplex\n");
	sceDevConsPrintf(console, "  100BaseTX Half-Duplex\n");
	sceDevConsPrintf(console, "* 100BaseTX Full-Duplex\n");
	break;
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsAttribute(console, YELLOW);
      sceDevConsPrintf(console, "  Please select mode.\n");
      sceDevConsAttribute(console, WHITE);
      sceDevConsLocate(console, 0, 18);
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;
    }

    /******************/
    /* カーソルの表示 */
    /******************/
    if(dev_more_edit){
      if(dev_more_edit2){
	sceDevConsLocate(console, cx, cy);
	if(ct > 30){
	  sceDevConsPut(console, '_', YELLOW);
	}else{
	  sceDevConsPut(console, disp_char[disp_char_no], YELLOW);
	}
      }else{
	sceDevConsLocate(console, cx, cy);
	disp_char_no = search_char((sceDevConsGet(console) & 0x00ff));
	sceDevConsLocate(console, cx, cy);
	if(ct > 30){
	  sceDevConsPut(console, '_', YELLOW);
	}else{
	  sceDevConsPut(console, disp_char[disp_char_no], YELLOW);
	}
      }
    }
  }

  /* 描画 */
  sceDevConsDraw(console);
}

/*****************************/
/* Dev More 画面を初期化する */
/*****************************/
static void
initialize(void)
{
  switch(viewer_init){
  case 1:
    /* all mode common initialize */
    dev_more_edit = 0;
    dev_more_edit2 = 0;
    idle_timeout_flag = 0;
    cur_init_flag = 1;
    if(viewer_mode == ADD_MODE || viewer_mode == ALL_ADD){
      viewer_init = 2;
    }else{
      viewer_init = 0;
    }
    break;

  case 2:
    /* set default */
    strcpy(env_data.chat_additional, "");
    strcpy(env_data.outside_set, "");
    env_data.idle_timeout = 10;
    viewer_init = 0;
    break;
  }
}

/************************/
/* カーソル位置の初期化 */
/************************/
static void
cur_init(int xmin, int xmax, int ymin, int ymax)
{
  switch(cur_init_flag){
  case 1:
    cy     = ymin;
    cy_min = ymin;
    cy_max = ymax;
  case 2:
    cx     = xmin;
    cx_min = xmin;
    cx_max = xmax;
    break;
  }
  cur_init_flag = 0;
}
