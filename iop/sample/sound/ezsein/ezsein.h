/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * I/O Processor Library Sample Program
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * ezsein - ezsein.h
 *    headder for EzSEIn
 */

/* SIF RPC コマンド
 *    0x8000ビットは戻り値があることを表す
 *    0x1000ビットは引数が構造体であること表す */

#define EZSEIN_INIT           0x8010
#define EZSEIN_QUIT           0x0020
#define EZSEIN_SETHD	      0x9050
#define EZSEIN_TRANSBDPACKET  0x9060
#define EZSEIN_TRANSBD        0x0070
#define EZSEIN_GETFILELENGTH  0x9080
#define EZSEIN_SETATTR        0x00a0
#define EZSEIN_SETVOL         0x00b0
#define EZSEIN_SETMAXVOICES   0x00c0

#define SEIN_TRANS_SIZE	256
#define SEIN_BUF_SIZE	(SEIN_TRANS_SIZE - (sizeof(sceCslSeStream) + sizeof(int)))
typedef struct {
    sceCslSeStream sestream;
    char buf[SEIN_BUF_SIZE];
    int flag;
} SEInBuf;

typedef struct {
    unsigned int     type;
    unsigned int     hdAddr;
    unsigned int     bdAddr;
    unsigned int     bdSize;
    unsigned int     spuAddr;
    char             bdName[128-4*5];
} EzSEInBank;

#define PORTATTR_MASK_PRIORITY  0x00FF
#define PORTATTR_MASK_MAXNOTE   0xFF00

/* SIF RPC リクエスト識別子
 *   サンプルなのでユーザー用番号。適宜変更のこと。 */
#define EZSEIN_DEV   0x00023456

/* デバッグ用 */

//#define PRINTF(x) printf x
#define PRINTF(x) 

#define ERROR(x) printf x
//#define ERROR(x) 

#define BASE_priority  32

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
