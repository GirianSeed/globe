/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Emotion Engine Library Sample Program
 *
 *                           - <fluid> -
 *
 *                          Version <1.00>
 *                               SJIS
 *
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                            <original.c>
 *                    <the way of optimize EE CORE>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Aug,22,2000     wada        First design
 *       0.02           Apr,29,2003     hana        change coding style
 */

/* 海の波の表現です。
水面の高さを100×100のfloat型の行列として、
水面を計算し,ポリゴン形式へ変換します。

kass の "Height-field fluds for computer graphics"  論文をもとに作成しています。
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
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

const int PCCONTROL = (SCE_PC0_CPU_CYCLE | SCE_PC_U0 | SCE_PC_K0 | SCE_PC_EXL0 |
					   SCE_PC1_DCACHE_MISS | SCE_PC_U1 | SCE_PC_K1 | SCE_PC_EXL1 | SCE_PC_CTE);

#define DEBUG_PRINT

extern u_int My_dma_start0 __attribute__ ((section(".vudata")));
extern u_int My_surface __attribute__ ((section(".vudata")));
extern u_int My_surface_end __attribute__ ((section(".vudata")));

extern u_int My_surface_polygon_left __attribute__ ((section(".vudata")));
extern u_int My_surface_polygon_right __attribute__ ((section(".vudata")));

extern u_int My_bottom_polygon_left __attribute__ ((section(".vudata")));
extern u_int My_bottom_polygon_right __attribute__ ((section(".vudata")));


#define NBALLS	4


// ---- params for display ----
#define SCREEN_WIDTH 640.0f
#define SCREEN_HEIGHT 224.0f
#define SCREEN_OFFSET_X 1728.0f
#define SCREEN_OFFSET_Y 1936.0f

#define FOV 42.0f
#define ASPECT 1.42f			// 640/480=1.42857
#define PI 3.14f


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
extern void DevInit(void);		/* Device Initialize routine */

/* DMA Wait */
void WaitDMA(void);				/* Waiting the end of Ch.8 DMA transfer */
void WaitVU1(void);				/* Waiting the end of VU1 calculation */

/* Matrix 演算 */
void TransporsedMatrix(float Dest[100][100], float Src[100][100]);
void SetViewScreenMatrix(void);
void RotTrsMatrix(sceVu0FMATRIX slm, sceVu0FVECTOR rot, sceVu0FVECTOR trs);
void MulMatrix(sceVu0FMATRIX svm, sceVu0FMATRIX vlm, sceVu0FMATRIX slm);

void wadaMulMatrix(sceVu0FMATRIX m, sceVu0FMATRIX a, sceVu0FMATRIX b);
void wadaOuterProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);
void wadaMulMatrixVector(sceVu0FVECTOR v0, sceVu0FMATRIX m, sceVu0FVECTOR v);
void wadaShowVector(sceVu0FVECTOR v0);
void wadaShowMatrix(sceVu0FMATRIX m0);
void wadaSubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);
void wadaNormalizeVector(sceVu0FVECTOR v);

/* 二つのオブジェクトの距離 */
float dist(sceVu0FVECTOR * src, sceVu0FVECTOR * dst);

/* カメラの位置設定 */

/* カメラマトリクス生成 */
void CreateViewingMatrix(sceVu0FVECTOR view, sceVu0FVECTOR interest);

/* オブジェクト初期位置, 角度設定 */
void MakeInitPosition(void);

/* 水の表現関数 */
void volumemodel(void);


void KassWave(float timestep);
void tridag(float a[], float b[], float c[], float r[], float u[], int trinum);

/* DMA Packet for water surface and spray */
void SetSurfaceMatrix(int id);	/* Make path1 packet data */

static sceGsDBuff db;
static sceVu0FVECTOR zerov;			/* all zero vector */
static u_int *dpmem[2], *dstaddr;	/* pointer for work area */
static sceDmaChan *d1, *d8;

/* Get ScrachPad Address for making DMApacket data */
#define GetDp(id)	((id&1)?(0x70002000):(0x70000000))

#define V4_32	0x6c			/* the mode of unpack insn. in VIFcode */

/* カメラの初期設定 */
sceVu0FVECTOR camera_p = { 25, 100, -25, 1.0f };

/* カメラZ方向 */
sceVu0FVECTOR camera_zd = { 0, 0, 1.0f, 1.0f };

/* カメラ上方向 */
sceVu0FVECTOR camera_yd = { 0, 1.0f, 0, 1.0f };

/* カメラ回転 */
sceVu0FVECTOR camera_rot = { 0, 0, 0, 0 };

/* 注視点位置 */
sceVu0FVECTOR interest = { 25, 50, 25, 0 };

/* 光源初期設定 */
sceVu0FVECTOR light0 = { 0.003, 0.009, 0.002, 1 };
sceVu0FVECTOR light1 = { 0.0, 0.0, 0.0, 1 };
sceVu0FVECTOR light2 = { 0.0, 0.0, -1.0, 1 };

/* 光源色 */
sceVu0FVECTOR color0 = { 120.0f, 120.0f, 120.0f, 1.0f };
sceVu0FVECTOR color1 = { 0.0f, 5.0f, 5.0f, 1.0f };
sceVu0FVECTOR color2 = { 0.0f, 0.0f, 0.0f, 1.0f };

/* 環境光 */
sceVu0FVECTOR ambient = { 5.0f, 5.0f, 5.0f, 1.0f };

/* 鏡面反射のため */
sceVu0FVECTOR interest_camera_vector = { 0.0f, 0.0f, 0.0f, 1.0f };

/* unit radian */
float delta = PI / 180.0f;

/* Volume model に必要な変数 */
float height[SX][SY] __attribute__ ((aligned(16)));

float dt;
float deltavol[SX][SY];

sceVu0FVECTOR color[NBALLS];	/* color of each object */

float height_n[100][100] __attribute__ ((aligned(128)));
float height_n_1[100][100] __attribute__ ((aligned(128)));
float height_n_2[100][100] __attribute__ ((aligned(128)));
float height_n2[100][100] __attribute__ ((aligned(128)));
float height_n_12[100][100] __attribute__ ((aligned(128)));
float height_n_22[100][100] __attribute__ ((aligned(128)));
float depth[100][100] __attribute__ ((aligned(128)));
float bottom[100][100] __attribute__ ((aligned(128)));
float e[100], f[100];
float e2[100], f2[100];

float *vector(long n1, long nh);
void free_vector(float *v, long nl, long nh);


u_int paddata;
u_char rdata[32];
u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__ ((aligned(64)));

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
	int i, j;
	float deltat = 0;

	u_int frame;				/* frame number counter */
	int oddeven;				/* field id flag(odd or even) */

	float timestep;

	/*
	   while(sceSifRebootIop("host0:/usr/local/sce/iop/modules/"IOP_IMAGE_file)
	   == 0){
	   printf("iop reset error\n");
	   }
	   while(sceSifSyncIop() == 0);
	 */



	/* controller read */
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
	dpmem[0] = (u_int*)memalign(128, sizeof(u_int) * 20 * 4 * NBALLS + 16);
	dpmem[1] = (u_int*)memalign(128, sizeof(u_int) * 20 * 4 * NBALLS + 16);

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


	/* open controller */
	scePadInit(0);
	scePadPortOpen(0, 0, pad_dma_buf);

	/* Make initial values for each object */
	MakeInitPosition();

	/* Initialize the volumemodel data */
	for (i = 0; i < SX; i++) {
		for (j = 0; j < SY; j++) {
		}
	}


	/* send GS initialize parmeter */
	d1->chcr.TTE = 1;

	sceDmaSend(d1, &My_dma_start0);

	SetViewScreenMatrix();

	oddeven = !sceGsSyncV(0);
	frame = 0;
	timestep = 0;

	/* inith */
	for (i = 0; i < 100; i++) {
		for (j = 0; j < 100; j++) {
			height_n[i][j] = height_n_1[i][j] = height_n_2[i][j] = 30.0f;
			bottom[i][j] = 20.0;
		}
	}

	height_n[31][31] = height_n_1[31][31] = height_n_2[31][31] = 150.0f;
	height_n[32][32] = height_n_1[32][32] = height_n_2[32][32] = 150.0f;

	/* メインループ */
	while (1) {
		deltat += 1.5f;

		/* read data of cotroller */
		if (scePadRead(0, 0, rdata) > 0) {
			paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
		} else {
			paddata = 0;
		}

		/* get the address of DMApacket area */
		dstaddr = dpmem[frame & 1];


		if (paddata & SCE_PADR1) {
			for (i = 40; i < 43; i++) {
				for (j = 40; j < 43; j++) {
					height_n_1[i][j] += 1.0f;
				}
			}
		}

		if (paddata & SCE_PADRright) {
			for (i = 28; i < 35; i++) {
				for (j = 28; j < 35; j++) {
					height_n_1[i][j] += 5.0f;
					height_n_2[i][j] += 5.0f;
				}
			}
		}

		if (paddata & SCE_PADRleft) {
			for (i = 28; i < 35; i++) {
				for (j = 28; j < 35; j++) {
					height_n_1[i][j] = bottom[i][j];
					height_n_2[i][j] = bottom[i][j];
				}
			}
		}

		if (paddata & SCE_PADRup) {
			for (i = 78; i < 85; i++) {
				for (j = 78; j < 85; j++) {
					height_n_1[i][j] += 5.0f;
					height_n_2[i][j] += 5.0f;
				}
			}
		}

		if (paddata & SCE_PADRdown) {
			for (i = 78; i < 85; i++) {
				for (j = 78; j < 85; j++) {
					height_n_1[i][j] = bottom[i][j];
					height_n_2[i][j] = bottom[i][j];
				}
			}
		}


		if (paddata & SCE_PADLleft) {
			camera_p[0] -= 2.0f;
		} else if (paddata & SCE_PADLright) {
			camera_p[0] += 2.0f;
		}
		// --- phi direction ---
		if (paddata & SCE_PADLup) {
			camera_p[1] += 2.0f;
		} else if (paddata & SCE_PADLdown) {
			camera_p[1] -= 2.0f;
		}


		/* 流体生成 */
		scePcStart(PCCONTROL, 0, 0);


		KassWave(timestep);

#ifdef DEBUG_PRINT
		scePrintf("%03d PC0 = %08d PC1 = %d\n", __LINE__, scePcGetCounter0(), scePcGetCounter1());
#endif

		/* local-world matrix set */
		RotTrsMatrix(local_world, theta[0], trans[0]);

		/* generate world-view, view-screen */
		CreateViewingMatrix(camera_p, interest);


		sceVu0MulMatrix(local_view, local_world, world_view);
		sceVu0MulMatrix(local_screen, local_view, view_screen);


		SetSurfaceMatrix(2);

		SetSurfaceMatrix(3);

		SetSurfaceMatrix(0);
		SetSurfaceMatrix(1);



		/* set DMAend tag at the last of DMApacket */
		*(u_int*)((u_int)(dstaddr) | 0x20000000) = 0x70000000;
		dstaddr += 1;
		*(u_int*)((u_int)(dstaddr) | 0x20000000) = 0x00000000;
		dstaddr += 1;
		*(u_int*)((u_int)(dstaddr) | 0x20000000) = 0x00000000;
		dstaddr += 1;
		*(u_int*)((u_int)(dstaddr) | 0x20000000) = 0x00000000;
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
		" sync.l\n"
	"WDMA:\n"
		"bc0f WDMA\n"
		"nop\n"
	);
}

void WaitVU1(void)
{
	__asm__ volatile (
	" WVU1:\n"
		"bc2t WVU1\n"
		"nop\n"
	);
}

/* TransporsedMatrix 
 * float 100*100行列の転置を行なう。
 *
 */
void TransporsedMatrix(float Dest[100][100], float Src[100][100])
{
	int X, Y, i, j;
	float *To, *From;
	int pcr = *D_PCR;

	From = (float *)0x70001000;
	To = (float *)0x70003000;
	*D_SQWC = (5 << 16) + 20;
	pcr = *D_PCR;
	for (Y = 0; Y < 5; Y++) {
		for (X = 0; X < 5; X++) {
			*D_PCR &= 0xfffff000;
			*D_PCR |= 0x200;
			*D_STAT = 0x300;
			*D9_QWC = 100;
			*D9_SADR = (u_int) From;
			*D9_MADR = (u_int) Src + X * 80 + Y * 8000;
			*D9_CHCR = 0x109;
			WaitDMA();
			for (i = 0; i < 20; i++) {
				for (j = 0; j < 20; j++) {
					*(To + j * 20 + i) = *(From + i * 20 + j);
				}
			}
			*D_PCR &= 0xfffff000;
			*D_PCR |= 0x100;
			*D_STAT = 0x300;
			*D8_QWC = 100;
			*D8_SADR = (u_int) To;
			*D8_MADR = (u_int) Dest + X * 8000 + Y * 80;
			*D8_CHCR = 0x108;
			WaitDMA();
		}
	}
	*D_PCR = pcr;
}

void MakeInitPosition(void)
{
	int i;

	theta[0][0] = 0.0f;
	theta[0][1] = 0.0f;
	theta[0][2] = 0.0f;
	trans[0][0] = (float)0.0f;
	trans[0][1] = (float)0.0f;
	trans[0][2] = (float)0.0f;
	dtrans[0][0] = (float)0.0f;
	dtrans[0][1] = (float)0.0f;
	dtrans[0][2] = (float)0.0f;
	//      color[0][0] = (float)0.0f;
	color[0][0] = (float)0.0f;
	color[0][1] = (float)120.0f;
	color[0][2] = (float)120.0f;
	color[0][3] = 1.0f;
	theta[1][0] = 0.0f;
	theta[1][1] = 0.0f;
	theta[1][2] = 0.0f;
	trans[1][0] = (float)0.0f;
	trans[1][1] = (float)0.0f;
	trans[1][2] = (float)0.0f;
	dtrans[1][0] = (float)0.0f;
	dtrans[1][1] = (float)0.0f;
	dtrans[1][2] = (float)0.0f;
	//      color[1][0] = (float)0.0f;
	color[1][0] = (float)0.0f;
	color[1][1] = (float)120.0f;
	color[1][2] = (float)120.0f;
	color[1][3] = 1.0f;
	theta[2][0] = 0.0f;
	theta[2][1] = 0.0f;
	theta[2][2] = 0.0f;
	trans[2][0] = (float)0.0f;
	trans[2][1] = (float)0.0f;
	trans[2][2] = (float)0.0f;
	dtrans[2][0] = (float)0.0f;
	dtrans[2][1] = (float)0.0f;
	dtrans[2][2] = (float)0.0f;
	color[2][0] = (float)100.0f;
	color[2][1] = (float)100.0f;
	color[2][2] = (float)0.0f;
	color[2][3] = 1.0f;
	theta[3][0] = 0.0f;
	theta[3][1] = 0.0f;
	theta[3][2] = 0.0f;
	trans[3][0] = (float)0.0f;
	trans[3][1] = (float)0.0f;
	trans[3][2] = (float)0.0f;
	dtrans[3][0] = (float)0.0f;
	dtrans[3][1] = (float)0.0f;
	dtrans[3][2] = (float)0.0f;
	color[3][0] = (float)100.0f;
	color[3][1] = (float)100.0f;
	color[3][2] = (float)0.0f;
	color[3][3] = 1.0f;
	
	for (i = 4; i < NBALLS; i++) {
		theta[i][0] = 0.0f;
		theta[i][1] = 0.0f;
		theta[i][2] = 0.0f;
		trans[i][0] = (float)0.0f;
		trans[i][1] = (float)0.0f;
		trans[i][2] = (float)0.0f;
		dtrans[i][0] = (float)0.0f;
		dtrans[i][1] = (float)0.0f;
		dtrans[i][2] = (float)0.0f;
		color[i][0] = (float)200.0f;
		color[i][1] = (float)200.0f;
		color[i][2] = (float)200.0f;
		color[i][3] = 128.0f;
	}

}


void SetViewScreenMatrix(void)
{
	/* (caution) include the right to left hand co-ordinate system */
	view_screen[0][0] = 1.0f * 500.0f;
	view_screen[0][1] = 0.0f;
	view_screen[0][2] = 2048.0f;
	view_screen[0][3] = 0.0f;
	view_screen[1][0] = 0.0f;
	view_screen[1][1] = -235.0f;	// 224/640=0.35
	view_screen[1][2] = 2048.0f;
	view_screen[1][3] = 0;
	view_screen[2][0] = 0.0f;
	view_screen[2][1] = 0.0f;
	view_screen[2][2] = -255.0038758f;
	view_screen[2][3] = 16777470.0f;
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

/* sceVu0MulMatrix の転置バージョン  */
void MulMatrix(sceVu0FMATRIX svm, sceVu0FMATRIX vlm, sceVu0FMATRIX slm)
{
	sceVu0FMATRIX svm0, vlm0, slm0;

	sceVu0TransposeMatrix(svm0, svm);
	sceVu0TransposeMatrix(vlm0, vlm);
	sceVu0MulMatrix(slm0, svm0, vlm0);
	sceVu0TransposeMatrix(slm, slm0);
}

float dist(sceVu0FVECTOR * src, sceVu0FVECTOR * dst)
{
	float ret;
	__asm__ volatile (
		"lqc2 vf4, 0x0(%1)\n"
		"lqc2 vf5, 0x0(%2)\n"
		"vsub.xyz vf5, vf5, vf4\n"
		"vmul.xyz vf4, vf5, vf5\n"
		"vmr32.xy vf5, vf4\n"
		"vmr32.x vf6, vf5\n"
		"vadd.x vf7, vf4, vf5\n"
		"vadd.x vf5, vf6, vf7\n"
		"vsqrt Q, vf5x\n"
		"vwaitq\n"
		"cfc2 $2, $vi22\n"
		"mtc1 $2, %0\n"
		: "=f" (ret) : "r"(src), "r"(dst) : "$2"
	);
	/* return the distance of two vectors */
	return ret;
}



void SetSurfaceMatrix(int id)
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
	*dp++ = 0;
	*dp++ = *(u_int*)&color1[0];
	*dp++ = *(u_int*)&color1[1];
	*dp++ = *(u_int*)&color1[2];
	*dp++ = 0;
	*dp++ = *(u_int*)&color2[0];
	*dp++ = *(u_int*)&color2[1];
	*dp++ = *(u_int*)&color2[2];
	*dp++ = 0;
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
	*dp++ = 0x30000000 | 20301;	/* size of object data (qword) */
	if (id == 0) {
		*dp++ = (u_int)&My_surface_polygon_left;	/* object data */
	} else if (id == 1) {
		*dp++ = (u_int)&My_surface_polygon_right;	/* object data */
	} else if (id == 2) {
		*dp++ = (u_int)&My_bottom_polygon_left;	/* object data */
	} else if (id == 3) {
		*dp++ = (u_int)&My_bottom_polygon_right;	/* object data */
	}

	*dp++ = 0;
	*dp++ = 0;
	if (id == 1 || id == 3) {
		WaitDMA();				/* Waiting the end of previous transer */
	}
	DPUT_D_STAT(0x100);			/* Clear the D_STAT.CIS8 for "bc0f" insn. */
	d8->sadr = (u_long128 *) (GetDp(id) & 0x0fffffff);
	sceDmaSendN(d8, dstaddr, ((u_int) dp - GetDp(id)) >> 4);
	/* change the address of DMApacket area */
	dstaddr += (((u_int) dp - GetDp(id)) >> 2);
}


void CreateViewingMatrix(sceVu0FVECTOR view, sceVu0FVECTOR inInterest)
{
	sceVu0FMATRIX viewMat, transMat;
	sceVu0FVECTOR viewVector, upVector, rightVector;

	/* View 座標系決定 */
	sceVu0SubVector(viewVector, inInterest, view);
	sceVu0Normalize(viewVector, viewVector);
	upVector[0] = 0.0f;
	upVector[1] = 1.0f;
	upVector[2] = 0.0f;
	upVector[3] = 1.0f;
	interest_camera_vector[0] = -viewVector[0];
	interest_camera_vector[1] = -viewVector[1];
	interest_camera_vector[2] = -viewVector[2];
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

void wadaMulMatrix(sceVu0FMATRIX m, sceVu0FMATRIX a, sceVu0FMATRIX b)
{
	m[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0];
	m[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1];
	m[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2];
	m[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3];
	m[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0];
	m[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1];
	m[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2];
	m[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3];
	m[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0];
	m[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1];
	m[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2];
	m[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3];
	m[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0];
	m[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1];
	m[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2];
	m[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3];
}


void wadaOuterProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
	v0[0] = v1[1] * v2[2] - v1[2] * v2[1];
	v0[1] = v1[2] * v2[0] - v1[0] * v2[2];
	v0[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void wadaNormalizeVector(sceVu0FVECTOR v)
{
	float n;

	n = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= n;
	v[1] /= n;
	v[2] /= n;
	v[3] = 1.0f;
}


void wadaMulMatrixVector(sceVu0FVECTOR v0, sceVu0FMATRIX m, sceVu0FVECTOR v)
{
	v0[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3];
	v0[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3];
	v0[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3];
	v0[3] = m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3];
}

void wadaSubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
	v0[0] = v1[0] - v2[0];
	v0[1] = v1[1] - v2[1];
	v0[2] = v1[2] - v2[2];
	v0[3] = v1[3] - v2[3];
}


void wadaShowVector(sceVu0FVECTOR v0)
{
	printf("v[0] %f, v[1] %f, v[2] %f, v[3] %f\n", v0[0], v0[1], v0[2], v0[3]);
	printf("\n");
	return ;
}

void wadaShowMatrix(sceVu0FMATRIX m0)
{
	printf("m[0][0] %f, m[0][1] %f, m[0][2] %f, m[0][3] %f\n", m0[0][0], m0[0][1], m0[0][2], m0[0][3]);
	printf("m[1][0] %f, m[1][1] %f, m[1][2] %f, m[1][3] %f\n", m0[1][0], m0[1][1], m0[1][2], m0[1][3]);
	printf("m[2][0] %f, m[2][1] %f, m[2][2] %f, m[2][3] %f\n", m0[2][0], m0[2][1], m0[2][2], m0[2][3]);
	printf("m[3][0] %f, m[3][1] %f, m[3][2] %f, m[3][3] %f\n", m0[3][0], m0[3][1], m0[3][2], m0[3][3]);
	printf("\n");
	return ;
}


/* この関数が処理の99%を占めるので、
 * 最適化の対象となる。
 */
void KassWave(float timestep)
{
	int i, j, k;
	float *p[SY];
	float *surface_polygon_left[SY];
	float *surface_polygon_right[SY];
	float *surface_polygon_normal_left[SY];
	float *surface_polygon_normal_right[SY];
	sceVu0FVECTOR work1, work2, work3;
	sceVu0FVECTOR work4, work5, work6;
	float right_hand_side[100];

	//  float epsilon = 1.0f; 
	float grav = 5.8f;
	float tau = 0.02f;			// viscousity // 高さに依存させた方がよい？
	float deltax = 1.1f;
	float deltime = 0.2f;
	
	(void)timestep;
	
	/* 高さ(y座標の計算) */
	/* ポインターセット */
	for (i = 0; i < SY; i++) {
		p[i] = (float *)(u_int) & My_surface + 12 * i + SX * 8 * i + 8;	/* object data */
	}

	for (i = 0; i < SY; i++) {
		surface_polygon_left[i] = (float *)(u_int)&My_surface_polygon_left
									+ 8 + 12 * i + SX * 8 * i;
		surface_polygon_right[i] = (float *)(u_int) & My_surface_polygon_right
									+ 8 + 12 * i + SX * 8 * i;
		surface_polygon_normal_left[i] = (float *)(u_int) & My_surface_polygon_left
									+ 8 + 12 * i + SX * 8 * i + SX * 4 + 4;
		surface_polygon_normal_right[i] = (float *)(u_int) & My_surface_polygon_right
									+ 8 + 12 * i + SX * 8 * i + SX * 4 + 4;
	}

	/* 生成 , 消滅 */

	/* 深さ計算 */
	for (i = 0; i < 100; i++) {
		for (j = 0; j < 100; j++) {
			depth[i][j] = height_n[i][j] - bottom[i][j];
			if (depth[i][j] < 0.01f) {
				depth[i][j] = 0.01f;
			}
		}
	}

	{
		float coff = grav * deltime * deltime / (2 * deltax * deltax);

		for (j = 0; j < 100; j++) {
			e[0] = 1.0f + (depth[j][0] + depth[j][1]) * coff;
			for (i = 1; i < 99; i++) {
				e[i] = 1.0f + (depth[j][i - 1] + 2.0f * depth[j][i] + depth[j][i + 1]) * coff;
			}

			e[99] = 1.0f + (depth[j][98] + depth[j][99]) * coff;
			for (i = 0; i < 99; i++) {

				f[i] = -(depth[j][i] + depth[j][i + 1]) * coff;
				right_hand_side[i] = height_n_1[j][i] + (1 - tau) * (height_n_1[j][i] - height_n_2[j][i]);
			}
			f[99] = -(depth[j][98] + depth[j][99]) * coff;
			right_hand_side[99] = height_n_1[j][99] + (1 - tau) * (height_n_1[j][99] - height_n_2[j][99]);
			// solve equation  
			tridag(f, e, f, right_hand_side, height_n[j], 100);
		}
	}


	memcpy(height_n_2, height_n_1, sizeof(height_n_1));
	memcpy(height_n_1, height_n, sizeof(height_n));
	/* transpose */
	for (i = 0; i < 100; i++) {
		for (j = 0; j < 100; j++) {
			height_n2[j][i] = height_n[i][j];
			height_n_12[j][i] = height_n_1[i][j];
			height_n_22[j][i] = height_n_2[i][j];
		}
	}

	{
		float coff = grav * deltime * deltime / (2.0f * deltax * deltax);

		for (j = 0; j < 100; j++) {
			e[0] = 1.0f + (depth[0][j] + depth[1][j]) * coff;
			for (i = 1; i < 99; i++) {
				e[i] = 1.0f + (depth[i - 1][j] + 2.0f * depth[i][j] + depth[i + 1][j]) * coff;
			}

			e[99] = 1.0f + (depth[98][j] + depth[99][j]) * coff;
			for (i = 0; i < 99; i++) {
				f[i] = -(depth[i][j] + depth[i + 1][j]) * coff;
				right_hand_side[i] = height_n_12[j][i] + (1 - tau) * (height_n_12[j][i] - height_n_22[j][i]);
			}

			f[99] = -(depth[98][j] + depth[99][j]) * coff;
			right_hand_side[99] = height_n_12[j][99] + (1 - tau) * (height_n_12[j][99] - height_n_22[j][99]);
			tridag(f, e, f, right_hand_side, height_n2[j], 100);
		}
	}


	memcpy(height_n_22, height_n_12, sizeof(height_n_12));
	memcpy(height_n_12, height_n2, sizeof(height_n2));
	for (i = 0; i < 100; i++) {
		for (j = 0; j < 100; j++) {
			height[i][j] = height_n2[j][i];
			height_n[i][j] = height_n2[j][i];
			height_n_2[i][j] = height_n_22[j][i];
			height_n_1[i][j] = height_n_12[j][i];
		}
	}

	/* ポリゴン座標への変換 */
	for (j = 0; j < SY; j++) {
		for (i = 0; i <= 396; i += 4) {
			p[j][i + 1] = height[i >> 2][j];
			height[i >> 2][j] = 0;
		}
	}

	/* polygon model に変換  */
	for (i = 0; i < 99; i++) {
		k = 0;
		for (j = 0; j < 50; j++) {
			surface_polygon_left[i][4 * k + 1] = p[i][4 * j + 1];
			surface_polygon_left[i][4 * k + 5] = p[i + 1][4 * j + 1];
			k += 2;
		}
	}
	k = 0;
	for (j = 0; j < 50; j++) {
		surface_polygon_left[99][4 * k + 1] = p[99][4 * j + 1];
		surface_polygon_left[99][4 * k + 5] = p[99][4 * j + 1];
		k += 2;
	}


	for (i = 0; i < 99; i++) {
		k = 0;
		for (j = 49; j < 99; j++) {
			surface_polygon_right[i][4 * k + 1] = p[i][4 * j + 1];
			surface_polygon_right[i][4 * k + 5] = p[i + 1][4 * j + 1];
			k += 2;
		}
	}

	k = 0;
	for (j = 49; j < 99; j++) {
		surface_polygon_right[99][4 * k + 1] = p[99][4 * j + 1];
		surface_polygon_right[99][4 * k + 5] = p[99][4 * j + 1];
		k += 2;
	}

	/* setsurfacenormal(void) */
	for (i = 0; i < 100; i++) {
		for (k = 2; k <= 99; k++) {
			// left lattice 
			work1[0] = surface_polygon_left[i][4 * k - 8] - surface_polygon_left[i][4 * k];
			work1[1] = surface_polygon_left[i][4 * k - 7] - surface_polygon_left[i][4 * k + 1];
			work1[2] = surface_polygon_left[i][4 * k - 6] - surface_polygon_left[i][4 * k + 2];
			work2[0] = surface_polygon_left[i][4 * k - 4] - surface_polygon_left[i][4 * k];
			work2[1] = surface_polygon_left[i][4 * k - 3] - surface_polygon_left[i][4 * k + 1];
			work2[2] = surface_polygon_left[i][4 * k - 2] - surface_polygon_left[i][4 * k + 2];
			// right lattice 
			work4[0] = surface_polygon_right[i][4 * k - 8] - surface_polygon_right[i][4 * k];
			work4[1] = surface_polygon_right[i][4 * k - 7] - surface_polygon_right[i][4 * k + 1];
			work4[2] = surface_polygon_right[i][4 * k - 6] - surface_polygon_right[i][4 * k + 2];
			work5[0] = surface_polygon_right[i][4 * k - 4] - surface_polygon_right[i][4 * k];
			work5[1] = surface_polygon_right[i][4 * k - 3] - surface_polygon_right[i][4 * k + 1];
			work5[2] = surface_polygon_right[i][4 * k - 2] - surface_polygon_right[i][4 * k + 2];
			if (k % 2) {
				sceVu0OuterProduct(work3, work2, work1);
				sceVu0OuterProduct(work6, work5, work4);
			} else {
				sceVu0OuterProduct(work3, work1, work2);
				sceVu0OuterProduct(work6, work4, work5);
			}

			sceVu0Normalize(work3, work3);
			sceVu0Normalize(work6, work6);
			memcpy(&surface_polygon_normal_left[i][4 * k], &work3[0], sizeof(float) * 3);
			memcpy(&surface_polygon_normal_right[i][4 * k], &work6[0], sizeof(float) * 3);
		}

		// 端点
		memcpy(&surface_polygon_normal_right[i][0], &surface_polygon_normal_left[i][396], sizeof(float) * 3);
		memcpy(&surface_polygon_normal_right[i][4], &surface_polygon_normal_left[i][392], sizeof(float) * 3);
	}
}

void tridag(float a[], float b[], float c[], float r[], float u[], int trinum)
{
	int j;
	float bet;
	float gam[100] __attribute__ ((aligned(64)));

	if (b[0] == 0.0f) {
		printf("Error 1\n");
	}
	u[0] = r[0] / (bet = b[0]);
	for (j = 1; j <= (trinum - 1); j++) {
		gam[j] = c[j - 1] / bet;
		bet = b[j] - a[j - 1] * gam[j];
		if (bet == 0.0f) {
			printf("Error 2 j=%d\n", j);
		}
		u[j] = (r[j] - a[j - 1] * u[j - 1]) / bet;
	}

	for (j = (trinum - 1); j >= 0; j--) {
		u[j] -= gam[j + 1] * u[j + 1];
	}
}

