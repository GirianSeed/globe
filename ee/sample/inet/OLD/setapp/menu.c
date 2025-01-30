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
 *                              menu.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.11      tetsu      Change for PPPoE
 */
#include "setapp.h"

#define FMAX0 (3)
#define FMAX1 (4)
#define FMAX2 (4)

/* Menu */
static char *disp_msg0[] = {"New Settings(Hardware Settings & Network Service Provider Settings)",
			    "Hardware Settings",
			    "Network Service Provider Settings"};
static char *disp_msg1[] = {"Add Hardware",
			    "Edit Hardware",
			    "Delete Hardware",
			    "Copy Hardware"};
static char *disp_msg2[] = {"Add Network Service Provider",
			    "Edit Network Service Provider",
			    "Delete Network Service Provider",
			    "Copy Network Service Provider"};

static int menu;	/* 現在選択している Menu の menu 番号 */

static int menu_pad(void);
static void initialize(void);

/************************************/
/* ネットワーク設定画面コントロール */
/************************************/
static int
menu_pad(void)
{
  u_short tpad;

  if((tpad = pad_read()) == 0) return (0);

  if(tpad & SCE_PADLup){
    switch(viewer_type){
    case 0:
      menu--;
      menu = LIMIT(menu, 0, FMAX0 - 1);
      break;
    case 1:
      menu--;
      menu = LIMIT(menu, 0, FMAX1 - 1);
      break;
    case 2:
      menu--;
      menu = LIMIT(menu, 0, FMAX2 - 1);
      break;
    }
  }
  if(tpad & SCE_PADLdown){
    switch(viewer_type){
    case 0:
      menu++;
      menu = LIMIT(menu, 0, FMAX0 - 1);
      break;
    case 1:
      menu++;
      menu = LIMIT(menu, 0, FMAX1 - 1);
      break;
    case 2:
      menu++;
      menu = LIMIT(menu, 0, FMAX2 - 1);
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
    if(get_return()){
      env_select      = 0;
      viewer_init     = 1;
      if(dev_select || ifc_select){
	list_update_arg = 12;
      }else{
	list_update_arg = 10;
      }
    }
  }
  if(tpad & SCE_PADRleft){
  }
  if(tpad & SCE_PADRright){
    switch(viewer_type){
    case 0:
      switch(menu){
      case 0:
	strcpy(list_msg, "Please select hardware that you want to add.");
	set_return(viewer_flag, viewer_type, viewer_mode, 2, 3, ALL_ADD);
	viewer_init          = 1;
	init_data(&env_data);
	init_data(&last_data);
	break;
      case 1:
	set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 1, viewer_mode);
	viewer_init = 1;
	break;
      case 2:
	set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 2, viewer_mode);
	viewer_init = 1;
	break;
      }
      break;
    case 1:
      switch(menu){
      case 0:
	strcpy(list_msg, "Please select hardware that you want to add.");
	set_return(viewer_flag, viewer_type, viewer_mode, 2, 0, ADD_MODE);
	viewer_init          = 1;
	break;
      case 1:
	strcpy(list_msg, "Please select hardware setting that you want to edit.");
	set_return(viewer_flag, viewer_type, viewer_mode, 2, 1, EDIT_MODE);
	viewer_init          = 1;
	break;
      case 2:
	strcpy(list_msg, "Please select hardware setting that you want to delete.");
	set_return(viewer_flag, viewer_type, viewer_mode, 2, 1, DELETE_MODE);
	viewer_init          = 1;
	break;
      case 3:
	strcpy(list_msg, "Please select hardware setting that you want to copy.");
	set_return(viewer_flag, viewer_type, viewer_mode, 2, 1, COPY_MODE);
	viewer_init          = 1;
	break;
      }
      break;
    case 2:
      switch(menu){
      case 0:
	strcpy(list_msg, "Please select hardware setting that you want to add.");
	set_return(viewer_flag, viewer_type, viewer_mode, 2, 1, ADD_MODE);
	viewer_init          = 1;
	break;
      case 1:
	strcpy(list_msg, "Please select NSP setting that you want to edit.");
	set_return(viewer_flag, viewer_type, viewer_mode, 2, 2, EDIT_MODE);
	viewer_init          = 1;
	break;
      case 2:
	strcpy(list_msg, "Please select NSP setting that you want to delete.");
	set_return(viewer_flag, viewer_type, viewer_mode, 2, 2, DELETE_MODE);
	viewer_init          = 1;
	break;
      case 3:
	strcpy(list_msg, "Please select NSP setting that you want to copy.");
	set_return(viewer_flag, viewer_type, viewer_mode, 2, 2, COPY_MODE);
	viewer_init          = 1;
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
menu_viewer(void)
{
  int i;

  if(Rpc_Check() == 0 && viewer_init) initialize();	/* 画面初期化 */
  if(viewer_init == 0) menu_pad();			/* コントローラ操作 */

  /* タイトル表示 */
  sceDevConsClear(console);
  sceDevConsLocate(console, 0, 0);
  sceDevConsPrintf(console, "Network Congiguration(menu)\n");
  sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
  sceDevConsPrintf(console, "\n" );

  /* メニュー表示 */
  switch(viewer_type){
  case 0:
    for(i = 0; i < FMAX0; i++){
      if(menu == i){
	sceDevConsPrintf(console,"* %s\n", disp_msg0[i]);
      }else{
	sceDevConsPrintf(console,"  %s\n", disp_msg0[i]);
      }
    }
    break;
  case 1:
    for(i = 0; i < FMAX1; i++){
      if(menu == i){
	sceDevConsPrintf(console,"* %s\n", disp_msg1[i]);
      }else{
	sceDevConsPrintf(console,"  %s\n", disp_msg1[i]);
      }
    }
    break;
  case 2:
    for(i = 0; i < FMAX2; i++){
      if(menu == i){
	sceDevConsPrintf(console,"* %s\n", disp_msg2[i]);
      }else{
	sceDevConsPrintf(console,"  %s\n", disp_msg2[i]);
      }
    }
    break;
  }

  /* Msg. */
  switch(viewer_type){
  case 0:
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "\n  Please select menu.\n");
    sceDevConsAttribute(console, WHITE);
    break;
  case 1:
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "\n  Hardware Setting, Please select menu.\n");
    sceDevConsAttribute(console, WHITE);
    break;
  case 2:
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "\n  NSP Setting, Please select menu.\n");
    sceDevConsAttribute(console, WHITE);
    break;
  }

  sceDevConsLocate(console, 0, 18);
  sceDevConsPrintf(console, "  Circle button : Select\n");
  sceDevConsPrintf(console, "  Eks button    : Cancel\n");

  /* 描画 */
  sceDevConsDraw(console);
}

/********************************/
/* ネットワーク設定画面の初期化 */
/********************************/
static void
initialize(void){
  /* connect_viewer initialize */
  if(viewer_init == 1){
    menu         = 0;
    viewer_init  = 0;
  }
}
