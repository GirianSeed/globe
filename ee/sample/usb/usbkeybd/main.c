/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      USB Keyboard Sample (for EE)
 *
 *                          Version 0.10
 *                          Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                              main.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *      0.10            Apr,13,2000     fukunaga   USB Keyboard sample
 * 
 */

#include <eekernel.h>
#include <eeregs.h>
#include <libgraph.h>
#include <stdio.h>
#include <sifdev.h>
#include <sifrpc.h>

#include "usbkeybd.h"


static u_char info[0x80] __attribute__((aligned(64)));     /* 接続情報を受信するバッファ */
static u_char location[0x80] __attribute__((aligned(64))); /* 位置情報(ポート番号)を受信するバッファ */
static u_char kdata[0x80] __attribute__((aligned(64)));    /* キーボードデータを受信するバッファ */


/* ---------------------------------------------
  Function Name	: main
  function     	: メイン関数
  Input Data	: none
  Output Data   : none
  Return Value	: none
----------------------------------------------*/
int main()
{
    int i,j,r,ret;

    sceSifInitRpc(0);

    /* loading usb driver */
    while(sceSifLoadModule("host0:/usr/local/sce/iop/modules/usbd.irx",
			0, NULL) < 0){
	printf("Can't load module usbd\n");
    }

    /* loading usb keyboard driver */
    while(sceSifLoadModule("host0:/usr/local/sce/iop/sample/usb/usbkeybd/usbkeybd.irx",0,NULL) < 0){
	printf("Can't load module usbkeybd\n");
    }
    
    /* init usb keyboard interface */
    sceUsbKeybdInit();

    sceGsResetPath();

    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);

    printf("USB Keyboard Sample\n");

    /* Main loop */
    while ( 1 ) {
        /* sceUsbKbGetInfo()で接続情報を取得 */
	ret = sceUsbKeybdGetInfo(info);
	if (ret != 0) {
	  printf("Error%d : sceUsbKeybdGetInfo\n",ret);
	  continue;
	}
	sceUsbKeybdSync(0); /* この関数が終るまでこのスレッドはSLEEPする */
#if 0 /* 接続情報を表示(デバッグ用) */
	for(i=0; i<info[0]+2; i++) {
	  printf("%02X ",info[i]);
	}
	printf("\n");
#endif

	/* 最大接続数分だけ接続情報の参照とデータのリードを繰り返す */
	for(i=0; i<info[0]; i++) {
	  if (info[i+2] == 0) { 
	    /* 番号iのキーボードは接続していないので,
	        次のキーボード(i+1)の接続情報を参照する */	    
	    continue; 
	  }
	  
	  ret = sceUsbKeybdRead(i,kdata);
	  if (ret != 0) {
	    printf("Error%d : sceUsbKeybdRead\n",ret);
	    continue;
	  }
	  sceUsbKeybdSync(0); /* この関数が終るまでこのスレッドはSLEEPする */
	  
	  if (kdata[1] == 0) {
	    /* 番号iのキーボードはデータがないので,
	        次のキーボード(i+1)のデータを取得する */
	    continue; 
	  }
	  
	  /* 番号iのキーボードが接続されているポート番号を取得 */
	  ret = sceUsbKeybdGetLocation(i,location);
	  if (ret != 0) {
	    printf("Error%d : sceUsbKeybdGetLocation\n",ret);
	    continue;
	  }
	  sceUsbKeybdSync(0); /* この関数が終るまでこのスレッドはSLEEPする */
	  
	  /* 取得したデータをデバッガに表示 */
	  printf("usbkeybd%d : ",i);
	  
	  printf("port=");
	  for(r = 0; r < 7 && location[r] != 0; r++)
	    printf("%s%d", ((r)? ",": ""), location[r]);
	  
	  printf(" : LED[%02X] ",kdata[0]);
	  printf("KEY[");
	  for(j=0; j<kdata[1]; j++) {
	    printf("%02X ",kdata[j+2]);
	  }
	  printf("]\n");
	}
	
	sceGsSyncV(0);
    } /* End of main loop */
}
