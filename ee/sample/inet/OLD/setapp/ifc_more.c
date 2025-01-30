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
 *                          ifc_more.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.16      tetsu      Change for PPPoE
 */
#include "setapp.h"

int ifc_more_edit;			/* Ifc Edit Flag */
int ifc_more_edit2;			/* Ifc Edit Flag2 */

/* カーソル情報 */
static int cur_init_flag;
static int cx, cx_min, cx_max;
static int cy, cy_min, cy_max;

static int ifc_more_pad(void);
static void initialize(void);
static void cur_init(int, int, int, int);

/************************************/
/* ネットワーク設定画面コントロール */
/************************************/
static int
ifc_more_pad(void)
{
  u_short tpad;

  if((tpad = pad_read()) == 0) return (0);

  if(tpad & SCE_PADLup){
    if(ifc_more_edit){
      ifc_more_edit2 = 1;
      disp_char_no++;
      if(CMAX <= disp_char_no) disp_char_no = 0;
      if(disp_char_no < 0) disp_char_no = CMAX - 1;
    }else{
      switch(viewer_type){
      case 0:
      case 1:
	cy--;
	cy = LIMIT(cy, cy_min, cy_max);
	cur_init_flag = 2;
	break;
      }
    }
  }
  if(tpad & SCE_PADLdown){
    if(ifc_more_edit){
      ifc_more_edit2 = 1;
      disp_char_no--;
      if(CMAX <= disp_char_no) disp_char_no = 0;
      if(disp_char_no < 0) disp_char_no = CMAX - 1;
    }else{
      switch(viewer_type){
      case 0:
      case 1:
	cy++;
	cy = LIMIT(cy, cy_min, cy_max);
	cur_init_flag = 2;
	break;
      }
    }
  }
  if(tpad & SCE_PADLleft){
    if(ifc_more_edit){
      ifc_more_edit2 = 0;
      switch(viewer_type){
      case 0:
      case 1:
	switch(cy){
	case 4:
	  write_char(env_data.dhcp_host_name, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 12:
	  write_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 13:
	  write_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      }
      cx--;
      cx = LIMIT(cx, cx_min, cx_max);
    }
  }
  if(tpad & SCE_PADLright){
    if(ifc_more_edit){
      ifc_more_edit2 = 0;
      switch(viewer_type){
      case 0:
      case 1:
	switch(cy){
	case 4:
	  write_char(env_data.dhcp_host_name, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 12:
	  write_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 13:
	  write_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      }
      cx++;
      cx = LIMIT(cx, cx_min, cx_max);
    }
  }
  if(tpad & SCE_PADRup){
    if(ifc_more_edit){
      ifc_more_edit2 = 0;
      switch(viewer_type){
      case 0:
      case 1:
	switch(cy){
	case 4:
	  insert_char(env_data.dhcp_host_name, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 12:
	  insert_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 13:
	  insert_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	}
	break;
      }
    }else{
      switch(viewer_type){
      case 0:
      case 1:
	switch(cy){
	case 4:
	case 12:
	case 13:
	  ifc_more_edit = 1;
	  ifc_more_edit2 = 0;
	  break;
	}
	break;
      }
    }
  }
  if(tpad & SCE_PADRdown){
    if(ifc_more_edit){
      ifc_more_edit = 0;
    }else{
      get_return();
      cur_init_flag = 1;
    }
  }
  if(tpad & SCE_PADRleft){
    if(ifc_more_edit){
      ifc_more_edit2 = 0;
      switch(viewer_type){
      case 0:
      case 1:
	switch(cy){
	case 4:
	  delete_char(env_data.dhcp_host_name, cx - cx_min);
	  break;
	case 12:
	  delete_char(env_data.dns1_address, cx - cx_min);
	  break;
	case 13:
	  delete_char(env_data.dns2_address, cx - cx_min);
	  break;
	}
	break;
      }
    }else{
      switch(viewer_type){
      case 0:
	strcpy(env_data.dhcp_host_name, "");
	break;
      case 1:
	strcpy(env_data.dns1_address, "");
	strcpy(env_data.dns2_address, "");
	break;
      }
    }
  }
  if(tpad & SCE_PADRright){
    if(ifc_more_edit){
      ifc_more_edit2 = 0;
      switch(viewer_type){
      case 0:
      case 1:
	switch(cy){
	case 4:
	  write_char(env_data.dhcp_host_name, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 12:
	  write_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 13:
	  write_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      }
      cx++;
      cx = LIMIT(cx, cx_min, cx_max);
    }else{
      switch(viewer_type){
      case 0:
      case 1:
	get_return();
	cur_init_flag = 1;
	break;
      }
    }
  }
  if(tpad & SCE_PADL1){
    viewer_type = viewer_type ? 0 : 1;
    viewer_init = 1;
  }
  if(tpad & SCE_PADL2){
    viewer_type = viewer_type ? 0 : 1;
    viewer_init = 1;
  }
  if(tpad & SCE_PADR1){
    viewer_type = viewer_type ? 0 : 1;
    viewer_init = 1;
  }
  if(tpad & SCE_PADR2){
    viewer_type = viewer_type ? 0 : 1;
    viewer_init = 1;
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
ifc_more_viewer(void)
{
  static int default_cy;

  if(Rpc_Check() == 0 && 0 != viewer_init) initialize();	/* 画面初期化 */

  if(viewer_init == 0) ifc_more_pad();				/* コントローラ操作 */

  /* タイトル表示 */
  sceDevConsClear(console);
  sceDevConsLocate(console, 0, 0);
  sceDevConsPrintf(console, "Network Configuration(ifc_more)\n");
  sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
  sceDevConsPrintf(console, "\n" );

  if(viewer_init){
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "Now Loading...\n");
    sceDevConsAttribute(console, WHITE);
  }else{
    switch(viewer_type){
      /*************************************/
      /* DHCP ホスト名とDNS アドレスの設定 */
      /*************************************/
    case 0:
      if(cur_init_flag){
	default_cy = 0;
	if(cur_init_flag == 1 && env_data.dhcp_host_name[0] == '\0'){
	  default_cy = 5;
	}
	cur_init(23, 71, 3, 5);
	if(default_cy){
	  cy = default_cy;
	  default_cy = 0;
	}
      }
      switch(cy){
      case 3:
	sceDevConsPrintf(console, "* ");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "Set\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsPrintf(console, "      DHCP Host Name : %s\n", env_data.dhcp_host_name);
	sceDevConsPrintf(console, "  Don't Set\n");
	sceDevConsPrintf(console, "\n");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Would you like to set DHCP Host Name?\n");
	sceDevConsAttribute(console, WHITE);
	break;
      case 4:
	sceDevConsPrintf(console, "* ");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "Set\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsPrintf(console, "    ->DHCP Host Name : %s\n", env_data.dhcp_host_name);
	sceDevConsPrintf(console, "  Don't Set\n");
	sceDevConsPrintf(console, "\n");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Would you like to set DHCP Host Name?\n");
	sceDevConsAttribute(console, WHITE);
	break;
      case 5:
	sceDevConsPrintf(console, "  Set\n");
	sceDevConsPrintf(console, "\n");
	sceDevConsPrintf(console, "* ");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "Don't Set\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsPrintf(console, "\n");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Would you like to set DHCP Host Name?\n");
	sceDevConsAttribute(console, WHITE);
	break;
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "\n");
      if(env_data.dns1_address[0] != '\0' || env_data.dns2_address[0] != '\0'){
	sceDevConsPrintf(console, "  Auto\n");
	sceDevConsPrintf(console, "* Manual\n");
	sceDevConsPrintf(console, "      Primary DNS Server Address   : %s\n", env_data.dns1_address);
	sceDevConsPrintf(console, "      Secondary DNS Server Address : %s\n", env_data.dns2_address);
      }else{
	sceDevConsPrintf(console, "* Auto\n");
	sceDevConsPrintf(console, "  Manual\n");
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "  Please select, how to get DNS Server Address.\n");
      break;

    case 1:
      if(cur_init_flag){
	default_cy = 0;
	if(cur_init_flag == 1 && (env_data.dns1_address[0] == '\0' && env_data.dns2_address[0] == '\0')){
	  default_cy = 10;
	}
	cur_init(37, 71, 10, 13);
	if(default_cy){
	  cy = default_cy;
	  default_cy = 0;
	}
      }
      if(env_data.dhcp_host_name[0] != 0){
	sceDevConsPrintf(console, "* Set\n");
	sceDevConsPrintf(console, "      DHCP Host Name : %s\n", env_data.dhcp_host_name);
	sceDevConsPrintf(console, "  Don't Set\n");
      }else{
	sceDevConsPrintf(console, "  Set\n");
	sceDevConsPrintf(console, "\n");
	sceDevConsPrintf(console, "* Don't Set\n");
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "  Would you like to set DHCP Host Name?\n");
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "\n");
      switch(cy){
      case 10:
	sceDevConsPrintf(console, "* ");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "Auto\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsPrintf(console, "  Manual\n");
	sceDevConsPrintf(console, "\n");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Please select, how to get DNS Server Address.\n");
	sceDevConsAttribute(console, WHITE);
	break;
     case 11:
	sceDevConsPrintf(console, "  Auto\n");
	sceDevConsPrintf(console, "* ");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "Manual\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsPrintf(console, "      Primary DNS Server Address   : %s\n", env_data.dns1_address);
	sceDevConsPrintf(console, "      Secondary DNS Server Address : %s\n", env_data.dns2_address);
	sceDevConsPrintf(console, "\n");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Please select, how to get DNS Server Address.\n");
	sceDevConsAttribute(console, WHITE);
	break;
      case 12:
	sceDevConsPrintf(console, "  Auto\n");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "* Manual\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsPrintf(console, "   -> Primary DNS Server Address   : %s\n", env_data.dns1_address);
	sceDevConsPrintf(console, "      Secondary DNS Server Address : %s\n", env_data.dns2_address);
	sceDevConsPrintf(console, "\n");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Please set DNS Server Address.\n");
	sceDevConsAttribute(console, WHITE);
	break;
      case 13:
	sceDevConsPrintf(console, "  Auto\n");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "* Manual\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsPrintf(console, "      Primary DNS Server Address   : %s\n", env_data.dns1_address);
	sceDevConsPrintf(console, "   -> Secondary DNS Server Address : %s\n", env_data.dns2_address);
	sceDevConsPrintf(console, "\n");
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  Please set DNS Server Address.\n");
	sceDevConsAttribute(console, WHITE);
	break;
      }
      break;
    }
    sceDevConsLocate(console, 0, 18);
    sceDevConsPrintf(console, "  Square button : Restore Defaults\n");
    sceDevConsPrintf(console, "  Circle button : Done\n");
    sceDevConsPrintf(console, "  Eks button    : Cancel\n");

    /******************/
    /* カーソルの表示 */
    /******************/
    if(ifc_more_edit){
      if(ifc_more_edit2){
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
/* Ifc_More 画面を初期化する */
/*****************************/
static void
initialize(void)
{
  switch(viewer_init){
    /**************************/
    /* モードにより動作を変更 */
    /**************************/
  case 1:
    /* all mode common initialize */
    ifc_more_edit = 0;
    ifc_more_edit2 = 0;
    cur_init_flag = 1;
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
