/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/*
 *      Copyright (C) 1998-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *        1.00          Oct.10.2000     yoshi       
 *        2.00          Jan.18.2003     kono        for ERX
 *        2.01          May.19.2003     kono        add english comment
 */
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include <eekernel.h>
#include <eeregs.h>

#if USE_ERX
#include <liberx.h>
#include <libc_export.h>
#include <libm_export.h>
#include <libgcc_export.h>
#endif	// USE_ERX

#include <libdma.h>
#include <libdev.h>
#include <libgraph.h>
#include <libpad.h>
#include <libcdvd.h>
#include <libsdr.h>

#include <sif.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <sdrcmd.h>


#ifdef CDROM
#define IOPRP			"cdrom0:\\MODULES\\"IOP_IMAGE_FILE";1"
#define SIO2MAN_IRX		"cdrom0:\\MODULES\\SIO2MAN.IRX;1"
#define PADMAN_IRX		"cdrom0:\\MODULES\\PADMAN.IRX;1"
#define LIBSD_IRX		"cdrom0:\\MODULES\\LIBSD.IRX;1"
#define SDRDRV_IRX		"cdrom0:\\MODULES\\SDRDRV.IRX;1"
#define LIBGRAPH_ERX		"cdrom0:\\MODULES\\LIBGRAPH.ERX;"
#define LIBDMA_ERX		"cdrom0:\\MODULES\\LIBDMA.ERX;1"
#define LIBPKT_ERX		"cdrom0:\\MODULES\\LIBPKT.ERX;1"
#define LIBDEV_ERX		"cdrom0:\\MODULES\\LIBDEV.ERX;1"
#define LIBVU0_ERX		"cdrom0:\\MODULES\\LIBVU0.ERX;1"
#define LIBIPU_ERX		"cdrom0:\\MODULES\\LIBIPU.ERX;1"
#define LIBMPEG_ERX		"cdrom0:\\MODULES\\LIBMPEG.ERX;1"
#define LIBPAD_ERX		"cdrom0:\\MODULES\\LIBPAD.ERX;1"
#define LIBSDR_ERX		"cdrom0:\\MODULES\\LIBSDR.ERX;1"
#define BALLS_ERX		"cdrom0:\\ERX\\BALLS.ERX;1"
#define IGA_ERX			"cdrom0:\\ERX\\IGA.ERX;1"
#define EZMPEG_ERX		"cdrom0:\\ERX\\EZMPEG.ERX;1"
#define BALLS_ERZ		"cdrom0:\\ERX\\BALLS.ERZ;1"
#define IGA_ERZ			"cdrom0:\\ERX\\IGA.ERZ;1"
#define EZMPEG_ERZ		"cdrom0:\\ERX\\EZMPEG.ERZ;1"
#define BALLS_BIN		"cdrom0:\\BIN\\BALLS.BIN;1"
#define IGA_BIN			"cdrom0:\\BIN\\IGA.BIN;1"
#define EZMPEG_BIN		"cdrom0:\\BIN\\EZMPEG.BIN;1"
#define PCM_FILENAME	"cdrom0:\\DATA\\KNOT_L.INT;1"
#define MPEG_FILENAME	"cdrom0:\\DATA\\EZ.M2V;1"
#else	// CDROM
#define IOPRP			"host0:/usr/local/sce/iop/modules/"IOP_IMAGE_file
#define SIO2MAN_IRX		"host0:/usr/local/sce/iop/modules/sio2man.irx"
#define PADMAN_IRX		"host0:/usr/local/sce/iop/modules/padman.irx"
#define LIBSD_IRX		"host0:/usr/local/sce/iop/modules/libsd.irx"
#define SDRDRV_IRX		"host0:/usr/local/sce/iop/modules/sdrdrv.irx"
#define LIBGRAPH_ERX	"host0:/usr/local/sce/ee/modules/libgraph.erx"
#define LIBDMA_ERX		"host0:/usr/local/sce/ee/modules/libdma.erx"
#define LIBPKT_ERX		"host0:/usr/local/sce/ee/modules/libpkt.erx"
#define LIBDEV_ERX		"host0:/usr/local/sce/ee/modules/libdev.erx"
#define LIBVU0_ERX		"host0:/usr/local/sce/ee/modules/libvu0.erx"
#define LIBIPU_ERX		"host0:/usr/local/sce/ee/modules/libipu.erx"
#define LIBMPEG_ERX		"host0:/usr/local/sce/ee/modules/libmpeg.erx"
#define LIBPAD_ERX		"host0:/usr/local/sce/ee/modules/libpad.erx"
#define LIBSDR_ERX		"host0:/usr/local/sce/ee/modules/libsdr.erx"
#define BALLS_ERX		"host0:./balls/balls.erx"
#define IGA_ERX			"host0:./iga/iga.erx"
#define EZMPEG_ERX		"host0:./ezmpeg/ezmpeg.erx"
#define BALLS_ERZ		"host0:./balls/balls.erx.gz"
#define IGA_ERZ			"host0:./iga/iga.erx.gz"
#define EZMPEG_ERZ		"host0:./ezmpeg/ezmpeg.erx.gz"
#define BALLS_BIN		"host0:./balls.bin"
#define IGA_BIN			"host0:./iga.bin"
#define EZMPEG_BIN		"host0:./ezmpeg.bin"
#define PCM_FILENAME	"host0:/usr/local/sce/data/sound/wave/knot_l.int"
#define MPEG_FILENAME	"host0:/usr/local/sce/data/movie/mpeg/ez.m2v"
#endif	// CDROM


// GS 関連
#define SCREEN_WIDTH			320
#define SCREEN_HEIGHT			224
#define OFFX					(((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY					(((4096-SCREEN_HEIGHT)/2)<<4)

static sceGsDBuff db;			// 描画環境


// サウンド関連
#define PCM_SIZE				768*1024
#define TRANS_CH				0
#define TRANS_MODE				SD_TRANS_MODE_WRITE | SD_BLOCK_LOOP

static int g_tidLibsdr;			// libsdrのコールバックスレッドのID
static void *g_pIopHeap;		// IOP側に確保したSPU2転送バッファのアドレス


// libpad用DMAバッファ
static u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned(64)));


// メニュー関係
#define MENU_LOCX	7
#define MENU_LOCY	6

// オーバーレイ
#if USE_OVERLAY

#define MENU_MAX	3

extern int main_balls(const char *);
extern int main_iga(const char *);
extern int main_ezmpeg(const char *);

// over.cmd で定義しているオーバレイアドレス
extern int _overlay_addr __attribute__((section(".data")));

typedef struct {
	const char *s;				// 画面表示デモタイトル
	const char *fn;				// BINファイル名
	int (*func)(const char *);	// 呼び出す関数
	const char *arg;			// 引数文字列
	u_int rdcnt;				// リード回数
	u_int errcnt;				// リードエラー回数
} DMENU;

static DMENU g_DMenus[MENU_MAX] = {
//	{ メニュー, BINファイル名,  関数名,      引数,          0, 0},
	{ "BALLS ", BALLS_BIN,		main_balls,	 NULL,			0, 0},
	{ "IGA   ", IGA_BIN,		main_iga,    NULL,			0, 0},
	{ "EZMPEG", EZMPEG_BIN,		main_ezmpeg, MPEG_FILENAME,	0, 0}
};

#endif	// USE_OVERLAY


// ERX使用
#if USE_ERX

#define MENU_MAX	6

extern SceErxLibraryHeader baseelf_entry;
extern int main_overlay(const char *);

#define LIBERX_THREAD_PRIORITY		10
#define LIBERX_MEMORY_SIZE			(8*1024*1024)
#define LIBERX_MAX_MODULES			32

typedef struct {
	const char *s;				// 画面表示デモタイトル
	const char *fn;				// ERXファイル名
	u_int mode;					// sceSifLoadModuleFile()で指定するモード
	const char *arg;			// 引数文字列
	u_int rdcnt;				// リード回数
	u_int errcnt;				// リードエラー回数
} DMENU;

static DMENU g_DMenus[MENU_MAX] = {
//	{ メニュー, ERXファイル名,  		モード					引数,           0, 0},
	{ "BALLS       ",	BALLS_ERX,		0,						NULL,			0, 0},
	{ "IGA         ",	IGA_ERX,		0,						NULL,			0, 0},
	{ "EZMPEG      ",	EZMPEG_ERX,		0,						MPEG_FILENAME,	0, 0},
	{ "BALLS(GZIP) ",	BALLS_ERZ,		SCE_ERX_ENCODE_GZIP,	NULL,			0, 0},
	{ "IGA(GZIP)   ",	IGA_ERZ,		SCE_ERX_ENCODE_GZIP,	NULL,			0, 0},
	{ "EZMPEG(GZIP)",	EZMPEG_ERZ,		SCE_ERX_ENCODE_GZIP,	MPEG_FILENAME,	0, 0}
};


#endif	// USE_ERX



#define VCNT 300			// 300 フレーム(約5秒)
static int AUTO_vcount;

// ERXモジュールに対してエクスポートする関数
void AutoReset(void);
int  AutoStep(void);

int main(int argc, const char *const argv[]);

static int LoadIrx(const char *pszIrxFileName, int argc, const char *argp);
#if USE_OVERLAY
static int LoadOverlayBinary(const char *pszBinFileName);
#endif	// USE_OVERLAY
#if USE_ERX
static void SetupErxEnvironment(void);
#endif	// USE_ERX

static int Gs_Initialize(void);

static int Spu2DataLoad(void);
static int Spu2AutoDmaStart(void);
static int Spu2AutoDmaStop(void);
static int Spu2TransCallback(int core, void *common);




// メンルーチン
int main(int argc, const char *const argv[])
{
	int port;
	int slot;
	int i;
	static short curs = 0;			// カーソル位置

	(void)argc;
	(void)argv;

#ifdef REPLACE
	sceSifInitRpc(0);
#ifdef CDROM
	sceCdInit(SCECdINIT);
#endif	// CDROM
	while (!sceSifRebootIop(IOPRP));
	while (!sceSifSyncIop());
#endif // REPLACE

	sceSifInitRpc(0);
	sceSifInitIopHeap();
	sceSifLoadFileReset();
	sceFsReset();

#ifdef CDROM
	{
		int mode  = SCECdINIT;			// 
		int media = SCECdCD;			// TODO: DVDを使う場合はSCECdDVDに変えてください。

		sceDevctl("cdrom0:", CDIOC_INIT, &mode, sizeof(int), NULL, 0);
		sceDevctl("cdrom0:", CDIOC_MMODE, &media, sizeof(int), NULL, 0);
	}
#endif	// CDROM

	// IOPモジュールロード
	LoadIrx(LIBSD_IRX,   0, NULL);		// libsd.irxの読みこみ
	LoadIrx(SDRDRV_IRX,  0, NULL);		// sdrvdrv.irxの読みこみ
	LoadIrx(SIO2MAN_IRX, 0, NULL);		// sio2man.irxの読みこみ
	LoadIrx(PADMAN_IRX,  0, NULL);		// padman.irxの読みこみ

	// コールバックスレッド等を動かすために、メインスレッドのプライオリティを下げる
	ChangeThreadPriority(GetThreadId(), 20);

#if USE_ERX
	// ERXライブラリの初期化
	SetupErxEnvironment();
#endif	// USE_ERX

	// サウンド
	sceSdRemoteInit();							// libsdrを初期化
	g_tidLibsdr = sceSdRemoteCallbackInit(5);	// コールバックスレッドを生成
												// メインスレッドより高いプライオリティを指定
	Spu2DataLoad();								// サウンドデータ読みこみ
	Spu2AutoDmaStart();							// AUTO DMA環境を初期化


	// コントローラライブラリ初期化
	port = 0;
	slot = 0;
	scePadInit(0);
	if (scePadPortOpen(port, slot, pad_dma_buf) == 0) {
		printf("ERROR: scePadPortOpen\n");
		return (0);
	}
	AutoReset();

	while (1) {
		int console;		// フォント表示用
		int frame;			// フレームのカウンタ
		u_short rpad = 0;	// パッド押し下げ情報

		// GS初期化
		Gs_Initialize();

		// フォントシステム初期化
		sceDevConsInit();
		console = sceDevConsOpen(OFFX + (16<<4), OFFY + (24<<4), 70, 40);
		sceDevConsClear(console);

		frame = 0;
		sceGsSyncPath(0, 0);
		while (1) {
			u_char rdata[32];
			u_short tpad;

			frame++;

			if (scePadRead(port, slot, rdata)==0) {
				// パッドデータの読みこみ失敗時
				tpad = rpad = 0;
			} else {
				// 前回の情報と比較して、ボタンの押し下げ情報を得る
				u_short paddata;
				paddata = 0xFFFF ^ ((rdata[2]<<8) | rdata[3]);
				tpad = paddata & ~rpad;
				rpad = paddata;
			}

			// カーソル位置の正規化
			if (curs < 0) {
				curs = MENU_MAX - 1;
			} else if (curs >= MENU_MAX) {
				curs = 0;
			}
			if (AutoStep()) {
				// 自動遷移のためのカウントが満了したとき
				tpad |= SCE_PADstart;			// [START]ボタン押し下げ情報をメイク
			}

			if (tpad & SCE_PADstart) {
				// [START]ボタン押し下げ時
#if USE_OVERLAY
				int errcnt;

				errcnt = LoadOverlayBinary(g_DMenus[curs].fn);
				g_DMenus[curs].rdcnt++;						// 読みこみ回数をインクリメント
				g_DMenus[curs].errcnt += errcnt;			// 累積エラー発生回数に加算
				AutoReset();

				// オーバーレイ呼び出し
				printf("*** call child No.%d ***\n", curs);
				g_DMenus[curs].func(g_DMenus[curs].arg);	// オーバレイ呼び出し
				printf("returned main.\n");					// 戻ってきた
				AutoReset();
#endif	// USE_OVERLAY

#if USE_ERX
				int modid;
				int res, result;

				// ERXモジュールの読み込み
				do {
					modid = sceErxLoadModuleFile(g_DMenus[curs].fn, g_DMenus[curs].mode, NULL);
					if (modid<0) {
						printf("sceErxLoadModuleFile(%s): %d(%08X)\n", g_DMenus[curs].fn, modid, modid);
						g_DMenus[curs].errcnt++;			// 累積エラー発生回数に加算
					}
				} while (modid<0);
				g_DMenus[curs].rdcnt++;						// 読みこみ回数をインクリメント

				// モジュールの起動
				res = sceErxStartModule(modid, g_DMenus[curs].fn, 0, "", &result);
				if (res < 0) {
					// モジュールの起動に失敗
					printf("module '%s' can not start. (error=%x)\n", g_DMenus[curs].fn, res);
					while (1);
				}
				printf("module '%s' loaded. (modid=%d)\n", g_DMenus[curs].fn, modid);

				// モジュール内の関数を呼び出し
				printf("*** call child No.%d ***(modid=%d)\n", curs, modid);
				AutoReset();
				main_overlay(g_DMenus[curs].arg);			// ERXモジュール呼び出し
				printf("returned main.\n");

				// モジュールの動作を停止
				res = sceErxStopModule(modid, 0, "", &result);
				printf("sceErxStopModule: %d, %08X\n", modid, res);

				// モジュールの使用していたメモリを解放
				res = sceErxUnloadModule(modid);
				printf("sceErxUnloadModule: %d, %08X\n", modid, res);

				AutoReset();
#endif	// USE_ERX
#ifdef AUTO
				curs++;							// 次のメニューへ
#endif	//AUTO
				break;
			}

			if (tpad & SCE_PADLup)   curs--;	// [↑]ボタン
			if (tpad & SCE_PADLdown) curs++;	// [↓]ボタン

			// フォント表示
			sceDevConsClear(console);
			sceDevConsLocate(console, MENU_LOCX, MENU_LOCY-3);
#if USE_OVERLAY
			sceDevConsPrintf(console, "OVERLAY SAMPLE\n");
#endif	// USE_OVERLAY
#if USE_ERX
			sceDevConsPrintf(console, "ERX SAMPLE\n");
#endif	// USE_ERX
			for (i=0; i<MENU_MAX; i++) {
				sceDevConsLocate(console, MENU_LOCX, MENU_LOCY+i);
				sceDevConsPrintf(console, "%s  %08d %08d\n", g_DMenus[i].s, g_DMenus[i].rdcnt, g_DMenus[i].errcnt);
			}
			sceDevConsLocate(console, MENU_LOCX-3, MENU_LOCY+curs);
			sceDevConsPrintf(console, ">>\n");
			sceDevConsDraw(console);

			// インタレース表示のハーフラスタ調整
			if (frame & 1) {
				sceGsSetHalfOffset(&db.draw1, 2048, 2048, sceGsSyncV(0) ^ 0x01);
			} else {
				sceGsSetHalfOffset(&db.draw0, 2048, 2048, sceGsSyncV(0) ^ 0x01);
			}
			sceGsSyncPath(0, 0);

			FlushCache(WRITEBACK_DCACHE);		// Dキャッシュのライトバック
			sceGsSwapDBuff(&db, frame);
		}
		sceDevConsClose(console);
	}

	// 入出力ブロックへの転送を停止
	sceSdRemote(1, rSdBlockTrans, TRANS_CH, SD_TRANS_MODE_STOP, NULL, 0);
	DeleteThread(g_tidLibsdr);
	return (0);
}



// IRXモジュールロード
static int LoadIrx(const char *pszIrxFileName, int argc, const char *argp)
{
	int modid;
	int result;
	do {
		modid = sceSifLoadStartModule(pszIrxFileName, argc, argp, &result);
		if (modid<0) {
			printf("LoadIrx Failed: %s, %d\n", pszIrxFileName, modid);
		}
	} while (modid<0);
	return (result);
}


#if USE_OVERLAY

// オーバレイファイルロード
static int LoadOverlayBinary(const char *pszBinFileName)
{
	int fd;
	int size;
	int errcnt;

	// オーバーレイファイルをオープン
	do {
		fd = sceOpen(pszBinFileName, SCE_RDONLY);
		if (fd<0) {
			printf("LoadOverlayBinary failed: %s, %d\n", pszBinFileName, fd);
		}
	} while (fd<0);

	// ファイルサイズを取得
	size = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);
	printf("overlay_addr = %p  size = %d\n", &_overlay_addr, size);

	// オーバーレイファイルの読み込み処理本体
	errcnt = 0;
	while ((sceRead(fd, (void *)(&_overlay_addr), size)) != size) {
		errcnt++;							// エラー発生回数をインクリメント
		printf("read failed\n");
	}
	sceClose(fd);

	FlushCache(WRITEBACK_DCACHE);			// Dキャッシュのライトバック
	FlushCache(INVALIDATE_ICACHE);			// Iキャッシュのインバリデート
	return (errcnt);
}

#endif	// USE_OVERLAY


// GS初期化
static int Gs_Initialize(void)
{
	static struct {
		union {
			u_long ul[2];
			sceGifTag tag;
		} gif;
		sceGsAlphaEnv alpha;
	} drawenv __attribute__((aligned(16)));

	// DMA初期化
	sceDmaReset(1);

	// GS初期化
	sceGsResetPath();
	sceGsSyncV(0);
	sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
	sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
							SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);
	db.clear0.rgbaq.R = 0x40;
	db.clear0.rgbaq.G = 0x40;
	db.clear0.rgbaq.B = 0x80;
	db.clear1.rgbaq.R = 0x40;
	db.clear1.rgbaq.G = 0x40;
	db.clear1.rgbaq.B = 0x80;

	drawenv.gif.ul[0] = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
	drawenv.gif.ul[1] = SCE_GIF_PACKED_AD;
	drawenv.gif.tag.NLOOP = sceGsSetDefAlphaEnv(&drawenv.alpha, 0);

	FlushCache(WRITEBACK_DCACHE);			// Dキャッシュのライトバック
	sceGsPutDrawEnv(&drawenv.gif.tag);
	sceGsSyncPath(0, 0);

	// 次の奇数フィールドまで待つ
	while (!sceGsSyncV(0));
	return (0);
}





// サウンド関連
static int Spu2DataLoad(void)
{
	// IOPにSPU2 AutoDMA用転送バッファを確保
	do {
		g_pIopHeap = sceSifAllocSysMemory(0, PCM_SIZE, NULL);
		if (g_pIopHeap==NULL) {
			printf("sceSifAllocSysMemory() failed\n");
		}
	} while (g_pIopHeap==NULL);


	// IOPに確保したバッファに、SPU2データを読み込ませる
	{
		int fd;
		int n;

		do {
			fd = sceOpen(PCM_FILENAME, SCE_RDONLY);
			if (fd<0) {
				printf("sceOpen() failed: %s, %d\n", PCM_FILENAME, fd);
			}
		} while (fd<0);

		n = 0;
		while (n < PCM_SIZE) {
			static u_char buf[16*1024] __attribute__((aligned(64)));
			sceSifDmaData sd;
			int did, res;

			res = sceRead(fd, buf, sizeof(buf));
			if (res<0) {
				printf("sceRead() failed: %d\n", res);
				continue;
			} else if (res==0) {
				// ファイルの終端まで
				break;
			}

			if ((n + res) > PCM_SIZE) {
				res = PCM_SIZE - n;
			}
			sd.data = (u_int)buf;
			sd.addr = (u_int)g_pIopHeap + n;
			sd.size = res;
			sd.mode = 0;
			do {
				did = sceSifSetDma( &sd, 1 );
			} while (did==0);
			while (sceSifDmaStat((u_int)did)>=0);

			n += res;
		}
		sceClose(fd);
	}
	return (0);
}


// AUTO DMA環境を初期化
static int Spu2AutoDmaStart(void)
{
	// SPU2を初期化
	sceSdRemote(1, rSdInit, SD_INIT_COLD);

	// マスターボリュームを設定
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MVOLL, 0x3FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MVOLR, 0x3FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MVOLL, 0x3FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MVOLR, 0x3FFF);

#if TRANS_CH==0
	// SPU2コア0の入力ブロックにAutoDMAするとき
	// コア#0は入力ブロック(Dry),ボイス出力(Dry,Wet)が有効
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MMIX,  SD_MMIX_MINR   | SD_MMIX_MINL   |
														SD_MMIX_MSNDER | SD_MMIX_MSNDEL |
														SD_MMIX_MSNDR  | SD_MMIX_MSNDL);

	// コア#1は外部入力(Dry,Wet),ボイス出力(Dry,Wet)が有効
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MMIX,  SD_MMIX_SINER  | SD_MMIX_SINEL  |
														SD_MMIX_SINR   | SD_MMIX_SINL   |
														SD_MMIX_MSNDER | SD_MMIX_MSNDEL |
														SD_MMIX_MSNDR  | SD_MMIX_MSNDL);
#else
	// SPU2コア1の入力ブロックにAutoDMAするとき
	// コア#0はボイス出力(Dry,Wet)が有効
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MMIX,  SD_MMIX_MSNDER | SD_MMIX_MSNDEL |
														SD_MMIX_MSNDR  | SD_MMIX_MSNDL);

	// コア#1は入力ブロック(Dry),外部入力(Dry,Wet),ボイス出力(Dry,Wet)が有効
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MMIX,  SD_MMIX_SINER  | SD_MMIX_SINEL  |
														SD_MMIX_SINR   | SD_MMIX_SINL   |
														SD_MMIX_MINR   | SD_MMIX_MINL   |
														SD_MMIX_MSNDER | SD_MMIX_MSNDEL |
														SD_MMIX_MSNDR  | SD_MMIX_MSNDL);
#endif


	// 割り込みハンドラを設定
	sceSdRemote(1, rSdSetTransIntrHandler, TRANS_CH, Spu2TransCallback, NULL);

	// 入出力ブロックへの転送を設定
	sceSdRemote(1, rSdBlockTrans, TRANS_CH, TRANS_MODE, g_pIopHeap, (PCM_SIZE/512)*512);

	// コア#1の外部入力(コア#0の出力)ボリュームを設定
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_AVOLL, 0x7FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_AVOLR, 0x7FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_BVOLL, 0x7FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_BVOLR, 0x7FFF);

	// コア#0サウンドデータ入力のボリュームを設定
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_AVOLL, 0);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_AVOLR, 0);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_BVOLL, 0x7FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_BVOLR, 0x7FFF);

	// エフェクト入力のボリュームを設定
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_EVOLL, 0);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_EVOLR, 0);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_EVOLL, 0);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_EVOLR, 0);
	return (0);
}


// AUTO DMA環境を停止
static int Spu2AutoDmaStop(void)
{
	// メインボリュームを0に絞る
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MVOLL, 0x0);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MVOLR, 0x0);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MVOLL, 0x0);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MVOLR, 0x0);

	// 割り込みハンドラを解除
	sceSdRemote(1, rSdSetTransIntrHandler, TRANS_CH, 0, NULL);

	// 入出力ブロックへの転送を停止
	sceSdRemote(1, rSdBlockTrans, TRANS_CH, SD_TRANS_MODE_STOP, NULL, 0);

	// サウンドデータ入力ブロックのボリュームを絞る
	sceSdRemote(1, rSdSetParam, TRANS_CH | SD_P_BVOLL, 0x0);
	sceSdRemote(1, rSdSetParam, TRANS_CH | SD_P_BVOLR, 0x0);
	return (0);
}


// SPU2転送コールバック関数
static int Spu2TransCallback(int core, void *common)
{
	(void)core;
	(void)common;
	return (0);
}


void AutoReset(void)
{
#if USE_ERX
	void *oldgp = sceErxSetModuleGp();
#endif // USE_ERX
	AUTO_vcount = VCNT;
#if USE_ERX
	sceErxSetGp(oldgp);
#endif // USE_ERX
	return;
}

int AutoStep(void)
{
#ifdef AUTO
	int res = 1;

#if USE_ERX
	void *oldgp = sceErxSetModuleGp();
#endif // USE_ERX

	if (AUTO_vcount==0) {
		// タイムアップしている
		res = 1;
	} else {
		// カウント値をデクリメント
		AUTO_vcount--;
		res = 0;
	}
#if USE_ERX
	sceErxSetGp(oldgp);
#endif // USE_ERX
	return (res);
#else
	return (0);
#endif
}



#if USE_ERX
void SetupErxEnvironment(void)
{
	void *pBuf;
	int modid, res;

	// liberx内のsysmemライブラリに使わせるメモリの確保
	pBuf = memalign(256, LIBERX_MEMORY_SIZE);
	res = sceErxInit(LIBERX_THREAD_PRIORITY, pBuf, LIBERX_MEMORY_SIZE, LIBERX_MAX_MODULES);
	printf("sceErxInit: %d\n", res);

	// ダイナミックリンクでロードされるERXのために
	// 本体ELF部分からエクスポートされる関数をliberxに登録

	// libgcc関係の関数をエクスポート
	sceErxRegisterLibraryEntries(sceLibgccCommonGetErxEntries());	// libgcc 算術補助関数をエクスポート

	sceErxRegisterLibraryEntries(sceLibcStdioGetErxEntries());	// stdio関係の関数をエクスポート
	sceErxRegisterLibraryEntries(sceLibcStdlibGetErxEntries());	// stdlib関係の関数をエクスポート
	sceErxRegisterLibraryEntries(sceLibcStringGetErxEntries());	// string関係の関数をエクスポート
	sceErxRegisterLibraryEntries(sceLibcMallocGetErxEntries());	// malloc関係の関数をエクスポート

	sceErxRegisterLibraryEntries(sceLibmFloatGetErxEntries());	// libm関係のfloat関数をエクスポート
	sceErxRegisterLibraryEntries(sceCdGetErxEntries());			// libcdvdの関数をエクスポート

#if 0
	sceErxRegisterLibraryEntries(sceGsGetErxEntries());			// libgraphの関数をエクスポート
	sceErxRegisterLibraryEntries(sceDmaGetErxEntries());		// libdmaの関数をエクスポート
	sceErxRegisterLibraryEntries(scePktGetErxEntries());		// libpktの関数をエクスポート
	sceErxRegisterLibraryEntries(sceDevGetErxEntries());		// libdevの関数をエクスポート
	sceErxRegisterLibraryEntries(sceVu0GetErxEntries());		// libvu0の関数をエクスポート
	sceErxRegisterLibraryEntries(sceIpuGetErxEntries());		// libipuの関数をエクスポート
	sceErxRegisterLibraryEntries(sceMpegGetErxEntries());		// libmpegの関数をエクスポート
	sceErxRegisterLibraryEntries(scePadGetErxEntries());		// libpadの関数をエクスポート
	sceErxRegisterLibraryEntries(sceSdGetErxEntries());			// libsdrの関数をエクスポート
#else
	{
		static const char *const apszErxFiles[] = {
			LIBGRAPH_ERX,
			LIBDMA_ERX,
			LIBPKT_ERX,
			LIBDEV_ERX,
			LIBVU0_ERX,
			LIBIPU_ERX,
			LIBMPEG_ERX,
			LIBPAD_ERX,
			LIBSDR_ERX
		};
		int i;
		for (i=0; i<(int)(sizeof(apszErxFiles)/sizeof(const char *)); i++) {
			// ライブラリのERXモジュールを読み込み
			do {
				modid = sceErxLoadModuleFile(apszErxFiles[i], SCE_ERX_SMEM_LOW, NULL);
			} while (modid<0);

			// 読み込んだERXモジュールを起動
			res = sceErxStartModule(modid, apszErxFiles[i], 0, "", &res);
			if (res < 0) {
				// モジュールの起動に失敗
				printf("module '%s' can not start. (error=%x)\n", apszErxFiles[i], res);
				printf("maybe dependeg library is not found...\n");
				while (1);
			}
			printf("module '%s' loaded. (modid=%d)\n", apszErxFiles[i], modid);
		}
	}
#endif

	// ベースELF部分からエクスポートする関数(SCEライブラリ以外)をliberxに登録
	sceErxRegisterLibraryEntries(&baseelf_entry);
	return;
}
#endif // USE_ERX

