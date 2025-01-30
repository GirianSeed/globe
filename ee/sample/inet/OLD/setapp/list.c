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
 *                              list.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.16      tetsu      Change for PPPoE
 */
#include "setapp.h"

char list_msg[MAX_LEN];			/* List Message */

int if_menu;				/* 現在選択している I/F Menu の menu 番号 */
int if_menu_max;			/* I/F Menu の最大値 */
int if_mode;				/* I/F List のモード */

static int *menu;
static int *menu_max;

/* Network Interface */
sceInetInterfaceList_t *if_list;	/* Network Interface ID + vendor,product のリスト */

static int list_pad(void);
static void initialize(void);

/************************************/
/* ネットワーク設定画面コントロール */
/************************************/
static int
list_pad(void)
{
  u_short tpad;
#ifdef HOST_READ_ONLY
  int i;
#endif /* HOST_READ_ONLY */

  if((tpad = pad_read()) == 0) return (0);

  if(tpad & SCE_PADLup){
#ifdef HOST_READ_ONLY
    if(viewer_mode == COPY_MODE){
      /* COPY_MODE の場合は全ての設定を選択できる */
      (*menu)--;
      (*menu) = LIMIT(*menu, 0, ((*menu_max) ? ((*menu_max) - 1) : 0));
    }else{
      /* COPY_MODE 以外の場合は HOST は書き込み不可なので選択できない */
      switch(viewer_type){
      case 0:
      case 3:
	if(if_list){
	  if(0 <= (*menu) - 1){
	    for(i = (*menu) - 1; 0 <= i; i--){
	      if((if_list + i)->select_device != SELECT_HOST){
		(*menu) = i;
		break;
	      }
	    }
	  }
	}
	break;
      case 1:
	if(dev_list){
	  if(0 <= (*menu) - 1){
	    for(i = (*menu) - 1; 0 <= i; i--){
	      if((dev_list + i)->select_device != SELECT_HOST){
		(*menu) = i;
		break;
	      }
	    }
	  }
	}
	break;
      case 2:
	if(ifc_list){
	  if(0 <= (*menu) - 1){
	    for(i = (*menu) - 1; 0 <= i; i--){
	      if((ifc_list + i)->select_device != SELECT_HOST){
		(*menu) = i;
		break;
	      }
	    }
	  }
	}
	break;
      }
    }
#else /* HOST_READ_ONLY */
    /* 全てのモードで選択できる */
    (*menu)--;
    (*menu) = LIMIT(*menu, 0, ((*menu_max) ? ((*menu_max) - 1) : 0));
#endif /* HOST_READ_ONLY */
  }
  if(tpad & SCE_PADLdown){
#ifdef HOST_READ_ONLY
    if(viewer_mode == COPY_MODE){
      /* COPY_MODE の場合は全ての設定を選択できる */
      (*menu)++;
      (*menu) = LIMIT(*menu, 0, ((*menu_max) ? ((*menu_max) - 1) : 0));
    }else{
      /* COPY_MODE 以外の場合は HOST は書き込み不可なので選択できない */
      switch(viewer_type){
      case 0:
      case 3:
	if(if_list){
	  if((*menu) + 1 < (*menu_max)){
	    for(i = (*menu) + 1; i < (*menu_max); i++){
	      if((if_list + i)->select_device != SELECT_HOST){
		(*menu) = i;
		break;
	      }
	    }
	  }
	}
	break;
      case 1:
	if(dev_list){
	  if((*menu) + 1 < (*menu_max)){
	    for(i = (*menu) + 1; i < (*menu_max); i++){
	      if((dev_list + i)->select_device != SELECT_HOST){
		(*menu) = i;
		break;
	      }
	    }
	  }
	}
	break;
      case 2:
	if(ifc_list){
	  if((*menu) + 1 < (*menu_max)){
	    for(i = (*menu) + 1; i < (*menu_max); i++){
	      if((ifc_list + i)->select_device != SELECT_HOST){
		(*menu) = i;
		break;
	      }
	    }
	  }
	}
	break;
      }
    }
#else /* HOST_READ_ONLY */
    /* 全てのモードで選択できる */
    (*menu)++;
    (*menu) = LIMIT(*menu, 0, ((*menu_max) ? ((*menu_max) - 1) : 0));
#endif /* HOST_READ_ONLY */
  }
  if(tpad & SCE_PADLleft){
  }
  if(tpad & SCE_PADLright){
  }
  if(tpad & SCE_PADRup){
  }
  if(tpad & SCE_PADRdown){
    if(get_return()){
      viewer_init = 1;
    }
  }
  if(tpad & SCE_PADRleft){
  }
  if(tpad & SCE_PADRright){
    if(*menu != -1){
      switch(viewer_type){
      case 0:
	switch(viewer_mode){
	case ADD_MODE:
	  /* 接続機器設定の追加 */
	  set_return(viewer_flag, viewer_type, viewer_mode, 3, -1, viewer_mode);
	  select_dev_device = (if_list + *menu)->select_device;
	  viewer_init       = 1;
	  break;
	}
	break;

      case 1:
	switch(viewer_mode){
	case ADD_MODE:
	  /* 接続プロバイダ設定の追加 */
	  set_return(viewer_flag, viewer_type, viewer_mode, 4, -1, viewer_mode);
	  select_ifc_device = (dev_list + *menu)->select_device;
	  dev_select        = 2;
	  viewer_init       = 1;
	  break;

	case EDIT_MODE:
	  /* 接続機器設定の編集 */
	  set_return(viewer_flag, viewer_type, viewer_mode, 3, -1, viewer_mode);
	  select_dev_device = (dev_list + *menu)->select_device;
	  dev_select        = 2;
	  viewer_init       = 1;
	  break;

	case DELETE_MODE:
	  /* 接続機器設定の削除 */
	  set_return(viewer_flag, viewer_type, viewer_mode, 3, -1, viewer_mode);
	  select_dev_device = (dev_list + *menu)->select_device;
	  viewer_init       = 1;
	  break;

	case COPY_MODE:
	  /* 接続機器設定の複製 */
	  set_return(viewer_flag, viewer_type, NO_RETURN, 3, -1, viewer_mode);
	  select_dev_device = NO_SELECT;
	  viewer_init       = 1;
	  break;
	}
	break;

      case 2:
	switch(viewer_mode){
	case EDIT_MODE:
	  /* 接続プロバイダ設定の編集 */
	  set_return(viewer_flag, viewer_type, viewer_mode, 4, -1, viewer_mode);
	  select_ifc_device = (ifc_list + *menu)->select_device;
	  viewer_init       = 1;
	  break;

	case DELETE_MODE:
	  /* 接続プロバイダ設定の削除 */
	  set_return(viewer_flag, viewer_type, viewer_mode, 4, -1, viewer_mode);
	  select_ifc_device = (ifc_list + *menu)->select_device;
	  viewer_init       = 1;
	  break;

	case COPY_MODE:
	  /* 接続プロバイダ設定の複製 */
	  set_return(viewer_flag, viewer_type, NO_RETURN, 4, -1, viewer_mode);
	  select_ifc_device = NO_SELECT;
	  viewer_init       = 1;
	  break;
	}
	break;

      case 3:
	switch(viewer_mode){
	case ALL_ADD:
	  /* 新規作成(接続機器設定と接続プロバイダ設定の追加) */
	  if((if_list + *menu)->id != -1){
	    if((if_list + *menu)->flag){
	      set_return(viewer_flag, viewer_type, viewer_mode, 4, -1, viewer_mode);
	      if((if_list + if_menu)->type & sceInetDevF_ARP){
		if((if_list + if_menu)->type & sceInetDevF_NIC){
		  env_data.type = 3; /* nic */
		}else{
		  env_data.type = 1; /* eth */
		}
	      }else{
		env_data.type = 2;   /* ppp */
	      }
	      strcpy(dev_usr_name, (if_list + if_menu)->vendor);
	      strcat(dev_usr_name, "/");
	      strcat(dev_usr_name, (if_list + if_menu)->product);
	    }else{
	      set_return(viewer_flag, viewer_type, viewer_mode, 3, -1, viewer_mode);
	    }
	    select_dev_device = (if_list + *menu)->select_device;
	    select_ifc_device = (if_list + *menu)->select_device;
	    viewer_init       = 1;
	  }
	  break;
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
  }
  if(tpad & SCE_PADselect){
  }

  return (1);
}

/************************/
/* ネットワーク設定画面 */
/************************/
void
list_viewer(void)
{
  int i;

  /* 状態チェック */
  switch(status){
  case IEV_Attach:
  case IEV_Detach:
  case IEV_Conf:
  case IEV_NoConf:
    viewer_init = 1;
  }
  status = -1;

  if(Rpc_Check() == 0 && viewer_init) initialize();	/* 画面初期化 */
  if(viewer_init == 0) list_pad();			/* コントローラ操作 */

  /* タイトル表示 */
  sceDevConsClear(console);
  sceDevConsLocate(console, 0, 0);
  sceDevConsPrintf(console, "Network Congiguration(list)\n");
  sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
  sceDevConsPrintf(console, "\n" );

  /* メニュー表示 */
  if(viewer_init){
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "Now Loading...\n");
    sceDevConsAttribute(console, WHITE);
  }else{
    if(*menu == -1){
      sceDevConsPrintf(console,"  (no list)\n");
    }else{
      for(i = 0; i < *menu_max; i++){
#ifdef HOST_READ_ONLY
	if(viewer_mode != COPY_MODE){
	  switch(viewer_type){
	  case 0:
	  case 3:
	    if(if_list){
	      if((if_list + i)->select_device == SELECT_HOST) continue;
	    }
	    break;
	  case 1:
	    if(dev_list){
	      if((dev_list + i)->select_device == SELECT_HOST) continue;
	    }
	    break;
	  case 2:
	    if(ifc_list){
	      if((ifc_list + i)->select_device == SELECT_HOST) continue;
	    }
	    break;
	  }
	}
#endif /* HOST_READ_ONLY */
	if(*menu == i){
	  switch(viewer_type){
	  case 0:
	    if(if_list){
	      sceDevConsPrintf(console,"* ");
	      print_device(console, (if_list + i)->select_device);
	      sceDevConsPrintf(console,"%s/%s\n", (if_list + i)->vendor, (if_list + i)->product);
	    }
	    break;
	  case 1:
	    if(dev_list){
	      sceDevConsPrintf(console,"* ");
	      print_device(console, (dev_list + i)->select_device);
	      sceDevConsPrintf(console,"%s\n", (dev_list + i)->usr_name);
	    }
	    break;
	  case 2:
	    if(ifc_list){
	      sceDevConsPrintf(console,"* ");
	      print_device(console, (ifc_list + i)->select_device);
	      sceDevConsPrintf(console,"%s\n", (ifc_list + i)->usr_name);
	    }
	    break;
	  case 3:
	    if(if_list){
	      sceDevConsPrintf(console,"* ");
	      sceDevConsAttribute(console, ((if_list + i)->id == -1) ? BLUE : WHITE);
	      print_device(console, (if_list + i)->select_device);
	      sceDevConsPrintf(console,"%s/%s\n", (if_list + i)->vendor, (if_list + i)->product);
	      sceDevConsAttribute(console, WHITE);
	    }
	    break;
	  }
	}else{
	  switch(viewer_type){
	  case 0:
	    if(if_list){
	      sceDevConsPrintf(console,"  ");
	      print_device(console, (if_list + i)->select_device);
	      sceDevConsPrintf(console,"%s/%s\n", (if_list + i)->vendor, (if_list + i)->product);
	    }
	    break;
	  case 1:
	    if(dev_list){
	      sceDevConsPrintf(console,"  ");
	      print_device(console, (dev_list + i)->select_device);
	      sceDevConsPrintf(console,"%s\n", (dev_list + i)->usr_name);
	    }
	    break;
	  case 2:
	    if(ifc_list){
	      sceDevConsPrintf(console,"  ");
	      print_device(console, (ifc_list + i)->select_device);
	      sceDevConsPrintf(console,"%s\n", (ifc_list + i)->usr_name);
	    }
	    break;
	  case 3:
	    if(if_list){
	      sceDevConsPrintf(console,"  ");
	      sceDevConsAttribute(console, ((if_list + i)->id == -1) ? BLUE : WHITE);
	      print_device(console, (if_list + i)->select_device);
	      sceDevConsPrintf(console,"%s/%s\n", (if_list + i)->vendor, (if_list + i)->product);
	      sceDevConsAttribute(console, WHITE);
	    }
	    break;
	  }
	}
      }
    }

    /* Msg. */
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "\n  %s\n", list_msg);
    sceDevConsAttribute(console, WHITE);

    sceDevConsLocate(console, 0, 18);
    sceDevConsPrintf(console, "  Circle button : Select\n");
    sceDevConsPrintf(console, "  Eks button    : Cancel\n");
  }

  /* 描画 */
  sceDevConsDraw(console);
}

/********************************/
/* ネットワーク設定画面の初期化 */
/********************************/
static void
initialize(void)
{
  int size;
#ifdef HOST_READ_ONLY
  int flag, i;
#endif /* HOST_READ_ONLY */

  /* connect_viewer initialize */
  if(viewer_init == 1){
    switch(viewer_type){
    case 0:
      menu     = &if_menu;
      menu_max = &if_menu_max;
      if_mode  = 0;
      break;
    case 1:
      menu     = &dev_menu;
      menu_max = &dev_menu_max;
      break;
    case 2:
      menu     = &ifc_menu;
      menu_max = &ifc_menu_max;
      break;
    case 3:
      menu     = &if_menu;
      menu_max = &if_menu_max;
      if_mode  = 1;
      break;
    }
    *menu      = 0;
  }

  if(viewer_type != 0 && viewer_type != 3){
    list_update(0);
    if(viewer_init == 0){
#ifdef HOST_READ_ONLY
      switch(viewer_type){
      case 1:
	if(viewer_mode == COPY_MODE){
	  if(dev_list == NULL){
	    *menu = -1;
	  }
	}else{
	  if(dev_list){
	    for(i = *menu, flag = 1; i < *menu_max; i++){
	      if((dev_list + i)->select_device != SELECT_HOST){
		*menu = i;
		flag = 0;
		break;
	      }
	    }
	    if(flag) *menu = -1;
	  }else{
	    *menu = -1;
	  }
	}
	break;
      case 2:
	if(viewer_mode == COPY_MODE){
	  if(ifc_list == NULL){
	    *menu = -1;
	  }
	}else{
	  if(ifc_list){
	    for(i = *menu, flag = 1; i < *menu_max; i++){
	      if((ifc_list + i)->select_device != SELECT_HOST){
		*menu = i;
		flag = 0;
		break;
	      }
	    }
	    if(flag) *menu = -1;
	  }else{
	    *menu = -1;
	  }
	}
	break;
      }
#else /* HOST_READ_ONLY */
      switch(viewer_type){
      case 1:
	if(dev_list == NULL){
	  *menu = -1;
	}
	break;
      case 2:
	if(ifc_list == NULL){
	  *menu = -1;
	}
	break;
      }
#endif /* HOST_READ_ONLY */
    }
  }else{
    list_update(10);
  }

  switch(viewer_init){
    /*******************/
    /* I/F List を取得 */
    /*******************/
  case 10:
    Rpc_Init();
    Rpc_GetInterfaceCount(if_mode);
    viewer_init = 11;
    break;

  case 11:
    *menu_max = Rpc_Get_Result();
    if(*menu_max <= 0){
      *menu = -1;
      viewer_init = 0;
    }else{
      if(if_list != NULL){
	free((void *)if_list);
	if_list = NULL;
      }

      size = sizeof(sceInetInterfaceList_t) * (*menu_max);
      if_list = (sceInetInterfaceList_t *)memalign(64, GET_SIZE(size));
      if(if_list == NULL){
	*menu = -1;
	*menu_max = 0;
	viewer_init  = 0;
	break;
      }

      Rpc_GetInterfaceList((int)if_list, *menu_max, if_mode);
      viewer_init = 12;
    }
    break;

  case 12:
    *menu_max = Rpc_Get_Result();
    if(*menu_max <= 0){
      free((void *)if_list);
      if_list = NULL;
      viewer_init = 0;
    }else{
      viewer_init  = 0;
    }
#ifdef HOST_READ_ONLY
    if(if_list){
      for(i = *menu, flag = 1; i < *menu_max; i++){
	if((if_list + i)->select_device != SELECT_HOST){
	  *menu = i;
	  flag = 0;
	  break;
	}
      }
      if(flag) *menu = -1;
    }else{
      *menu = -1;
    }
#else /* HOST_READ_ONLY */
    if(if_list == NULL){
      *menu = -1;
    }
#endif /* HOST_READ_ONLY */
    break;
  }
}
