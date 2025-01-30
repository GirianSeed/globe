/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      USB Keyboard Sample (for EE)
 *
 *                          Version 0.20
 *                          Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                              usbkeybd.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *      0.10            Apr,13,2000     fukunaga   USB Keyboard sample
 *      0.20            Jan,17,2001     fukunaga   
 * 
 */

#include <eekernel.h>
#include <eeregs.h>
#include <libgraph.h>
#include <stdio.h>
#include <sifdev.h>
#include <sifrpc.h>

#include "usbkeybd.h"

/*----- Variables -----*/
#define SSIZE 0x20
static sceSifClientData cdUsbKeybd;
static u_int sdata[SSIZE/sizeof(u_int)] __attribute__((aligned(64)));
/* static u_int rdata[SSIZE/sizeof(u_int)] __attribute__((aligned(64))); */
static int semaid_rpc;

/*----- Prototype -----*/
static void rpccall_end(void* data);


/* ---------------------------------------------
  Function Name	: sceUsbKeybdInit
  function     	: RPC�̓o�^
  Input Data	: none
  Output Data   : none
  Return Value	: 0(����I��),not0(�ُ�I��)
----------------------------------------------*/
int sceUsbKeybdInit(void)
{
	unsigned int i;
	struct SemaParam sema_p;
	
	/* Create Sema */
	sema_p.initCount = 1;
	sema_p.maxCount = 1;
	semaid_rpc = CreateSema(&sema_p);
	if (semaid_rpc == -1) {
	  printf("CreateSema Error\n");
	  while(1);
	}
	
	/* Resister RPC */
	sceSifInitRpc(0);
	
	while( 1 ) {
		if (sceSifBindRpc( &cdUsbKeybd, SCE_RPC_USB_KEYBD, 0) < 0) {
			printf("bind errr\n");
			while( 1 );
		}
		if (cdUsbKeybd.serve != 0) break;
		i = 0x10000;
		while(i --);
	}
	
	return 0;
}

/* ---------------------------------------------
  Function Name	: sceUsbKeybdGetInfo
  function     	: �L�[�{�[�h�ڑ����̎擾
  Input Data	: none
  Output Data   : info(�L�[�{�[�h�ڑ����)
                  ��64byte�A���C�����g���ꂽSSIZE���̃o�b�t�@���K�v
                   info[0]:�ő�ڑ��\��m
                   info[1]:���݂̐ڑ���
		   info[2]:�L�[�{�[�hNo.0�̐ڑ����(1�̂Ƃ��ڑ���)
                     :
                   info[m+1]:�L�[�{�[�hNo.(m-1)�̐ڑ����(1�̂Ƃ��ڑ���)
  Return Value	: 0(����I��),not0(�ُ�I��)
----------------------------------------------*/
int sceUsbKeybdGetInfo(u_char *info)
{
	int ret;
        
	WaitSema(semaid_rpc); /* �Z�}�t�H�̉����,rpccall_end�֐��ōs���� */
	ret = sceSifCallRpc(&cdUsbKeybd, 1, SIF_RPCM_NOWAIT,
	              &sdata[0],SSIZE, info, SSIZE, rpccall_end, 0);
	if (ret != 0) { return ret; }
	
	return 0;
}

/* ---------------------------------------------
  Function Name	: sceUsbKeybdRead
  function     	: �L�[�{�[�h�f�[�^�̃��[�h
  Input Data	: no(�L�[�{�[�hNo) ���f�o�C�XID�ł͂���܂���B
  Output Data   : data(�L�[�f�[�^) 
                  ��64byte�A���C�����g���ꂽSSIZE���̃o�b�t�@���K�v
                   data[0]:LED���
                           bit0  :NUM LOCK
                           bit1  :CAPS LOCK
                           bit2  :SCROLL LOCK
                           bit3  :COMPOSE
                           bit4  :KANA
                           bit5-7:not used
                   data[1]:�L�[�f�[�^�T�C�Y n (0�̂Ƃ���f�[�^)
		   data[2]:Modifier keys
                           bit0:Left-Ctrl
                           bit1:Left-Shift
                           bit2:Left-Alt(Win),Left-option(Mac)
                           bit3:Left-Win(Win),Left-Apple(Mac)
                           bit4:Right-Ctrl
                           bit5:Right-Shift
                           bit6:Right-Alt(Win),Right-option(Mac)
                           bit7:Right-Win(Win),Right-Apple(Mac)
		   data[3]:Reserved
                   data[4]:�L�[�R�[�h
                     :
                   data[2+n-1]:�L�[�R�[�h
  Return Value	: 0(����I��),not0(�ُ�I��)
----------------------------------------------*/
int sceUsbKeybdRead(u_int no,u_char *data)
{
	int ret;
	
	sdata[0] = no;
	
	WaitSema(semaid_rpc); /* �Z�}�t�H�̉����,rpccall_end�֐��ōs���� */
	ret = sceSifCallRpc(&cdUsbKeybd, 2, SIF_RPCM_NOWAIT,
	              &sdata[0],SSIZE, data, SSIZE, rpccall_end, 0);
	if (ret != 0) { return ret; }
	
	return 0;
}

/* ---------------------------------------------
  Function Name	: sceUsbKeybdGetLocation
  function     	: �L�[�{�[�h�ڑ��ʒu�̎擾
  Input Data	: no(�L�[�{�[�hNo) ���f�o�C�XID�ł͂���܂���B
  Output Data   : location(�L�[�{�[�h�ڑ��ʒu���)
                  ��64byte�A���C�����g���ꂽSSIZE���̃o�b�t�@���K�v
		   location[0]:�{�̂̃|�[�g�ԍ�(���݂��Ȃ��Ƃ��͂O)
		   location[1]:HUB1�̃|�[�g�ԍ�(���݂��Ȃ��Ƃ��͂O)
		   location[2]:HUB2�̃|�[�g�ԍ�(���݂��Ȃ��Ƃ��͂O)
		   location[3]:HUB3�̃|�[�g�ԍ�(���݂��Ȃ��Ƃ��͂O)
		   location[4]:HUB4�̃|�[�g�ԍ�(���݂��Ȃ��Ƃ��͂O)
		   location[5]:HUB5�̃|�[�g�ԍ�(���݂��Ȃ��Ƃ��͂O)
		   location[6]:��ɂO
  Return Value	: 0(����I��),not0(�ُ�I��)
----------------------------------------------*/
int sceUsbKeybdGetLocation(int no,u_char *location)
{
	int ret;
	
	sdata[0] = no;
        
	WaitSema(semaid_rpc); /* �Z�}�t�H�̉����,rpccall_end�֐��ōs���� */
	ret = sceSifCallRpc(&cdUsbKeybd, 3, SIF_RPCM_NOWAIT,
	              &sdata[0],SSIZE, location, SSIZE, rpccall_end, 0);
	if (ret != 0) { return ret; }
	
	return 0;
}

/* -----------------------------------------------------
  Function Name	: sceUsbKeybdSync
  function     	: ���s���̃R�}���h���I������܂ő҂�
  Input Data	: mode(0:�u���b�N 1:�m���u���b�N)
  Output Data   : none
  Return Value	: 0(�I��),1(���s��),-1(mode��0,1�ȊO)
------------------------------------------------------*/
int sceUsbKeybdSync(int mode) 
{
        switch(mode)
	  {
	  case 0:
	    WaitSema(semaid_rpc);
	    SignalSema(semaid_rpc);
	    return 0;
	    break;
	  case 1:
	    return sceSifCheckStatRpc((sceSifRpcData *) &cdUsbKeybd);
	  }

	return -1;
}

/* -----------------------------------------------------
  Function Name	: rpccall_end
  function     	: RPC�R�[�����I�������Ƃ��Ɋ��荞�݋֎~
                  ��ԂŌĂ΂��֐�
  Input Data	: data
  Output Data   : none
  Return Value	: none
------------------------------------------------------*/
static void rpccall_end(void* data)
{
  /* scePrintf("rpccall_end\n"); */
  iSignalSema(semaid_rpc);
}

