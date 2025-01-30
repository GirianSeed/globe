/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      USB Keyboard Sample (for EE)
 *
 *                          Version 0.20
 *                          Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                              main.c
 *
 *      Version       Date           Design     Log
 *  --------------------------------------------------------------------
 *      0.10          Feb,5,2001     fukunaga   USB Keyboard Library sample
 *      0.20          Apr,9,2001     fukunaga   
 */


#include <eekernel.h>
#include <eeregs.h>
#include <libgraph.h>
#include <stdio.h>
#include <stdlib.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libusbkb.h>  /* USB Keyboard Library */


/* Prototype */
static void init_newkeyboard(int i);
static void print_readdata(int i);

/* Variables */
static USBKBINFO_t info;   /* �ڑ������i�[����o�b�t�@ */
static u_char location[7]; /* �ʒu���(�|�[�g�ԍ�)���i�[����o�b�t�@ */
static USBKBDATA_t kdata;  /* �L�[�{�[�h�f�[�^���i�[����o�b�t�@ */

#define MAX_KEYBD 8
static int Arrangement[MAX_KEYBD]; /* �L�[�z����L������ϐ� */
static int ReadMode[MAX_KEYBD];    /* ���[�h���[�h(����/�p�P�b�g)���L������ϐ� */

#define USBKEYBD_ARG "keybd=8\0debug=1"  /* usbkb.irx �ɓn���I�v�V���� */

/* ---------------------------------------------
  Function Name	: main
  function     	: ���C���֐�
  Input Data	: none
  Output Data   : none
  Return Value	: none
----------------------------------------------*/
int main()
{
    int i,j;
    int ret;
    int result;
    int max_connect;
    u_char *old_status;
    char *option = USBKEYBD_ARG;
    int modid_usbd, modid_usbkb;
    
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
    sceGsResetPath();
    
    sceSifInitRpc(0);

    /*----- Loading usb driver -----*/
    while( (modid_usbd = sceSifLoadModule("host0:/usr/local/sce/iop/modules/usbd.irx", 
					  0, NULL)) < 0){
	printf("Can't load module usbd\n");
    }
    
    /*----- Loading usb keyboard driver -----*/
    while( (modid_usbkb = sceSifLoadModule("host0:/usr/local/sce/iop/modules/usbkb.irx",
					 sizeof(USBKEYBD_ARG),option)) < 0){
	printf("Can't load module usbkb\n");
    }
    
    /*----- Init usb keyboard interface -----*/
    ret = sceUsbKbInit(&max_connect);
    if (ret == USBKB_NG) {
      printf("Initialize error\n");
      while(1);
    }
    
    old_status = malloc(max_connect);
    for(i=0; i<max_connect; i++) { old_status[i] = 0; }
    
    printf("USB Keyboard Sample\n");
    
    /*----- Main loop -----*/
    while ( 1 ) {
      /* sceUsbKbGetInfo()�Őڑ������擾�B
	  ���̃T���v���ł�VSync���Ɏ擾���Ă��邪,
	  �v���O�����̊J�n���Ɉ�x�����擾����,sceUsbKbRead()�ŃG���[��
	  ��������΍Ăю擾����Ƃ������@�����邩������܂���B
       */
      ret = sceUsbKbGetInfo(&info);
      if (ret != USBKB_OK) {
	printf("Error%d : sceUsbKbGetInfo\n",ret);
	continue;
      }
      sceUsbKbSync(USBKB_WAIT,&result); /* ���̊֐����I��܂ł��̃X���b�h��SLEEP���� */
      if (result != USBKB_OK) {
	printf("Error%d : sceUsbKbSync\n",result);
	continue;
      }
      
#if 0 /* �ڑ�����\��(�f�o�b�O�p) */
      for(i=0; i<max_connect; i++) {
	printf("%02X ",info.status[i]);
      }
      printf("\n");
#endif
      
      /* �ő�ڑ����������ڑ����̎Q�Ƃƃf�[�^�̃��[�h���J��Ԃ� */
      for(i=0; i<max_connect; i++) {
	if (info.status[i] == 0) { 
	  /* �ԍ�i�̃L�[�{�[�h�͐ڑ����Ă��Ȃ��̂�,���̃L�[�{�[�h(i+1)�̐ڑ������Q�Ƃ��� */
	  continue;
	}
	
	/* �ԍ�i�̃L�[�{�[�h���ڑ�����Ă��� */
	if (old_status[i] == 0) {
	  /* �O��̐ڑ���񂪁u���ڑ��v�Ȃ̂�,�u�V�K�ڑ��v�̃L�[�{�[�h */
	  printf("New keyboard %d is connected\n",i);
	  init_newkeyboard(i);
	}
	
	/* Read keyboard data */
	ret = sceUsbKbRead(i,&kdata);
	if (ret != USBKB_OK) {
	  printf("Error%d : sceUsbKbRead\n",ret);
	  continue;
	}
	sceUsbKbSync(USBKB_WAIT,&result); /* ���̊֐����I��܂ł��̃X���b�h��SLEEP���� */
	if (result != USBKB_OK) {
	  printf("Error%d : sceUsbKbSync\n",result);
	  continue;
	}
	
	if (kdata.len == 0) continue;
	
	for(j=0; j < kdata.len; j++) {

	  /* ���ȃL�[��������,����<->�A���t�@�x�b�g�ؑւ� */
	  if ( kdata.keycode[j] == (USBKEYC_KANA|USBKB_RAWDAT) ) {
	    switch(Arrangement[i]) 
	      {
	      case USBKB_ARRANGEMENT_106:
		Arrangement[i] = USBKB_ARRANGEMENT_106_KANA;
		printf("usbkeybd%d : *KANA MODE*\n",i);
		break;
	      case USBKB_ARRANGEMENT_106_KANA:
		Arrangement[i] = USBKB_ARRANGEMENT_106;
		printf("usbkeybd%d : *ALPHABET MODE*\n",i);
		break;
	      default:
		break;
	      }
	    sceUsbKbSetArrangement(i,Arrangement[i]); /* �L�[�z��̐ؑւ� */
	  }

	  /* F1�L�[��������,�p�P�b�g���[�h<->�������̓��[�h�ؑւ� */
	  if ( kdata.keycode[j] == (USBKEYC_F1|USBKB_RAWDAT) ) {
	    switch(ReadMode[i]) 
	      {
	      case USBKB_RMODE_INPUTCHAR:
		ReadMode[i] = USBKB_RMODE_PACKET;
		printf("usbkeybd%d : *PACKET MODE*\n",i);
		break;
	      case USBKB_RMODE_PACKET:
		ReadMode[i] = USBKB_RMODE_INPUTCHAR;
		printf("usbkeybd%d : *INPUTCHAR MODE*\n",i);
		break;
	      default:
		break;
	      }
	    sceUsbKbSetReadMode(i,ReadMode[i]); /* ���[�h���[�h�̐ؑւ� */
	  }

	  /* Ctrl+Alt+Delete��,�I������IOP���W���[����Unload */
	  if (( kdata.keycode[j] == (USBKEYC_DELETE|USBKB_RAWDAT) ) &&
	      ( kdata.mkey & (USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT) ) &&
	      ( kdata.mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL) )
	      ) 
	    {
	      sceUsbKbEnd();
	      /* Unload usbkb.irx */
	      if (sceSifStopModule(modid_usbkb,0,NULL,&result) < 0) {
		printf("Stop usbkb.irx : failed\n");
	      }
	      if (sceSifUnloadModule(modid_usbkb) < 0) {
		printf("Unload usbkb.irx : failed\n");
	      }
	      /* Unload usbd.irx */
	      if (sceSifStopModule(modid_usbd,0,NULL,&result) < 0) {
		printf("Stop usbd.irx : failed\n");
	      }
	      if (sceSifUnloadModule(modid_usbd) < 0) {
		printf("Unload usbd.irx : failed\n");
	      }
	      printf("USB keyboard sample done.\n");
		return 0;
	    }
	  
	}
	
	/* �ԍ�i�̃L�[�{�[�h���ڑ�����Ă���|�[�g�ԍ����擾 */
	ret = sceUsbKbGetLocation(i,location);
	if (ret != USBKB_OK) {
	  printf("Error%d : sceUsbKbGetLocation\n",ret);
	  continue;
	}
	sceUsbKbSync(USBKB_WAIT,&result);  /* ���̊֐����I��܂ł��̃X���b�h��SLEEP���� */
	if (result != USBKB_OK) {
	  printf("Error%d : sceUsbKbSync\n",result);
	  continue;
	}
	
	/*----- Print read data -----*/
	print_readdata(i);
      }
      
      /* �ڑ�����ۑ��B�����r���ĐV�K�ڑ��𔻒肷��̂Ɏg�� */
      for(i=0; i<max_connect; i++) { old_status[i] = info.status[i]; }
      
      sceGsSyncV(0);
      
    } /* end of Main loop */
}


/* ---------------------------------------------
  Function Name	: init_newkeyboard
  function     	: �V�K�ɐڑ����ꂽ�L�[�{�[�h��������
  Input Data	: i (�L�[�{�[�h�ԍ�)
  Output Data   : none
  Return Value	: none
----------------------------------------------*/
static void init_newkeyboard(int i)
{
        int ret;
	int result;
	
	ret = sceUsbKbSetLEDStatus(i,USBKB_LED_NUM_LOCK);
	if (ret != USBKB_OK) {
	  printf("Error%d : sceUsbKbSetLEDStatus\n",ret);
	} else {
	  sceUsbKbSync(USBKB_WAIT,&result);  /* ���̊֐����I��܂ł��̃X���b�h��SLEEP���� */
	  if (result != USBKB_OK) {
	    printf("Error%d : sceUsbKbSync\n",result);
	  }
	}
	
	ret = sceUsbKbSetLEDMode(i,USBKB_LED_MODE_AUTO1);
	if (ret != USBKB_OK) {
	  printf("Error%d : sceUsbKbSetLEDMode\n",ret);
	} else {
	  sceUsbKbSync(USBKB_WAIT,&result);  /* ���̊֐����I��܂ł��̃X���b�h��SLEEP���� */
	  if (result != USBKB_OK) {
	    printf("Error%d : sceUsbKbSync\n",result);
	  }
	}
	
	sceUsbKbClearRbuf(i);
	sceUsbKbSetRepeat(i,30,2);
	sceUsbKbSetCodeType(i,USBKB_CODETYPE_ASCII);
	Arrangement[i] = USBKB_ARRANGEMENT_106;
	sceUsbKbSetArrangement(i,Arrangement[i]);
	ReadMode[i] = USBKB_RMODE_INPUTCHAR;
	sceUsbKbSetReadMode(i,ReadMode[i]);
}


/* ---------------------------------------------
  Function Name	: print_readdata
  function     	: �ǂݍ��񂾃f�[�^��\������
  Input Data	: i (�L�[�{�[�h�ԍ�)
  Output Data   : none
  Return Value	: none
----------------------------------------------*/
static void print_readdata(int i)
{
        u_short kcode;
	int j,k;
	
	printf("usbkeybd%d : ",i);
	
	printf("port=");
	for(k = 0; k < 7 && location[k] != 0; k++)
	  printf("%s%d", ((k)? ",": ""), location[k]);
	
	printf(" : LED[%02X] ",kdata.led);
	
	printf("MKEY[%02X] ",kdata.mkey);
	
	printf("KEY[");
	
	for(j=0; j < kdata.len; j++) {
	  kcode = kdata.keycode[j];
	  if (kcode & USBKB_RAWDAT) {
	    printf(" %04X ",kcode);
	    continue;
	  }
	  if (kcode & USBKB_KEYPAD) {
	    /* 10 key */
	    if ((kcode & 0x00ff) == '\n') { printf(" (\\n) "); continue;}
	    printf(" (%c)  ",kcode & ~USBKB_KEYPAD);
	    continue;
	  }
	  /* Normal key */
	  if ((kcode & 0x00ff) == '\0') { printf(" \'\\0\' "); continue;}
	  if ((kcode & 0x00ff) == '\n') { printf(" \'\\n\' "); continue;}
	  if ((kcode & 0x00ff) == '\t') { printf(" \'\\t\' "); continue;}
	  if ((kcode & 0x00ff) == '\b') { printf(" \'\\b\' "); continue;}
	  if (kcode <= 127) {
	    printf(" \'%c\'  ",kcode); /* ASCII */
	  } else {
	    printf(" J %02X ",kcode); /* JIS */
	  }
	}
	printf("]\n");
}
