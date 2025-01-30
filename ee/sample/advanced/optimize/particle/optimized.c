/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Emotion Engine Library Sample Program
 *
 *                           - <particle> -
 *
 *                          Version <1.00>
 *                               SJIS
 *
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                            <optimize.c>
 *                    <the way of optimize EE CORE>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Aug,22,2000     wada        First Version
 *       0.02           Sep,26,2000     kumagae     VER8,VER9追加
 *       0.03           Sep,27,2000     kumagae     Doc追加
 *       1.00           Nov, 6,2000     kumagae     first version
 *       1.01           Apr,29,2003     hana        change coding style
 */

/*
あらかじめ初期状態のサイクル数を測定します。このプログラムの演算の特徴は
motion2関数が99%の演算時間を消費している部分です。よってチューニングの対象は
motion2関数になるので、この関数で消費しているサイクル数を計測しました。

           NormalCycle    dcache-miss 
0.初期状態 8.6M           169k
1.VER1     7.4M           112k
2.VER2     6.6M            77k
3.VER3     6.6M            39k
4.VER4     5.8M            42k
5.VER5     5.6M           100k
(VER5のときだけKernelで消費されるサイクルが出てくる。各モードの割合は
User 3.8M Kernel 1.4M Excep 0.3M Idle 0.1M)
           NormalCycle    dcache-miss  PeakCycle
6.VER6     4.5Mcycle         42k        6.3M
7.VER7     4.5Mcycle         42k        5.1M
8.VER8     4.18Mcycle        42k       4.78M
9.VER9     4.19Mcycle        43k       4.61M

VER5とVER6は排他的になっています。
VER5 VU0micro mode
VER6 VU0macro mode

各バージョンの違い
VER1では,iのforループを全体で行なうように変更しています。
VER2では,VER1の結果削除できる冗長な配列を削除しています。
VER3では,VER2で小さくなった配列をSPR上に載せています。
VER4では,sceVu0関数を使えるところに適用しています。
VER5では,VU0microを用いています。
VER6では,VU0macroを用いています。

VER7以降はVER6をベースにピーク時のサイクル数を削減していきます。
VER7はcosf,sinfをSPR上にテーブルを作っています。
VER8ではsceVu0関数をアセンブリ言語で置き換えます。次に
memcpyを呼び出す回数減らしました。
VER8でもピーク時に4.7M以上になるフレームが1,2箇所あったので、
VER9では,ピークを抑える仕組みを入れています。

以上で1frame(1/60s)で処理が完了していることを確認できます。
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <eekernel.h>
#include <eeregs.h>
#include <math.h>
#include <libdma.h>
#include <libgraph.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libvu0.h>
#include <libpad.h>
#include <libpc.h>
#include <libpkt.h>

/*Version選択 無効時は最適化なし*/
#define VER9

#define DEBUG_PRINT

/*PC Counter設定*/
const int PCCONTROL = (SCE_PC0_CPU_CYCLE | SCE_PC_U0 | SCE_PC_K0 | SCE_PC_EXL0 |
					   SCE_PC1_DCACHE_MISS | SCE_PC_U1 | SCE_PC_K1 | SCE_PC_EXL1 | SCE_PC_CTE);

/*VER5のとき,WaitSemaで1つしかないスレッドが待ち状態に入ると,RUN状態のスレッドがなくなるため
カーネルのサイクル数が見かけ上増えてしまいます。正確にカーネルで消費されたサイクル数を求めるためには,
IDLE_THREADを有効にする必要があります。
idle_cycle = IDLE_THREAD有効時のカーネルサイクル - IDLE_THREAD無効時のカーネルサイクル*/
//#define IDLE_THREAD 


#ifdef VER9
#define VER8
#endif
#ifdef VER8
#define VER7
#endif
#ifdef VER7
#define VER6
#endif
#ifdef VER6
#define VER4
#endif
#ifdef VER5
#define VER4
#endif
#ifdef VER4
#define VER3
#endif
#ifdef VER3
#define VER2
#endif
#ifdef VER2
#define VER1
#endif


/*
SPR上のデータ管理構造体
0x70000000-0x7000063f:op一時的利用
0x70000640-0x70000c7f:cp一時的利用
0x70000c80-0x70000e0f:SIN定数利用
0x70000e10-0x70000fff:COS定数利用
*/
#ifdef VER7
float *SINF = (float *)0x70000c80;
float *COSF = (float *)0x70000e10;
#endif


extern u_int My_dma_start0 __attribute__ ((section(".vudata")));
extern u_int My_surface_polygon_left __attribute__ ((section(".vudata")));
extern u_int My_surface_polygon_right __attribute__ ((section(".vudata")));
extern u_int My_surface_polygon_3 __attribute__ ((section(".vudata")));
extern u_int My_surface_polygon_4 __attribute__ ((section(".vudata")));

extern u_int My_surface_polygon_ground __attribute__ ((section(".vudata")));

#define NBALLS	400
#define RAD 30

// ---- params for display ----
#define SCREEN_WIDTH 640.0f
#define SCREEN_HEIGHT 224.0f
#define SCREEN_OFFSET_X 1728.0f
#define SCREEN_OFFSET_Y 1936.0f

#define FOV 42.0f
#define ASPECT 1.42f
#define PI 3.141592653589793238f

/* SX: mesh numbers of X-axis , SY: mesh numbers of Y-axis for surface model*/
#define SX 100
#define SY 100

/* 変換行列 */

sceVu0FMATRIX local_world;
sceVu0FMATRIX world_view;
sceVu0FMATRIX view_screen;

sceVu0FMATRIX local_view;
sceVu0FMATRIX local_screen;


/* オブジェクトの回転および平行移動 */
sceVu0FVECTOR theta[NBALLS];	/* angle of each object */
sceVu0FVECTOR trans[NBALLS];	/* position of each object */
sceVu0FVECTOR dtheta[NBALLS];	/* rotation quantity per one frame */
sceVu0FVECTOR dtrans[NBALLS];	/* transfer quantity per one frame */
sceVu0FVECTOR glight;			/* light vector */

/* デバイス初期化 */
void DevInit(void);				/* Device Initialize routine */

#ifdef VER9
int gResetCount;
#endif

/* DMA Wait */
void WaitDMA(void);				/* Waiting the end of Ch.8 DMA transfer */
void WaitVU1(void);				/* Waiting the end of VU1 calculation */

/* Matrix 演算 */
void SetViewScreenMatrix(void);
void SetCameraPosition(sceVu0FVECTOR outCamera_p, sceVu0FVECTOR inInterest, float inTheta, float inPhi);
void RotTrsMatrix(sceVu0FMATRIX slm, sceVu0FVECTOR rot, sceVu0FVECTOR trs);

/* 二つのオブジェクトの距離 */
float dist(sceVu0FVECTOR * src, sceVu0FVECTOR * dst);

/* カメラの位置設定 */

/* カメラマトリクス生成 */
void CreateViewingMatrix(sceVu0FVECTOR view, sceVu0FVECTOR interest, float camera_phi);

/* オブジェクト初期位置, 角度設定 */
void MakeInitPosition(void);

/* DMA Packet */
void motion2(int idd);
void Setpacket(int id);			/* Make path1 packet data */
void setinitialparameta(void);
void setinitialparameta2(void);

static sceGsDBuff db;
static sceVu0FVECTOR zerov;			/* all zero vector */
static u_int *dpmem[2], *dstaddr;	/* pointer for work area */
static sceDmaChan *d1, *d8;

/* Get ScrachPad Address for making DMApacket data */
#define GetDp(id)	((id&1)?(0x70002000):(0x70000000))

#define V4_32	0x6c			/* the mode of unpack insn. in VIFcode */

/* カメラの初期設定 */
sceVu0FVECTOR camera_p = { 50, 100, -200, 1.0f };

/* カメラZ方向 */
sceVu0FVECTOR camera_zd = { 0, 0, 1.0f, 1.0f };

/* カメラ上方向 */
sceVu0FVECTOR camera_yd = { 0, 1.0f, 0, 1.0f };

/* カメラ回転 */
sceVu0FVECTOR camera_rot = { 0, 0, 0, 0 };

/* 注視点位置 */
sceVu0FVECTOR interest = { 50, 60, 25, 1.0f };

/* 光源位置初期設定 */
sceVu0FVECTOR light0 = { -1.0, 0.0, 0.0, 1 };
sceVu0FVECTOR light1 = { 1.0, 1.0, 0.0, 1 };
sceVu0FVECTOR light2 = { 0.0, 0.0, 1.0, 1 };

/* 光源色 (0(min) to 1(max))*/
sceVu0FVECTOR color0 = { 1.0f, 1.0f, 1.0f, 1.0f };
sceVu0FVECTOR color1 = { 1.0f, 1.0f, 1.0f, 1.0f };
sceVu0FVECTOR color2 = { 1.0f, 1.0f, 1.0f, 1.0f };

/* 環境光 */
sceVu0FVECTOR ambient = { 0.0f, 0.0f, 0.0f, 1.0f };

sceVu0FVECTOR velocity[300][100];

/* unit radian */
float delta = PI / 180.0f;

/* Volume model に必要な変数 */
float dt;

sceVu0FVECTOR color[NBALLS];	/* color of each object */

sceVu0FVECTOR flowout = { 50, 0, 40 };	/* color of each object */

int lifetime1[100][100];

/* controller read */
u_int paddata;
u_char rdata[32];
u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__ ((aligned(64)));

float timestep = 0.1f;
float camera_radius = 300;

#ifdef IDLE_THREAD
#define STACK_SIZE 409600
char idle_stack[STACK_SIZE] __attribute__ ((aligned(16)));
#endif

#ifdef IDLE_THREAD
void idle(void *p)
{
	int i = 1;
	(void)p;

	while (1) {
		i = i + i;
	}
	return;
}

/* CreateStartThread
 * スレッドを作成し,起動します。
 * 
 * void (*entry)(void*) 実行を開始する関数のアドレス
 * void *stack スレッドが使用するスタック領域
 * int stacksize スタックサイズ
 * int prio 優先度
 * void *arg 引数
 *
 * 戻り値 スレッドID
 */
int CreateStartThread(void (*entry) (void *), void *stack, int stacksize, int prio, void *arg)
{
	struct ThreadParam tp;
	int id;

	tp.entry = entry;
	tp.stack = stack;
	tp.stackSize = stacksize;
	tp.initPriority = prio;
	tp.gpReg = &_gp;
	id = CreateThread(&tp);
	StartThread(id, arg);
	return id;
}
#endif


#ifdef VER5
int VU0_end_sema;

/* Vu0Handler
 * VER5でしか使用しないVU0の割り込みハンドラ
 * ハンドラの内部では,2つのことを行ないます。
 * VU0をREADY状態にするために,STOP状態から一旦RUN状態にする。
 * VU0microの終了待ちをしているセマフォを解放する。
 */
int Vu0Handler(int ca)
{
	__asm__ volatile (
		"cfc2 $8, $vi26\n"
		"ctc2 $8, $vi27\n"
		"vnop\n"
		"vnop\n"
		"vcallmsr $vi27\n"
		: : : "$8"
	);

	iSignalSema(VU0_end_sema);
	ExitHandler();
	return -1;
}
long128 Mscal __attribute__ ((aligned(128)));
long128 pBase[10] __attribute__ ((aligned(128)));


extern u_int My_dma_start __attribute__ ((section(".vudata")));

#endif
void DevInit(void)
{
	sceDmaEnv denv;

	sceDmaReset(1);
	sceDmaGetEnv(&denv);
	denv.notify = 0x0100;		/* enable Ch.8 CPCOND */
	sceDmaPutEnv(&denv);

	sceGsResetPath();
}

int main(void)
{
	int idd = 0;

	// float d=0.3f;
	// float s=0.1f;
	int PeakCycle = 0;

	float camera_theta = -0.5f * PI, camera_phi = 30.0f * delta;
	u_int frame;				/* frame number counter */
	int oddeven;				/* field id flag(odd or even) */

	sceSifInitRpc(0);
	while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx", 0, NULL) < 0) {
		printf("Can't load module sio2man\n");
	}
	while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx", 0, NULL) < 0) {
		printf("Can't load module padman\n");
	}
	/*
	 * initialize for all devices and parameters.
	 */
	DevInit();

	d1 = sceDmaGetChan(1);
	d8 = sceDmaGetChan(8);

	/* DMApacket area */
	dpmem[0] = (u_int *) malloc(sizeof(u_int) * 20 * 4 * NBALLS);
	dpmem[1] = (u_int *) malloc(sizeof(u_int) * 20 * 4 * NBALLS);

	/* all zero vector */
	zerov[0] = 0.0f;
	zerov[1] = 0.0f;
	zerov[2] = 0.0f;
	zerov[3] = 0.0f;

	/* GSリセット  */
	sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);

	/* GS ダブルバッファ構造体セット */
	sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, 640, 224, SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);

	/* 背景色 */
	db.clear0.rgbaq.R = 0x0;
	db.clear0.rgbaq.G = 0x0;
	db.clear0.rgbaq.B = 0x30;
	db.clear1.rgbaq.R = 0x0;
	db.clear1.rgbaq.G = 0x0;
	db.clear1.rgbaq.B = 0x30;

#ifdef VER5
	{
		struct SemaParam sema;
		sceVif0Packet Vif0Packet;
		int qwc = 100;
		int cp = 0x70000640;

		sema.initCount = 0;
		sema.maxCount = 1;
		VU0_end_sema = CreateSema(&sema);
#ifdef IDLE_THREAD
		CreateStartThread(idle, idle_stack, sizeof(idle_stack), 127, NULL);
#endif
		AddIntcHandler(INTC_VU0, Vu0Handler, 0);

		sceVif0PkInit(&Vif0Packet, &Mscal);
		sceVif0PkAddData(&Vif0Packet, SCE_VIF0_SET_MSCAL(0, 0));
		sceVif0PkTerminate(&Vif0Packet);

		*D0_QWC = 0;
		*D0_TADR = (u_int)&My_dma_start;
		DPUT_D0_CHCR(DGET_D0_CHCR() | 0x105);
		sceVif0PkInit(&Vif0Packet, pBase);
		sceVif0PkAlign(&Vif0Packet, 2, 3);
		sceVif0PkAddData(&Vif0Packet, SCE_VIF0_SET_UNPACK(0, qwc, 0xc, 0));
		sceVif0PkRef(&Vif0Packet, (long128 *) ((u_int) cp + 0x80000000), qwc, 0, 0, 0);
		sceVif0PkRefe(&Vif0Packet, &Mscal, 1, 0, 0, 0);
		sceVif0PkTerminate(&Vif0Packet);
		FlushCache(0);
	}

#endif
#ifdef VER7
	{
		int i;

		for (i = 0; i < 100; i++) {
			SINF[i] = sinf(3.6f * i * PI / 180.f);
			COSF[i] = cosf(3.6f * i * PI / 180.f);
		}
	}

#endif
	/* open controller */
	scePadInit(0);
	scePadPortOpen(0, 0, pad_dma_buf);

	/* Make initial values for each object */
	MakeInitPosition();

	setinitialparameta();

	/* send GS initialize parmeter */
	d1->chcr.TTE = 1;
	sceDmaSend(d1, &My_dma_start0);

	SetViewScreenMatrix();

	oddeven = !sceGsSyncV(0);
	frame = 0;


	/* メインループ */
	while (1) {
#ifdef VER9
		gResetCount = 0;
#endif
		idd++;
		/* read data of cotroller */
		if (scePadRead(0, 0, rdata) > 0) {
			paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
		} else {
			paddata = 0;
		}

		/* get the address of DMApacket area */
		dstaddr = dpmem[frame & 1];


		if (paddata & SCE_PADLleft) {
			camera_theta += 1.0f * PI / 180;
		} else if (paddata & SCE_PADLright) {
			camera_theta -= 1.0f * PI / 180;
		}

		if (paddata & SCE_PADLup) {
			camera_phi += 1.0f * PI / 180;
		} else if (paddata & SCE_PADLdown) {
			camera_phi -= 1.0f * PI / 180;
		}

		if (camera_theta > PI) {
			camera_theta = PI;
		} else if (camera_theta < -PI) {
			camera_theta = -PI;
		}

		if (camera_phi > 0.49f * PI) {
			camera_phi = 0.49f * PI;
		} else if (camera_phi < -0.49f * PI) {
			camera_phi = -0.49f * PI;
		}


		/*
		   // --- phi direction ---
		   if (paddata&SCE_PADLup) {
		   camera_p[1] += 2.0f;
		   } else if (paddata&SCE_PADLdown) {
		   camera_p[1] -= 2.0f;
		   }
		 */


		/* local-world matrix set */
		RotTrsMatrix(local_world, theta[0], trans[0]);

		/* generate world-view, view-screen */

		SetCameraPosition(camera_p, interest, camera_theta, camera_phi);


		CreateViewingMatrix(camera_p, interest, camera_phi);

		sceVu0MulMatrix(local_view, local_world, world_view);
		sceVu0MulMatrix(local_screen, local_view, view_screen);

		/* ground */
		Setpacket(100);

		scePcStart(PCCONTROL, 0, 0);
		motion2(idd);

		scePcStop();
		if (PeakCycle < scePcGetCounter0()) {
			PeakCycle = scePcGetCounter0();
		}
#ifdef DEBUG_PRINT
		printf("PeakCycle %08d PC0 = %08d PC1 = %08d\n", PeakCycle, scePcGetCounter0(), scePcGetCounter1());
#endif

		Setpacket(0);
		Setpacket(1);
		Setpacket(2);



		/* set DMAend tag at the last of DMApacket */
		*(u_int *) ((u_int)(dstaddr) | 0x20000000) = 0x70000000;
		dstaddr += 1;
		*(u_int *) ((u_int)(dstaddr) | 0x20000000) = 0x00000000;
		dstaddr += 1;
		*(u_int *) ((u_int)(dstaddr) | 0x20000000) = 0x00000000;
		dstaddr += 1;
		*(u_int *) ((u_int)(dstaddr) | 0x20000000) = 0x00000000;
		dstaddr += 1;


		/* Waiting the end of making DMApacket by SetMatrix() */
		WaitDMA();

		/* Waiting the end of drawing previous frame */
		sceGsSyncPath(0, 0);

		/* Waiting the head of V-Blank */
		oddeven = !sceGsSyncV(0);

		/* Add half pixel to offset address for interlace */
		sceGsSetHalfOffset((frame & 1) ? (&db.draw1) : (&db.draw0), 2048, 2048, oddeven);

		/* Writeback data from D-Cache to Memory */
		FlushCache(WRITEBACK_DCACHE);

		sceGsSwapDBuff(&db, frame);	/* Change Draw/Disp buffers */

		/* waitng double buffer parameter setting to GS */
		while (DGET_D2_CHCR() & 0x0100) {
		}

		/* Sending DMApacket by path1 */
		sceDmaSend(d1, dpmem[frame & 1]);
		frame++;
	}
	return 0;
}

void WaitDMA(void)
{
	__asm__ volatile (
		"sync.l\n"
	"WDMA:\n"
		"bc0f WDMA\n"
		"nop\n"
	);
}

void WaitVU1(void)
{
	__asm__ volatile (
	"WVU1:\n"
		"bc2t WVU1\n"
		"nop\n"
	);
}

void MakeInitPosition(void)
{
	dtrans[0][0] = (float)0.0f;
	dtrans[0][1] = (float)0.0f;
	dtrans[0][2] = (float)0.0f;

	color[0][0] = (float)50.0f;
	color[0][1] = (float)150.0f;
	color[0][2] = (float)150.0f;
	color[0][3] = 1.0f;


	color[1][0] = (float)50.0f;
	color[1][1] = (float)150.0f;
	color[1][2] = (float)150.0f;
	color[1][3] = 1.0f;

	color[2][0] = (float)50.0f;
	color[2][1] = (float)150.0f;
	color[2][2] = (float)150.0f;
	color[2][3] = 1.0f;


	color[3][0] = (float)250.0f;
	color[3][1] = (float)250.0f;
	color[3][2] = (float)250.0f;
	color[3][3] = 1.0f;

	color[100][0] = (float)100.0f;
	color[100][1] = (float)100.0f;
	color[100][2] = (float)100.0f;
	color[100][3] = 1.0f;
}


void SetViewScreenMatrix(void)
{
	/* (caution) include the right to left hand co-ordinate system */
	view_screen[0][0] = 1.0f * 500.0f;
	view_screen[0][1] = 0.0f;
	//        view_screen[0][2] = SCREEN_WIDTH / 2.0f + SCREEN_OFFSET_X;
	view_screen[0][2] = 2048.0f;
	view_screen[0][3] = 0.0f;

	view_screen[1][0] = 0.0f;
	view_screen[1][1] = -235.0f;	// 224/640=0.35
	//        view_screen[1][1] = -0.47*500.0f; // 224/640=0.35
	//        view_screen[1][2] = SCREEN_HEIGHT / 2.0f + SCREEN_OFFSET_Y;
	view_screen[1][2] = 2048.0f;
	view_screen[1][3] = 0;

	view_screen[2][0] = 0.0f;
	view_screen[2][1] = 0.0f;
	view_screen[2][2] = -255.0038758f;
	view_screen[2][3] = 16777470.0f;

	//        view_screen[2][2] = (65536*1.0-1.0*16777215)/(65536-1.0);
	//        view_screen[2][3] = 1.0*65536*(16777215-1.0)/(65536-1.0);

	view_screen[3][0] = 0.0f;
	view_screen[3][1] = 0.0f;
	view_screen[3][2] = 1.0f;
	view_screen[3][3] = 0.0f;

}

void RotTrsMatrix(sceVu0FMATRIX vlm, sceVu0FVECTOR rot, sceVu0FVECTOR trs)
{

	float c0, c1, c2;
	float s0, s1, s2;

	c0 = cosf(rot[0]);
	c1 = cosf(rot[1]);
	c2 = cosf(rot[2]);
	s0 = sinf(rot[0]);
	s1 = sinf(rot[1]);
	s2 = sinf(rot[2]);

	vlm[0][0] = c1 * c2;
	vlm[0][1] = s0 * s1 * c2 - s2 * c0;
	vlm[0][2] = s1 * c0 * c2 + s0 * s2;
	vlm[0][3] = trs[0];
	vlm[1][0] = s2 * c1;
	vlm[1][1] = s0 * s1 * s2 + c0 * c2;
	vlm[1][2] = s1 * c0 * s2 - s0 * c2;
	vlm[1][3] = trs[1];
	vlm[2][0] = -s1;
	vlm[2][1] = s0 * c1;
	vlm[2][2] = c0 * c1;
	vlm[2][3] = trs[2];
	vlm[3][0] = 0.0f;
	vlm[3][1] = 0.0f;
	vlm[3][2] = 0.0f;
	vlm[3][3] = 1.0f;

}




void Setpacket(int id)
{
	u_int *dp;

	dp = (u_int *) GetDp(id);

	/* DMAcnt (qwc = 18) */
	*dp++ = 0x10000000 | 18;
	*dp++ = 0;
	*dp++ = 0;
	*dp++ = 0;

	*dp++ = 0;
	*dp++ = SCE_VIF1_SET_BASE(120, 0);
	*dp++ = SCE_VIF1_SET_OFFSET(240, 0);
	*dp++ = SCE_VIF1_SET_UNPACK(0, 17, V4_32, 0);

	/* local -> Screen matrix ; VU-MEM 0-3 */
	*dp++ = *(u_int*)&(local_screen[0][0]);
	*dp++ = *(u_int*)&(local_screen[1][0]);
	*dp++ = *(u_int*)&(local_screen[2][0]);
	*dp++ = *(u_int*)&(local_screen[3][0]);

	*dp++ = *(u_int*)&(local_screen[0][1]);
	*dp++ = *(u_int*)&(local_screen[1][1]);
	*dp++ = *(u_int*)&(local_screen[2][1]);
	*dp++ = *(u_int*)&(local_screen[3][1]);

	*dp++ = *(u_int*)&(local_screen[0][2]);
	*dp++ = *(u_int*)&(local_screen[1][2]);
	*dp++ = *(u_int*)&(local_screen[2][2]);
	*dp++ = *(u_int*)&(local_screen[3][2]);

	*dp++ = *(u_int*)&(local_screen[0][3]);
	*dp++ = *(u_int*)&(local_screen[1][3]);
	*dp++ = *(u_int*)&(local_screen[2][3]);
	*dp++ = *(u_int*)&(local_screen[3][3]);


	/* local -> world matrix ; VU-MEM 4-7 */
	*dp++ = *(u_int*)&(local_world[0][0]);
	*dp++ = *(u_int*)&(local_world[1][0]);
	*dp++ = *(u_int*)&(local_world[2][0]);
	*dp++ = *(u_int*)&(local_world[3][0]);

	*dp++ = *(u_int*)&(local_world[0][1]);
	*dp++ = *(u_int*)&(local_world[1][1]);
	*dp++ = *(u_int*)&(local_world[2][1]);
	*dp++ = *(u_int*)&(local_world[3][1]);

	*dp++ = *(u_int*)&(local_world[0][2]);
	*dp++ = *(u_int*)&(local_world[1][2]);
	*dp++ = *(u_int*)&(local_world[2][2]);
	*dp++ = *(u_int*)&(local_world[3][2]);

	*dp++ = 0;
	*dp++ = 0;
	*dp++ = 0;
	*dp++ = 0x3f800000;

	/* light matrix (world) ; VU-MEM 8-11 */

	*dp++ = *(u_int*)&light0[0];
	*dp++ = *(u_int*)&light1[0];
	*dp++ = *(u_int*)&light2[0];
	*dp++ = 0;

	*dp++ = *(u_int*)&light0[1];
	*dp++ = *(u_int*)&light1[1];
	*dp++ = *(u_int*)&light2[1];
	*dp++ = 0;

	*dp++ = *(u_int*)&light0[2];
	*dp++ = *(u_int*)&light1[2];
	*dp++ = *(u_int*)&light2[2];
	*dp++ = 0;

	*dp++ = 0;
	*dp++ = 0;
	*dp++ = 0;
	*dp++ = 0x3f800000;


	/* light color matrix ; VU-MEM 12-15 */
	*dp++ = *(u_int*)&color0[0];
	*dp++ = *(u_int*)&color0[1];
	*dp++ = *(u_int*)&color0[2];
	*dp++ = *(u_int*)&color0[3];

	*dp++ = *(u_int*)&color1[0];
	*dp++ = *(u_int*)&color1[1];
	*dp++ = *(u_int*)&color1[2];
	*dp++ = *(u_int*)&color1[3];

	*dp++ = *(u_int*)&color2[0];
	*dp++ = *(u_int*)&color2[1];
	*dp++ = *(u_int*)&color2[2];
	*dp++ = *(u_int*)&color2[3];

	*dp++ = *(u_int*)&ambient[0];
	*dp++ = *(u_int*)&ambient[1];
	*dp++ = *(u_int*)&ambient[2];
	*dp++ = 0;

	/* vertex color 16 */
	*dp++ = *(u_int*)&color[id][0];
	*dp++ = *(u_int*)&color[id][1];
	*dp++ = *(u_int*)&color[id][2];
	*dp++ = *(u_int*)&color[id][3];


	/* DMAref (qwc = 2912) */
	if (id == 100) {
		*dp++ = 0x30000000 | 14;	/* size of object data (qword) */
	} else {
		*dp++ = 0x30000000 | 20175;	/* size of object data (qword) */
	}

	if (id == 0) {
		*dp++ = (u_int)&My_surface_polygon_left;	/* object data */
	} else if (id == 1) {
		*dp++ = (u_int)&My_surface_polygon_right;	/* object data */
	} else if (id == 2) {
		*dp++ = (u_int)&My_surface_polygon_3;	/* object data */
		id = 0;
	} else if (id == 3) {
		*dp++ = (u_int)&My_surface_polygon_4;	/* object data */
		id = 1;
	} else if (id == 100) {
		*dp++ = (u_int)&My_surface_polygon_ground;
		id = 0;

	}

	*dp++ = 0;
	*dp++ = 0;

	if (id) {
		WaitDMA();				/* Waiting the end of previous transer */
	}
	DPUT_D_STAT(0x100);			/* Clear the D_STAT.CIS8 for "bc0f" insn. */
	d8->sadr = (u_long128 *) (GetDp(id) & 0x0fffffff);
	sceDmaSendN(d8, dstaddr, ((u_int) dp - GetDp(id)) >> 4);

	/* change the address of DMApacket area */
	dstaddr += (((u_int) dp - GetDp(id)) >> 2);
}


void CreateViewingMatrix(sceVu0FVECTOR view, sceVu0FVECTOR inInterest, float camera_phi)
{
	sceVu0FMATRIX viewMat, transMat;
	sceVu0FVECTOR viewVector, upVector, rightVector;

	(void)camera_phi;
	
	/* View 座標系決定 */
	sceVu0SubVector(viewVector, inInterest, view);

	sceVu0Normalize(viewVector, viewVector);

	upVector[0] = 0.0f;
	upVector[1] = 1.0f;
	upVector[2] = 0.0f;
	upVector[3] = 1.0f;


	sceVu0OuterProduct(rightVector, upVector, viewVector);

	sceVu0Normalize(rightVector, rightVector);

	sceVu0OuterProduct(upVector, viewVector, rightVector);

	sceVu0Normalize(upVector, upVector);

	memset(world_view, 0, sizeof(sceVu0FMATRIX));
	memset(viewMat, 0, sizeof(sceVu0FMATRIX));

	/* x 軸 */
	world_view[0][0] = rightVector[0];
	world_view[0][1] = rightVector[1];
	world_view[0][2] = rightVector[2];

	/* y 軸 */
	world_view[1][0] = upVector[0];
	world_view[1][1] = upVector[1];
	world_view[1][2] = upVector[2];

	/* z 軸 */
	world_view[2][0] = viewVector[0];
	world_view[2][1] = viewVector[1];
	world_view[2][2] = viewVector[2];

	world_view[3][3] = 1.0f;

	memset(transMat, 0, sizeof(sceVu0FMATRIX));

	transMat[0][0] = 1.0f;
	transMat[1][1] = 1.0f;
	transMat[2][2] = 1.0f;
	transMat[3][3] = 1.0f;

	transMat[0][3] = -camera_p[0];
	transMat[1][3] = -camera_p[1];
	transMat[2][3] = -camera_p[2];

	sceVu0MulMatrix(world_view, transMat, world_view);
}

void setinitialparameta(void)
{
	int i, j;
	float *surface_polygon_left[SY], *surface_polygon_right[SY];
	float *surface_polygon_left_normal[SY], *surface_polygon_right_normal[SY];
	float f;

	for (i = 0; i < SY; i++) {
		surface_polygon_left[i] = (float *)(u_int)&My_surface_polygon_left
									+ 5 + 7 * i + SX * 8 * i;
		surface_polygon_right[i] = (float *)(u_int)&My_surface_polygon_right
									+ 5 + 7 * i + SX * 8 * i;
		surface_polygon_left_normal[i] = (float *)(u_int)&My_surface_polygon_left
									+ 5 + 7 * i + SX * 8 * i + 100 * 4 + 1;
		surface_polygon_right_normal[i] = (float *)(u_int)&My_surface_polygon_right
									+ 5 + 7 * i + SX * 8 * i + 100 * 4 + 1;
	}


	for (i = 0; i < SY; i++) {
		for (f = 0, j = 0; j < 100; f += 3.6, j++) {
			surface_polygon_left[i][4 * j] = flowout[0];
			surface_polygon_left[i][4 * j + 1] = 0.0f;
			surface_polygon_left[i][4 * j + 2] = flowout[2];

			surface_polygon_right[i][4 * j] = flowout[0];
			surface_polygon_right[i][4 * j + 1] = 0.0f;
			surface_polygon_right[i][4 * j + 2] = flowout[2];

			surface_polygon_left_normal[i][4 * j] = 0;
			surface_polygon_left_normal[i][4 * j + 1] = 1;
			surface_polygon_left_normal[i][4 * j + 2] = 0;
			surface_polygon_left_normal[i][4 * j + 3] = 128.0f;

			surface_polygon_right_normal[i][4 * j] = 0;
			surface_polygon_right_normal[i][4 * j + 1] = 1;
			surface_polygon_right_normal[i][4 * j + 2] = 0;
			surface_polygon_right_normal[i][4 * j + 3] = 128.0f;

			lifetime1[i][j] = rand() % 128;
		}
	}

	for (i = 0; i < 300; i++) {
		for (f = 0, j = 0; j < 100; f += 3.6, j++) {
			velocity[i][j][0] = 0;
			velocity[i][j][1] = 0;
			velocity[i][j][2] = 0;

		}
	}

}


void setinitialparameta2(void)
{
	int i, j;
	float *surface_polygon_left[SY];
	float *surface_polygon_right[SY];
	float *surface_polygon_3[SY];
	float *surface_polygon_4[SY];

	float *surface_polygon_left_normal[SY];
	float *surface_polygon_right_normal[SY];
	float *surface_polygon_3_normal[SY];
	float *surface_polygon_4_normal[SY];

	float f;

	/* ポインターセット */

	for (i = 0; i < SY; i++) {
		surface_polygon_left[i] = (float *)(u_int)&My_surface_polygon_left
									+ 5 + 7 * i + SX * 8 * i;
		surface_polygon_right[i] = (float *)(u_int)&My_surface_polygon_right
									+ 5 + 7 * i + SX * 8 * i;
		surface_polygon_3[i] = (float *)(u_int)&My_surface_polygon_3
									+ 5 + 7 * i + SX * 8 * i;
		surface_polygon_4[i] = (float *)(u_int)&My_surface_polygon_4
									+ 5 + 7 * i + SX * 8 * i;

		surface_polygon_left_normal[i] = (float *)(u_int)&My_surface_polygon_left
									+ 5 + 7 * i + SX * 8 * i + 100 * 4 + 1;
		surface_polygon_right_normal[i] = (float *)(u_int)&My_surface_polygon_right
									+ 5 + 7 * i + SX * 8 * i + 100 * 4 + 1;
		surface_polygon_3_normal[i] = (float *)(u_int)&My_surface_polygon_3
									+ 5 + 7 * i + SX * 8 * i + 100 * 4 + 1;
		surface_polygon_4_normal[i] = (float *)(u_int)&My_surface_polygon_4
									+ 5 + 7 * i + SX * 8 * i + 100 * 4 + 1;

	}


	for (i = 0; i < SY; i++) {
		for (f = 0, j = 0; j < 100; f += 3.6, j++) {
			surface_polygon_left[i][4 * j] = 50.0f + (rand() % 20) * sinf(3.6f * i * PI / 180);
			surface_polygon_left[i][4 * j + 1] = (rand() % 100) * 0.1f;
			surface_polygon_left[i][4 * j + 2] = 25.0f + (rand() % 20) * cosf(3.6f * i * PI / 180);

			surface_polygon_right[i][4 * j] = 50.0f + (rand() % 20) * sinf(3.6f * i * PI / 180);
			surface_polygon_right[i][4 * j + 1] = (rand() % 100) * 0.1f;
			surface_polygon_right[i][4 * j + 2] = 25.0f + (rand() % 20) * cosf(3.6f * i * PI / 180);

			surface_polygon_3[i][4 * j] = 50.0f + (rand() % 20) * sinf(3.6f * i * PI / 180);
			surface_polygon_3[i][4 * j + 1] = (rand() % 100) * 0.1f;
			surface_polygon_3[i][4 * j + 2] = 25.0f + (rand() % 20) * cosf(3.6f * i * PI / 180);

			surface_polygon_4[i][4 * j] = 50.0f + (rand() % 20) * sinf(3.6f * i * PI / 180);
			surface_polygon_4[i][4 * j + 1] = (rand() % 100) * 0.1f;
			surface_polygon_4[i][4 * j + 2] = 25.0f + (rand() % 20) * cosf(3.6f * i * PI / 180);

			lifetime1[i][j] = 127;

		}
	}


	for (i = 0; i < 300; i++) {
		for (j = 0; j < 100; j++) {
			velocity[i][j][0] = 0;
			velocity[i][j][1] = 0;
			velocity[i][j][2] = 0;

		}
	}

}


void motion2(int idd)
{
	int i, j;

	float *surface_polygon[300];
	float *surface_polygon_normal[300];

#ifdef VER2
#ifdef VER3
	sceVu0FVECTOR *op = (sceVu0FVECTOR *) 0x70000000;
	sceVu0FVECTOR *cp = (sceVu0FVECTOR *) 0x70000640;
#else
	sceVu0FVECTOR op[100], cp[100];
#endif
#else
	sceVu0FVECTOR op[300][100], cp[300][100];
#endif
#ifdef VER5
	int qwc = 100;
#endif
#ifndef VER6
#ifndef VER5
	sceVu0FVECTOR diffvec;
	float distance1 = 3, dis = 0, d = 0;
#endif
#endif
	
	(void)idd;

	for (i = 0; i < 100; i++) {
		surface_polygon[i] = (float *)((u_int)&My_surface_polygon_right)
								+ 5 + 7 * i + SX * 8 * i;
		surface_polygon_normal[i] = (float *)((u_int)&My_surface_polygon_right)
								+ 5 + 7 * i + SX * 8 * i + 100 * 4 + 1;

		surface_polygon[i + 100] = (float *)((u_int)&My_surface_polygon_left)
									+ 5 + 7 * i + SX * 8 * i;
		surface_polygon_normal[i + 100] = (float *)((u_int)&My_surface_polygon_left)
									+ 5 + 7 * i + SX * 8 * i + 100 * 4 + 1;

		surface_polygon[i + 200] = (float *)((u_int)&My_surface_polygon_3)
									+ 5 + 7 * i + SX * 8 * i;
		surface_polygon_normal[i + 200] = (float *)((u_int)&My_surface_polygon_3)
									+ 5 + 7 * i + SX * 8 * i + 100 * 4 + 1;
	}

#ifdef VER5

	EnableIntc(INTC_VU0);
	__asm__ volatile (
		"addi $8, $0 ,8\n"
		"ctc2 $8, $vi28\n"
		: : : "$8"
	);
#endif


#ifdef VER1

#ifdef VER2

	for (i = 0; i < 300; i++) {

#ifdef VER8
		memcpy(op, surface_polygon[i], sizeof(float) * 400);
#endif
		for (j = 0; j < 100; j++) {
#ifdef VER4
#ifndef VER8
			memcpy(op[j], &surface_polygon[i][4 * j], sizeof(float) * 3);
#endif

#ifdef VER8
			__asm__ volatile (
				"lqc2 vf1, 0(%1)\n"
				"lqc2 vf2, 0(%2)\n"
				"vadd.xyz vf3, vf1, vf2\n"
				"sqc2 vf3, 0(%0)\n"
				: : "r"(cp[j]), "r"(op[j]), "r"(velocity[i][j]) : "memory"
			);
#else
			sceVu0AddVector(cp[j], op[j], velocity[i][j]);
#endif

#else
			op[j][0] = surface_polygon[i][4 * j];
			op[j][1] = surface_polygon[i][4 * j + 1];
			op[j][2] = surface_polygon[i][4 * j + 2];
			cp[j][0] = op[j][0] + velocity[i][j][0];
			cp[j][1] = op[j][1] + velocity[i][j][1];
			cp[j][2] = op[j][2] + velocity[i][j][2];
#endif
		}
		cp[0][0] += (cp[0][2] - 50.f) * (rand() % 30) * 0.001f;
		cp[0][1] += (float)((rand() % 32) * 0.005f);
		cp[0][2] += (25 - cp[0][0]) * (rand() % 30) * 0.001f;

		cp[50][0] += cp[50][1] * (cp[50][2] - 50) * 0.001f;
		cp[50][1] += (float)((rand() % 32) * 0.01f);
		cp[50][2] += cp[50][1] * (25 - cp[50][0]) * 0.001f;

#ifdef VER5
		/*cp[100]をch0へDMA転送し,
		   vu0micro命令を実行する。
		   終了はvu0_end_semaで確認
		 */

		*D0_TADR = (u_int)pBase + 16;
		*D0_MADR = (u_int)pBase;
		*D0_QWC = 1;

		DPUT_D0_CHCR(0x10000105);

		WaitSema(VU0_end_sema);

		/*vu0mem上のデータをSPR上にDMA転送 */
		*D9_MADR = (u_int)0x11004000;
		*D9_SADR = (u_int)cp;
		*D9_QWC = qwc;

		DPUT_D9_CHCR(DGET_D9_CHCR() | D_CHCR_STR_M);

		WaitDMA();


#else
#ifdef VER6
		//0.1  0x3dcccccd
		//0.3  0x3e99999a
		__asm__ volatile (
			"li			$10, 0x3dcccccd\n"
			"li			$11, 0x3e99999a\n"
			"mtc1		$10, $f1\n"
			"mtc1		$11, $f2\n"
			"li			$10, 0x70000640\n"
			"li			$11, 0x70000c70\n"
			"lqc2		vf4, 0($10)\n"
		"1:\n"
			"lqc2		vf5, 0x10($10)\n"
			"vsub.xyz	vf6, vf5, vf4\n"
			"vmul.xyz	vf7, vf6, vf6\n"
			"vaddy.x	vf7, vf7, vf7\n"
			"vaddz.x	vf7, vf7, vf7\n"
			"qmfc2		$2, vf7\n"
			"mtc1		$2, $f3\n"
			"rsqrt.s	$f4, $f2, $f3\n"
			"sub.s		$f5, $f4, $f1\n"
			"mfc1		$8, $f5\n"
			"qmtc2		$8, $vf9\n"
			"vmulx.xyz	$vf7, $vf6, $vf9\n"
			"vsub.xyz	vf6, vf4, vf7\n"
			"sqc2		vf6, 0($10)\n"
			"vadd.xyz	vf4, vf5, vf7\n"
			"sqc2		vf4, 0x10($10)\n"
			"addiu		$10, $10, 16\n"
			"bne		$10, $11, 1b\n"
			"nop\n"
			
			: : : "$2", "$8", "$10", "$11", "memory"
		);

#else
		for (j = 0; j < 99; j++) {
			sceVu0SubVector(diffvec, cp[j + 1], cp[j]);
			dis = sqrtf(sceVu0InnerProduct(diffvec, diffvec));
			d = (distance1 - dis) * 0.1f;
			sceVu0ScaleVectorXYZ(diffvec, diffvec, d / dis);
			sceVu0SubVector(cp[j], cp[j], diffvec);
			sceVu0AddVector(cp[j + 1], cp[j + 1], diffvec);
		}
#endif
#endif



		for (j = 0; j < 100; j++) {
#ifdef VER4
#ifdef VER8
			__asm__ volatile (
				"lqc2 vf1, 0x0(%1)\n"
				"lqc2 vf2, 0x0(%2)\n"
				"vsub.xyz vf3, vf1, vf2\n"
				"sqc2 vf3, 0x0(%0)\n"
				: : "r"(velocity[i][j]), "r"(cp[j]), "r"(op[j]) : "memory"
			);
#else
			sceVu0SubVector(velocity[i][j], cp[j], op[j]);
#endif
#else
			velocity[i][j][0] = cp[j][0] - op[j][0];
			velocity[i][j][1] = cp[j][1] - op[j][1];
			velocity[i][j][2] = cp[j][2] - op[j][2];
#endif
		}
		if (cp[0][1] > 150) {
#ifdef VER9
			if (gResetCount < 20) {
				gResetCount++;
#endif
				for (j = 0; j < 100; j++) {
#ifdef VER7
					cp[j][0] = 50.0f + (rand() % 10) * SINF[i];
#else
					cp[j][0] = 50.0f + (rand() % 10) * sinf(3.6f * i * PI / 180.f);
#endif
					cp[j][1] = (rand() % 100) * 0.2f - 10.f;
#ifdef VER7
					cp[j][2] = 25.0f + (rand() % 10) * COSF[i];
#else
					cp[j][2] = 25.0f + (rand() % 10) * cosf(3.6f * i * PI / 180.f);
#endif

					velocity[i][j][0] = 0.0f;
					velocity[i][j][1] = 0.0f;
					velocity[i][j][2] = 0.0f;
#ifdef VER9
				}
#endif
			}
		}

		for (j = 0; j < 100; j++) {
#ifdef VER4
			memcpy(&surface_polygon[i][4 * j], cp[j], sizeof(float) * 3);
#else
			surface_polygon[i][4 * j] = cp[j][0];
			surface_polygon[i][4 * j + 1] = cp[j][1];
			surface_polygon[i][4 * j + 2] = cp[j][2];
#endif
			surface_polygon_normal[i][4 * j + 3] = 150.f - cp[j][1];
		}
	}

#ifdef VER5
	DisableIntc(INTC_VU0);
#endif


#else

	for (i = 0; i < 300; i++) {
		for (j = 0; j < 100; j++) {
			op[i][j][0] = surface_polygon[i][4 * j];
			op[i][j][1] = surface_polygon[i][4 * j + 1];
			op[i][j][2] = surface_polygon[i][4 * j + 2];
		}
		/* position */

		for (j = 0; j < 100; j++) {
			cp[i][j][0] = op[i][j][0] + velocity[i][j][0];
			cp[i][j][1] = op[i][j][1] + velocity[i][j][1];
			cp[i][j][2] = op[i][j][2] + velocity[i][j][2];
		}

		cp[i][0][0] += (cp[i][0][2] - 50) * (rand() % 30) * 0.001f;
		cp[i][0][1] += (float)((rand() % 32) * 0.005f);
		cp[i][0][2] += (25 - cp[i][0][0]) * (rand() % 30) * 0.001f;

		cp[i][50][0] += cp[i][50][1] * (cp[i][50][2] - 50) * 0.001f;
		cp[i][50][1] += (float)((rand() % 32) * 0.01f);
		cp[i][50][2] += cp[i][50][1] * (25 - cp[i][50][0]) * 0.001f;


		/* つながり */


		for (j = 0; j < 99; j++) {
			sceVu0SubVector(diffvec, cp[i][j + 1], cp[i][j]);
			dis = sqrtf(sceVu0InnerProduct(diffvec, diffvec));
			d = (distance1 - dis) * 0.1f;
			sceVu0ScaleVectorXYZ(diffvec, diffvec, d / dis);
			sceVu0SubVector(cp[i][j], cp[i][j], diffvec);
			sceVu0AddVector(cp[i][j + 1], cp[i][j + 1], diffvec);
		}



		for (j = 0; j < 100; j++) {
			velocity[i][j][0] = cp[i][j][0] - op[i][j][0];
			velocity[i][j][1] = cp[i][j][1] - op[i][j][1];
			velocity[i][j][2] = cp[i][j][2] - op[i][j][2];
		}



		if (cp[i][0][1] > 150) {
			for (j = 0; j < 100; j++) {
				cp[i][j][0] = 50.0f + (rand() % 10) * sinf(3.6f * i * PI / 180);
				cp[i][j][1] = (rand() % 100) * 0.2f - 10;
				cp[i][j][2] = 25.0f + (rand() % 10) * cosf(3.6f * i * PI / 180);
				velocity[i][j][0] = 0.0f;
				velocity[i][j][1] = 0.0f;
				velocity[i][j][2] = 0.0f;
			}
		}


		for (j = 0; j < 100; j++) {
			surface_polygon[i][4 * j] = cp[i][j][0];
			surface_polygon[i][4 * j + 1] = cp[i][j][1];
			surface_polygon[i][4 * j + 2] = cp[i][j][2];
			surface_polygon_normal[i][4 * j + 3] = 150 - cp[i][j][1];
		}
	}



#endif
#else
	for (i = 0; i < 300; i++) {
		for (j = 0; j < 100; j++) {
			op[i][j][0] = surface_polygon[i][4 * j];
			op[i][j][1] = surface_polygon[i][4 * j + 1];
			op[i][j][2] = surface_polygon[i][4 * j + 2];
		}
	}

	/* position */
	for (i = 0; i < 300; i++) {
		for (j = 0; j < 100; j++) {
			cp[i][j][0] = op[i][j][0] + velocity[i][j][0];
			cp[i][j][1] = op[i][j][1] + velocity[i][j][1];
			cp[i][j][2] = op[i][j][2] + velocity[i][j][2];
		}
	}

	for (i = 0; i < 300; i++) {
		cp[i][0][0] += (cp[i][0][2] - 50) * (rand() % 30) * 0.001f;
		cp[i][0][1] += (float)((rand() % 32) * 0.005f);
		cp[i][0][2] += (25 - cp[i][0][0]) * (rand() % 30) * 0.001f;

		cp[i][50][0] += cp[i][50][1] * (cp[i][50][2] - 50) * 0.001f;
		cp[i][50][1] += (float)((rand() % 32) * 0.01f);
		cp[i][50][2] += cp[i][50][1] * (25 - cp[i][50][0]) * 0.001f;
	}

	/* つながり */

	for (i = 0; i < 300; i++) {
		for (j = 0; j < 99; j++) {
			sceVu0SubVector(diffvec, cp[i][j + 1], cp[i][j]);
			dis = sqrtf(sceVu0InnerProduct(diffvec, diffvec));
			d = (distance1 - dis) * 0.1f;
			sceVu0ScaleVectorXYZ(diffvec, diffvec, d / dis);
			sceVu0SubVector(cp[i][j], cp[i][j], diffvec);
			sceVu0AddVector(cp[i][j + 1], cp[i][j + 1], diffvec);
		}
	}

	for (i = 0; i < 300; i++) {
		for (j = 0; j < 100; j++) {
			velocity[i][j][0] = cp[i][j][0] - op[i][j][0];
			velocity[i][j][1] = cp[i][j][1] - op[i][j][1];
			velocity[i][j][2] = cp[i][j][2] - op[i][j][2];
		}
	}

	for (i = 0; i < 300; i++) {
		if (cp[i][0][1] > 150) {
			for (j = 0; j < 100; j++) {
				cp[i][j][0] = 50.0f + (rand() % 10) * sinf(3.6f * i * PI / 180);
				cp[i][j][1] = (rand() % 100) * 0.2f - 10;
				cp[i][j][2] = 25.0f + (rand() % 10) * cosf(3.6f * i * PI / 180);
				velocity[i][j][0] = 0.0f;
				velocity[i][j][1] = 0.0f;
				velocity[i][j][2] = 0.0f;
			}
		}
	}
	for (i = 0; i < 300; i++) {
		for (j = 0; j < 100; j++) {
			surface_polygon[i][4 * j] = cp[i][j][0];
			surface_polygon[i][4 * j + 1] = cp[i][j][1];
			surface_polygon[i][4 * j + 2] = cp[i][j][2];
			surface_polygon_normal[i][4 * j + 3] = 150 - cp[i][j][1];
		}
	}
#endif
}

void SetCameraPosition(sceVu0FVECTOR outCamera_p, sceVu0FVECTOR inInterest, float inTheta, float inPhi)
{

	outCamera_p[0] = camera_radius * cosf(inPhi) * cosf(inTheta) + inInterest[0];
	outCamera_p[1] = camera_radius * sinf(inPhi) + inInterest[1];
	outCamera_p[2] = camera_radius * cosf(inPhi) * sinf(inTheta) + inInterest[2];
	outCamera_p[3] = 1.0f;

}

