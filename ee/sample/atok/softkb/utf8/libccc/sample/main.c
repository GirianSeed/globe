/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library libccc Version 1.1
 */
/*
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     <libccc - main.c>
 */

#include <sys/types.h>
#include <eetypes.h>
#include <eekernel.h>

#include <sifrpc.h>
#include <sifdev.h>
#include <stdio.h>
#include <string.h>

#include "libccc.h"

// �����R�[�h�ϊ����C�u�����e�X�g�v���O����
// SJIS�ŏ����ꂽ host0:test.txt �� utf8 �֕ϊ����ďo�́B >test.utf8.txt
// ����� sjis �֕ϊ����ďo�́B >test.sjis.txt

#define IRXPATH	        "host0:/usr/local/sce/iop/modules/"

static sceCccJISCS _table_jis2ucs[65536];
static sceCccUCS2 _table_ucs2jis[65536];


int main( void ){

	printf("\nlibccc sample program\n>host0:test.txt -> test.utf8.txt, test.sjis.txt\n");

	// �����������i�m���ɓ��������߂�IOP�����u�[�g���Ă���j
	{
		int result;

		sceSifInitRpc(0);

	       	sceSifRebootIop(IRXPATH""IOP_IMAGE_file);
		while(0 == sceSifSyncIop()){
		}

		sceSifInitRpc(0);

		if(0 > (result = sceSifLoadModule(IRXPATH"sio2man.irx", 0, NULL))){
		    printf("sceSifLoadModule(%s) failed %08x\n", IRXPATH"sio2man.irx", -result);
			return -1;
		}
	}

	// �ϊ��e�[�u����������
	{
		{
			int fd = sceOpen("host0:../lib/jis2ucs.bin", SCE_RDONLY);
			if(0 > fd){
				printf("sceOpen(host0:../lib/jis2ucs.bin, SCE_RDONLY) failed\n");
				return -1;
			}
			sceRead(fd, _table_jis2ucs, sizeof(_table_jis2ucs));
			sceClose(fd);
		}

		{
			int fd = sceOpen("host0:../lib/ucs2jis.bin", SCE_RDONLY);
			if(0 > fd){
				printf("sceOpen(host0:../lib/ucs2jis.bin, SCE_RDONLY) failed\n");
				return -1;
			}
			sceRead(fd, _table_ucs2jis, sizeof(_table_ucs2jis));
			sceClose(fd);
		}

		sceCccSetTable(_table_jis2ucs, _table_ucs2jis);
	}


	// �R�[�h�ϊ��e�X�g
	//
	{
		char utf8_buf[4096];	// utf8�̃e�L�X�g
		char sjis_buf[4096];	// sjis�̃e�L�X�g

		// �I���W�i����SJIS�e�L�X�g��ǂݍ��݁B�ő�Sk�o�C�g
		{
			int fd;
			int size;
			fd = sceOpen("host0:test.txt", SCE_RDONLY);
			if(0 > fd){
				printf("sceOpen(host0:test.txt, SCE_RDONLY) failed\n");
				return -1;
			}
			size = sceRead(fd, sjis_buf, sizeof(sjis_buf) - 1);
			sjis_buf[size] = '\0';
			sceClose(fd);
		}

		// SJIS����UTF8�֕ϊ��i�o�b�t�@�͏d�Ȃ��Ă��Ă͂����Ȃ��j
		sceCccSJIStoUTF8(utf8_buf, sizeof(utf8_buf), sjis_buf);

		// UTF8�̕�������t�@�C���֏��������i�ő�SK�o�C�g�j
		{
			int fd;
			fd = sceOpen("host0:test.utf8.txt", SCE_CREAT | SCE_TRUNC | SCE_WRONLY);
			if(0 > fd){
				printf("sceOpen(host0:test.utf8.txt, SCE_CREAT | SCE_TRUNC | SCE_WRONLY) failed\n");
				return -1;
			}
			sceWrite(fd, utf8_buf, strlen(utf8_buf));
			sceClose(fd);
		}

		// UTF8����SJIS�֕ϊ�
		sceCccUTF8toSJIS(sjis_buf, sizeof(sjis_buf), utf8_buf);

		// SJIS�e�L�X�g���t�@�C���֏�������
		{
			int fd;
			fd = sceOpen("host0:test.sjis.txt", SCE_CREAT | SCE_TRUNC | SCE_WRONLY);
			if(0 > fd){
				printf("sceOpen(host0:test.sjis.txt, SCE_CREAT | SCE_TRUNC | SCE_WRONLY) failed\n");
				return -1;
			}
			sceWrite(fd, sjis_buf, strlen(sjis_buf));
			sceClose(fd);
		}
	}

	printf("finish!\n");

	return 0;
}
