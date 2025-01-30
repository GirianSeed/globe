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
/* �������[�J�[�h�ݒ�t�@�C����� */
/**********************************/
#define MAX_MC_COMBINATION	6

static char *mc_combination_name[] = {"Combination1", "Combination2", "Combination3", "Combination4", "Combination5", "Combination6"};
static int mc_combination_flag[MAX_MC_COMBINATION];
static int mc_combination_stat[MAX_MC_COMBINATION];
static int mc_combination_menu;

/**************************/
/* HDD �ݒ�t�@�C����� */
/**************************/
#define MAX_HDD_COMBINATION	10

static char *hdd_combination_name[] = {"Combination1", "Combination2", "Combination3", "Combination4", "Combination5",
				       "Combination6", "Combination7", "Combination8", "Combination9", "Combination10"};
static int hdd_combination_flag[MAX_HDD_COMBINATION];
static int hdd_combination_stat[MAX_HDD_COMBINATION];
static int hdd_combination_menu;

/**************************/
/* �z�X�g�ݒ�t�@�C����� */
/**************************/
#define MAX_HOST_COMBINATION	10

static char *host_combination_name[] = {"Combination1", "Combination2", "Combination3", "Combination4", "Combination5",
					"Combination6", "Combination7", "Combination8", "Combination9", "Combination10"};
static int host_combination_flag[MAX_HOST_COMBINATION];
static int host_combination_stat[MAX_HOST_COMBINATION];
static int host_combination_menu;

/************************/
/* ���ʐݒ�t�@�C����� */
/************************/
static int set_combination_device = NO_SELECT;	/* �g�ݍ��킹�̃f�o�C�X�𒼐ڎw�肷��t���O */
static int select_combination_device;		/* ���ݑI������Ă���f�o�C�X */
static char **combination_name;			/* ���݂̃f�o�C�X��ɂ�����g�ݍ��킹�� */
static int set_combination_menu = -1;		/* �g�ݍ��킹�̔ԍ��𒼐ڎw�肷��t���O */
static int *combination_menu;			/* ���ݑI�����Ă���g�ݍ��킹�̔ԍ� */
static int combination_menu_max;		/* ���݂̃f�o�C�X��ɂ�����ő�g�ݍ��킹�� */
static int *combination_flag;			/* ���݂̃f�o�C�X��ɂ�����g�ݍ��킹�̎g�p�E�s�t���O */
static int *combination_stat;			/* ���݂̃f�o�C�X��ɂ�����g�ݍ��킹�̓o�^�ρE���o�^�t���O */
static int combination_num;			/* �g�ݍ��킹(Combination)������\���������������� */

sceNetCnfList2_t *env_list;			/* Env List */
sceNetCnfList2_t *ifc_list;			/* Ifc List */
sceNetCnfList2_t *dev_list;			/* Dev List */

int env_menu_max;				/* Env Menu �̍ő吔 */
int ifc_menu;					/* ���ݑI�����Ă��� Ifc Menu �� menu �ԍ� */
int ifc_menu_max;				/* Ifc Menu �̍ő吔 */
int dev_menu;					/* ���ݑI�����Ă��� Dev Menu �� menu �ԍ� */
int dev_menu_max;				/* Dev Menu �̍ő吔 */

int env_select;					/* Env Select Flag */
int ifc_select;					/* Ifc Select Flag */
int dev_select;					/* Dev Select Flag */

char env_usr_name[MAX_LEN];			/* Env �t�@�C����(user name) */
char ifc_usr_name[MAX_LEN];			/* Ifc �t�@�C����(user name) */
char dev_usr_name[MAX_LEN];			/* Dev �t�@�C����(user name) */
char ifc_usr_name_disp[MAX_LEN];		/* Ifc �t�@�C����(user name)�\���p */
char dev_usr_name_disp[MAX_LEN];		/* Dev �t�@�C����(user name)�\���p */
char ifc_usr_name_bak[MAX_LEN];			/* Ifc �t�@�C����(user name)�o�b�N�A�b�v */
char dev_usr_name_bak[MAX_LEN];			/* Dev �t�@�C����(user name)�o�b�N�A�b�v */

int list_update_arg;				/* list_update() ���� */

/**********************/
/* ���̑��A�ڑ���� */
/**********************/
int select_error_device;			/* �G���[���N�����Ă���f�o�C�X */
int device_error_type;				/* �f�o�C�X�G���[�^�C�v */
int mc_error_type;				/* �������[�J�[�h�G���[�^�C�v */
int status;					/* ��ԕ\�� */
static int cy, cy_min, cy_max;			/* �J�[�\����� */
static int connect_phase = 0;			/* Connect Phase(�ڑ������ԍ�) */
static int connect_stat  = 0;			/* Connect Status(0<:�G���[, 0:���ڑ�, 1:�ڑ�������, 2:�ڑ�) */
static int redial_stat   = 0;			/* Redial Status(0: ���_�C�A�����łȂ�, 1: ���_�C�A�����ł���) */
static int connect_type  = 0;			/* Connect Type(0:all memory, 1:dev memory & ifc file, 2:ifc memory & dev file
						   3: ifc file & dev file, 4: combination file) */
/********************/
/* �v���g�^�C�v�錾 */
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
/* �l�b�g���[�N�ڑ���ʃR���g���[�� */
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
	/* (�����P) dev, ifc ���Ƀ��X�g�����݂��Ȃ���΂Ȃ�Ȃ� */
	if(dev_list != NULL && ifc_list != NULL){
	  /*(�����Q)  dev, ifc ���Ƀt�@�C���łȂ���΂Ȃ�Ȃ� */
	  if((dev_select == 1 || dev_select == 2) && (ifc_select == 1 || ifc_select == 2)){
	    /* (�����R) dev, ifc ���ɓ���f�o�C�X��łȂ���΂Ȃ�Ȃ� */
	    if(select_combination_device == (dev_list + dev_menu)->select_device &&
	       select_combination_device == (ifc_list + ifc_menu)->select_device){
#ifdef HOST_READ_ONLY
	      /* (�����S) �z�X�g�ł����Ă͂Ȃ�Ȃ� */
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
	/* �{���ɏ����Ă����ł����H */
	delete_flag = -4;
	break;

      case 0:
	/* �폜���܂����H */
	delete_flag = -4;
	break;
      }
    }
  }
  if(tpad & SCE_PADRleft){
    if(device_error_type == 0){
      switch(cy){
      case 3:
	/* (�����P) �g�ݍ��킹�����݂��Ȃ���΂Ȃ�Ȃ� */
	if((*(combination_stat + (*combination_menu))) == 1){
#ifdef HOST_READ_ONLY
	  /* (�����Q) �z�X�g�ł����Ă͂Ȃ�Ȃ� */
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

	  /* �g�ݍ��킹���ꏄ�����玟�̃f�o�C�X���������� */
	  select_combination_device = change_device(select_combination_device);

	  /* �g�ݍ��킹�̃f�o�C�X�������� */
	  init_combination_device(select_combination_device);

	  /* �g�ݍ��킹���̍ő�l���`�F�b�N */
	  if((*combination_menu) == combination_menu_max) (*combination_menu) = 0;

	  /* �g�ݍ��킹(dev_usr_name & ifc_usr_name)���擾����t���O�𗧂Ă� */
	  viewer_init     = 10;
	}else{
	  /* �g�ݍ��킹���̍ő�l���`�F�b�N */
	  if((*combination_menu) == combination_menu_max) (*combination_menu) = 0;

	  /* �g�ݍ��킹(dev_usr_name & ifc_usr_name)���擾����t���O�𗧂Ă� */
	  viewer_init = 10;
	}
	break;

      case 4:
	/* (�����P) dev_list �����݂��Ȃ���΂Ȃ�Ȃ� */
	if(dev_list){
	  dev_menu++;
	  if(dev_menu == dev_menu_max){
	    /* �ꏄ������ (no select) ��Ԃɂ��� */
	    strcpy(dev_usr_name, "(no select)");
	    dev_menu   = -1;
	    dev_select = 0;
	  }else{
	    /* �ڑ��@��ݒ肪�P�ƂőI������Ă����� */
	    strcpy(dev_usr_name, (dev_list + dev_menu)->usr_name);
	    dev_select = 2;
	  }
	}
	break;

      case 5:
	/* (�����P) ifc_list �����݂��Ȃ���΂Ȃ�Ȃ� */
	if(ifc_list){
	  ifc_menu++;
	  if(ifc_menu == ifc_menu_max){
	    /* �ꏄ������ (no select) ��Ԃɂ��� */
	    strcpy(ifc_usr_name, "(no select)");
	    ifc_menu   = -1;
	    ifc_select = 0;
	  }else{
	    /* �ڑ��v���o�C�_�ݒ肪�P�ƂőI������Ă����� */
	    strcpy(ifc_usr_name, (ifc_list + ifc_menu)->usr_name);
	    ifc_select = 2;
	  }
	}
	break;
      }
    }else{
      switch(delete_flag){
      case -4:
	/* ���̃f�o�C�X�͌��邱�Ƃ��ł��܂��� */
	delete_flag = 0;
	device_error_type = 0;
	env_select = 0;
	dev_select = 0;
	ifc_select = 0;
	viewer_init = 1;
	list_update_arg = 10;
	break;

      case -3:
	/* �{���ɏ����Ă����ł����H */
	delete_flag = 1;
	break;

      case -2:
	/* �폜���� */
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
	/* �폜�Ɏ��s���܂��� */
	delete_flag = -4;
	break;

      case 0:
	if(select_error_device == SELECT_HOST){
	  /* ���̃f�o�C�X�͌��邱�Ƃ��ł��܂��� */
	  delete_flag = 0;
	  device_error_type = 0;
	  env_select = 0;
	  dev_select = 0;
	  ifc_select = 0;
	  viewer_init = 1;
	  list_update_arg = 10;
	}else{
	  /* �폜���܂����H */
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
/* �l�b�g���[�N�ڑ���� */
/************************/
void
connect_viewer(void)
{
  if(Rpc_Check() == 0 && viewer_init) initialize();	/* ��ʏ����� */

  /* ��ԃ`�F�b�N */
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
	/* ���ݕ\�����Ă�����e�͕ς��Ȃ� */
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

  if(Rpc_Check() == 0 && 0 < add_flag) add_env();					/* Env �t�@�C���쐬 */
  if(Rpc_Check() == 0 && 0 < delete_flag && device_error_type == 0) delete_env();	/* Env �t�@�C���폜 */
  if(Rpc_Check() == 0 && 0 < delete_flag && device_error_type != 0) delete_all();	/* �ݒ�t�@�C���S�폜 */
  if(Rpc_Check() == 0 && connect_stat == 1) connect();					/* �ڑ��E�ڑ����� */

  if(viewer_init == 0 && add_flag <= 0 && delete_flag <= 0) connect_pad();	/* �R���g���[������ */

  /* �^�C�g���\�� */
  sceDevConsClear(console);
  sceDevConsLocate(console, 0, 0);
  sceDevConsPrintf(console, "Network Connection (Version %s)\n", SETAPP_VER);
  sceDevConsPrintf(console, "------------------------------------------------------------------------\n" );
  sceDevConsPrintf(console, "\n" );

  if(viewer_init){
    /* ���������\�� */
    sceDevConsAttribute(console, YELLOW);
    sceDevConsPrintf(console, "Now Loading...\n");
    sceDevConsAttribute(console, WHITE);
  }else{
    /* ���� PlayStation2 �̐ݒ�̏ꍇ�Ɛݒ肪���Ă�ꍇ�S�Ă̐ݒ���폜���邩�ǂ����q�˂� */
    if(device_error_type != 0){
      switch(delete_flag){
      case -4:
	/* ���̃f�o�C�X�͌��邱�Ƃ��ł��܂��� */
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  ");
	print_device(console, select_error_device);
	sceDevConsPrintf(console, "device can't view\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsLocate(console, 0, 18);
	sceDevConsPrintf(console, "  Circle button   : Done\n");
	break;

      case -3:
	/* �{���ɏ����Ă����ł����H */
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
	/* �폜���� */
	sceDevConsAttribute(console, YELLOW);
	sceDevConsPrintf(console, "  ");
	print_device(console, select_error_device);
	sceDevConsPrintf(console, "setting all delete complete.\n");
	sceDevConsAttribute(console, WHITE);
	sceDevConsLocate(console, 0, 18);
	sceDevConsPrintf(console, "  Circle button   : Done\n");
	break;

      case -1:
	/* �폜�Ɏ��s���܂��� */
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
	  /* ���̃f�o�C�X�͌��邱�Ƃ��ł��܂��� */
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
	  /* �폜���܂����H */
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
	/* ���҂������� */
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
      /* �g�ݍ��킹�̕\�� */
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

      /* �ڑ��@��̕\�� */
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

      /* �ڑ��v���o�C�_�̕\�� */
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

      /* �I�����Ă���t�@�C���ԍ���\�� */
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

      /* ���b�Z�[�W�̕\�� */
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

  /* �`�� */
  sceDevConsDraw(console);
}

/********************************/
/* �l�b�g���[�N�ڑ���ʂ̏����� */
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
    /* �\������ Env �t�@�C����ǂݍ��� */
    /***********************************/
  case 10:
    Rpc_Init();
    if((*combination_menu) != -1){
      /* �I�������g�ݍ��킹�ԍ��̏�Ԃ��`�F�b�N */
      for(i = 0; i < env_menu_max; i++){
	if(strcmp((env_list + i)->usr_name, (*(combination_name + (*combination_menu)))) == 0
	   && (env_list + i)->select_device == select_combination_device){
	  (*(combination_flag + (*combination_menu))) = (env_list + i)->flag; /* �g�p�E�s�� */
	  (*(combination_stat + (*combination_menu))) = 1;                    /* �o�^�ρE���o�^ */
	}
      }
      /* �g�ݍ��킹�t�@�C����ǂݍ��� */
      if((*(combination_stat + (*combination_menu)))){
	Rpc_sceNetCnfLoadEntry(&env_data, 0, (*(combination_name + (*combination_menu))), select_combination_device);
      }
      viewer_init = 11;
    }else{
      viewer_init = 11;
    }
    break;

  case 11:
    get_env_usr_name((*(combination_stat + (*combination_menu)))); /* Env User Name ���擾���� */
    get_ifc_usr_name((*(combination_stat + (*combination_menu)))); /* Ifc User Name ���擾���� */
    get_dev_usr_name((*(combination_stat + (*combination_menu)))); /* Dev User Name ���擾���� */
    viewer_init = 0;
    break;

    /************************************/
    /* �ǉ��E�ҏW�����ݒ�𒼐ڎw�肷�� */
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
	  /* �ۑ�������K���e�f�o�C�X�̐擪�ɂ��� */
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
	  /* �ۑ�������K���e�f�o�C�X�̐擪�ɂ��� */
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
/* Env User Name ���擾���� */
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
/* Ifc User Name ���擾���� */
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
/* Dev User Name ���擾���� */
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
/* Env, Ifc, Dev List ���X�V���� */
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
    /* env_list �̐����擾 */
    Rpc_Init();
    Rpc_sceNetCnfGetCount(0);
    viewer_init = 2;
    break;

  case 2:
    /* �̈���m�ۂ��āAenv_list ���擾 */
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
    /* env_list �擾���� */
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
    /* ifc_list �̐����擾 */
    Rpc_Init();
    Rpc_sceNetCnfGetCount(1);
    viewer_init = 5;
    break;

  case 5:
    /* �̈���m�ۂ��āAifc_list ���擾 */
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
    /* ifc_list �擾���� */
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
    /* dev_list �̐����擾 */
    Rpc_Init();
    Rpc_sceNetCnfGetCount(2);
    viewer_init = 8;
    break;

  case 8:
    /* �̈���m�ۂ��āAdev_list ���擾 */
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
    /* dev_list �擾���� */
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
/* usr_name(sys_name) ���� sys_name(usr_name) ���擾 */
/*****************************************************/
static int
get_another_name(char *sys_name, char *usr_name, sceNetCnfList2_t *list, int menu_max, int select_device)
{
  int i = 0, flag;

  /* flag init */
  flag = 1;

  /* usr_name(sys_name) ���� sys_name(usr_name) ���擾 */
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
/* Env �t�@�C���� Add ���� */
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
      /* Memory Card �^�C�v�̃`�F�b�N */
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
	mc_error_type = -4; /* �ݒ�t�@�C�������ő�l */
	break;
      case sceNETCNF_CAPACITY_ERROR:
	mc_error_type = -2; /* �e�ʂ�����Ȃ� */
	break;
      default:
	mc_error_type = -3; /* �ݒ�t�@�C�����쐬�ł��Ȃ� */
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
/* Env �t�@�C�����폜���� */
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
/* �ݒ�t�@�C����S�č폜���� */
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
/* �ڑ��E�ڑ��������� */
/**********************/
static void
connect(void)
{
  static int addr = 0;
  static int id = 0;

  switch(connect_phase){
    /************************************/
    /* EE ��̐ݒ�f�[�^�� IOP ��ɓ]�� */
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
    /* netcnf.irx �ɐݒ�𔽉f�����āA�ڑ����� */
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
    /* �ڑ��������� */
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
/* ���̃f�o�C�X���������� */
/**************************/
int
change_device(int now_device)
{
  int next_device;

  /* Initialize */
  next_device = 0;

  /* MC -> HDD -> HOST �̏��Ńg�O������ */
  switch(now_device){
  case SELECT_MC:
    /* �f�o�C�X���`�F�b�N���A���݂��Ȃ���Ύ��̃f�o�C�X�փg�O������ */
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
    /* �f�o�C�X���`�F�b�N���A���݂��Ȃ���Ύ��̃f�o�C�X�փg�O������ */
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
    /* �f�o�C�X���`�F�b�N���A���݂��Ȃ���Ύ��̃f�o�C�X�փg�O������ */
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
/* �f�o�C�X�|�C���^������������ */
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
/* �f�o�C�X���o�͂��� */
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
