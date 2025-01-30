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


// GS �֘A
#define SCREEN_WIDTH			320
#define SCREEN_HEIGHT			224
#define OFFX					(((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY					(((4096-SCREEN_HEIGHT)/2)<<4)

static sceGsDBuff db;			// �`���


// �T�E���h�֘A
#define PCM_SIZE				768*1024
#define TRANS_CH				0
#define TRANS_MODE				SD_TRANS_MODE_WRITE | SD_BLOCK_LOOP

static int g_tidLibsdr;			// libsdr�̃R�[���o�b�N�X���b�h��ID
static void *g_pIopHeap;		// IOP���Ɋm�ۂ���SPU2�]���o�b�t�@�̃A�h���X


// libpad�pDMA�o�b�t�@
static u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned(64)));


// ���j���[�֌W
#define MENU_LOCX	7
#define MENU_LOCY	6

// �I�[�o�[���C
#if USE_OVERLAY

#define MENU_MAX	3

extern int main_balls(const char *);
extern int main_iga(const char *);
extern int main_ezmpeg(const char *);

// over.cmd �Œ�`���Ă���I�[�o���C�A�h���X
extern int _overlay_addr __attribute__((section(".data")));

typedef struct {
	const char *s;				// ��ʕ\���f���^�C�g��
	const char *fn;				// BIN�t�@�C����
	int (*func)(const char *);	// �Ăяo���֐�
	const char *arg;			// ����������
	u_int rdcnt;				// ���[�h��
	u_int errcnt;				// ���[�h�G���[��
} DMENU;

static DMENU g_DMenus[MENU_MAX] = {
//	{ ���j���[, BIN�t�@�C����,  �֐���,      ����,          0, 0},
	{ "BALLS ", BALLS_BIN,		main_balls,	 NULL,			0, 0},
	{ "IGA   ", IGA_BIN,		main_iga,    NULL,			0, 0},
	{ "EZMPEG", EZMPEG_BIN,		main_ezmpeg, MPEG_FILENAME,	0, 0}
};

#endif	// USE_OVERLAY


// ERX�g�p
#if USE_ERX

#define MENU_MAX	6

extern SceErxLibraryHeader baseelf_entry;
extern int main_overlay(const char *);

#define LIBERX_THREAD_PRIORITY		10
#define LIBERX_MEMORY_SIZE			(8*1024*1024)
#define LIBERX_MAX_MODULES			32

typedef struct {
	const char *s;				// ��ʕ\���f���^�C�g��
	const char *fn;				// ERX�t�@�C����
	u_int mode;					// sceSifLoadModuleFile()�Ŏw�肷�郂�[�h
	const char *arg;			// ����������
	u_int rdcnt;				// ���[�h��
	u_int errcnt;				// ���[�h�G���[��
} DMENU;

static DMENU g_DMenus[MENU_MAX] = {
//	{ ���j���[, ERX�t�@�C����,  		���[�h					����,           0, 0},
	{ "BALLS       ",	BALLS_ERX,		0,						NULL,			0, 0},
	{ "IGA         ",	IGA_ERX,		0,						NULL,			0, 0},
	{ "EZMPEG      ",	EZMPEG_ERX,		0,						MPEG_FILENAME,	0, 0},
	{ "BALLS(GZIP) ",	BALLS_ERZ,		SCE_ERX_ENCODE_GZIP,	NULL,			0, 0},
	{ "IGA(GZIP)   ",	IGA_ERZ,		SCE_ERX_ENCODE_GZIP,	NULL,			0, 0},
	{ "EZMPEG(GZIP)",	EZMPEG_ERZ,		SCE_ERX_ENCODE_GZIP,	MPEG_FILENAME,	0, 0}
};


#endif	// USE_ERX



#define VCNT 300			// 300 �t���[��(��5�b)
static int AUTO_vcount;

// ERX���W���[���ɑ΂��ăG�N�X�|�[�g����֐�
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




// �������[�`��
int main(int argc, const char *const argv[])
{
	int port;
	int slot;
	int i;
	static short curs = 0;			// �J�[�\���ʒu

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
		int media = SCECdCD;			// TODO: DVD���g���ꍇ��SCECdDVD�ɕς��Ă��������B

		sceDevctl("cdrom0:", CDIOC_INIT, &mode, sizeof(int), NULL, 0);
		sceDevctl("cdrom0:", CDIOC_MMODE, &media, sizeof(int), NULL, 0);
	}
#endif	// CDROM

	// IOP���W���[�����[�h
	LoadIrx(LIBSD_IRX,   0, NULL);		// libsd.irx�̓ǂ݂���
	LoadIrx(SDRDRV_IRX,  0, NULL);		// sdrvdrv.irx�̓ǂ݂���
	LoadIrx(SIO2MAN_IRX, 0, NULL);		// sio2man.irx�̓ǂ݂���
	LoadIrx(PADMAN_IRX,  0, NULL);		// padman.irx�̓ǂ݂���

	// �R�[���o�b�N�X���b�h���𓮂������߂ɁA���C���X���b�h�̃v���C�I���e�B��������
	ChangeThreadPriority(GetThreadId(), 20);

#if USE_ERX
	// ERX���C�u�����̏�����
	SetupErxEnvironment();
#endif	// USE_ERX

	// �T�E���h
	sceSdRemoteInit();							// libsdr��������
	g_tidLibsdr = sceSdRemoteCallbackInit(5);	// �R�[���o�b�N�X���b�h�𐶐�
												// ���C���X���b�h��荂���v���C�I���e�B���w��
	Spu2DataLoad();								// �T�E���h�f�[�^�ǂ݂���
	Spu2AutoDmaStart();							// AUTO DMA����������


	// �R���g���[�����C�u����������
	port = 0;
	slot = 0;
	scePadInit(0);
	if (scePadPortOpen(port, slot, pad_dma_buf) == 0) {
		printf("ERROR: scePadPortOpen\n");
		return (0);
	}
	AutoReset();

	while (1) {
		int console;		// �t�H���g�\���p
		int frame;			// �t���[���̃J�E���^
		u_short rpad = 0;	// �p�b�h�����������

		// GS������
		Gs_Initialize();

		// �t�H���g�V�X�e��������
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
				// �p�b�h�f�[�^�̓ǂ݂��ݎ��s��
				tpad = rpad = 0;
			} else {
				// �O��̏��Ɣ�r���āA�{�^���̉����������𓾂�
				u_short paddata;
				paddata = 0xFFFF ^ ((rdata[2]<<8) | rdata[3]);
				tpad = paddata & ~rpad;
				rpad = paddata;
			}

			// �J�[�\���ʒu�̐��K��
			if (curs < 0) {
				curs = MENU_MAX - 1;
			} else if (curs >= MENU_MAX) {
				curs = 0;
			}
			if (AutoStep()) {
				// �����J�ڂ̂��߂̃J�E���g�����������Ƃ�
				tpad |= SCE_PADstart;			// [START]�{�^�����������������C�N
			}

			if (tpad & SCE_PADstart) {
				// [START]�{�^������������
#if USE_OVERLAY
				int errcnt;

				errcnt = LoadOverlayBinary(g_DMenus[curs].fn);
				g_DMenus[curs].rdcnt++;						// �ǂ݂��݉񐔂��C���N�������g
				g_DMenus[curs].errcnt += errcnt;			// �ݐσG���[�����񐔂ɉ��Z
				AutoReset();

				// �I�[�o�[���C�Ăяo��
				printf("*** call child No.%d ***\n", curs);
				g_DMenus[curs].func(g_DMenus[curs].arg);	// �I�[�o���C�Ăяo��
				printf("returned main.\n");					// �߂��Ă���
				AutoReset();
#endif	// USE_OVERLAY

#if USE_ERX
				int modid;
				int res, result;

				// ERX���W���[���̓ǂݍ���
				do {
					modid = sceErxLoadModuleFile(g_DMenus[curs].fn, g_DMenus[curs].mode, NULL);
					if (modid<0) {
						printf("sceErxLoadModuleFile(%s): %d(%08X)\n", g_DMenus[curs].fn, modid, modid);
						g_DMenus[curs].errcnt++;			// �ݐσG���[�����񐔂ɉ��Z
					}
				} while (modid<0);
				g_DMenus[curs].rdcnt++;						// �ǂ݂��݉񐔂��C���N�������g

				// ���W���[���̋N��
				res = sceErxStartModule(modid, g_DMenus[curs].fn, 0, "", &result);
				if (res < 0) {
					// ���W���[���̋N���Ɏ��s
					printf("module '%s' can not start. (error=%x)\n", g_DMenus[curs].fn, res);
					while (1);
				}
				printf("module '%s' loaded. (modid=%d)\n", g_DMenus[curs].fn, modid);

				// ���W���[�����̊֐����Ăяo��
				printf("*** call child No.%d ***(modid=%d)\n", curs, modid);
				AutoReset();
				main_overlay(g_DMenus[curs].arg);			// ERX���W���[���Ăяo��
				printf("returned main.\n");

				// ���W���[���̓�����~
				res = sceErxStopModule(modid, 0, "", &result);
				printf("sceErxStopModule: %d, %08X\n", modid, res);

				// ���W���[���̎g�p���Ă��������������
				res = sceErxUnloadModule(modid);
				printf("sceErxUnloadModule: %d, %08X\n", modid, res);

				AutoReset();
#endif	// USE_ERX
#ifdef AUTO
				curs++;							// ���̃��j���[��
#endif	//AUTO
				break;
			}

			if (tpad & SCE_PADLup)   curs--;	// [��]�{�^��
			if (tpad & SCE_PADLdown) curs++;	// [��]�{�^��

			// �t�H���g�\��
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

			// �C���^���[�X�\���̃n�[�t���X�^����
			if (frame & 1) {
				sceGsSetHalfOffset(&db.draw1, 2048, 2048, sceGsSyncV(0) ^ 0x01);
			} else {
				sceGsSetHalfOffset(&db.draw0, 2048, 2048, sceGsSyncV(0) ^ 0x01);
			}
			sceGsSyncPath(0, 0);

			FlushCache(WRITEBACK_DCACHE);		// D�L���b�V���̃��C�g�o�b�N
			sceGsSwapDBuff(&db, frame);
		}
		sceDevConsClose(console);
	}

	// ���o�̓u���b�N�ւ̓]�����~
	sceSdRemote(1, rSdBlockTrans, TRANS_CH, SD_TRANS_MODE_STOP, NULL, 0);
	DeleteThread(g_tidLibsdr);
	return (0);
}



// IRX���W���[�����[�h
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

// �I�[�o���C�t�@�C�����[�h
static int LoadOverlayBinary(const char *pszBinFileName)
{
	int fd;
	int size;
	int errcnt;

	// �I�[�o�[���C�t�@�C�����I�[�v��
	do {
		fd = sceOpen(pszBinFileName, SCE_RDONLY);
		if (fd<0) {
			printf("LoadOverlayBinary failed: %s, %d\n", pszBinFileName, fd);
		}
	} while (fd<0);

	// �t�@�C���T�C�Y���擾
	size = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);
	printf("overlay_addr = %p  size = %d\n", &_overlay_addr, size);

	// �I�[�o�[���C�t�@�C���̓ǂݍ��ݏ����{��
	errcnt = 0;
	while ((sceRead(fd, (void *)(&_overlay_addr), size)) != size) {
		errcnt++;							// �G���[�����񐔂��C���N�������g
		printf("read failed\n");
	}
	sceClose(fd);

	FlushCache(WRITEBACK_DCACHE);			// D�L���b�V���̃��C�g�o�b�N
	FlushCache(INVALIDATE_ICACHE);			// I�L���b�V���̃C���o���f�[�g
	return (errcnt);
}

#endif	// USE_OVERLAY


// GS������
static int Gs_Initialize(void)
{
	static struct {
		union {
			u_long ul[2];
			sceGifTag tag;
		} gif;
		sceGsAlphaEnv alpha;
	} drawenv __attribute__((aligned(16)));

	// DMA������
	sceDmaReset(1);

	// GS������
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

	FlushCache(WRITEBACK_DCACHE);			// D�L���b�V���̃��C�g�o�b�N
	sceGsPutDrawEnv(&drawenv.gif.tag);
	sceGsSyncPath(0, 0);

	// ���̊�t�B�[���h�܂ő҂�
	while (!sceGsSyncV(0));
	return (0);
}





// �T�E���h�֘A
static int Spu2DataLoad(void)
{
	// IOP��SPU2 AutoDMA�p�]���o�b�t�@���m��
	do {
		g_pIopHeap = sceSifAllocSysMemory(0, PCM_SIZE, NULL);
		if (g_pIopHeap==NULL) {
			printf("sceSifAllocSysMemory() failed\n");
		}
	} while (g_pIopHeap==NULL);


	// IOP�Ɋm�ۂ����o�b�t�@�ɁASPU2�f�[�^��ǂݍ��܂���
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
				// �t�@�C���̏I�[�܂�
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


// AUTO DMA����������
static int Spu2AutoDmaStart(void)
{
	// SPU2��������
	sceSdRemote(1, rSdInit, SD_INIT_COLD);

	// �}�X�^�[�{�����[����ݒ�
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MVOLL, 0x3FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MVOLR, 0x3FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MVOLL, 0x3FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MVOLR, 0x3FFF);

#if TRANS_CH==0
	// SPU2�R�A0�̓��̓u���b�N��AutoDMA����Ƃ�
	// �R�A#0�͓��̓u���b�N(Dry),�{�C�X�o��(Dry,Wet)���L��
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MMIX,  SD_MMIX_MINR   | SD_MMIX_MINL   |
														SD_MMIX_MSNDER | SD_MMIX_MSNDEL |
														SD_MMIX_MSNDR  | SD_MMIX_MSNDL);

	// �R�A#1�͊O������(Dry,Wet),�{�C�X�o��(Dry,Wet)���L��
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MMIX,  SD_MMIX_SINER  | SD_MMIX_SINEL  |
														SD_MMIX_SINR   | SD_MMIX_SINL   |
														SD_MMIX_MSNDER | SD_MMIX_MSNDEL |
														SD_MMIX_MSNDR  | SD_MMIX_MSNDL);
#else
	// SPU2�R�A1�̓��̓u���b�N��AutoDMA����Ƃ�
	// �R�A#0�̓{�C�X�o��(Dry,Wet)���L��
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MMIX,  SD_MMIX_MSNDER | SD_MMIX_MSNDEL |
														SD_MMIX_MSNDR  | SD_MMIX_MSNDL);

	// �R�A#1�͓��̓u���b�N(Dry),�O������(Dry,Wet),�{�C�X�o��(Dry,Wet)���L��
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MMIX,  SD_MMIX_SINER  | SD_MMIX_SINEL  |
														SD_MMIX_SINR   | SD_MMIX_SINL   |
														SD_MMIX_MINR   | SD_MMIX_MINL   |
														SD_MMIX_MSNDER | SD_MMIX_MSNDEL |
														SD_MMIX_MSNDR  | SD_MMIX_MSNDL);
#endif


	// ���荞�݃n���h����ݒ�
	sceSdRemote(1, rSdSetTransIntrHandler, TRANS_CH, Spu2TransCallback, NULL);

	// ���o�̓u���b�N�ւ̓]����ݒ�
	sceSdRemote(1, rSdBlockTrans, TRANS_CH, TRANS_MODE, g_pIopHeap, (PCM_SIZE/512)*512);

	// �R�A#1�̊O������(�R�A#0�̏o��)�{�����[����ݒ�
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_AVOLL, 0x7FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_AVOLR, 0x7FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_BVOLL, 0x7FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_BVOLR, 0x7FFF);

	// �R�A#0�T�E���h�f�[�^���͂̃{�����[����ݒ�
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_AVOLL, 0);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_AVOLR, 0);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_BVOLL, 0x7FFF);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_BVOLR, 0x7FFF);

	// �G�t�F�N�g���͂̃{�����[����ݒ�
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_EVOLL, 0);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_EVOLR, 0);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_EVOLL, 0);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_EVOLR, 0);
	return (0);
}


// AUTO DMA�����~
static int Spu2AutoDmaStop(void)
{
	// ���C���{�����[����0�ɍi��
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MVOLL, 0x0);
	sceSdRemote(1, rSdSetParam, SD_CORE_0 | SD_P_MVOLR, 0x0);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MVOLL, 0x0);
	sceSdRemote(1, rSdSetParam, SD_CORE_1 | SD_P_MVOLR, 0x0);

	// ���荞�݃n���h��������
	sceSdRemote(1, rSdSetTransIntrHandler, TRANS_CH, 0, NULL);

	// ���o�̓u���b�N�ւ̓]�����~
	sceSdRemote(1, rSdBlockTrans, TRANS_CH, SD_TRANS_MODE_STOP, NULL, 0);

	// �T�E���h�f�[�^���̓u���b�N�̃{�����[�����i��
	sceSdRemote(1, rSdSetParam, TRANS_CH | SD_P_BVOLL, 0x0);
	sceSdRemote(1, rSdSetParam, TRANS_CH | SD_P_BVOLR, 0x0);
	return (0);
}


// SPU2�]���R�[���o�b�N�֐�
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
		// �^�C���A�b�v���Ă���
		res = 1;
	} else {
		// �J�E���g�l���f�N�������g
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

	// liberx����sysmem���C�u�����Ɏg�킹�郁�����̊m��
	pBuf = memalign(256, LIBERX_MEMORY_SIZE);
	res = sceErxInit(LIBERX_THREAD_PRIORITY, pBuf, LIBERX_MEMORY_SIZE, LIBERX_MAX_MODULES);
	printf("sceErxInit: %d\n", res);

	// �_�C�i�~�b�N�����N�Ń��[�h�����ERX�̂��߂�
	// �{��ELF��������G�N�X�|�[�g�����֐���liberx�ɓo�^

	// libgcc�֌W�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceLibgccCommonGetErxEntries());	// libgcc �Z�p�⏕�֐����G�N�X�|�[�g

	sceErxRegisterLibraryEntries(sceLibcStdioGetErxEntries());	// stdio�֌W�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceLibcStdlibGetErxEntries());	// stdlib�֌W�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceLibcStringGetErxEntries());	// string�֌W�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceLibcMallocGetErxEntries());	// malloc�֌W�̊֐����G�N�X�|�[�g

	sceErxRegisterLibraryEntries(sceLibmFloatGetErxEntries());	// libm�֌W��float�֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceCdGetErxEntries());			// libcdvd�̊֐����G�N�X�|�[�g

#if 0
	sceErxRegisterLibraryEntries(sceGsGetErxEntries());			// libgraph�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceDmaGetErxEntries());		// libdma�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(scePktGetErxEntries());		// libpkt�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceDevGetErxEntries());		// libdev�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceVu0GetErxEntries());		// libvu0�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceIpuGetErxEntries());		// libipu�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceMpegGetErxEntries());		// libmpeg�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(scePadGetErxEntries());		// libpad�̊֐����G�N�X�|�[�g
	sceErxRegisterLibraryEntries(sceSdGetErxEntries());			// libsdr�̊֐����G�N�X�|�[�g
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
			// ���C�u������ERX���W���[����ǂݍ���
			do {
				modid = sceErxLoadModuleFile(apszErxFiles[i], SCE_ERX_SMEM_LOW, NULL);
			} while (modid<0);

			// �ǂݍ���ERX���W���[�����N��
			res = sceErxStartModule(modid, apszErxFiles[i], 0, "", &res);
			if (res < 0) {
				// ���W���[���̋N���Ɏ��s
				printf("module '%s' can not start. (error=%x)\n", apszErxFiles[i], res);
				printf("maybe dependeg library is not found...\n");
				while (1);
			}
			printf("module '%s' loaded. (modid=%d)\n", apszErxFiles[i], modid);
		}
	}
#endif

	// �x�[�XELF��������G�N�X�|�[�g����֐�(SCE���C�u�����ȊO)��liberx�ɓo�^
	sceErxRegisterLibraryEntries(&baseelf_entry);
	return;
}
#endif // USE_ERX

