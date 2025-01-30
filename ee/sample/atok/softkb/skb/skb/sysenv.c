/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */

#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <eekernel.h>
#include <sifdev.h>
#include <libscf.h>

#include "shareddef.h"
#include "sysenv.h"

extern gImport_t		g_import;

#define SYSCONF_INI_PATH		HDD_MOUNT"/conf/system.ini"


static int parseSysEnv(const char *pData, u_short *pSystemIni);
static char *getOneLineStrings(char *tmp, char *src);

typedef struct
{
	char *id;		// ID
	int defv;		// 初期設定値
} env_t;

const env_t g_setting[] =
{
	// keyboard		既定値
	{"type = ",		0		},	// type		#0:106		1:101
	{"repeatw = ",	0		},	// repeatw	#0:short	1:middle	2:long
	{"repeats = ",	0		},	// repeats	#0:fast		1:middle	2:slow

	// mouse
	{"speed = ",	0		},	// speed	#0:fast		1:middle	2:slow
	{"dblclk = ",	0		},	// dblclk	#0:fast		1:middle	2:slow
	{"lr = ",		0		},	// lr		#0:left hand 1:right hand

	// atok
	{"mode = ",		0		},	// mode		#0:roma		1:kana
	{"bind = ",		0		},	// bind		#0:setting1	1:setting2

	// laststatus
	{"softkb_onoff = ",		0},	// softkb_onoff		#0:		1:
	{"softkb_qwert = ",		0},	// softkb_qwert		#0:		1:
};


typedef struct
{
	char id[256];		// 設定の名前(ID)
	int v;				// 値
} sysEnv_t;



//============================================================================
//	Sysenv_GetLang												ref: sysenv.h
//
//	言語モード取得
//
//	戻り値: int : 言語モード
int Sysenv_GetLang(void)
{
	return sceScfGetLanguage();
}


//============================================================================
//	Sysenv_ReadSystemIni										ref: sysenv.h
//
//	HDD内の__system.iniを読む
//
//	u_short *pSystemIni : システム設定格納バッファへのポインタ
//
//	戻り値: int : 0:成功/1:失敗
#if 1//SKB_EX SysIniPath
int Sysenv_ReadSystemIni(u_short *pSystemIni, char *fsName )
#else
int Sysenv_ReadSystemIni(u_short *pSystemIni)
#endif
{
	char *pData;
	int size;
	int fd;
	#if 1//SKB_EX SysIniPath
	char systemIni[1024];

	strcpy(systemIni, fsName);
	strcat(systemIni,"/conf/system.ini");
	printf("system ini = %s\n",systemIni);
	fd = sceOpen( systemIni, SCE_RDONLY);
	#else
	fd = sceOpen(SYSCONF_INI_PATH, SCE_RDONLY);
	#endif
	if(fd < 0)
	{
		printf("system ini cannot open\n");
		return -1;
	}
	size = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);

	pData = g_import.malign(16, size);
	if(pData == NULL)
	{
		printf("system ini cannot malloc\n");
		return 1;
	}
	sceRead(fd, pData, sizeof(char)*size);
	sceClose(fd);

	parseSysEnv(pData, pSystemIni);		// システム環境設定を取得
	printf("system ini parse complete\n");

	// CAUTION : USBキーボードライブラリとHDDシステム環境設定ファイルの既定値が違う
	// USBキーボードライブラリに合わせる
	// USBキーボードライブラリ : 101KEY=0/106KEY=1
	// OSD環境設定             : 101KEY=1/106KEY=0
	if(g_import.usSystemIni[ENV_KEYBD_TYPE] <= 1)
	{
		g_import.usSystemIni[ENV_KEYBD_TYPE] ^= 0x00000001;
	}

//	g_import.usSystemIni[ENV_ATOK_BIND] = 0;	// DEBUG : 強制IMEキーバインド
//	g_import.usSystemIni[ENV_ATOK_BIND] = 1;	// DEBUG : 強制ATOKキーバインド
//	g_import.usSystemIni[ENV_KEYBD_REPEATW] = ENV_KEYREPW_LONG;	// DEBUG : 強制リピートウェイト設定
//	g_import.usSystemIni[ENV_KEYBD_REPEATS] = ENV_KEYREPS_SLOW;	// DEBUG : 強制リピートスタート設定

	g_import.mfree(pData);

	return 0;
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	parseSysEnv
//
//	HDDシステム設定を取得
//
//	const char *pData : 読み込んだシステム設定データへのポインタ
//	u_short *pSystemIni : システム設定格納バッファへのポインタ
//
//	戻り値: int : 0:成功
static int parseSysEnv(const char *pData, u_short *pSystemIni)
{
	int i=0;
	int envNum = sizeof(g_setting)/sizeof(env_t);
	char aLine[512];
	char *pCnt = (char *)pData;
	int  cLen;
	char cValue[10];

	while(1)
	{
		// 一行取得
		pCnt = getOneLineStrings(aLine, pCnt);
		if(pCnt == NULL)
		{
			break;
		}
		// 予約語との比較
		for(i=0; i<envNum; i++)
		{
			cLen = strlen(g_setting[i].id);
			if(strncmp(aLine, g_setting[i].id, cLen) == 0)
			{
				// OK
				sprintf(cValue, "%s", &aLine[cLen]);
				*(pSystemIni+i) = atoi(cValue);
				printf("%s%d\n", g_setting[i].id, pSystemIni[i]);
				break;
			}
		}
	}

	return 0;
}


//============================================================================
//	getOneLineStrings
//
//	一行文字列取得
//	文字列が始まるまでのスペース/タブは無視
//
//	char *tmp : 1行分の文字列格納用バッファへのポインタ
//	char *src : 読み込んだシステム設定データへのポインタ
//
//	戻り値: char * : NULL : エラー/読み進んだ分進めたシステム設定データへのポインタ
static char *getOneLineStrings(char *tmp, char *src)
{
	int i=0;
	int flag=0;
	while(*src != '\0')
	{
		if(*src == ';')
		{
			src = src+1;
			tmp[i] = '\0';
			while(*src++ != '\n');	// 行末まで行ったら終了
			return src;
		}
		if(*src == '\n')
		{
			src = src+1;
			tmp[i] = '\0';
			return src;
		}

		// 先頭のスペース/タブは無視
		if((*src == ' ' || *src == '\t') && flag != 1)
		{
			src = src+1;
			continue;
		}

		flag = 1;		// 先頭のスペース/タブは無視 フラグON
		tmp[i] = *src++;
		i++;
	}

	return NULL;
}
