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

// 文字コード変換ライブラリテストプログラム
// SJISで書かれた host0:test.txt を utf8 へ変換して出力。 >test.utf8.txt
// さらに sjis へ変換して出力。 >test.sjis.txt

#define IRXPATH	        "host0:/usr/local/sce/iop/modules/"

static sceCccJISCS _table_jis2ucs[65536];
static sceCccUCS2 _table_ucs2jis[65536];


int main( void ){

	printf("\nlibccc sample program\n>host0:test.txt -> test.utf8.txt, test.sjis.txt\n");

	// 初期化処理（確実に動かすためにIOPをリブートしている）
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

	// 変換テーブルを初期化
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


	// コード変換テスト
	//
	{
		char utf8_buf[4096];	// utf8のテキスト
		char sjis_buf[4096];	// sjisのテキスト

		// オリジナルのSJISテキストを読み込み。最大４kバイト
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

		// SJISからUTF8へ変換（バッファは重なっていてはいけない）
		sceCccSJIStoUTF8(utf8_buf, sizeof(utf8_buf), sjis_buf);

		// UTF8の文字列をファイルへ書きだし（最大４Kバイト）
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

		// UTF8からSJISへ変換
		sceCccUTF8toSJIS(sjis_buf, sizeof(sjis_buf), utf8_buf);

		// SJISテキストをファイルへ書きだし
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
