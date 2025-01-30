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
 *                              ifc.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.16      tetsu      Change for PPPoE
 *        1.5           2001.07.26      tetsu      type bug fix
 */
#include "setapp.h"

#ifdef HOST_READ_ONLY
#define SAVE_TYPE_MAX		(2)
#else /* HOST_READ_ONLY */
#define SAVE_TYPE_MAX		(3)
#endif /* HOST_READ_ONLY */

#define DHCP_TYPE_MAX		(2)
#define PPPOE_TYPE_MAX		(2)

int select_ifc_device;
static int save_type;

#ifdef HOST_READ_ONLY
static char *save_type_msg[] = {"Save Memroy Card",
				"Save HDD"};
#else /* HOST_READ_ONLY */
static char *save_type_msg[] = {"Save Memroy Card",
				"Save HDD",
				"Save HOST"};
#endif /* HOST_READ_ONLY */

static char *dhcp_type_msg[] = {"no use", "use"};
static char *pppoe_type_msg[] = {"no use", "use"};

static int ifc_edit;	/* Ifc Edit Flag */
static int ifc_edit2;	/* Ifc Edit Flag2 */

/* カーソル情報 */
static int cur_init_flag;
static int cx, cx_min, cx_max;
static int cy, cy_min, cy_max;

static int ifc_pad(void);
static void initialize(void);
static void delete_ifc(void);
static void add_ifc(void);
static void cur_init(int, int, int, int);

/************************************/
/* ネットワーク設定画面コントロール */
/************************************/
static int
ifc_pad(void)
{
  u_short tpad;
  int dhcp_tmp, pppoe_tmp, i;

  if((tpad = pad_read()) == 0) return (0);

  if(tpad & SCE_PADLup){
    if(ifc_edit){
      ifc_edit2 = 1;
      disp_char_no++;
      if(CMAX <= disp_char_no) disp_char_no = 0;
      if(disp_char_no < 0) disp_char_no = CMAX - 1;
    }else{
      switch(viewer_type){
      case 2:
      case 3:
      case 4:
      case 6:
      case 7:
	cy--;
	cy = LIMIT(cy, cy_min, cy_max);
	cur_init_flag = 2;
	break;
      case 5:
	dhcp_tmp      = (int)env_data.dhcp - 1;
	dhcp_tmp      = LIMIT(dhcp_tmp, 0, DHCP_TYPE_MAX - 1);
	env_data.dhcp = (u_char)dhcp_tmp;
	break;
      case 8:
	if(save_type != -1 && add_flag == 0){
	  if(0 <= save_type - 1){
	    for(i = save_type - 1; 0 <= i; i--){
	      switch(i){
	      case 0:
		if(device_stat & DEVICE_STAT_MC_ON){
		  if(viewer_mode == COPY_MODE){
		    if((ifc_list + ifc_menu)->select_device != SELECT_MC) save_type = i;
		  }else{
		    save_type = i;
		  }
		}
		break;
	      case 1:
		if(device_stat & DEVICE_STAT_HDD_ON){
		  if(viewer_mode == COPY_MODE){
		    if((ifc_list + ifc_menu)->select_device != SELECT_HDD) save_type = i;
		  }else{
		    save_type = i;
		  }
		}
		break;
#ifndef HOST_READ_ONLY
	      case 2:
		if(device_stat & DEVICE_STAT_HOST_ON){
		  if(viewer_mode == COPY_MODE){
		    if((ifc_list + ifc_menu)->select_device != SELECT_HOST) save_type = i;
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
      case 9:
	pppoe_tmp      = (int)env_data.pppoe - 1;
	pppoe_tmp      = LIMIT(pppoe_tmp, 0, PPPOE_TYPE_MAX - 1);
	env_data.pppoe = (u_char)pppoe_tmp;
	break;
      }
    }
  }
  if(tpad & SCE_PADLdown){
    if(ifc_edit){
      ifc_edit2 = 1;
      disp_char_no--;
      if(CMAX <= disp_char_no) disp_char_no = 0;
      if(disp_char_no < 0) disp_char_no = CMAX - 1;
    }else{
      switch(viewer_type){
      case 2:
      case 3:
      case 4:
      case 6:
      case 7:
	cy++;
	cy = LIMIT(cy, cy_min, cy_max);
	cur_init_flag = 2;
	break;
      case 5:
	dhcp_tmp      = (int)env_data.dhcp + 1;
	dhcp_tmp      = LIMIT(dhcp_tmp, 0, DHCP_TYPE_MAX - 1);
	env_data.dhcp = (u_char)dhcp_tmp;
	break;
      case 8:
	if(save_type != -1 && add_flag == 0){
	  if(save_type + 1 < SAVE_TYPE_MAX){
	    for(i = save_type + 1; i < SAVE_TYPE_MAX; i++){
	      switch(i){
	      case 0:
		if(device_stat & DEVICE_STAT_MC_ON){
		  if(viewer_mode == COPY_MODE){
		    if((ifc_list + ifc_menu)->select_device != SELECT_MC) save_type = i;
		  }else{
		    save_type = i;
		  }
		}
		break;
	      case 1:
		if(device_stat & DEVICE_STAT_HDD_ON){
		  if(viewer_mode == COPY_MODE){
		    if((ifc_list + ifc_menu)->select_device != SELECT_HDD) save_type = i;
		  }else{
		    save_type = i;
		  }
		}
		break;
#ifndef HOST_READ_ONLY
	      case 2:
		if(device_stat & DEVICE_STAT_HOST_ON){
		  if(viewer_mode == COPY_MODE){
		    if((ifc_list + ifc_menu)->select_device != SELECT_HOST) save_type = i;
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
      case 9:
	pppoe_tmp      = (int)env_data.pppoe + 1;
	pppoe_tmp      = LIMIT(pppoe_tmp, 0, PPPOE_TYPE_MAX - 1);
	env_data.pppoe = (u_char)pppoe_tmp;
	break;
      }
    }
  }
  if(tpad & SCE_PADLleft){
    if(ifc_edit){
      ifc_edit2 = 0;
      switch(viewer_type){
      case 2:
	switch(cy){
	case 3:
	  write_char(env_data.auth_name, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 4:
	  write_char(env_data.auth_key, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 3:
	switch(cy){
	case 6:
	  write_char(env_data.phone_numbers1, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 7:
	  write_char(env_data.phone_numbers2, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 8:
	  write_char(env_data.phone_numbers3, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 4:
	switch(cy){
	case 5:
	  write_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 6:
	  write_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 6:
	switch(cy){
	case 3:
	  write_char(env_data.address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 4:
	  write_char(env_data.netmask, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 5:
	  write_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 6:
	  write_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 7:
	  write_char(env_data.gateway, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 7:
	switch(cy){
	case 3:
	  write_char(ifc_usr_name, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      }
      cx--;
      cx = LIMIT(cx, cx_min, cx_max);
    }
  }
  if(tpad & SCE_PADLright){
    if(ifc_edit){
      ifc_edit2 = 0;
      switch(viewer_type){
      case 2:
	switch(cy){
	case 3:
	  write_char(env_data.auth_name, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 4:
	  write_char(env_data.auth_key, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 3:
	switch(cy){
	case 6:
	  write_char(env_data.phone_numbers1, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 7:
	  write_char(env_data.phone_numbers2, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 8:
	  write_char(env_data.phone_numbers3, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 4:
	switch(cy){
	case 5:
	  write_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 6:
	  write_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 6:
	switch(cy){
	case 3:
	  write_char(env_data.address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 4:
	  write_char(env_data.netmask, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 5:
	  write_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 6:
	  write_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 7:
	  write_char(env_data.gateway, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 7:
	switch(cy){
	case 3:
	  write_char(ifc_usr_name, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      }
      cx++;
      cx = LIMIT(cx, cx_min, cx_max);
    }
  }
  if(tpad & SCE_PADRup){
    if(ifc_edit){
      ifc_edit2 = 0;
      switch(viewer_type){
      case 2:
	switch(cy){
	case 3:
	  insert_char(env_data.auth_name, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 4:
	  insert_char(env_data.auth_key, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	}
	break;
      case 3:
	switch(cy){
	case 6:
	  insert_char(env_data.phone_numbers1, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 7:
	  insert_char(env_data.phone_numbers2, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 8:
	  insert_char(env_data.phone_numbers3, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	}
	break;
      case 4:
	switch(cy){
	case 5:
	  insert_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 6:
	  insert_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	}
	break;
      case 6:
	switch(cy){
	case 3:
	  insert_char(env_data.address, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 4:
	  insert_char(env_data.netmask, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 5:
	  insert_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 6:
	  insert_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	case 7:
	  insert_char(env_data.gateway, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	}
	break;
      case 7:
	switch(cy){
	case 3:
	  insert_char(ifc_usr_name, cx - cx_min, disp_char[disp_char_no], cx_max, cx_min);
	  break;
	}
	break;
      }
    }else{
      switch(viewer_type){
      case 4:
	switch(cy){
	case 5:
	case 6:
	  ifc_edit      = 1;
	  ifc_edit2     = 0;
	  break;
	}
	break;
      case 2:
      case 3:
      case 6:
      case 7:
	ifc_edit      = 1;
	ifc_edit2     = 0;
	break;
      }
    }
  }
  if(tpad & SCE_PADRdown){
    if(ifc_edit){
      ifc_edit = 0;
    }else{
      switch(add_flag){
      case -4:
	add_flag = 0;
	break;
      default:
	if(get_return() == 1){
	  viewer_init  = 1;
	}
	cur_init_flag = 1;
	break;
      }
    }
  }
  if(tpad & SCE_PADRleft){
    if(ifc_edit){
      ifc_edit2 = 0;
      switch(viewer_type){
      case 2:
	switch(cy){
	case 3:
	  delete_char(env_data.auth_name, cx - cx_min);
	  break;
	case 4:
	  delete_char(env_data.auth_key, cx - cx_min);
	  break;
	}
	break;
      case 3:
	switch(cy){
	case 6:
	  delete_char(env_data.phone_numbers1, cx - cx_min);
	  break;
	case 7:
	  delete_char(env_data.phone_numbers2, cx - cx_min);
	  break;
	case 8:
	  delete_char(env_data.phone_numbers3, cx - cx_min);
	  break;
	}
	break;
      case 4:
	switch(cy){
	case 5:
	  delete_char(env_data.dns1_address, cx - cx_min);
	  break;
	case 6:
	  delete_char(env_data.dns2_address, cx - cx_min);
	  break;
	}
	break;
      case 6:
	switch(cy){
	case 3:
	  delete_char(env_data.address, cx - cx_min);
	  break;
	case 4:
	  delete_char(env_data.netmask, cx - cx_min);
	  break;
	case 5:
	  delete_char(env_data.dns1_address, cx - cx_min);
	  break;
	case 6:
	  delete_char(env_data.dns2_address, cx - cx_min);
	  break;
	case 7:
	  delete_char(env_data.gateway, cx - cx_min);
	  break;
	}
	break;
      case 7:
	switch(cy){
	case 3:
	  delete_char(ifc_usr_name, cx - cx_min);
	  break;
	}
	break;
      }
    }else{
      switch(viewer_type){
      case 4:
	cy = 3;
	strcpy(env_data.dns1_address, "0.0.0.0");
	strcpy(env_data.dns2_address, "0.0.0.0");
	break;
      case 6:
	viewer_init = 4;
	break;
      case 7:
	if(env_data.type != 2 && env_data.dhcp){
	  set_return(viewer_flag, viewer_type, viewer_mode, 6, 0, viewer_mode);
	  viewer_init = 1;
	}
	break;
      case 8:
	if(add_flag == 0){
	  ifc_select  = 3;
	  if(viewer_mode == ALL_ADD) dev_select = 3;
	  save_data();
	  set_return(0, -1, DEFAULT, 1, 0, DEFAULT);
	  viewer_init = 1;
	}
	break;
      }
    }
  }
  if(tpad & SCE_PADRright){
    if(ifc_edit){
      ifc_edit2 = 0;
      switch(viewer_type){
      case 2:
	switch(cy){
	case 3:
	  write_char(env_data.auth_name, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 4:
	  write_char(env_data.auth_key, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 3:
	switch(cy){
	case 6:
	  write_char(env_data.phone_numbers1, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 7:
	  write_char(env_data.phone_numbers2, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 8:
	  write_char(env_data.phone_numbers3, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 4:
	switch(cy){
	case 5:
	  write_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 6:
	  write_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 6:
	switch(cy){
	case 3:
	  write_char(env_data.address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 4:
	  write_char(env_data.netmask, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 5:
	  write_char(env_data.dns1_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 6:
	  write_char(env_data.dns2_address, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	case 7:
	  write_char(env_data.gateway, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      case 7:
	switch(cy){
	case 3:
	  write_char(ifc_usr_name, cx - cx_min, disp_char[disp_char_no], cx_max);
	  break;
	}
	break;
      }
      cx++;
      cx = LIMIT(cx, cx_min, cx_max);
    }else{
      switch(viewer_type){
      case 0:
	set_return(viewer_flag, viewer_type, NO_RETURN, viewer_flag, 1, viewer_mode);
	delete_flag          = 1;
	break;
      case 1:
	if(delete_flag < 0){
	  set_return(0, -1, DEFAULT, 1, 0, DEFAULT);
	  viewer_init = 1;
	  delete_flag = 0;
	}
	break;
      case 2:
	if(env_data.pppoe){
	  set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 4, viewer_mode);
	}else{
	  set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 3, viewer_mode);
	}
	cur_init_flag        = 1;
	break;
      case 3:
	set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 4, viewer_mode);
	cur_init_flag        = 1;
	break;
      case 4:
	if(cy == 3){
	  strcpy(env_data.dns1_address, "");
	  strcpy(env_data.dns2_address, "");
	}
	set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 7, viewer_mode);
	viewer_init          = 3;
	cur_init_flag        = 1;
	break;
      case 5:
	if(env_data.dhcp){
	  set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 7, viewer_mode);
	  viewer_init        = 3;
	}else{
	  set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 6, viewer_mode);
	  if(viewer_mode == ADD_MODE || viewer_mode == ALL_ADD) viewer_init = 4;
	}
	cur_init_flag        = 1;
	break;
      case 6:
	set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 7, viewer_mode);
	viewer_init          = 3;
	cur_init_flag        = 1;
	break;
      case 7:
	set_return(viewer_flag, viewer_type, NO_RETURN, viewer_flag, 8, viewer_mode);
	break;
      case 8:
	switch(add_flag){
	case -1:
	  mc_error_type = 0;
	  if(viewer_mode != COPY_MODE) ifc_select = 3;
	case -2:
	  add_flag = -3;
	  break;
	case -4:
	  add_flag = -5;
	  break;
	case 0:
	  if(save_type != -1){
	    switch(save_type){
	    case 0:
	      select_ifc_device = SELECT_MC;
	      if(viewer_mode == ALL_ADD) select_dev_device = SELECT_MC;
	      break;
	    case 1:
	      select_ifc_device = SELECT_HDD;
	      if(viewer_mode == ALL_ADD) select_dev_device = SELECT_HDD;
	      break;
#ifndef HOST_READ_ONLY
	    case 2:
	      select_ifc_device = SELECT_HOST;
	      if(viewer_mode == ALL_ADD) select_dev_device = SELECT_HOST;
	      break;
#endif /* HOST_READ_ONLY */
	    }

	    switch(viewer_mode){
	    case COPY_MODE:
	      add_type   = 2; /* COPY_MODE */
	      add_flag   = 1;
	      break;
	    case ADD_MODE:
	    case ALL_ADD:
	      add_type   = 1; /* ADD_MODE & ALL_ADD */
	      add_flag   = 1;
	      ifc_select = 2;
	      if(viewer_mode == ALL_ADD) dev_select = 2;
	      break;
	    default:
	      add_type   = 0; /* EDIT_MODE */
	      add_flag   = 1;
	      ifc_select = 2;
	      break;
	    }
	  }
	  break;
	}
	break;
      case 9:
	if(env_data.pppoe){
	  set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 2, viewer_mode);
	  viewer_init = 4;
	}else{
	  set_return(viewer_flag, viewer_type, viewer_mode, viewer_flag, 5, viewer_mode);
	}
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
ifc_viewer(void)
{
  int i;

  /* 状態チェック */
  switch(status){
  case IEV_Conf:
  case IEV_NoConf:
    switch(select_ifc_device){
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
  if(Rpc_Check() == 0 && 0 != add_flag) add_ifc();			/* Ifc ファイル作成 */
  if(Rpc_Check() == 0 && 0 != delete_flag) delete_ifc();		/* Ifc ファイル削除 */

  if(viewer_init == 0 && add_flag <= 0 && delete_flag <= 0) ifc_pad();	/* コントローラ操作 */

  /* タイトル表示 */
  sceDevConsClear(console);
  sceDevConsLocate(console, 0, 0);
  sceDevConsPrintf(console, "Network Configuration(ifc)\n");
  sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
  sceDevConsPrintf(console, "\n" );

  if(viewer_init){
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "Now Loading...\n");
    sceDevConsAttribute(console, WHITE);
  }else{
    switch(viewer_type){
      /**************************/
      /* Ifc ファイルを削除する */
      /**************************/
    case 0:
      sceDevConsAttribute(console, YELLOW);
      sceDevConsPrintf(console, "  You will delete %s.\n", (ifc_list + ifc_menu)->usr_name);
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

      /************************************/
      /* ユーザ ID とパスワードを設定する */
      /************************************/
    case 2:
      if(cur_init_flag) cur_init(13, 71, 3, 4);
      if(cy == 3){
	sceDevConsPrintf(console, "* User ID  : %s\n", env_data.auth_name);
      }else{
	sceDevConsPrintf(console, "  User ID  : %s\n", env_data.auth_name);
      }
      if(cy == 4){
	sceDevConsPrintf(console, "* Password : %s\n", env_data.auth_key);
      }else{
	sceDevConsPrintf(console, "  Password : %s\n", env_data.auth_key);
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "\n");
      sceDevConsAttribute(console, YELLOW);
      sceDevConsPrintf(console, "  Please set User ID and Password.\n");
      sceDevConsAttribute(console, WHITE);
      sceDevConsLocate(console, 0, 18);
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;

      /**********************/
      /* 電話番号を設定する */
      /**********************/
    case 3:
      if(cur_init_flag) cur_init(16, 71, 6, 8);
      sceDevConsPrintf(console, "  User ID     : %s\n", env_data.auth_name);
      sceDevConsPrintf(console, "  Password    : %s\n", env_data.auth_key);
      sceDevConsPrintf(console, "\n");
      switch(cy){
      case 6:
	sceDevConsPrintf(console, "* Tel. Number1: %s\n", env_data.phone_numbers1);
	sceDevConsPrintf(console, "  Tel. Number2: %s\n", env_data.phone_numbers2);
	sceDevConsPrintf(console, "  Tel. Number3: %s\n", env_data.phone_numbers3);
	break;
      case 7:
	sceDevConsPrintf(console, "  Tel. Number1: %s\n", env_data.phone_numbers1);
	sceDevConsPrintf(console, "* Tel. Number2: %s\n", env_data.phone_numbers2);
	sceDevConsPrintf(console, "  Tel. Number3: %s\n", env_data.phone_numbers3);
	break;
      case 8:
	sceDevConsPrintf(console, "  Tel. Number1: %s\n", env_data.phone_numbers1);
	sceDevConsPrintf(console, "  Tel. Number2: %s\n", env_data.phone_numbers2);
	sceDevConsPrintf(console, "* Tel. Number3: %s\n", env_data.phone_numbers3);
	break;
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsAttribute(console, YELLOW);
      sceDevConsPrintf(console, "  Please set telephone number.\n");
      sceDevConsAttribute(console, WHITE);
      sceDevConsLocate(console, 0, 18);
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;

      /**********************/
      /* DNS アドレスの設定 */
      /**********************/
    case 4:
      if(cur_init_flag){
	if(cur_init_flag == 1){
	  cur_init(37, 71, 3, 6);
	  if(env_data.dns1_address[0] != '\0' && strcmp(env_data.dns1_address, "0.0.0.0") != 0){
	    cy = 4;
	  }
	}else{
	  cur_init(37, 71, 3, 6);
	}
      }
      switch(cy){
      case 3:
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
      case 4:
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
      case 5:
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
      case 6:
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
      sceDevConsLocate(console, 0, 18);
      sceDevConsPrintf(console, "  Square button : Restore Defaults\n");
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;

      /***************/
      /* DHCP の設定 */
      /***************/
    case 5:
      for(i = 0; i < DHCP_TYPE_MAX; i++){
	if(env_data.dhcp == (u_char)i){
	  sceDevConsPrintf(console,"* %s\n", dhcp_type_msg[i]);
	}else{
	  sceDevConsPrintf(console,"  %s\n", dhcp_type_msg[i]);
	}
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "  Please select \"no use\" or \"use\" DHCP.\n");
      sceDevConsLocate(console, 0, 18);
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;

      /****************/
      /* その他の設定 */
      /****************/
    case 6:
      if(cur_init_flag) cur_init(33, 71, 3, 7);
      if(cy == 3){
	sceDevConsPrintf(console, "* IP Address                   : %s\n", env_data.address);
      }else{
	sceDevConsPrintf(console, "  IP Address                   : %s\n", env_data.address);
      }
      if(cy == 4){
	sceDevConsPrintf(console, "* Netmask                      : %s\n", env_data.netmask);
      }else{
	sceDevConsPrintf(console, "  Netmask                      : %s\n", env_data.netmask);
      }
      if(cy == 5){
	sceDevConsPrintf(console, "* Primary DNS Server Address   : %s\n", env_data.dns1_address);
      }else{
	sceDevConsPrintf(console, "  Primary DNS Server Address   : %s\n", env_data.dns1_address);
      }
      if(cy == 6){
	sceDevConsPrintf(console, "* Secondary DNS Server Address : %s\n", env_data.dns2_address);
      }else{
	sceDevConsPrintf(console, "  Secondary DNS Server Address : %s\n", env_data.dns2_address);
      }
      if(cy == 7){
	sceDevConsPrintf(console, "* Default Router Address       : %s\n", env_data.gateway);
      }else{
	sceDevConsPrintf(console, "  Default Router Address       : %s\n", env_data.gateway);
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsAttribute(console, YELLOW);
      sceDevConsPrintf(console, "  Please set other information.\n");
      sceDevConsAttribute(console, WHITE);
      sceDevConsLocate(console, 0, 18);
      sceDevConsPrintf(console, "  Square button : Restore Defaults\n");
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;

      /****************/
      /* 設定名の設定 */
      /****************/
    case 7:
      if(cur_init_flag) cur_init(21, 71, 3, 3);
      sceDevConsPrintf(console, "  NSP Setting name : ");
      sceDevConsAttribute(console, YELLOW);
      sceDevConsPrintf(console, "%s\n", ifc_usr_name);
      sceDevConsAttribute(console, WHITE);
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "  Please set NSP setting name.\n");
      sceDevConsLocate(console, 0, 18);
      if(env_data.type != 2 && env_data.dhcp) sceDevConsPrintf(console, "  Square button : Advanced Settings\n");
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;

      /************************/
      /* 設定の保存を確認する */
      /************************/
    case 8:
      if(add_flag){
	switch(add_flag){
	case -1:
	  sceDevConsAttribute(console, YELLOW);
	  switch(mc_error_type){
	  case -1:
	    sceDevConsPrintf(console, "  Not PlayStation 2 Memory Card !\n");
	    break;
	  case -2:
	    switch(select_ifc_device){
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
	  switch(select_ifc_device){
	  case NO_SELECT:
	    for(i = 0; i < SAVE_TYPE_MAX; i++){
	      switch(i){
	      case 0:
		if((device_stat & DEVICE_STAT_MC_ON) != DEVICE_STAT_MC_ON) continue;
		if(viewer_mode == COPY_MODE){
		  if((ifc_list + ifc_menu)->select_device == SELECT_MC) continue;
		}
		break;
	      case 1:
		if((device_stat & DEVICE_STAT_HDD_ON) != DEVICE_STAT_HDD_ON) continue;
		if(viewer_mode == COPY_MODE){
		  if((ifc_list + ifc_menu)->select_device == SELECT_HDD) continue;
		}
		break;
#ifndef HOST_READ_ONLY
	      case 2:
		if((device_stat & DEVICE_STAT_HOST_ON) != DEVICE_STAT_HOST_ON) continue;
		if(viewer_mode == COPY_MODE){
		  if((ifc_list + ifc_menu)->select_device == SELECT_HOST) continue;
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

      /***************/
      /* PPPoE の設定 */
      /***************/
    case 9:
      for(i = 0; i < PPPOE_TYPE_MAX; i++){
	if(env_data.pppoe == (u_char)i){
	  sceDevConsPrintf(console,"* %s\n", pppoe_type_msg[i]);
	}else{
	  sceDevConsPrintf(console,"  %s\n", pppoe_type_msg[i]);
	}
      }
      sceDevConsPrintf(console, "\n");
      sceDevConsPrintf(console, "  Please select \"no use\" or \"use\" PPP over Ethernet.\n");
      sceDevConsLocate(console, 0, 18);
      sceDevConsPrintf(console, "  Circle button : Done\n");
      sceDevConsPrintf(console, "  Eks button    : Cancel\n");
      break;
    }

    /******************/
    /* カーソルの表示 */
    /******************/
    if(ifc_edit){
      if(ifc_edit2){
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

/************************/
/* Ifc 画面を初期化する */
/************************/
static void
initialize(void)
{
  int i, no, cmp_no;
  char ifc_usr_name_tmp[MAX_LEN];
  char ifc_usr_name_org[MAX_LEN];

  switch(viewer_init){
    /**************************/
    /* モードにより動作を変更 */
    /**************************/
  case 1:
    /* all mode common initialize */
    delete_flag   = 0;
    add_flag      = 0;
    add_type      = 0;
    ifc_edit      = 0;
    ifc_edit2     = 0;
    cur_init_flag = 1;
    switch(select_ifc_device){
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
	if((ifc_list + ifc_menu)->select_device == SELECT_MC){
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
	if((ifc_list + ifc_menu)->select_device == SELECT_HDD){
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
	if((ifc_list + ifc_menu)->select_device == SELECT_HOST){
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
      viewer_type = 8;
      viewer_init = 0;
      break;
    case DELETE_MODE:
      viewer_type = 0;
      viewer_init = 0;
      break;
    case ADD_MODE:
      init_data(&env_data);
      strcpy(dev_usr_name, (dev_list + dev_menu)->usr_name);
      Rpc_Init();
      Rpc_sceNetCnfLoadEntry(&env_data, 2, dev_usr_name, (dev_list + dev_menu)->select_device);
      viewer_init = 2;
      break;
    default:
      if(viewer_mode != ALL_ADD){
	init_data(&env_data);
	Rpc_Init();
	Rpc_sceNetCnfLoadEntry(&env_data, 1, (ifc_list + ifc_menu)->usr_name, (ifc_list + ifc_menu)->select_device);
      }
      viewer_init = 2;
      break;
    }
    break;

  case 2:
    switch(env_data.type){
    case 1: /* eth */
      if(viewer_mode == ADD_MODE || viewer_mode == ALL_ADD){
	viewer_type = 9;
      }else{
	viewer_type = 5;
      }
      break;
    case 2: /* ppp */
      viewer_type = 2;
      break;
    case 3: /* nic */
      if(viewer_mode == ADD_MODE || viewer_mode == ALL_ADD){
	viewer_type = 9;
      }else{
	viewer_type = 5;
      }
      break;
    }
    if(viewer_mode == ADD_MODE || viewer_mode == ALL_ADD){
      if(env_data.type == 2){
	viewer_init = 4;
      }else{
	viewer_init = 0;
      }
    }else{
      viewer_init = 3;
    }
    break;

    /**************************/
    /* デフォルト設定名の設定 */
    /**************************/
  case 3:
    if(viewer_mode == ADD_MODE || viewer_mode == ALL_ADD){
      /* デフォルト名の設定 */
      switch(env_data.type){
      case 1: /* eth */
      case 3: /* nic */
	strcpy(ifc_usr_name, dev_usr_name);
	strcat(ifc_usr_name, "'s setting");
	break;
      case 2: /* ppp */
	strcpy(ifc_usr_name, env_data.auth_name);
	strcat(ifc_usr_name, "'s setting");
	break;
      }

      /* デフォルト名の重複チェック */
      no = 1;
      strcpy(ifc_usr_name_org, ifc_usr_name);
    IFC_NAME_RE_CHECK:
      cmp_no = no;
      for(i = 0; i < ifc_menu_max; i++){
	if(strcmp((ifc_list + i)->usr_name, ifc_usr_name) == 0) no++;
      }
      if(cmp_no != no){
	strcpy(ifc_usr_name_tmp, ifc_usr_name_org);
	sprintf(ifc_usr_name, "%s%d", ifc_usr_name_tmp, no);
	goto IFC_NAME_RE_CHECK;
      }
    }else{
      strcpy(ifc_usr_name, (ifc_list + ifc_menu)->usr_name);
      strcpy(ifc_usr_name_bak, ifc_usr_name);
    }
    viewer_init = 0;
    break;

    /**************************/
    /* デフォルト値を設定する */
    /**************************/
  case 4:
    switch(env_data.type){
    case 1:
    case 3:
      if(env_data.pppoe == 1){
	strcpy(env_data.address, "");
	strcpy(env_data.netmask, "");
	strcpy(env_data.dns1_address, "0.0.0.0");
	strcpy(env_data.dns2_address, "0.0.0.0");
	strcpy(env_data.gateway, "");
	strcpy(env_data.auth_name, "Your user ID");
	strcpy(env_data.auth_key, "Password");
      }else{
	strcpy(env_data.address, "0.0.0.0");
	strcpy(env_data.netmask, "0.0.0.0");
	strcpy(env_data.dns1_address, "0.0.0.0");
	strcpy(env_data.dns2_address, "0.0.0.0");
	strcpy(env_data.gateway, "0.0.0.0");
	strcpy(env_data.auth_name, "");
	strcpy(env_data.auth_key, "");
      }
      break;
    case 2:
      strcpy(env_data.auth_name, "Your user ID");
      strcpy(env_data.auth_key, "Password");
      if(env_data.pppoe != 1){
	strcpy(env_data.phone_numbers1, "00-0000-0000");
	strcpy(env_data.phone_numbers2, "");
	strcpy(env_data.phone_numbers3, "");
      }
      strcpy(env_data.dns1_address, "0.0.0.0");
      strcpy(env_data.dns2_address, "0.0.0.0");
      break;
    }
    viewer_init = 0;
    break;
  }
}

/**************************/
/* Ifc ファイルを削除する */
/**************************/
static void
delete_ifc(void)
{
  switch(delete_flag){
  case 1:
    Rpc_Init();
    Rpc_sceNetCnfDeleteEntry(1, (ifc_list + ifc_menu)->usr_name, (ifc_list + ifc_menu)->select_device);
    delete_flag = 2;
    break;

  case 2:
    if(Rpc_Get_Result() < 0){
      delete_flag = -1;
    }else{
      delete_flag = -2;
    }
    break;
  }
}

/***********************************/
/* Ifc ファイルを Add or Edit する */
/***********************************/
static void
add_ifc(void)
{
  static int addr = 0;
  static int id = 0;
  int type, free, format, cmd, result, r;

  switch(add_flag){
  case 1:
    switch(select_ifc_device){
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
	mc_error_type = -1;
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
      mc_error_type = -3;
    }else{
      id = send_iop((int)&env_data, addr, sizeof(sceNetCnfEnvData_t), 0);
      add_flag = 3;
    }
    break;

  case 3:
    if(dma_check(id) < 0){
      switch(add_type){
      case 2:
	Rpc_sceNetCnfCopyEntry(1, (ifc_list + ifc_menu)->usr_name, (ifc_list + ifc_menu)->usr_name,
			       (ifc_list + ifc_menu)->select_device, select_ifc_device);
	break;
      case 1:
	Rpc_sceNetCnfAddEntry((sceNetCnfEnvData_t *)addr, 1, ifc_usr_name, select_ifc_device);
	break;
      case 0:
	Rpc_sceNetCnfEditEntry((sceNetCnfEnvData_t *)addr, 1, ifc_usr_name_bak, ifc_usr_name, select_ifc_device);
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
      if(viewer_mode == ALL_ADD && (if_list + if_menu)->flag == 0){
	Rpc_sceNetCnfAddEntry((sceNetCnfEnvData_t *)addr, 2, dev_usr_name, select_dev_device);
	add_flag = 5;
      }else{
	Rpc_FreeSysMemory(addr);
	add_flag = -2;
      }
    }
    break;

  case 5:
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
	add_flag     = -1;
	mc_error_type = -5;
      }
    }
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
