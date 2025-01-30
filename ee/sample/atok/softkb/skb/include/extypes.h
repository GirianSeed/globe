/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
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

// 追加の型を定義

#ifndef __EXTYPE_H__
#define __EXTYPE_H__

#include <libvu0.h>

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif


typedef unsigned char   u_char8;
typedef unsigned short  u_short16;
typedef unsigned int    u_short32;
typedef unsigned int    u_int32;
typedef unsigned int    u_long32;
typedef unsigned long   u_long64;

typedef char    char8;
typedef short   short16;
typedef int		short32;
typedef int		int32;
typedef int		long32;
typedef long    long64;

// 2次元ベクトル方の定義
typedef struct {
	short x __attribute__((aligned(4)));
	short y;
} SPOINT __attribute__((aligned(4)));

typedef struct {
	int x __attribute__((aligned(8)));
	int y;
} IPOINT __attribute__((aligned(8)));

typedef struct {
	long x __attribute__((aligned(8)));
	long y;
} LPOINT __attribute__((aligned(8)));

typedef struct {
	float x __attribute__((aligned(16)));
	float y;
} FPOINT __attribute__((aligned(16)));


// 4次元ベクトル型の定義
typedef struct {
	short x __attribute__((aligned(8)));
	short y, z, w;
} SKB_SVECTOR __attribute__((aligned(8)));

typedef struct {
	int x __attribute__((aligned(16)));
	int y, z, w;
} SKB_IVECTOR __attribute__((aligned(16)));

typedef struct {
	long x __attribute__((aligned(16)));
	long y, z, w;
} SKB_LVECTOR __attribute__((aligned(16)));

typedef struct {
	float x __attribute__((aligned(16)));
	float y, z, w;
} SKB_FVECTOR __attribute__((aligned(16)));

// 矩形型の定義
typedef struct {
	short x __attribute__((aligned(8)));
	short y, w, h;
} SRECT __attribute__((aligned(8)));

typedef struct {
	int x __attribute__((aligned(16)));
	int y, w, h;
} IRECT __attribute__((aligned(16)));

typedef struct {
	long x __attribute__((aligned(16)));
	long y, w, h;
} LRECT __attribute__((aligned(16)));

typedef struct {
	float x __attribute__((aligned(16)));
	float y, w, h;
} FRECT __attribute__((aligned(16)));

// マトリクス型の定義
typedef struct {
	SKB_FVECTOR v[4] __attribute__((aligned(16)));
} SKB_FMATRIX __attribute__((aligned(16)));

typedef struct {
	SPOINT v[2] __attribute__((aligned(16)));
} SPOINTx2 __attribute__((aligned(16)));

// 256ビットのデータを扱うとき、エイリアシングによる最適化ミスを防ぐ目的で使う共用体
typedef union {
	u_long128 aqw[4]  __attribute__((aligned(16)));
	u_long64  aul[8]  __attribute__((aligned(16)));
	long64	  al[8]   __attribute__((aligned(16)));
	u_int	  aui[16] __attribute__((aligned(16)));
	int		  ai[16]  __attribute__((aligned(16)));
	u_short	  aus[32] __attribute__((aligned(16)));
	short	  as[32]  __attribute__((aligned(16)));
	u_char	  auc[64] __attribute__((aligned(16)));
	char	  ac[64]  __attribute__((aligned(16)));
	float	  af[16]  __attribute__((aligned(16)));
	SKB_FMATRIX   fm      __attribute__((aligned(16)));
	SKB_FVECTOR   afv[4]  __attribute__((aligned(16)));
	SKB_IVECTOR   aiv[4]  __attribute__((aligned(16)));
	FRECT     afr[4]  __attribute__((aligned(16)));
	IRECT     air[4]  __attribute__((aligned(16)));
	sceVu0FMATRIX sfm     __attribute__((aligned(16)));
	sceVu0FVECTOR asfv[4] __attribute__((aligned(16)));
	sceVu0IVECTOR asiv[4] __attribute__((aligned(16)));
} union256 __attribute__((aligned(16)));

// 128ビットのデータを扱うとき、エイリアシングによる最適化ミスを防ぐ目的で使う共用体
typedef union {
	u_long128 qw      __attribute__((aligned(16)));
	u_long64  aul[2]  __attribute__((aligned(16)));
	long64	  al[2]   __attribute__((aligned(16)));
	u_int	  aui[4]  __attribute__((aligned(16)));
	int		  ai[4]   __attribute__((aligned(16)));
	u_short	  aus[8]  __attribute__((aligned(16)));
	short	  as[8]   __attribute__((aligned(16)));
	u_char	  auc[16] __attribute__((aligned(16)));
	char	  ac[16]  __attribute__((aligned(16)));
	float	  af[4]   __attribute__((aligned(16)));
	SKB_FVECTOR   fv      __attribute__((aligned(16)));
	SKB_IVECTOR   iv      __attribute__((aligned(16)));
	FRECT	  fr      __attribute__((aligned(16)));
	IRECT	  ir      __attribute__((aligned(16)));
	sceVu0FVECTOR sfv __attribute__((aligned(16)));
	sceVu0IVECTOR siv __attribute__((aligned(16)));
} union128 __attribute__((aligned(16)));

// 64ビットのデータを扱うとき、エイリアシングによる最適化ミスを防ぐ目的で使う共用体
typedef union {
	u_long64 ul;
	u_int	aui[2];
	int		ai[2];
	u_short	aus[4];
	short	as[4];
	u_char	auc[8];
	char	ac[8];
	float	af[2];
} union64 __attribute__((aligned(8)));

// 32ビットのデータを扱うとき、エイリアシングによる最適化ミスを防ぐ目的で使う共用体
typedef union {
	u_int	ui;
	int		i;
	u_short	aus[2];
	short	as[2];
	u_char	auc[4];
	char	ac[4];
	float	f;
} union32 __attribute__((aligned(4)));

// 色を示す32ビット型の定義と、それを扱うマクロ
typedef unsigned int    RGBA32;

#define RGBA(_r, _g, _b, _a)	\
			((RGBA32)((  (_a)<<24) | ((_b)<<16) | ((_g)<<8) | (_r)))

#define RGBA80(_r, _g, _b)	\
			((RGBA32)(((0x80)<<24) | ((_b)<<16) | ((_g)<<8) | (_r)))

#define RGBA00(_r, _g, _b)	\
			((RGBA32)(((0x00)<<24) | ((_b)<<16) | ((_g)<<8) | (_r)))



#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// __EXTYPE_H__

