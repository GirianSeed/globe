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
	int defv;		// �����ݒ�l
} env_t;

const env_t g_setting[] =
{
	// keyboard		����l
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
	char id[256];		// �ݒ�̖��O(ID)
	int v;				// �l
} sysEnv_t;



//============================================================================
//	Sysenv_GetLang												ref: sysenv.h
//
//	���ꃂ�[�h�擾
//
//	�߂�l: int : ���ꃂ�[�h
int Sysenv_GetLang(void)
{
	return sceScfGetLanguage();
}


//============================================================================
//	Sysenv_ReadSystemIni										ref: sysenv.h
//
//	HDD����__system.ini��ǂ�
//
//	u_short *pSystemIni : �V�X�e���ݒ�i�[�o�b�t�@�ւ̃|�C���^
//
//	�߂�l: int : 0:����/1:���s
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

	parseSysEnv(pData, pSystemIni);		// �V�X�e�����ݒ���擾
	printf("system ini parse complete\n");

	// CAUTION : USB�L�[�{�[�h���C�u������HDD�V�X�e�����ݒ�t�@�C���̊���l���Ⴄ
	// USB�L�[�{�[�h���C�u�����ɍ��킹��
	// USB�L�[�{�[�h���C�u���� : 101KEY=0/106KEY=1
	// OSD���ݒ�             : 101KEY=1/106KEY=0
	if(g_import.usSystemIni[ENV_KEYBD_TYPE] <= 1)
	{
		g_import.usSystemIni[ENV_KEYBD_TYPE] ^= 0x00000001;
	}

//	g_import.usSystemIni[ENV_ATOK_BIND] = 0;	// DEBUG : ����IME�L�[�o�C���h
//	g_import.usSystemIni[ENV_ATOK_BIND] = 1;	// DEBUG : ����ATOK�L�[�o�C���h
//	g_import.usSystemIni[ENV_KEYBD_REPEATW] = ENV_KEYREPW_LONG;	// DEBUG : �������s�[�g�E�F�C�g�ݒ�
//	g_import.usSystemIni[ENV_KEYBD_REPEATS] = ENV_KEYREPS_SLOW;	// DEBUG : �������s�[�g�X�^�[�g�ݒ�

	g_import.mfree(pData);

	return 0;
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	parseSysEnv
//
//	HDD�V�X�e���ݒ���擾
//
//	const char *pData : �ǂݍ��񂾃V�X�e���ݒ�f�[�^�ւ̃|�C���^
//	u_short *pSystemIni : �V�X�e���ݒ�i�[�o�b�t�@�ւ̃|�C���^
//
//	�߂�l: int : 0:����
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
		// ��s�擾
		pCnt = getOneLineStrings(aLine, pCnt);
		if(pCnt == NULL)
		{
			break;
		}
		// �\���Ƃ̔�r
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
//	��s������擾
//	�����񂪎n�܂�܂ł̃X�y�[�X/�^�u�͖���
//
//	char *tmp : 1�s���̕�����i�[�p�o�b�t�@�ւ̃|�C���^
//	char *src : �ǂݍ��񂾃V�X�e���ݒ�f�[�^�ւ̃|�C���^
//
//	�߂�l: char * : NULL : �G���[/�ǂݐi�񂾕��i�߂��V�X�e���ݒ�f�[�^�ւ̃|�C���^
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
			while(*src++ != '\n');	// �s���܂ōs������I��
			return src;
		}
		if(*src == '\n')
		{
			src = src+1;
			tmp[i] = '\0';
			return src;
		}

		// �擪�̃X�y�[�X/�^�u�͖���
		if((*src == ' ' || *src == '\t') && flag != 1)
		{
			src = src+1;
			continue;
		}

		flag = 1;		// �擪�̃X�y�[�X/�^�u�͖��� �t���OON
		tmp[i] = *src++;
		i++;
	}

	return NULL;
}
