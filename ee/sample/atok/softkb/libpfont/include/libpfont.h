/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#ifndef __libpfont__
#define __libpfont__

#include <eetypes.h>
#include <libvu0.h>

#include <libvifpk.h>

#if defined(__cplusplus)
extern "C"{
#endif

/* フォントデータ定義構造体群 */

typedef struct _scePFontTag{
	u_int id;
	u_int version;
	int size;
	int reserved;
}scePFontTag;

typedef struct _scePFontInfo{
	scePFontTag tag;

	char name[32];				/* name(UTF8) */
	char comment[32];			/* comment(UTF8) */
	short max_ascent;
	short max_descent;
	short max_width;
	short reserved;

	int block_num;
	int block_offset[0];
}scePFontInfo;

typedef struct _scePFontBlockAttribute{
	u_int color_mode:3;
	u_int propotional:1;
	u_int reserved:28;
}scePFontBlockAttribute;

typedef struct _scePFontBlock{
	scePFontTag tag;

	scePFontBlockAttribute attr;
	float scale_x;
	float scale_y;
	short image_width;
	short image_height;
	short max_ascent;
	short max_descent;
	short max_width;
	short reserved;

	struct{
		int character_num;
		int image_offset;

		int codeindex_num;
		int codeindex_offset;

		int codemap_num;
		int codemap_offset;

		int proportional_num;
		int proportional_offset;

		int kerning_num;
		int kerning_offset;

		int clut_num;
		int clut_offset;
	}data;

}scePFontBlock;

typedef struct _scePFontCodeIndex{
	int start_code;
	int end_code;
	int map_index;
	int chr_index;
}scePFontCodeIndex;

typedef struct _scePFontProportional{
	short base_x;
	short base_y;
	short l_bearing;
	short r_bearing;
	short ascent;
	short descent;
	short width;
	short kerning;
}scePFontPropotional;

typedef struct _scePFontGlyph{
	scePFontBlock const* block;
	u_long128 const* image;
	scePFontPropotional const* prop;
	void const* kerning;
}scePFontGlyph;

/* フォント用の矩形領域表現 */
typedef struct _scePFontFRect{
	float top;
	float bottom;
	float left;
	float right;
}scePFontFRect;


/* コールバックに渡される関数テーブル */
typedef struct _scePFontControl{

	/* 文字取得
	   return : 文字コード（0=終了,-1=エラー）*/
	int (* const Getc)(int fd);

	/* 文字の取得位置を１文字後退
	   return : 0=成功, -1=先頭位置だった */
	int (* const Ungetc)(int fd);

	/* 文字出力
	   chr : 文字コード
	   return : 0=成功,-1=メモリー不足,-2=グリフが無い */
	int (* const Putc)(int fd, int chr);

	/* 文字出力(描画はせず座標計算のみ)
	   chr : 文字コード
	   return : 0=成功,-1=メモリー不足,-2=グリフが無い */
	int (* const PutcX)(int fd, int chr);

	/* 文字を出力した場合のサイズを取得
	   chr : 文字コード
	   pRect_i : 内側の矩形を受けとる変数へのポインタ
	   pRect_o : 外側の矩形を受けとる変数へのポインタ
	   return : 0=成功,-1=メモリー不足,-2=グリフが無い */
	int (* const Calc)(int fd, int chr, scePFontFRect* pRect_i, scePFontFRect* pRect_o);

}scePFontControl;

/* フォント初期化に必要なメインメモリーのサイズを計算する。
   (固定サイズのワーク＋文字数*文字ワーク)
   num : キャッシュする文字数
   return : 文字数分をキャッシュするのに必要なメモリーサイズ（バイト単位）*/
size_t scePFontCalcCacheSize(int num);

/* フォント初期化
   scePFontCalcCacheSizeにより計算した十分なメモリーを確保して渡す。
   scePFontRelease()をするまで内部で保持し続ける。
   num : キャッシュする文字数（最低１文字以上必要）
   work : キャッシュ用ワークメモリー（128bitアラインメント)
   return : フォント識別子(0>エラー) */
int scePFontInit(int num, u_long128* pWork);

/* フォントリソース解放。関連づけ破棄。*/
void scePFontRelease(int fd);

/* GSパケットのコンテキストを指定
   ctxt : GSコンテキスト(0<default> or 1)
   return: 以前の値 */
int scePFontSetGsCtxt(int fd, int ctxt);

/* テクスチャ転送用のGSメモリー
   １文字の転送に満たない場合の動作は保証しない。
   このメモリーの情報を破壊した場合scePFontFlush()で無効化すること。
   fd : フォント識別子
   tbp : GSのワードアドレス
   size : GSメモリーのワード数(2048ワードの倍数)
   cbp : 32bitCLUT用ワードアドレス(256エントリ分必要) */
void scePFontSetTexMem(int fd, u_int tbp, u_int size, u_int cbp);

/* 出力途中の文字列を破棄し、キャッシュをクリア。
   変形マトリクスを単位行列に、LOCATEを(0,0,0,0)に設定。
   colorをデフォルト(1.0,1.0,1.0,1.0)に設定。
   fd : フォント識別子 */
void scePFontFlush(int fd);

/* フォントデータと関連づけ
   fd : フォント識別子
   pData : フォントデータ（qword境界にあること）
   return : 0=成功,-1=失敗 */
int scePFontAttachData(int fd, u_long128 const* pData);

/* 関連づけられているフォント情報への参照を返す
   fd : フォント識別子
   return : フォント情報への参照（エラー時はNULL）*/
scePFontInfo const* scePFontGetFontInfo(int fd);

/* 関連づけられているフォントから指定ブロックへの参照を取り出す。
   fd : フォント識別子
   idx : ブロック番号
   return : フォントブロック情報への参照（エラー時はNULL）*/
scePFontBlock const* scePFontGetBlock(int fd, int idx);

/* 関連づけられているフォントから指定文字コードに対応するグリフを取得。
   fd : フォント識別子
   chr : 文字コード
   pGlyph : glyphデータを受けとるための変数へのポインタ。
   return : 0=成功,-1=失敗 */
int scePFontGetGlyph(int fd, int chr, scePFontGlyph* pGlyph);

/* 座標変換マトリクスの設定(ローカル→スクリーン）
   fd : フォント識別子
   pMatrix : 座標変換マトリクスへのポインタ */
void scePFontSetScreenMatrix(int fd, sceVu0FMATRIX const* pMatrix);

/* 現在の座標変換マトリクスを取得
   fd : フォント識別子
   pMatrix : 座標変換マトリクスを受けとる変数へのアドレス */
void scePFontGetScreenMatrix(int fd, sceVu0FMATRIX* pMatrix);

/* 変形マトリクスの設定。文字単位のスケーリングや回転や移動
   fd : フォント識別子
   pMatrix : 変形マトリクスへのポインタ */
void scePFontSetFontMatrix(int fd, sceVu0FMATRIX const* pMatrix);

/* 現在の変形マトリクスを取得
  （中心は base_x+width/2,base_y）
   fd : フォント識別子
   pMatrix : 変形マトリクスを受けとる変数へのアドレス */
void scePFontGetFontMatrix(int fd, sceVu0FMATRIX* pMatrix);

/* 描画開始位置設定
   fd : フォント識別子
   pLocate : 描画位置 */
void scePFontSetLocate(int fd, sceVu0FVECTOR const* pLocate);

/* カレントの描画位置取得
   fd : フォント識別子
   pLocate : 位置を受けとる変数へのポインタ */
void scePFontGetLocate(int fd, sceVu0FVECTOR* pLocate);

/* 描画色設定（元の文字の色に乗算される）
   fd : フォント識別子
   pColor : 色 */
void scePFontSetColor(int fd, sceVu0FVECTOR const* pColor);

/* 描画色取得
   fd : フォント識別子
   pColor : 色を受けとる変数へのポインタ */
void scePFontGetColor(int fd, sceVu0FVECTOR* pColor);

/* 字間を設定
   fd : フォント識別子
   pitch : pitch */
void scePFontSetPitch(int fd, float pitch);

/* 字間を取得
   fd : フォント識別子
   width : 0=デフォルト 1<=固定文字幅 */
float scePFontGetPitch(int fd);

/* 計算上の文字幅を設定（表示幅は変わらない）
   fd : フォント識別子
   width : 0=デフォルト 1<=固定文字幅 */
void scePFontSetWidth(int fd, short width);

/* 設定されている文字幅を取得
   fd : フォント識別子
   width : 0=デフォルト 1<=固定文字幅 */
short scePFontGetWidth(int fd);




/* 描画パケットについての捕捉。
   パケットはcntで付けられ閉じられる。
   アルファはonで描画される。
   コンテキスト１が使用される。
   ALPHA_0とTEST_0は外部で設定すること。*/

/* １文字出力
   fd : フォント識別子
   packet : 初期化済みパケット
   size : 最大パケットサイズ(qwc)
   chr : 出力文字（UCS2）
   return : 0=成功, -1=失敗（パケットに空きがない）*/
int scePFontPutc(int fd, sceVif1Packet* pPacket, int size, int chr);

/* 文字列出力（パケット化）
   fd : フォント識別子
   packet : 初期化済みパケット
   size : 最大パケットサイズ(qwc)
   str : 文字列
   return : 0=終了, 1=続きがある, -1=パケットが小さすぎる */
int scePFontPuts(int fd, sceVif1Packet* pPacket, int size, char const* str);

/* フォント出力継続
   fd : フォント識別子
   packet : 初期化済みパケット
   size : 最大パケットサイズ(qwc)
   return : 0=終了, 1=続きがある, -1=パケットが小さすぎる */
int scePFontPutsContinue(int fd, sceVif1Packet* pPacket, int size);

/* フォント出力範囲計算（ローカル座標系）
   （現在のステータスは保存される）
   fd : フォント識別子
   str : 出力テストする文字列
   pRect_i : 内側の矩形を受けとる変数へのポインタ
   pRect_o : 外側の矩形を受けとる変数へのポインタ */
void scePFontCalcRect(int fd, char const* str, scePFontFRect* pRect_i, scePFontFRect* pRect_o);

/* フィルター関数設定（scePFontPuts, scePFontPutsContinue, scePFontCalcRectから呼ばれる）
   func : フィルター関数へのポインタ。NULLを渡すとデフォルト実装。*/
#if 0 /* sample (default) */
/* フィルター関数
   fd : フォント識別子
   pCtrl : フォント制御用構造体
   return  : 0=終了, 1=続きがある, -1=パケットが小さすぎる */
int default_filter(int fd, scePFontControl* pCtrl){
	int stat = -1;
	int c;
	while(0 < (c = pCtrl->Getc(fd))){
		if(-1 == pCtrl->Putc(fd, c)){
			pCtrl->Ungetc(fd);
			return stat;
		}
		stat = 1;
	};
	return 0;
}
#endif
void scePFontSetFilter(int fd, int (*filter)(int fd, scePFontControl* pCtrl));

/* libpfont ライブラリのエントリの取得。 */
void *scePFontGetErxEntries(void);

#if defined(__cplusplus)
}
#endif

#endif /* __pfont__ */
