/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
 */
/* 
 *                libpfont Library Sample Program
 *
 *                           - basic -
 *
 *                          Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                             sample.c
 *                      main function of basic.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Jan,14,2002      gens
 */

#include <string.h>
#include <malloc.h>

#include <eetypes.h>
#include <sifdev.h>

#include <libdma.h>
#include <libgraph.h>
#include <libpfont.h>

#include "sample.h"

typedef struct
{
	int				ode;			/* 画面描画同期 */
	u_int			frame;			/* フレーム数 */
	sceGsDBuff		db;				/* ダブルバッファ */

	/*** ここから font 関連 ***/
	u_long128*		font_data;		/* データ（.pf ファイル） */
	int				fd;				/* 識別子 */

	sceVu0FMATRIX	font_screen;	/* スクリーン投影用マトリクス */
	sceVu0FMATRIX	font_matrix;	/* 変形用マトリクス */
	sceVu0FVECTOR	font_locate;	/* 描画開始位置 */

	sceVu0FVECTOR	font_color;		/* 色 */
	float			font_pitch;		/* 字間 */
	int				font_putx;		/* 改行時のために左端を覚えておく */

	/*** ここまで font 関連 ***/

} SAMPLE_WORK;

static SAMPLE_WORK		_sw;
static char				_sample_text[4096];

static sceVif1Packet	_Packet;
static sceVif1Packet*	_pPacket;
static u_long128		_font_cache[64];	/* 1k bytes */
static u_long128*		_pktbuf[2];
static int				_exit_flag = 0;

/* スプライト用GIFタグ */
union
{
	u_long d[2];
	u_long128 tag;
} const s_gif = {{ SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeUL }};

/* デフォルトテキスト */
static char	c_readtext[] = "host0:utf8.txt";

/* フォントデータ */
static char	c_readfont[] = "host0:sample.pf";

/* フィルター関数 */
static int _sampleFontFilter(int fd, scePFontControl* pCtrl);

static void sampleDrawKick(void);
static void sampleClear(sceGsDBuff* pDB);
static void samplePuts(char const* str, int x, int y);

/* おまけ： 四角を描画する */
static void sampleDrawSolidRect(
	int left, int right, int top, int bottom, u_int rgba);

int appInit(void)
{
	/* ワーク初期化 */
	_pPacket		= &_Packet;

	_pktbuf[0]		= (u_long128*)(SCRATCH_PAD_RAM);
	_pktbuf[1]		= (u_long128*)(SCRATCH_PAD_RAM + PKTBUF_SIZE);

	memset(&_sw, 0, sizeof(_sw));
	memset(_pPacket, 0, sizeof(sceVif1Packet));
	memset(_font_cache, 0, sizeof(u_long128) * 64);
	memset(_pktbuf[0], 0, sizeof(u_long128) * PKTBUF_SIZE * 2);

	_sw.font_data	= NULL;
	_sw.fd			= -1;

	/* パケット初期化 */
	sceVif1PkInit(_pPacket, _pktbuf[0]);

	/*** ここから font 関連 ***/
	/* フォントデータ読み込みワーク確保 */
	{
		int		filed;		/* ファイル */

		if(NULL == (_sw.font_data = (u_long128*)memalign(16, FONT_DATASIZE)))
		{
			scePrintf("Memory alloc faild.\n");
			return -1;
		}
		/* サンプルフォントデータ読み込み */
		if(0 > (filed = sceOpen(c_readfont, SCE_RDONLY)))
		{
			scePrintf("sceOpen(\"%s\", SCE_RDONLY) failed (%d)\n",
				c_readfont, filed);
			return -1;
		}
		sceRead(filed, _sw.font_data, FONT_DATASIZE);
		sceClose(filed);
	}
	/* フォント初期化 */
	{
		int		csize = scePFontCalcCacheSize(2) - scePFontCalcCacheSize(1);
		int		fsize = scePFontCalcCacheSize(1) - csize;
		int		num   = (sizeof(_font_cache) - fsize) / csize;

		/* フォント管理構造体初期化設定 */
		_sw.fd	= scePFontInit(num, _font_cache);

		if(sizeof(_font_cache) < scePFontCalcCacheSize(num))
		{
			scePrintf("font cache size faild.\n");
			return -1;
		}
		/* フォントイメージ用のテクスチャ領域を設定 */
		scePFontSetTexMem(
			_sw.fd, FONT_GSMEM * 64, FONT_GSMEMSIZE * 64, CLUT_GSMEM * 64);

		/* フォントデータを結びつける */
		if(0 != scePFontAttachData(_sw.fd, _sw.font_data))
		{
			scePrintf("scePFontAttachData(sw.fd, sw.font_data) failed.\n");
			return -1;
		}
		/* 独自処理のためのフィルターをセット */
		scePFontSetFilter(_sw.fd, _sampleFontFilter);

		/* フォントデータ初期化 */
		{
			_sw.font_color[0]	= 0xff;		/* R */
			_sw.font_color[1]	= 0xff;		/* G */
			_sw.font_color[2]	= 0xff;		/* B */
			_sw.font_color[3]	= 0xff;		/* A */

			_sw.font_pitch	= 2.5f;			/* 字間 */
		}
	}
	/* 表示属性初期化 */
	{
		/* スクリーン投影用マトリクス設定 */
		sceVu0UnitMatrix(_sw.font_screen);
		{
			sceVu0FMATRIX	m;

			sceVu0UnitMatrix(m);

			m[1][1] = 0.5f;			/* 縦サイズを半分にする */
			sceVu0MulMatrix(_sw.font_screen, m, _sw.font_screen);
		}
		_sw.font_screen[3][0]	= 2048.0f - SCREEN_WIDTH / 2;
		_sw.font_screen[3][1]	= 2048.0f - SCREEN_HEIGHT / 2;

		/* 文字変形用マトリクス設定 */
		sceVu0UnitMatrix(_sw.font_matrix);
	}
	/*** ここまで font 関連 ***/

	return 0;
}

/* アプリケーション終了、解放処理 */
void appExit(void)
{
	if(0 <= _sw.fd)
	{
		scePFontRelease(_sw.fd);
		_sw.fd	= -1;
	}
	if(NULL != _sw.font_data)
	{
		free(_sw.font_data);
		_sw.font_data	= NULL;
	}
}

int appRun(int argc, char* argv[])
{
	sceGsDBuff*		pDB;

	/* サンプルテキストデータ読み込み */
	{
		char*	filename = c_readtext;
		int		filed;
		int		len;

		if(1 < argc)
		{
			/* パラメタ入力があったならば、それをファイル名とみなす */
			/* （例: run main.elf host0:myutf8.txt） */
			filename	= argv[1];
		}
		if(0 > (filed = sceOpen(filename, SCE_RDONLY)))
		{
			scePrintf("sceOpen(\"%s\", SCE_RDONLY) failed (%d)\n",
				filename, filed);
			return filed;
		}
		len		= sceRead(filed, _sample_text, sizeof(_sample_text) - 1);
		_sample_text[len]	= '\0';

		sceClose(filed);
	}
	FlushCache(WRITEBACK_DCACHE);

	pDB	= (sceGsDBuff*)(((u_int)(&_sw.db) & 0x0fffffffU) | 0x20000000U);

	/* ダブルバッファ指定（画面をクリアしたくない場合は SCE_GS_NOCLEAR） */
	sceGsSetDefDBuff(pDB, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		SCE_GS_ZNOUSE, SCE_GS_PSMZ24, SCE_GS_CLEAR);

	_sw.frame 	= 0;
	_exit_flag	= 0;

	/* 前回描画域をクリアする */
	sampleClear(pDB);

	/* メインループ */
	for(;;)
	{
		/* VSync待ち */
		_sw.ode = ! sceGsSyncV(0);

		/* ダブルバッファ切替え */
		{
			int		side = _sw.frame & 1;

//			scePrintf("frame=%d ode=%d side = %d\n", _sw.frame, _sw.ode, side);

			/* インターレス描画処理 */
			sceGsSetHalfOffset(side ? &pDB->draw1 : &pDB->draw0,
				2048, 2048, _sw.ode);

			sceGsSwapDBuff(pDB, side);

			/* フレーム数インクリメント */
			_sw.frame++;
		}
		/* 終了チェック */
		if(0 != _exit_flag)
		{
			/* 終了時描画域をクリアする */
			sampleClear(pDB);

			break;
		}
		/* 5 秒後に強制終了させる */
		if(60 * 5 < _sw.frame)
		{
			scePrintf("time up!\n");
			_exit_flag	= 1;
		}
		/* 緑ウィンドウ */
		sampleDrawSolidRect(100, SCREEN_WIDTH - 100, 50, SCREEN_HEIGHT -50,
			0xff446644U);

		/* 文字出力 */
		samplePuts(_sample_text, 100 + 10, 50 + 5);
	}
	return 0;
}

/*** ここから font 関連 ***/
/* フォントコールバック処理 */
static int _sampleFontFilter(int fd, scePFontControl* pCtrl)
{
	int		stat = -1;
	int		c;

	while(0 < (c = pCtrl->Getc(fd)))
	{
		/* １文字取得 */
		if(0x20 > c)
		{
			/* コントロールコード */
			switch(c)
			{
				case '\n':	/* 改行処理 */
				{
					sceVu0FVECTOR v;
					scePFontGetLocate(fd, &v);

					v[0]	= (float)_sw.font_putx;		/* 最初のX座標 */
					v[1]	+= 26.0f;					/* 適当な行間 */
					scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);

					break;
				}
				case '\t':	/* タブ */
				{
					sceVu0FVECTOR v;
					scePFontGetLocate(fd, &v);

					v[0]	+= 32.0f;					/* 適当なタブ幅 */
					scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);

					break;
				}
				case 0x0d:	/* Windows files 改行削除 */
					break;
				default:;
			}
		}
		else
		{
#ifdef _DEBUG // {
			/* 文字が登録されているかチェックする。 */
			{
				scePFontGlyph	glyph;

				if(-1 == scePFontGetGlyph(fd, c, &glyph))
				{
					/* 文字が登録されていない */
					scePrintf("This is not support font ! (0x%08x)\n", c);
					_exit_flag	= 1;
				}
			}
#endif // }
			/* 通常文字描画処理 */
			if(-1 == pCtrl->Putc(fd, c))
			{
				/* パケット領域が不足したため取得した文字を戻して抜ける */
				pCtrl->Ungetc(fd);

				return stat;
			}
			stat	= 1;
		}
	}
	return 0;
}

/* フォント出力 */
static void samplePuts(char const* str, int x, int y)
{
	/* フォントデータ初期化 */
	{
		_sw.font_putx	= x;
		_sw.font_locate[0]	= (float)x;
		_sw.font_locate[1]	= (float)(y * 2) + 26.0f;
	}
	/* フォントの座標やマトリクスを与える */
	scePFontFlush(_sw.fd);
	scePFontSetScreenMatrix(_sw.fd, (sceVu0FMATRIX const*)&_sw.font_screen);
	scePFontSetFontMatrix(_sw.fd, (sceVu0FMATRIX const*)&_sw.font_matrix);

	/* 位置 */
	scePFontSetLocate(_sw.fd, (sceVu0FVECTOR const*)&_sw.font_locate);
	/* 色 */
	scePFontSetColor(_sw.fd, (sceVu0FVECTOR const*)&_sw.font_color);
	/* 字間 */
	scePFontSetPitch(_sw.fd, _sw.font_pitch);

	/* フォントを描画するための環境を設定 */
	{
		sceVif1PkCnt(_pPacket, 0);
		sceVif1PkOpenDirectCode(_pPacket, 0);
		sceVif1PkOpenGifTag(_pPacket, s_gif.tag);

		/* 半透明 */
		sceVif1PkAddGsAD(_pPacket,
			SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1));
		sceVif1PkAddGsAD(_pPacket,
			SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 1, 0, 1, 0));

		sceVif1PkCloseGifTag(_pPacket);
		sceVif1PkCloseDirectCode(_pPacket);
	}
	/* 文字処理開始 */
	/* scePFontPuts の中から先に設定したfilterが呼ばれる */
	if(1 == scePFontPuts(_sw.fd, _pPacket, PKTBUF_SIZE - 1, str))
	{
		do
		{
			/* 溜ったパケットを出力 */
			sampleDrawKick();

			/* 継続処理 */
		} while(1 == scePFontPutsContinue(_sw.fd, _pPacket, PKTBUF_SIZE - 1));
	}
	/* 描画 */
	sampleDrawKick();

	return;
}

/*** ここまで font 関連 ***/

/* 描画開始 */
static void sampleDrawKick(void)
{
	sceDmaChan*		dc = sceDmaGetChan(SCE_DMA_VIF1);
	u_int			addr = (u_int)_pPacket->pBase;

	/* END タグの追加 */
	sceVif1PkEnd(_pPacket, 0);

	/* ターミネイト */
	sceVif1PkTerminate(_pPacket);

	/* DMA 転送を許可する */
	dc->chcr.TTE	= 1;

	/* DMA 転送完了待ち */
	sceDmaSync(dc, 0, 0);

	/* DMA 転送開始 */
	{
		/* SPR上のアドレスの場合DMAに設定するアドレスは */
		/* 31bit目を1にする必要があるので変換する。 */
		/* Runtime Library Release 2.5 以降は関数内部で行うので必要ない処理 */
		if((addr >> 28 ) == 7 )
		{
			addr	&= 0x0fffffff;
			addr	|= D_MADR_SPR_M;
		}
		/* ここまで */
	}
	sceDmaSend(dc, (void*)addr);

	/* 初期化 */
	sceVif1PkInit(_pPacket,
		(_pPacket->pBase == _pktbuf[0]) ? _pktbuf[1] : _pktbuf[0]);

	/* データ転送の終了待ち */
	sceGsSyncPath(0, 0);

	return;
}

/* 描画クリア */
static void sampleClear(sceGsDBuff* pDB)
{
	int		side = _sw.frame & 1;

	/* 終了時描画域をクリアする */
	sceGsSwapDBuff(pDB, side);
    sceGsSyncPath(0, 0);
	sceGsSyncV(0);
	sceGsSwapDBuff(pDB, side ^ 1);
    sceGsSyncPath(0, 0);
	sceGsSyncV(0);

	return;
}

/* 矩形描画 */
static void sampleDrawSolidRect(
	int left, int right, int top, int bottom, u_int rgba)
{
	sceVif1PkCnt(_pPacket, 0);
	sceVif1PkOpenDirectCode(_pPacket, 0);
	sceVif1PkOpenGifTag(_pPacket, s_gif.tag);

	/* トライアングルストリップ */
	sceVif1PkAddGsAD(_pPacket, SCE_GS_PRIM,
		SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 0, 0, 0, 0, 0, 0, 0));

	/* 色 */
	sceVif1PkAddGsAD(_pPacket, SCE_GS_TEST_1,
		SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1));
	sceVif1PkAddGsAD(_pPacket, SCE_GS_RGBAQ, (u_long)rgba);

	/* 頂点 */
	sceVif1PkAddGsAD(_pPacket, SCE_GS_XYZ2,
		SCE_GS_SET_XYZ((SCREEN_LEFT + left) << 4,
		(SCREEN_TOP + top) << 4, 0));
	sceVif1PkAddGsAD(_pPacket, SCE_GS_XYZ2,
		SCE_GS_SET_XYZ((SCREEN_LEFT + right) << 4,
		(SCREEN_TOP + top) << 4, 0));
	sceVif1PkAddGsAD(_pPacket, SCE_GS_XYZ2,
		SCE_GS_SET_XYZ((SCREEN_LEFT + left) << 4,
		(SCREEN_TOP + bottom) << 4, 0));
	sceVif1PkAddGsAD(_pPacket, SCE_GS_XYZ2,
		SCE_GS_SET_XYZ((SCREEN_LEFT + right) << 4,
		(SCREEN_TOP + bottom) << 4, 0));

	sceVif1PkCloseGifTag(_pPacket);
	sceVif1PkCloseDirectCode(_pPacket);

	/* 描画開始 */
	sampleDrawKick();

	return;
}

/* [eof] */
