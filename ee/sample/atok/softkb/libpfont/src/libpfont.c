/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#include <eekernel.h>
#include <eetypes.h>
//#include <assert.h>
#include <string.h>

#define __INLINE__
#include "xvifpk.h"
#undef __INLINE__


#include <libpfont.h>
#include <libgraph.h>

#define SET_VECTOR(_v, _x, _y, _z, _w)	{ _v.f.x = _x; _v.f.y = _y; _v.f.z = _z; _v.f.w = _w; }
#define SET_VECTORxy(_v, _x, _y)	{ _v.f.x = _x; _v.f.y = _y; }

// VECTOR 型
typedef u_long128	scePFontVector;

// VECTOR 型 X
typedef union _scePFontVectorX{
	struct{
		float x;
		float y;
		float z;
		float w;
	}f;
	struct{
		int x;
		int y;
		int z;
		int w;
	}i;
	scePFontVector d;
	sceVu0FVECTOR sce;
}scePFontVectorX __attribute__((aligned(16)));

// MATRIX 型
typedef struct _scePFontMatrix{
	scePFontVector v[4];
}scePFontMatrix  __attribute__((aligned(16)));

// MATRIX 型 X
typedef union _scePFontMatrixX{
	float f[4][4];
	int i[4][4];
	scePFontMatrix d;
	sceVu0FMATRIX sce;
}scePFontMatrixX __attribute__((aligned(16)));

// 頂点情報
typedef struct _scePFontVertex{
	scePFontVectorX stq;
	scePFontVectorX rgb;
	scePFontVectorX xyz;
}scePFontVertex __attribute__((aligned(16)));

// フォントのキャッシュ管理構造体
typedef struct _scePFontCache{
	int code;				// 文字コード(UCS4?)
	int texvalid;			// キャッシュが有効かどうかのフラグ(テクスチャーがGSにあれば1)
	int texidx;				// GSのキャッシュ領域のインデックス(-1:無効)
	struct _scePFontCache* pNext;

	scePFontGlyph glyph;		// グリフ情報
}scePFontCache;

// 簡易テクスチャーキャッシュ。
// ピクセルモードや、フォントサイズが変わるたびにフラッシュされる。
typedef struct _scePFontTexEnv{
	u_int size;			// テクスチャキャッシュのサイズ
	u_int tbp;			// テクスチャキャッシュアドレス
	u_int cbp;			// CLUT アドレス

	u_short psm;		// 現在のピクセルモード
	u_short w;			// 現在のフォント幅
	u_short h;			// 現在のフォント高さ

	u_short num;		// 現在のエントリー数
	u_short tbw;		// 現在の tbw
	u_short tbh;		// 現在の tbh

	u_short tw;			// 現在の tw
	u_short th;			// 現在の th

	int resetenv;		// テクスチャ環境の再設定フラグ

	scePFontBlock const* block;	// 現在参照しているblock(NULL=キャッシュ無効)

}scePFontTexEnv;

// 状態
typedef struct _scePFontContext{
	int calc;				// 描画領域を計算するかどうかのフラグ
	int draw;				// 実際の描画を行なうかどうか
	int width;				// 文字幅
	float pitch;			// ピッチ
	scePFontMatrixX transform;	// 変形マトリクス
	scePFontVectorX color;		// 色
	scePFontVectorX locate;	// 出力座標

	scePFontFRect area_i;		// 描画領域計算ワーク（内側）
	scePFontFRect area_o;		// 描画領域計算ワーク（外側）
}scePFontContext;

// フォント情報
typedef struct _scePFontStat{

	scePFontMatrixX screen;	// スクリーンマトリクス
	scePFontMatrixX transx;	// マトリクス計算ワーク
	scePFontVertex calcv[12];	// 頂点列計算ワーク

	scePFontContext context;	// 状態

	scePFontTexEnv texenv;		// テクスチャー環境

	int cache_num;			// キャッシュできる数
	scePFontCache* pCacheBase;	// キャッシュへのポインタ
	scePFontCache* pCacheTop;	// キャッシュの優先度の高いもの

	scePFontInfo const* pFont;	// フォント情報

	int gs_ctxt;			// GS パケットのコンテキスト
	u_int packet_size;		// 出力中のパケットサイズ
	sceVif1Packet* pPacket;	// 出力対象のパケット

	u_char const* str_base;	// 出力文字列の先頭
	u_char const* str;		// 出力文字列の参照位置

	int (*filter)(int fd, scePFontControl* pCtrl);	// フィルター

}scePFontStat;

#if 0
static void _dumpVertex(scePFontVertex const* pVertex);
static void _dumpVertexH(scePFontVertex const* pVertex);
#endif

//
static int _scePFontDefaultFilter(int fd, scePFontControl* pCtrl);

//
static int _scePFontCalcPacketSpace(scePFontStat* pThis);
static int _scePFontCalcDepth(u_int value);
static int _scePFontGetColorDepth(scePFontBlock const* pBlock);
static void _scePFontCalcNazo(int* w, int* h, int page);
static int _scePFontSetupTexCache(scePFontStat* pThis, scePFontBlock const* pBlock);
static int _scePFontUpdateTex(scePFontStat* pThis, scePFontCache* pCache);
static int _scePFontSetupTexEnv(scePFontStat* pThis);
static void _scePFontFlushTex(scePFontStat* pThis);
static void _scePFontResetArea(scePFontStat* pThis);
static scePFontCache* _scePFontGetCache(scePFontStat* pThis, int code);


//
static int _scePFontPSign(float v);
static scePFontVector _scePFontFTOI0Vector(scePFontVector v0);
static scePFontVector _scePFontScaleVector(scePFontVector v0, float scale);
//static scePFontVector _scePFontSetVector(float x, float y, float z, float w);
static scePFontVector _scePFontAddVector(scePFontVector v0, scePFontVector v1);
static void _scePFontCopyVertex(scePFontVertex* v0, scePFontVertex const* v1);
//static void _scePFontCopyMatrix(scePFontMatrix* m0, scePFontMatrix const* m1);
static void _scePFontUnitMatrix(scePFontMatrix* m);
//static void _scePFontMulVectorNI(scePFontVector* vs, int num, int il, scePFontVector v0);
//static void _scePFontApplyMatrix(scePFontVector* vs, scePFontMatrix const* m0);
//static void _scePFontApplyMatrixNI(scePFontVector* vs, int num, int il, scePFontMatrix const* m0);
static void _scePFontApplyMatrixNIxy(scePFontVector* vs, int num, int il, scePFontMatrix const* m0);
static void _scePFontRotTransPersVN(scePFontVertex* vertex, int num, scePFontMatrix const* m0, scePFontVector v0);
static void _scePFontScaleMatrix(scePFontMatrix* m0, scePFontMatrix const* m1, scePFontVector v0);
//static void _scePFontMinMax(scePFontVector* min, scePFontVector* max, scePFontVector v0);
static void _scePFontMinMax4(scePFontVector* min, scePFontVector* max, scePFontVector v0, scePFontVector v1, scePFontVector v2, scePFontVector v3);
static int _scePFontISqrt(int v);

static void _scePFontRefLoadImage(sceVif1Packet* pPacket, u_int bp, u_int psm, u_int bw, u_long128 const* pImage, u_int size, u_int x, u_int y, u_int w, u_int h);

// scePFontControl へセットされる関数
static int _scePFont_Getc(int fd);
static int _scePFont_Ungetc(int fd);
static int _scePFont_Putc(int fd, int chr);
static int _scePFont_Putcx(int fd, int chr);
static int _scePFont_Calc(int fd, int chr, scePFontFRect* pRect_i, scePFontFRect* pRect_o);

static scePFontControl s_scePFontControl = {
	_scePFont_Getc,
	_scePFont_Ungetc,
	_scePFont_Putc,
	_scePFont_Putcx,
	_scePFont_Calc,
};

/* ------------------------------------------------ */

static int _scePFontPSign(float v){
	union{
		float f;
		struct{
			u_int f:23;
			u_int e:8;
			u_int s:1;
		}b;
	} const tmp = { v };

	if((0 == tmp.b.f) && (0 == tmp.b.e)){
		return 0;
	}

	return (0 == tmp.b.s) ? 1 : -1;
}

static scePFontVector _scePFontFTOI0Vector(scePFontVector v0){
#define _CR_	"\n\t"
	__asm__ volatile(
		"qmtc2			%0, vf4"_CR_
		"vftoi0.xyzw	vf4, vf4"_CR_
		"qmfc2			%0, vf4"_CR_
		: "=r"(v0) : "0"(v0));
	return v0;
#undef _CR_
}

static scePFontVector _scePFontScaleVector(scePFontVector v0, float scale){
#define _CR_	"\n\t"
	unsigned int work;
	__asm__ volatile(
		"qmtc2			%0, vf4"_CR_
		"mfc1			%1, %2"_CR_
		"qmtc2			%1, vf5"_CR_
		"vmulx.xyzw		vf4, vf4, vf5"_CR_
		"qmfc2			%0, vf4"_CR_
		: "=r"(v0), "=r"(work) : "f"(scale), "0"(v0));
	return v0;
#undef _CR_
}

#if 0
static scePFontVector _scePFontSetVector(float x, float y, float z, float w){
#define _CR_	"\n\t"
	scePFontVector v0;
	unsigned int w0;
	unsigned int w1;
	__asm__ volatile(
		"mfc1			%1, %6"_CR_
		"mfc1			%2, %5"_CR_
		"pextlw			%0, %1, %2"_CR_
		"mfc1			%1, %4"_CR_
		"mfc1			%2, %3"_CR_
		"pextlw			%1, %1, %2"_CR_
		"pcpyld			%0, %0, %1"_CR_
		: "=&r"(v0), "=&r"(w0), "=&r"(w1): "f"(x), "f"(y), "f"(z), "f"(w));
	return v0;
#undef _CR_
}
#endif

static scePFontVector _scePFontAddVector(scePFontVector v0, scePFontVector v1){
#define _CR_	"\n\t"
	__asm__ volatile(
		"qmtc2			%0, vf4"_CR_
		"qmtc2			%1, vf5"_CR_
		"vadd.xyzw		vf4, vf4, vf5"_CR_
		"qmfc2			%0, vf4"_CR_
		: "=r"(v0) : "r"(v1), "0"(v0));
	return v0;
#undef _CR_
}

static void _scePFontCopyVertex(scePFontVertex* v0, scePFontVertex const* v1){
#define _CR_	"\n\t"
	u_long128 w0;
	u_long128 w1;
	u_long128 w2;
	__asm__ volatile(
		"lq		%0, 0x00(%4)"_CR_
		"lq		%1, 0x10(%4)"_CR_
		"lq		%2, 0x20(%4)"_CR_
		"sq		%0, 0x00(%3)"_CR_
		"sq		%1, 0x10(%3)"_CR_
		"sq		%2, 0x20(%3)"_CR_
		: "=&r"(w0), "=&r"(w1), "=&r"(w2) : "r"(v0), "r"(v1) : "memory");
#undef _CR_
}

#if 0
static void _scePFontCopyMatrix(scePFontMatrix* m0, scePFontMatrix const* m1){
#define _CR_	"\n\t"
	u_long128 w0;
	u_long128 w1;
	u_long128 w2;
	u_long128 w3;
	__asm__ volatile(
		"lq		%0, 0x00(%5)"_CR_
		"lq		%1, 0x10(%5)"_CR_
		"lq		%2, 0x20(%5)"_CR_
		"lq		%3, 0x30(%5)"_CR_
		"sq		%0, 0x00(%4)"_CR_
		"sq		%1, 0x10(%4)"_CR_
		"sq		%2, 0x20(%4)"_CR_
		"sq		%3, 0x30(%4)"_CR_
		: "=&r"(w0), "=&r"(w1), "=&r"(w2), "=&r"(w3) : "r"(m0), "r"(m1) : "memory");
#undef _CR_
}
#endif

static void _scePFontUnitMatrix(scePFontMatrix* m){
#define _CR_	"\n\t"
	__asm__ volatile(
		"vsub.xyzw	vf4, vf0, vf0"_CR_
		"vadd.w		vf4, vf4, vf0"_CR_
		"vmr32.xyzw	vf5, vf4"_CR_
		"vmr32.xyzw	vf6, vf5"_CR_
		"vmr32.xyzw	vf7, vf6"_CR_
		"sqc2		vf4, 0x30(%0)"_CR_
		"sqc2		vf5, 0x20(%0)"_CR_
		"sqc2		vf6, 0x10(%0)"_CR_
		"sqc2		vf7, 0x0(%0)"_CR_
		: : "r"(m) : "memory");
#undef _CR_
}

#if 0
static void _scePFontMulVectorNI(scePFontVector* vs, int num, int il, scePFontVector v0){
#define _CR_	"\n\t"
	il <<= 4;
	__asm__ volatile(
		"qmtc2			%2, vf4"_CR_

		"0:"_CR_
		"lqc2			vf5, 0x0(%0)"_CR_
		"addi			%1, %1, -1"_CR_
		"vmul.xyzw		vf6, vf4, vf5"_CR_
		"sqc2			vf6, 0x0(%0)"_CR_
		"addu			%0, %0, %3"_CR_
		"bne			$0, %1, 0b"_CR_
		: "=r"(vs), "=r"(num) : "r"(v0), "r"(il), "0"(vs), "1"(num) : "memory");
#undef _CR_
}
#endif

#if 0
static void _scePFontApplyMatrix(scePFontVector* vs, scePFontMatrix const* m0){
#define _CR_	"\n\t"
	__asm__ volatile(
		"lqc2			vf4, 0x00(%1)"_CR_
		"lqc2			vf5, 0x10(%1)"_CR_
		"lqc2			vf6, 0x20(%1)"_CR_
		"lqc2			vf7, 0x30(%1)"_CR_

		"lqc2			vf8, 0x0(%0)"_CR_
		"vmulax.xyzw	ACC, vf4, vf8"_CR_
		"vmadday.xyzw	ACC, vf5, vf8"_CR_
		"vmaddaz.xyzw	ACC, vf6, vf8"_CR_
		"vmaddw.xyzw	vf8, vf7, vf8"_CR_
		"sqc2			vf8, 0x0(%0)"_CR_
		: : "r"(vs), "r"(m0) : "memory");
#undef _CR_
}
#endif

#if 0
static void _scePFontApplyMatrixNI(scePFontVector* vs, int num, int il, scePFontMatrix const* m0){
#define _CR_	"\n\t"
	il <<= 4;
	__asm__ volatile(
		"lqc2			vf4, 0x00(%2)"_CR_
		"lqc2			vf5, 0x10(%2)"_CR_
		"lqc2			vf6, 0x20(%2)"_CR_
		"lqc2			vf7, 0x30(%2)"_CR_

		"0:"_CR_
		"lqc2			vf8, 0x0(%0)"_CR_
		"addi			%1, %1, -1"_CR_
		"vmulax.xyzw	ACC, vf4, vf8"_CR_
		"vmadday.xyzw	ACC, vf5, vf8"_CR_
		"vmaddaz.xyzw	ACC, vf6, vf8"_CR_
		"vmaddw.xyzw	vf8, vf7, vf8"_CR_
		"sqc2			vf8, 0x0(%0)"_CR_
		"addu			%0, %0, %3"_CR_
		"bne			$0, %1, 0b"_CR_
		: "=r"(vs), "=r"(num) : "r"(m0), "r"(il), "0"(vs), "1"(num) : "memory");
#undef _CR_
}
#endif

// vs->z=0 vs->w=1 として計算
static void _scePFontApplyMatrixNIxy(scePFontVector* vs, int num, int il, scePFontMatrix const* m0){
#define _CR_	"\n\t"
	il <<= 4;
	__asm__ volatile(
		"lqc2			vf4, 0x00(%2)"_CR_	// m0[0]
		"lqc2			vf5, 0x10(%2)"_CR_	// m0[1]
		//"lqc2			vf6, 0x20(%2)"_CR_	// m0[2]
		"lqc2			vf7, 0x30(%2)"_CR_	// m0[3]

		"0:"_CR_
		"lqc2			vf8, 0x0(%0)"_CR_	// vf8 = *vs
		"addi			%1, %1, -1"_CR_		// num--
		//"vmulax.xyzw	ACC, vf4, vf8"_CR_	// acc = vf4 * vf8
		//"vmadday.xyzw	ACC, vf5, vf8"_CR_	// acc = acc + vf5 * vf8
		//"vmaddaz.xyzw	ACC, vf6, vf8"_CR_	// acc = acc + vf6 * vf8
		//"vmaddw.xyzw	vf8, vf7, vf8"_CR_	// vf8 = acc + vf7 * vf8
		"vaddax.xyzw	ACC, vf7, vf0"_CR_	// acc = vf7
		"vmaddax.xyzw	ACC, vf4, vf8"_CR_	// acc = acc + vf4 * vf8
		"vmaddy.xyzw	vf8, vf5, vf8"_CR_	// vf8 = acc + vf5 * vf8
		"sqc2			vf8, 0x0(%0)"_CR_	// *vs = vf8
		"addu			%0, %0, %3"_CR_		// vs += il
		"bne			$0, %1, 0b"_CR_

		: "=r"(vs), "=r"(num) : "r"(m0), "r"(il), "0"(vs), "1"(num) : "memory");
#undef _CR_
}

static void _scePFontRotTransPersVN(scePFontVertex* vertex, int num, scePFontMatrix const* m0, scePFontVector v0){

#define _CR_	"\n\t"

	__asm__ volatile(
		"qmtc2			%3, vf10"_CR_			// v0
		"lqc2			vf4, 0x00(%2)"_CR_		// m0[0]
		"lqc2			vf5, 0x10(%2)"_CR_		// m0[1]
		"lqc2			vf6, 0x20(%2)"_CR_		// m0[2]
		"lqc2			vf7, 0x30(%2)"_CR_		// m0[3]
		"vsub.zw		vf10, vf10, vf10"_CR_	// vf10.zw = 0

		"0:"_CR_
		"lqc2			vf8, 0x20(%0)"_CR_		// xyzw
		"lqc2			vf9, 0x00(%0)"_CR_		// st
		"addi			%1, %1, -1"_CR_			// num--
		"vmulax.xyzw	ACC, vf4, vf8"_CR_
		"vmadday.xyzw	ACC, vf5, vf8"_CR_
		"vmaddaz.xyzw	ACC, vf6, vf8"_CR_
		"vmaddw.xyzw	vf8, vf7, vf8"_CR_		// mul matrix
		"vdiv			Q, vf0w, vf8w"_CR_
		"vmul.xyzw		vf9, vf9, vf10"_CR_		// st normalize?
		"vaddw.z		vf9, vf9, vf0"_CR_		// vf9.z = vf0.w(1)
		"vwaitq"_CR_
		"vmulq.xyz		vf8, vf8, Q"_CR_		// xyz
		"vmulq.xyz		vf9, vf9, Q"_CR_		// stq
		"vftoi4.xy		vf8, vf8"_CR_			// xy = f2i4(xy)
		"vftoi0.z		vf8, vf8"_CR_			// z = f2i0(z)
		"vsub.w			vf8, vf8, vf8"_CR_		// w = 0
		"sqc2			vf9, 0x00(%0)"_CR_
		"sqc2			vf8, 0x20(%0)"_CR_
		"addi			%0, %0, 0x30"_CR_
		"bne			$0, %1, 0b"_CR_

		: "=r"(vertex), "=r"(num) : "r"(m0), "r"(v0), "0"(vertex), "1"(num) : "memory");
#undef _CR_
}

static void _scePFontScaleMatrix(scePFontMatrix* m0, scePFontMatrix const* m1, scePFontVector v0){
#define _CR_	"\n\t"
	__asm__ volatile(
		"qmtc2			%2, vf8"_CR_
		"lqc2			vf4, 0x00(%1)"_CR_
		"lqc2			vf5, 0x10(%1)"_CR_
		"lqc2			vf6, 0x20(%1)"_CR_
		"lqc2			vf7, 0x30(%1)"_CR_
		"vmul.xyzw		vf4, vf4, vf8"_CR_
		"vmul.xyzw		vf5, vf5, vf8"_CR_
		"vmul.xyzw		vf6, vf6, vf8"_CR_
		"vmul.xyzw		vf7, vf7, vf8"_CR_
		"sqc2			vf4, 0x00(%0)"_CR_
		"sqc2			vf5, 0x10(%0)"_CR_
		"sqc2			vf6, 0x20(%0)"_CR_
		"sqc2			vf7, 0x30(%0)"_CR_
		:: "r"(m0), "r"(m1), "r"(v0) : "memory");
#undef _CR_
}

static void _scePFontMinMax4(scePFontVector* min, scePFontVector* max, scePFontVector v0, scePFontVector v1, scePFontVector v2, scePFontVector v3){
#define _CR_	"\n\t"
	__asm__ volatile(
		"lqc2		vf4, 0(%0)"_CR_
		"lqc2		vf5, 0(%1)"_CR_

		"qmtc2		%2, vf6"_CR_
		"vmini.xyzw	vf4, vf4, vf6"_CR_
		"vmax.xyzw	vf5, vf5, vf6"_CR_

		"qmtc2		%3, vf6"_CR_
		"vmini.xyzw	vf4, vf4, vf6"_CR_
		"vmax.xyzw	vf5, vf5, vf6"_CR_

		"qmtc2		%4, vf6"_CR_
		"vmini.xyzw	vf4, vf4, vf6"_CR_
		"vmax.xyzw	vf5, vf5, vf6"_CR_

		"qmtc2		%5, vf6"_CR_
		"vmini.xyzw	vf4, vf4, vf6"_CR_
		"vmax.xyzw	vf5, vf5, vf6"_CR_

		"sqc2		vf4, 0(%0)"_CR_
		"sqc2		vf5, 0(%1)"_CR_
		:: "r"(min), "r"(max), "r"(v0), "r"(v1), "r"(v2), "r"(v3) : "memory");
#undef _CR_
}

#if 0
static void _scePFontMinMax(scePFontVector* min, scePFontVector* max, scePFontVector v0){
#define _CR_	"\n\t"

	__asm__ volatile(
		"lqc2		vf4, 0(%0)"_CR_
		"lqc2		vf5, 0(%1)"_CR_
		"qmtc2		%2, vf6"_CR_
		"vmini.xyzw	vf4, vf4, vf6"_CR_
		"vmax.xyzw	vf5, vf5, vf6"_CR_
		"sqc2		vf4, 0(%0)"_CR_
		"sqc2		vf5, 0(%1)"_CR_
		:: "r"(min), "r"(max), "r"(v0) : "memory");
#undef _CR_
}
#endif

static int _scePFontISqrt(int v){
	float const fs = (float)v;
	float fd;
	__asm__ volatile("sqrt.s	%0, %1\n" : "=f"(fd) : "f"(fs));
	return (int)fd;
}

/* ------------------------------------------------ */

size_t scePFontCalcCacheSize(int num){
	return sizeof(scePFontStat) + sizeof(scePFontCache) * num;
}

int scePFontInit(int num, u_long128* pWork){

	scePFontStat* pThis = (scePFontStat*)pWork;

	memset(pThis, 0, sizeof(scePFontStat));

	pThis->cache_num = num;
	pThis->pCacheBase = (scePFontCache*)((u_int)pWork + sizeof(scePFontStat));

	pThis->filter = _scePFontDefaultFilter;
	_scePFontUnitMatrix(&pThis->screen.d);

	pThis->context.width = 0;

	{
		int count = pThis->cache_num;
		scePFontCache* pCache = pThis->pCacheBase;
		scePFontCache* pPrev = NULL;
		pThis->pCacheTop = pCache;
		while(count--){
			pCache->code = 0x00000000U;
			pCache->texvalid = 0;
			pCache->texidx = -1;
			pCache->pNext = NULL;
			if(NULL != pPrev){
				pPrev->pNext = pCache;
			}
			pPrev = pCache;
			pCache = (pCache + 1);
		}
	}

	scePFontFlush((int)pThis);

	return (int)pThis;
}

void scePFontRelease(int fd){
	scePFontStat* pThis = (scePFontStat*)fd;
	memset(pThis, 0, sizeof(scePFontStat));
}

int scePFontSetGsCtxt(int fd, int ctxt){
	int old_ctxt;
	scePFontStat* pThis = (scePFontStat*)fd;
	old_ctxt = pThis->gs_ctxt;
	pThis->gs_ctxt = ctxt;
	return old_ctxt;
}

void scePFontSetTexMem(int fd, u_int tbp, u_int size, u_int cbp){
	scePFontStat* pThis = (scePFontStat*)fd;

	pThis->texenv.size = size;
	pThis->texenv.cbp = cbp;
	pThis->texenv.tbp = tbp;
	_scePFontFlushTex(pThis);
}

void scePFontFlush(int fd){
	scePFontStat* pThis = (scePFontStat*)fd;

	SET_VECTOR(pThis->context.locate, 0.0f, 0.0f, 0.0f, 0.0f);
	SET_VECTOR(pThis->context.color, 1.0f, 1.0f, 1.0f, 1.0f);
	_scePFontUnitMatrix(&pThis->context.transform.d);

	pThis->str_base = NULL;
	pThis->str = NULL;

	{
		scePFontCache* pCache = pThis->pCacheTop;
		do{
			pCache->code = 0;
		}while(NULL != (pCache = pCache->pNext));
	}

	_scePFontFlushTex(pThis);
}

int scePFontAttachData(int fd, u_long128 const* pData){
	scePFontStat* pThis = (scePFontStat*)fd;

	scePFontInfo const* pFont = (scePFontInfo const*)pData;
	if((0x00000000U != pFont->tag.id) || (0x00000000U != pFont->tag.version)){
		return -1;
	}

	pThis->pFont = pFont;

	scePFontFlush(fd);

	return 0;
}

scePFontInfo const* scePFontGetFontInfo(int fd){
	scePFontStat* pThis = (scePFontStat*)fd;
	return pThis->pFont;
}

scePFontBlock const* scePFontGetBlock(int fd, int idx){
	scePFontStat* pThis = (scePFontStat*)fd;

	if(NULL == pThis->pFont){
		return NULL;
	}

	if(idx >= pThis->pFont->block_num){
		return NULL;
	}

	return (scePFontBlock const*)((u_int)pThis->pFont + pThis->pFont->block_offset[idx]);
}

int scePFontGetGlyph(int fd, int chr, scePFontGlyph* pGlyph){

	int num = 0;
	scePFontBlock const* pBlock;

	// block 探索開始
	while(NULL != (pBlock = scePFontGetBlock(fd, num++))){
		int const max = pBlock->data.codeindex_num;
		scePFontCodeIndex const* const pCodeIndex = (scePFontCodeIndex const*)((u_int)pBlock + pBlock->data.codeindex_offset);

#if 0
#if 0
		printf("image_width=%d\n", pBlock->image_width);
		printf("image_height=%d\n", pBlock->image_height);
		printf("max_ascent=%d\n", pBlock->max_ascent);
		printf("max_descent=%d\n", pBlock->max_descent);
		printf("max_width=%d\n", pBlock->max_width);
#endif
		printf("character_num=%d\n", pBlock->data.character_num);
		printf("codeindex_num=%d\n", pBlock->data.codeindex_num);
		printf("codemap_num=%d\n", pBlock->data.codemap_num);

		//printf("proportional_num=%d\n", pBlock->data.proportional_num);

		printf("start_code=%d\n", pCodeIndex[0].start_code);
		printf("end_code=%d\n", pCodeIndex[max - 1].end_code);
#endif

		// block に chr が含まれている可能性があるかチェック
		if((chr < pCodeIndex[0].start_code) || (chr > pCodeIndex[max - 1].end_code)){
			continue;
		}

		{
			scePFontCodeIndex const* pCI = NULL;

			// code_map から chr を探索
			{
				int c;
				int u = -1;
				int d = max;

				do{
					scePFontCodeIndex const* pCur;
					c = (u + d) / 2;

					pCur = &pCodeIndex[c];

					if(pCur->start_code > chr){
						d = c;

					}else if(pCur->end_code < chr){
						u = c;

					}else{
						pCI = pCur;
						break;
					}
				}while((u + 1) != d);
			}

			if(NULL == pCI){
				continue;
			}

			// chr が含まれているコードセットがあれば情報を取り出し終了
			{
				int const ofs = chr - pCI->start_code;
				u_short const* pCodeMap = (u_short const*)((u_int)pBlock + pBlock->data.codemap_offset);
				u_short const idx = pCodeMap[pCI->map_index + ofs];

				if(0xffffU == idx){
					// 対応する文字が無い
					continue;
				}

				// グリフ情報
				{
					int chr_idx = pCI->chr_index + idx;
					int img_size = ((pBlock->image_width * pBlock->image_height * _scePFontGetColorDepth(pBlock)) / 8 + 15) & ~15;

					pGlyph->block = pBlock;
					pGlyph->image = (u_long128 const*)((u_int)pBlock + pBlock->data.image_offset + img_size * chr_idx);
					if(pBlock->attr.propotional){
						pGlyph->prop = (scePFontPropotional const*)((u_int)pBlock + pBlock->data.proportional_offset + sizeof(scePFontPropotional) * chr_idx);
					}else{
						pGlyph->prop = (scePFontPropotional const*)((u_int)pBlock + pBlock->data.proportional_offset);
					}
					pGlyph->kerning = NULL;
				}

				// 文字発見
				return 0;
			}
		}
	}

	// 文字がなかった
	return -1;
}

void scePFontSetScreenMatrix(int fd, sceVu0FMATRIX const* pMatrix){
	scePFontStat* pThis = (scePFontStat*)fd;
	sceVu0CopyMatrix(pThis->screen.sce, *(sceVu0FMATRIX*)(void*)pMatrix);
}

void scePFontGetScreenMatrix(int fd, sceVu0FMATRIX* pMatrix){
	scePFontStat* pThis = (scePFontStat*)fd;
	sceVu0CopyMatrix(*pMatrix, pThis->screen.sce);
}

void scePFontSetFontMatrix(int fd, sceVu0FMATRIX const* pMatrix){
	scePFontStat* pThis = (scePFontStat*)fd;
	sceVu0CopyMatrix(pThis->context.transform.sce, *(sceVu0FMATRIX*)(void*)pMatrix);
}

void scePFontGetFontMatrix(int fd, sceVu0FMATRIX* pMatrix){
	scePFontStat* pThis = (scePFontStat*)fd;
	sceVu0CopyMatrix(*pMatrix, pThis->context.transform.sce);
}

void scePFontSetLocate(int fd, sceVu0FVECTOR const* pLocate){
	scePFontStat* pThis = (scePFontStat*)fd;
	sceVu0CopyVector(pThis->context.locate.sce, *(sceVu0FVECTOR*)(void*)pLocate);
}

void scePFontGetLocate(int fd, sceVu0FVECTOR* pLocate){
	scePFontStat* pThis = (scePFontStat*)fd;
	sceVu0CopyVector(*pLocate, pThis->context.locate.sce);
}

void scePFontSetColor(int fd, sceVu0FVECTOR const* pColor){
	scePFontStat* pThis = (scePFontStat*)fd;
	sceVu0CopyVector(pThis->context.color.sce, *(sceVu0FVECTOR*)(void*)pColor);
}

void scePFontGetColor(int fd, sceVu0FVECTOR* pColor){
	scePFontStat* pThis = (scePFontStat*)fd;
	sceVu0CopyVector(*pColor, pThis->context.color.sce);
}

void scePFontSetPitch(int fd, float pitch){
	scePFontStat* pThis = (scePFontStat*)fd;
	pThis->context.pitch = pitch;
}

float scePFontGetPitch(int fd){
	scePFontStat* pThis = (scePFontStat*)fd;
	return pThis->context.pitch;
}

void scePFontSetWidth(int fd, short width){
	scePFontStat* pThis = (scePFontStat*)fd;
	pThis->context.width = width;
}

short scePFontGetWidth(int fd){
	scePFontStat* pThis = (scePFontStat*)fd;
	return pThis->context.width;
}

int scePFontPutc(int fd, sceVif1Packet* pPacket, int size, int chr){
	scePFontStat* pThis = (scePFontStat*)fd;
	pThis->context.draw = 1;
	pThis->context.calc = 0;
	pThis->texenv.resetenv = 1;
	pThis->pPacket = pPacket;
	pThis->packet_size = size;
	return _scePFont_Putc(fd, chr);
}

int scePFontPuts(int fd, sceVif1Packet* pPacket, int size, char const* str){
	scePFontStat* pThis = (scePFontStat*)fd;
	pThis->context.draw = 1;
	pThis->context.calc = 0;
	pThis->texenv.resetenv = 1;
	pThis->str_base = (u_char const*)str;
	pThis->str = (u_char const*)str;
	return scePFontPutsContinue(fd, pPacket, size);
}

int scePFontPutsContinue(int fd, sceVif1Packet* pPacket, int size){
	scePFontStat* pThis = (scePFontStat*)fd;

	if(NULL == pThis->str_base){
		return 0;
	}

	pThis->pPacket = pPacket;
	pThis->packet_size = size;

	return pThis->filter(fd, &s_scePFontControl);
}

void scePFontSetFilter(int fd, int (*filter)(int fd, scePFontControl* pCtrl)){
	scePFontStat* pThis = (scePFontStat*)fd;
	if(NULL == filter){
		filter = _scePFontDefaultFilter;
	}
	pThis->filter = filter;
}

void scePFontCalcRect(int fd, char const* str, scePFontFRect* pRect_i, scePFontFRect* pRect_o){
	scePFontStat* pThis = (scePFontStat*)fd;

	scePFontContext _backup;
	memcpy(&_backup, &pThis->context, sizeof(_backup));

	_scePFontResetArea(pThis);

	pThis->str_base = (u_char const*)str;
	pThis->str = (u_char const*)str;
	pThis->context.calc = 1;	// 領域計算
	pThis->context.draw = 0;	// 描画禁止

	// 計算
	pThis->filter(fd, &s_scePFontControl);

	// 結果のコピー
	if(NULL != pRect_i){
		*pRect_i = pThis->context.area_i;
	}
	if(NULL != pRect_o){
		*pRect_o = pThis->context.area_o;
	}

	// 状態復帰
	memcpy(&pThis->context, &_backup, sizeof(pThis->context));
}

static void _scePFontResetArea(scePFontStat* pThis){
	scePFontVectorX* pRef = (scePFontVectorX*)&pThis->context.locate;
	float const x = pRef->f.x;
	float const y = pRef->f.y;

	pThis->context.area_i.left = x;
	pThis->context.area_i.right = x;
	pThis->context.area_i.top = y;
	pThis->context.area_i.bottom = y;

	pThis->context.area_o = pThis->context.area_i;
}

static int _scePFontDefaultFilter(int fd, scePFontControl* pCtrl){
	int stat = -1;
	int c;
	while(0 < (c = pCtrl->Getc(fd))){

		if(-1 == pCtrl->Putc(fd, c)){
			pCtrl->Ungetc(fd);
			return stat;
		}
		stat = 1;
	}

	return 0;
}

static int _scePFontCalcPacketSpace(scePFontStat* pThis){
	return (int)pThis->packet_size - (((u_int)pThis->pPacket->pCurrent - (u_int)pThis->pPacket->pBase + sizeof(u_long128) - 1) / sizeof(u_long128));
}

static int _scePFontCalcDepth(u_int value){
	int lzc;
	__asm__ volatile("plzcw %0, %1" : "=r"(lzc) : "r"(value - 1));
	return 31 - lzc;
}

static int _scePFontGetColorDepth(scePFontBlock const* pBlock){
	int result = pBlock->attr.color_mode << 3;
	if(0 == result){
		result = 4;
	}
	return result;
}

static void _scePFontCalcNazo(int* w, int* h, int page){
	int i = _scePFontISqrt(page);
	while(0 != (page % i)){
		i--;
	}
	*w = i;
	*h = page / i;
	//printf("%d %d\n", *w, *h);
}

static void _scePFontFlushTex(scePFontStat* pThis){
	// キャッシュ無効化
	pThis->texenv.block = NULL;
	{
		scePFontCache* pCache = pThis->pCacheTop;
		do{
			pCache->texvalid = 0;
		}while(NULL != (pCache = pCache->pNext));
	}
}

static void _scePFontRefLoadImage(sceVif1Packet* pPacket, u_int bp, u_int psm, u_int bw, u_long128 const* pImage, u_int size, u_int x, u_int y, u_int w, u_int h){

	xVif1PkCnt(pPacket, 0);
	xVif1PkOpenDirectHLCode(pPacket, 0);
	xVif1PkOpenGifTag2(pPacket, SCE_GIF_SET_TAG(0, 0, 0, 0, 0, 1), 0xeUL);
	xVif1PkAddGsAD(pPacket, SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, bp / 64, bw / 64, psm));
	xVif1PkAddGsAD(pPacket, SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, x, y, 0));
	xVif1PkAddGsAD(pPacket, SCE_GS_TRXREG, SCE_GS_SET_TRXREG(w, h));
	xVif1PkAddGsAD(pPacket, SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
	xVif1PkCloseGifTag(pPacket);
	xVif1PkOpenGifTag2(pPacket, SCE_GIF_SET_TAG(0, 1, 0, 0, 2, 0), 0UL);
	xVif1PkCloseGifTag2(pPacket, size);
	xVif1PkCloseDirectHLCode(pPacket);
	xVif1PkRef(pPacket, pImage, size, 0, SCE_VIF1_SET_DIRECTHL(size, 0), 0);
	// 全部で8qwords
}

static int _scePFontSetupTexCache(scePFontStat* pThis, scePFontBlock const* pBlock){
	u_short const _psm[] = { SCE_GS_PSMT4, SCE_GS_PSMT8, SCE_GS_PSMCT16, SCE_GS_PSMCT24, SCE_GS_PSMCT32 };
	int const color = pBlock->attr.color_mode;
	int image_width;
	int image_height;

	// bitbltに適したサイズへ調整
	{
		u_short const _iw[] = { 8 - 1, 8 - 1, 4 - 1, 8 - 1, 2 - 1 };
		u_short const _ih[] = { 4 - 1, 2 - 1, 2 - 1, 2 - 1, 2 - 1 };
		image_width = (pBlock->image_width + 2 + _iw[color]) & ~_iw[color];
		image_height = (pBlock->image_height + 2 + _ih[color]) & ~_ih[color];
	}

	// テクスチャーの環境に変化があれば再設定
	if((_psm[color] != pThis->texenv.psm) || (image_width != pThis->texenv.w) || (image_height != pThis->texenv.h)){
		u_short const _tbw[] = { 128, 128, 64, 64, 64 };
		u_short const _tbh[] = { 128, 64, 64, 32, 32 };
#if 0
		int const tbw = _tbw[color];
		int const tbh = _tbh[color] * ((pThis->texenv.size + 2047) / 2048);
#else
		int tbw;
		int tbh;
		_scePFontCalcNazo(&tbw, &tbh, ((int)pThis->texenv.size + 2047) / 2048);
		tbw *= _tbw[color];
		tbh *= _tbh[color];
#endif

		// キャッシュの初期化
		pThis->texenv.w = image_width;
		pThis->texenv.h = image_height;
		pThis->texenv.num = (tbw / pThis->texenv.w) * (tbh / pThis->texenv.h);
		pThis->texenv.psm = _psm[color];
		pThis->texenv.tw = _scePFontCalcDepth((u_int)tbw);
		pThis->texenv.th = _scePFontCalcDepth((u_int)tbh);
		pThis->texenv.tbw = tbw;
		pThis->texenv.tbh = tbh;

		{
			scePFontCache* ptr = pThis->pCacheTop;
			int const num = pThis->texenv.num;
			int idx = 0;
			do{
				ptr->texidx = (num > idx) ? idx : -1;
				ptr->texvalid = 0;
				idx++;
			}while(NULL != (ptr = ptr->pNext));
		}

		// テクスチャ環境更新
		pThis->texenv.resetenv = 1;
	}

	// インデックスカラーの時はCLUTを転送 8qword
	if(1 >= color){
		int space;
		int count = (0 == color) ? 1 : 16;
		u_int x = 0;
		u_int y = 0;
		// フォーマット上 clut は 128bit アラインメントにある。
		u_long128 const* clut = (u_long128 const*)((u_int)pBlock + pBlock->data.clut_offset);

		space = _scePFontCalcPacketSpace(pThis);
		// 必要なパケット領域があるか調べる。
		if((8 * count) > space){
			// パケット領域が無いので中断
			return 0;
		}
		space -= 8 * count;

		// テクスチャキャッシュ更新
		pThis->texenv.resetenv = 1;

		// CLUTイメージを転送
		while(count--){
			_scePFontRefLoadImage(pThis->pPacket, pThis->texenv.cbp, SCE_GS_PSMCT32, 64, clut, 16 / 4, x, y, 8, 2);
			clut += 16 / 4;
			if(0 == x){
				x = 8;
			}else{
				x = 0;
				y += 2;
			}
		}
		//assert(space == _scePFontCalcPacketSpace(pThis));
	}

	return 1;
}

// キャッシュを有効化
// （GSメモリーへテクスチャーの転送をする）
static int _scePFontUpdateTex(scePFontStat* pThis, scePFontCache* pCache){

	scePFontBlock const* pBlock;

	pBlock = pCache->glyph.block;

	// テクスチャキャッシュの準備
	if(pThis->texenv.block != pBlock){
		if(0 == _scePFontSetupTexCache(pThis, pBlock)){
			return 0;
		}

		pThis->texenv.block = pBlock;
	}

	{
		// bitbltに不適切なサイズだから適切になるようにイメージを加工する。
		// パケット領域に直接イメージを構築。

		sceVif1Packet* pPacket;
		int space;
		int const depth = _scePFontGetColorDepth(pBlock);
		int const w = pThis->texenv.w;
		int const h = pThis->texenv.h;
		int size = (w * h * depth + 127) / 128;

		space = _scePFontCalcPacketSpace(pThis);
		if((1 + 5 + 1 + size + 2) > space){
			// イメージを構築するパケット領域が足らない
			return 0;
		}
		space -= 1 + 5 + 1 + size + 2;

		// cnt 1 qword
		{
			pPacket = pThis->pPacket;
			xVif1PkCnt(pPacket, 0);
			xVif1PkOpenDirectHLCode(pPacket, 0);
		}

		// bitblt 5 qword
		{
			int const tbw = pThis->texenv.tbw;
			int const dw = tbw / w;
			int const x = (pCache->texidx % dw) * w;
			int const y = (pCache->texidx / dw) * h;
			xVif1PkOpenGifTag2(pPacket, SCE_GIF_SET_TAG(0, 0, 0, 0, 0, 1), 0xeL);
			xVif1PkAddGsAD(pPacket, SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(0, 0, 0, pThis->texenv.tbp / 64, tbw / 64, pThis->texenv.psm));
			xVif1PkAddGsAD(pPacket, SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, x, y, 0));
			xVif1PkAddGsAD(pPacket, SCE_GS_TRXREG, SCE_GS_SET_TRXREG(w, h));
			xVif1PkAddGsAD(pPacket, SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));
			xVif1PkCloseGifTag(pPacket);
		}

		// giftag 1 qword
		{
			xVif1PkOpenGifTag2(pPacket, SCE_GIF_SET_TAG(0, 0, 0, 0, 2, 0), 0UL);
			xVif1PkCloseGifTag2(pPacket, (u_int)size);
		}

		// image size
		// イメージ処理
		{
			u_int const* pSrc = (u_int const*)pCache->glyph.image;
			u_int* pDst = xVif1PkReserve(pPacket, (u_int)size * 4);

			int ww;
			int hh;
			int count0;
			int count1;
			int brem;
			int shift;
			u_int dt;
			u_int mask;

			{
				int len0 = (1 + pBlock->image_width) * depth;
				int len1 = w * depth;

				ww = len1 / 32;
				hh = pBlock->image_height;

				count0 = len0 / 32;
				count1 = ww - count0;
				brem = len0 & 31;
				if(0 != brem){
					count1--;
				}
				mask = (1 << brem) - 1;
			}

			{
				u_int* pEnd = pDst + ww;
				while(pEnd != pDst){
					*pDst++ = 0;
				}
			}

			dt = 0;
			shift = 32 - depth;

			do{
				if(0 != count0){
					int count = count0;

					if(0 == shift){
						do{
							*pDst++ = dt;
							dt = *pSrc++;
						}while(--count);
					}else{
						do{
							u_long tmp = (u_long)dt;
							dt = *pSrc++;
							tmp |= (u_long)dt << 32;
							*pDst++ = (u_int)(tmp >> shift);
						}while(--count);
					}
				}

				if(0 == brem){
					dt = 0;

				}else{
					u_long tmp = dt;
					u_int _shift = shift + brem;
					if(32 < _shift){
						tmp |= (u_long)*pSrc << 32;
					}
					*pDst++ = (u_int)(tmp >> shift) & mask;
					tmp >>= _shift;
					tmp <<= _shift;
					shift = _shift - depth;
					if(32 <= shift){
						pSrc++;
						dt = (u_int)(tmp >> 32);
					}else{
						dt = (u_int)tmp;
					}
					shift &= 31;
				}

				{
					int count = count1;
					while(count--){
						*pDst++ = 0;
					}
				}
			}while(--hh);

			{
				u_int* pEnd = pDst + ww;
				while(pEnd != pDst){
					*pDst++ = 0;
				}
			}
		}

		// texflush 2qword
		{
			xVif1PkOpenGifTag2(pPacket, SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeUL);
			xVif1PkAddGsAD(pPacket, SCE_GS_TEXFLUSH, 0UL);
			xVif1PkCloseGifTag(pPacket);
			xVif1PkCloseDirectHLCode(pPacket);
		}

		//assert(space == _scePFontCalcPacketSpace(pThis));
	}

	//printf("%08x : loadtex %04x<%c> (%d)\n", (u_int)pCache, pCache->code, pCache->code, pThis->texenv.num);

	return 1;
}

static int _scePFontSetupTexEnv(scePFontStat* pThis){

	sceVif1Packet* pPacket = pThis->pPacket;
	int space;


	space = _scePFontCalcPacketSpace(pThis);
	if(4 > space){
		// パケット領域不足
		return 0;
	}
	space -= 4;

	xVif1PkCnt(pPacket, 0);
	xVif1PkOpenDirectCode(pPacket, 0);
	xVif1PkOpenGifTag2(pPacket, SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeL);
	xVif1PkAddGsAD(pPacket, SCE_GS_TEX1_1 + pThis->gs_ctxt, SCE_GS_SET_TEX1(0, 0, 1, 1, 0, 0, 0));
	xVif1PkAddGsAD(pPacket, SCE_GS_TEX0_1 + pThis->gs_ctxt, SCE_GS_SET_TEX0(pThis->texenv.tbp / 64, pThis->texenv.tbw / 64, pThis->texenv.psm, pThis->texenv.tw, pThis->texenv.th, 1, 0, pThis->texenv.cbp / 64, SCE_GS_PSMCT32, 0, 0, 1));
	xVif1PkCloseGifTag(pPacket);
	xVif1PkCloseDirectCode(pPacket);
	// 全部で4qword

	//assert(space == _scePFontCalcPacketSpace(pThis));

	return 1;
}

// キャッシュを探し、見つかればそれを返す。
// ない場合グリフを検索し新たにキャッシュへ登録。
static scePFontCache* _scePFontGetCache(scePFontStat* pThis, int code){

	scePFontCache* pCache = pThis->pCacheTop;
	scePFontCache* pTexRef = NULL;
	scePFontCache* pPrev = pCache;

	while(code != pCache->code){
		if(-1 != pCache->texidx){
			// 最後に使われていたTEXキャッシュを記憶しておく
			pTexRef = pCache;
		}

		if((NULL == pCache->pNext) || (0 == pCache->code)){
			// キャッシュに無い。
			break;
		}

		pPrev = pCache;
		pCache = pCache->pNext;
	}

	// キャッシュ更新
	if(pCache != pThis->pCacheTop){
		pPrev->pNext = pCache->pNext;
		pCache->pNext = pThis->pCacheTop;
		pThis->pCacheTop = pCache;

		// 必要ならTEXキャッシュの割り当て
		if((-1 == pCache->texidx) && (pTexRef != pCache) && (NULL != pTexRef)){
			// 仮のテクスチャキャッシュがあれば割り当てる。
			pCache->texidx = pTexRef->texidx;
			pCache->texvalid = 0;
			pTexRef->texidx = -1;
			pTexRef->texvalid = 0;
		}
	}

	if(pCache->code == code){
		if(NULL != pCache->glyph.block){
			return pCache;
		}

		// 無効なグリフだった。
		return NULL;

	}else{
		pCache->texvalid = 0;
		pCache->code = code;

		//printf("%08x : search glyph %04x<%c> (%d)\n", (u_int)pCache, pCache->code, pCache->code, pThis->cache_num);

		// グリフの情報を取得
		if(0 != scePFontGetGlyph((int)pThis, code, &pCache->glyph)){
			// グリフそのものが無い
			pCache->glyph.block = NULL;
			return NULL;
		}
	}

	return pCache;
}

static int _scePFont_Putcx(int fd, int chr){
	int result;
	scePFontStat* pThis = (scePFontStat*)fd;
	int _draw = pThis->context.draw;
	pThis->context.draw = 0;
	result = _scePFont_Putc(fd, chr);
	pThis->context.draw = _draw;
	return result;
}

static int _scePFont_Calc(int fd, int chr, scePFontFRect* pRect_i, scePFontFRect* pRect_o){
	int result;
	scePFontStat* pThis = (scePFontStat*)fd;

	scePFontContext _backup;
	memcpy(&_backup, &pThis->context, sizeof(_backup));

	_scePFontResetArea(pThis);

	pThis->context.calc = 1;	// 領域計算
	pThis->context.draw = 0;	// 描画禁止

	// 計算
	result = _scePFont_Putc(fd, chr);

	// 結果のコピー
	if(NULL != pRect_i){
		*pRect_i = pThis->context.area_i;
	}
	if(NULL != pRect_o){
		*pRect_o = pThis->context.area_o;
	}

	// 状態復帰
	memcpy(&pThis->context, &_backup, sizeof(pThis->context));

	return result;
}

/* 文字の重心はベースライン上でwidthの半分の位置と仮定 */
static int _scePFont_Putc(int fd, int chr){
	scePFontStat* pThis = (scePFontStat*)fd;

	scePFontCache* pCache;

	pCache = _scePFontGetCache(pThis, chr);
	if(NULL == pCache){
		// グリフが見つからなかった
		return -2;
	}

	{
		scePFontVertex* vertex;
		int tex_x = 0;
		int tex_y = 0;

		if(0 != pThis->context.draw){
			int space;

			// テクスチャー準備
			if(0 == pCache->texvalid){
				if(0 == _scePFontUpdateTex(pThis, pCache)){
					// テクスチャーを転送するためのパケット領域が不足
					return -1;
				}
				pCache->texvalid = 1;
			}

			// テクスチャー環境準備
			if(0 != pThis->texenv.resetenv){
				if(0 == _scePFontSetupTexEnv(pThis)){
					// テクスチャー環境を設定するためのパケット領域が不足
					return -1;
				}
				pThis->texenv.resetenv = 0;
			}


			// 40 = dmatag + 2 + giftag + vertex(3 * 12)
			space = _scePFontCalcPacketSpace(pThis);
			if(40 > space){
				// 文字描画に必要なパケット領域が不足
				return -1;
			}
			space -= 40;

			// テクスチャの位置を計算
			{
				int const w = pThis->texenv.w;
				int const h = pThis->texenv.h;
				int const dw = pThis->texenv.tbw / w;
				int const idx = pCache->texidx;
				tex_x = (idx % dw) * w + 1;
				tex_y = (idx / dw) * h + 1;
			}

			// 文字描画パケットの構築
			{
				sceVif1Packet* pPacket = pThis->pPacket;
				xVif1PkCnt(pPacket, 0);
				xVif1PkOpenDirectCode(pPacket, 0);

				// CLAMP を設定 2 qword
				xVif1PkOpenGifTag2(pPacket, SCE_GIF_SET_TAG(0, 0, 0, 0, 0, 1), 0xeL);
				xVif1PkAddGsAD(pPacket, SCE_GS_CLAMP_1 + pThis->gs_ctxt, SCE_GS_SET_CLAMP(2, 2, tex_x - 1, tex_x + pCache->glyph.block->image_width, tex_y - 1, tex_y + pCache->glyph.block->image_height));
				xVif1PkCloseGifTag(pPacket);

				xVif1PkOpenGifTag2(pPacket, SCE_GIF_SET_TAG(0, 1, 1, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRIFAN, 1, 1, 0, 1, 0, 0, pThis->gs_ctxt, 0), 0, 3), 0x512L);
				vertex = (scePFontVertex*)(void*)xVif1PkReserve(pThis->pPacket, 3 * 4 * 12);
				xVif1PkCloseGifTag(pPacket);
				xVif1PkCloseDirectCode(pPacket);
			}

			//assert(space == _scePFontCalcPacketSpace(pThis));

			// 色情報の設定
			{
				int i;
				scePFontVector rgb;

				// 色情報の正規化
				rgb = _scePFontScaleVector(pThis->context.color.d, 128.0f);
				rgb = _scePFontFTOI0Vector(rgb);
				// 頂点列へセット
				for(i = 0; i < 11; i++){
					vertex[i].rgb.d = rgb;
				}
			}

			// テクスチャ座標の設定
			{
				float base_x = (float)(pCache->glyph.prop->base_x + tex_x);
				float base_y = (float)(pCache->glyph.prop->base_y + tex_y);
				float width = (float)pCache->glyph.prop->width + base_x;
				float ascent = -(float)(pCache->glyph.prop->ascent + 1) + base_y;
				float descent = -(float)(pCache->glyph.prop->descent - 1) + base_y;
				float l_bearing = (float)(pCache->glyph.prop->l_bearing - 1) + base_x;
				float r_bearing = (float)(pCache->glyph.prop->r_bearing + 1) + base_x;

				SET_VECTORxy(vertex[0].stq, base_x, base_y);
				SET_VECTORxy(vertex[1].stq, base_x, ascent);
				SET_VECTORxy(vertex[2].stq, l_bearing, ascent);
				SET_VECTORxy(vertex[3].stq, l_bearing, base_y);
				SET_VECTORxy(vertex[4].stq, l_bearing, descent);
				SET_VECTORxy(vertex[5].stq, base_x, descent);

				SET_VECTORxy(vertex[6].stq, width, descent);
				SET_VECTORxy(vertex[7].stq, r_bearing, descent);
				SET_VECTORxy(vertex[8].stq, r_bearing, base_y);
				SET_VECTORxy(vertex[9].stq, r_bearing, ascent);
				SET_VECTORxy(vertex[10].stq, width, ascent);
			}

		}else{
			vertex = pThis->calcv;
		}

		// 描画座標の設定
		{
			float xl = (float)pCache->glyph.prop->width / -2.0f;
			float xr = (float)pCache->glyph.prop->width + xl;
			float ascent = -(float)(pCache->glyph.prop->ascent + 1);
			float descent = -(float)(pCache->glyph.prop->descent - 1);
			float l_bearing = (float)(pCache->glyph.prop->l_bearing - 1) + xl;
			float r_bearing = (float)(pCache->glyph.prop->r_bearing + 1) + xl;

			SET_VECTORxy(vertex[0].xyz, xl, 0.0f);
			SET_VECTORxy(vertex[1].xyz, xl, ascent);
			SET_VECTORxy(vertex[2].xyz, l_bearing, ascent);
			SET_VECTORxy(vertex[3].xyz, l_bearing, 0.0f);
			SET_VECTORxy(vertex[4].xyz, l_bearing, descent);
			SET_VECTORxy(vertex[5].xyz, xl, descent);
			SET_VECTORxy(vertex[6].xyz, xr, descent);
			SET_VECTORxy(vertex[7].xyz, r_bearing, descent);
			SET_VECTORxy(vertex[8].xyz, r_bearing, 0.0f);
			SET_VECTORxy(vertex[9].xyz, r_bearing, ascent);
			SET_VECTORxy(vertex[10].xyz, xr, ascent);
		}

		{	// 変形マトリクスをコピー＆スケーリング＆オフセット
			scePFontVectorX const scale = {{ pCache->glyph.block->scale_x, pCache->glyph.block->scale_y, 1.0f, 1.0f }};
			float xl = 0.0f;
			float xr = 0.0f;

			_scePFontScaleMatrix(&pThis->transx.d, &pThis->context.transform.d, scale.d);

			{
				float wh;
				if(0 != pThis->context.width){
					wh = (float)pThis->context.width / 2.0f;
				}else{
					wh = (float)pCache->glyph.prop->width / 2.0f;
				}
#if 1
				{	// 文字の傾きに応じて字の送り幅を変更
					scePFontVectorX v[4];

					{
						float ascent = -(float)pCache->glyph.prop->ascent;
						float descent = -(float)pCache->glyph.prop->descent;

						SET_VECTORxy(v[0], -wh, ascent);
						SET_VECTORxy(v[1], +wh, ascent);
						SET_VECTORxy(v[2], +wh, descent);
						SET_VECTORxy(v[3], -wh, descent);
					}

					_scePFontApplyMatrixNIxy(&v[0].d, 4, 1, &pThis->transx.d);

					xl = 0.0f;
					xr = 0.0f;

					{
						int i;
						float _xb = pThis->transx.f[3][0];
						float _yb = pThis->transx.f[3][1];
						float _x0;
						float _x1;
						float _y0;
						float _y1;
						int _ys0;
						int _ys1;
						_x0 = v[3].f.x - _xb;
						_y0 = v[3].f.y - _yb;
						_ys0 = _scePFontPSign(_y0);

						for(i = 0; i < 4; i++, _x0 = _x1, _y0 = _y1, _ys0 = _ys1){
							float x;

							_x1 = v[i].f.x - _xb;
							_y1 = v[i].f.y - _yb;
							_ys1 = _scePFontPSign(_y1);

							if(0 == _ys1){
								x = _x1;

							}else if((_ys0 != _ys1) && (0 != _ys0)){
								x = _x0 + (_x1 - _x0) * (0.0f - _y0) / (_y1 - _y0);

							}else{
								continue;
							}
							if(x < xl){
								xl = x;
							}
							if(x > xr){
								xr = x;
							}
						}
					}
				}
#else

				xl = -wh;
				xr = wh;
#endif
			}
			pThis->context.locate.f.x += -xl;
			pThis->transx.d.v[3] = _scePFontAddVector(pThis->context.locate.d, pThis->transx.d.v[3]);
			pThis->context.locate.f.x += xr + pThis->context.pitch;
		}

		//_dumpVertex(vertex);

		// 変形
		_scePFontApplyMatrixNIxy(&vertex[0].xyz.d, 11, 3, &pThis->transx.d);

		//_dumpVertex(vertex);

		// 描画領域計算
		if(0 != pThis->context.calc){

			// 内側の矩形
			{
				scePFontVectorX min = {
					{
						pThis->context.area_i.left,
						pThis->context.area_i.top,
						0.0f,
						0.0f
					}
				};
				scePFontVectorX max = {
					{
						pThis->context.area_i.right,
						pThis->context.area_i.bottom,
						0.0f,
						0.0f	
					}
				};
#if 1
				_scePFontMinMax4(&min.d, &max.d, vertex[1].xyz.d, vertex[4].xyz.d, vertex[5].xyz.d, vertex[10].xyz.d);
#else
				_scePFontMinMax(&min.d, &max.d, vertex[1].xyz.d);
				_scePFontMinMax(&min.d, &max.d, vertex[4].xyz.d);
				_scePFontMinMax(&min.d, &max.d, vertex[5].xyz.d);
				_scePFontMinMax(&min.d, &max.d, vertex[10].xyz.d);
#endif
				pThis->context.area_i.left = min.f.x;
				pThis->context.area_i.right = max.f.x;
				pThis->context.area_i.top = min.f.y;
				pThis->context.area_i.bottom = max.f.y;
			}

			// 外側の矩形
			{
				scePFontVectorX min = {
					{
						pThis->context.area_o.left,
						pThis->context.area_o.top,
						0.0f,	
						0.0f
					}
				};
				scePFontVectorX max = {
					{
						pThis->context.area_o.right,
						pThis->context.area_o.bottom,
						0.0f,
						0.0f
					}
				};
#if 1
				_scePFontMinMax4(&min.d, &max.d, vertex[2].xyz.d, vertex[3].xyz.d, vertex[7].xyz.d, vertex[9].xyz.d);
#else
				_scePFontMinMax(&min.d, &max.d, vertex[2].xyz.d);
				_scePFontMinMax(&min.d, &max.d, vertex[3].xyz.d);
				_scePFontMinMax(&min.d, &max.d, vertex[7].xyz.d);
				_scePFontMinMax(&min.d, &max.d, vertex[9].xyz.d);
#endif
				pThis->context.area_o.left = min.f.x;
				pThis->context.area_o.right = max.f.x;
				pThis->context.area_o.top = min.f.y;
				pThis->context.area_o.bottom = max.f.y;
			}
		}

		//_dumpVertex(vertex);

		if(0 != pThis->context.draw){
			// stの正規化パラメーター
			scePFontVectorX const scale = {{ 1.0f / (float)(1 << pThis->texenv.tw),
										  1.0f / (float)(1 << pThis->texenv.th),
										  1.0f, 0.0f }};

			// 実際の描画座標に変換
			_scePFontRotTransPersVN(vertex, 11, &pThis->screen.d, scale.d);

			// 重なっている頂点は値をコピー
			_scePFontCopyVertex(&vertex[11], &vertex[1]);

			//_dumpVertexH(vertex);
		}
	}

	return 0;
}

static int _scePFont_Getc(int fd){
	scePFontStat* pThis = (scePFontStat*)fd;

	u_char const* str = pThis->str;
	u_char c;
	int code;

	c = *str++;

	if('\0' == c){
		return 0;

	}else if(0x80U > c){
		code = (int)c;

	}else{
		int count;

		if(0xc0U > c){
			return -1;	// illegal code

		}else if(0xe0U > c){
			code = (int)(c & 0x1fU);
			count = 1;

		}else if(0xf0U > c){
			code = (int)(c & 0x0fU);
			count = 2;

		}else if(0xf8U > c){
			code = (int)(c & 0x07U);
			count = 3;

		}else{
			return -1;	// illegal code
		}

		do{
			c = *str++;
			if(0x80 != (c & 0xc0U)){
				return -1;	// illegal code
			}
			code = (code << 6) | (int)(c & 0x3fU);
		}while(--count);
	}

	pThis->str = str;

	return code;
}

static int _scePFont_Ungetc(int fd){
	scePFontStat* pThis = (scePFontStat*)fd;

	u_char const* str = pThis->str;
	u_char const* const str_end = pThis->str_base;

	if(str_end == str){
		return -1;
	}

	do{
		u_char c = *--str;
		if((0x80U > c) || ((0xc0U <= c) && (0xf7U >= c))){
			break;
		}
	}while(str_end != str);

	pThis->str = str;

	return 0;
}

#if 0

#include <stdio.h>

static void _dumpVertex(scePFontVertex const* pVertex){
	scePFontVectorX const* stq;
	scePFontVectorX const* xyz;
	printf("[vertex]\n");
	stq = &pVertex[2].stq; xyz = &pVertex[2].xyz;
	printf("  (%f %f) (%f %f %f)\n", stq->f.x, stq->f.y, xyz->f.x, xyz->f.y, xyz->f.z);
	stq = &pVertex[4].stq; xyz = &pVertex[4].xyz;
	printf("  (%f %f) (%f %f %f)\n", stq->f.x, stq->f.y, xyz->f.x, xyz->f.y, xyz->f.z);
	stq = &pVertex[7].stq; xyz = &pVertex[7].xyz;
	printf("  (%f %f) (%f %f %f)\n", stq->f.x, stq->f.y, xyz->f.x, xyz->f.y, xyz->f.z);
	stq = &pVertex[9].stq; xyz = &pVertex[9].xyz;
	printf("  (%f %f) (%f %f %f)\n", stq->f.x, stq->f.y, xyz->f.x, xyz->f.y, xyz->f.z);
}

static void _dumpVertexH(scePFontVertex const* pVertex){
	scePFontVectorX const* stq;
	scePFontVectorX const* xyz;
	printf("[vertex]\n");
	stq = &pVertex[2].stq; xyz = &pVertex[2].xyz;
	printf("  (%f %f %f) (%04x %04x %04x %04x)\n", stq->f.x, stq->f.y, stq->f.z, xyz->i.x, xyz->i.y, xyz->i.z, xyz->i.w);
	stq = &pVertex[4].stq; xyz = &pVertex[4].xyz;
	printf("  (%f %f %f) (%04x %04x %04x %04x)\n", stq->f.x, stq->f.y, stq->f.z, xyz->i.x, xyz->i.y, xyz->i.z, xyz->i.w);
	stq = &pVertex[7].stq; xyz = &pVertex[7].xyz;
	printf("  (%f %f %f) (%04x %04x %04x %04x)\n", stq->f.x, stq->f.y, stq->f.z, xyz->i.x, xyz->i.y, xyz->i.z, xyz->i.w);
	stq = &pVertex[9].stq; xyz = &pVertex[9].xyz;
	printf("  (%f %f %f) (%04x %04x %04x %04x)\n", stq->f.x, stq->f.y, stq->f.z, xyz->i.x, xyz->i.y, xyz->i.z, xyz->i.w);
}
#endif

/* [eof] */
