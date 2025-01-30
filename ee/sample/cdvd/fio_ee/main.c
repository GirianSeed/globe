/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */ 
/* 
 *              Emotion Engine Library Sample Program
 * 
 *                         - CD/DVD -
 * 
 *                         Version <0.02>
 *                           Shift-JIS
 * 
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 * 
 *                            <sample.c>
 *                     <main function of file read>
 * 
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.01            May,15,2001     kashiwabara  first   Version
 */


#include <eekernel.h>
#include <eeregs.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <stdio.h>
#include <string.h>
#include <libcdvd.h>

#define MEDIA_CD 	/* 読み込みメディアは CD */

/* ================================================================
 *
 *      Program
 *
 * ================================================================ */

#ifndef btoi
#define btoi(b)         ((b)/16*10 + (b)%16)            /* BCD to u_char */
#endif

unsigned char  bf[2048 * 1024] __attribute__ ((aligned(64)));
    /* 読み込みバッファ読み込み速度向上の為(aligned(64))がお勧めです。*/
unsigned char  toc[1024];	/* TOC読み込みバッファ */

static u_char main_stack[512 * 16] __attribute__ ((aligned(16)));
                                /* start_main()関数のスタック */
#define MAIN_STACK_SIZE (512 * 16) /* start_main()関数のスタックサイズ */
static u_char poff_stack[512 * 8] __attribute__ ((aligned(16)));
				/* PowerOff()関数のスタック */
#define POFF_STACK_SIZE (512 * 8) /* PowerOff()関数のスタックサイズ */
#define WAIT_16MSEC	(16 * 1000)

/* PS2HDD キャッシュ初期化の為の文字列 */
static char *cash_str= "pfs0:/,1,16";

/* PS2パワーオフ処理 */
static void PowerOff( void *arg )
{
    int poff_semid;

    poff_semid= *((int *)arg);
    WaitSema(poff_semid);
    scePrintf("Power Off request has come.\n");
    /* close all files */
    sceDevctl("pfs:", PDIOC_CLOSEALL, NULL, 0, NULL, 0);
    /* dev9 power off, need to power off PS2 */
    while (sceDevctl("dev9x:", DDIOC_OFF, NULL, 0, NULL, 0) < 0) ;
    /* PS2 power off */
    while (sceDevctl("cdrom0:", CDIOC_POWEROFF, NULL, 0, NULL, 0) < 0);
    while (1);
}

/* PS2パワーオフ割り込みハンドラ */
static void poff_handler( void *poff_semid )
{
    scePrintf("Power Off Called\n");
    iSignalSema(*((int *)poff_semid));
}

/* メイン処理関数 */
static void start_main(void *argp)
{
    int	 rfd, wfd, fd, cnt0, ret, lpcnt, disk_type,mediaCD= 0,
	 last_track, readsize, trayflg, traycnt, zonesz;
    unsigned char *cp;
    char *cw, rfilename[32],wfilename[32],rbuf[64];
    sceCdCLOCK brtc, srtc, ertc;
    u_int stime, etime, uiarg;

    scePrintf("sample start.\n");
    sceSifInitRpc(0);

    sceFsReset();

    scePrintf("Init\n");
    uiarg= 0;
    sceDevctl("cdrom0:", CDIOC_INIT, &uiarg, 4, rbuf, 4);

    scePrintf("scePowerOffHandler() call\n");
    /* PS２標準入出力パワーオフハンドラの登録 */
    scePowerOffHandler( poff_handler, argp );

    /* キャッシュの為のパーテーションの作成 */
    printf("Create main Partition.\n");
    if ((fd = sceOpen("hdd0:test,fpasswd,rpasswd,256M,PFS",
                                            SCE_CREAT|SCE_RDWR)) < 0) {
        printf("could not create partition, %d.\n", fd);
    }else{
        sceClose(fd);

        printf("Format pfs.\n");
        zonesz = 8192;
        if ((ret = sceFormat("pfs:", "hdd0:test,fpasswd", &zonesz, sizeof(int)))
								 < 0){
            printf("cannot format partition, %d.\n", ret);
            return;
        }
        printf("Mount pfs.\n");
        if ((ret = sceMount("pfs0:", "hdd0:test,fpasswd", 0, NULL, 0)) < 0) {
            printf("cannot mount partition, %d.\n", ret);
            return;
        }

        /* キャッシュの初期化 */
        if ((ret = sceDevctl("cdrom0:", CDIOC_FSCACHEINIT, cash_str, 
					strlen(cash_str), NULL, 0)) < 0) {
            printf("cannot cache Init, %d.\n", ret);
            return;
        }
    }

    scePrintf(" Drive Ready Wait\n");
    uiarg= 0;
    sceDevctl("cdrom0:", CDIOC_DISKRDY, &uiarg, 4, rbuf, 4);

    scePrintf(" Media Mode Set\n");
#ifdef MEDIA_CD
    uiarg= SCECdCD;
#else
    uiarg= SCECdDVD;
#endif
    sceDevctl("cdrom0:", CDIOC_MMODE, &uiarg, 4, NULL, 0);

    lpcnt= 0;
    while (1){	
	if (lpcnt){
	    /* メディア交換 */
	    traycnt= 0;
	    uiarg= 2;
	    sceDevctl("cdrom0:", CDIOC_TRAYREQ, &uiarg, 4, &trayflg, 4);
	    scePrintf("Change your PlayStation CD/DVD-ROM\n");
	    do {

                /* 他スレッドへの影響を考えリソース返却 */
		DelayThread(WAIT_16MSEC * 15);

		uiarg= 2;
		while(sceDevctl("cdrom0:", CDIOC_TRAYREQ, &uiarg, 4, &trayflg, 4) < 0){;
                    /* 他スレッドへの影響を考えリソース返却 */
		    DelayThread(WAIT_16MSEC);
		}
		traycnt+= trayflg;
	        sceDevctl("cdrom0:", CDIOC_GETDISKTYP,
						 NULL, 0, &disk_type, 4);
	    }while (!traycnt ||
		    (disk_type == SCECdNODISC) ||
		    (disk_type == SCECdDETCT )    );
	    uiarg= 0;
	    sceDevctl("cdrom0:", CDIOC_DISKRDY, &uiarg, 4, rbuf, 4);
	}

	uiarg= 0;
	sceDevctl("cdrom0:", CDIOC_DISKRDY, &uiarg, 4, rbuf, 4);

	scePrintf("Rtc Read \n");
	sceDevctl("cdrom0:", CDIOC_READCLOCK, NULL, 0, &brtc, sizeof(sceCdCLOCK));
        scePrintf("year= %d mth= %d day= %d hour= %d min= %d sec= %d\n",
                   btoi(brtc.year), btoi(brtc.month & 0x7f), btoi(brtc.day),
		   btoi(brtc.hour), btoi(brtc.minute), btoi(brtc.second));
        scePrintf("rtc= 0x%02x\n",brtc.stat);

	scePrintf(" GetDiskType   ");
	sceDevctl("cdrom0:", CDIOC_GETDISKTYP, NULL, 0, &disk_type, 4);
	mediaCD= 0;
	switch (disk_type){
	    case SCECdIllegalMedia:
		scePrintf("Disk Type= IllegalMedia\n"); break;
	    case SCECdPS2DVD:
		scePrintf("Disk Type= PlayStation2 DVD\n"); break;
	    case SCECdPS2CD:
		scePrintf("Disk Type= PlayStation2 CD\n");
						 mediaCD= 1; break;
	    case SCECdPS2CDDA:
		scePrintf("Disk Type= PlayStation2 CD with CDDA\n");
						 mediaCD= 1; break;
	    case SCECdPSCD:
		scePrintf("Disk Type= PlayStation CD\n");
						 mediaCD= 1; break;
	    case SCECdPSCDDA:
		scePrintf("Disk Type= PlayStation CD with CDDA\n");
						 mediaCD= 1; break;
	    case SCECdDVDV:
		scePrintf("Disk Type= DVD video\n"); break;
	    case SCECdCDDA:
		scePrintf("Disk Type= CD-DA\n");
						 mediaCD= 1; break;
	    case SCECdDETCT:
		scePrintf("Working\n"); break;
	    case SCECdNODISC: 
		scePrintf("Disk Type= No Disc\n"); break;
	    default:
		scePrintf("Disk Type= OTHER DISK\n"); break;
	}

	uiarg= 0;
	sceDevctl("cdrom0:", CDIOC_DISKRDY, &uiarg, 4, rbuf, 4);

	if (mediaCD){
	    scePrintf("Get TOC ");
	    sceDevctl("cdrom0:", CDIOC_GETTOC, NULL, 0, &toc, 1024);
	    cp= (u_char *)toc;
            last_track= (int)btoi(*(cp + 17));
            scePrintf("track max= %d\n",last_track);
            for (cnt0= 0,cp+= 30; cnt0 < last_track; cnt0+= 1){
             	scePrintf("track No= %d abs Minute= %d Second= %d Frame= %d\n\n",
                    cnt0 + 1, btoi(*(cp + 7)), btoi(*(cp + 8)), btoi(*(cp + 9)));
                cp+= 10;
            }
        }
	/* 標準入出力でファイルを読む */
        rfd = sceOpen("cdrom0:\\SYSTEM.CNF;1",SCE_RDONLY);
        if ( rfd < 0 ){
            scePrintf("Can't open SYSTEM.CNF\n");
	    lpcnt++;
            continue;
        }
        readsize = sceLseek(rfd, 0, SCE_SEEK_END);
    	sceLseek(rfd, 0, SCE_SEEK_SET);
        sceRead(rfd, bf, readsize);
        sceClose(rfd);


	/* PlayStation実行のファイル名を取り出す。*/
	cw= (char *)bf;
	for (cnt0= 0;
	    (*cw != '\n') && (*cw != '\r') && (cnt0 < readsize); cnt0++) cw++;
	if (cnt0 == readsize){
	    scePrintf("Sorry: File name not Correct\n");
	    lpcnt++;
            continue;
	}
	*cw= 0;
	cw= (char *)bf;
	for (cnt0= 0; (*cw != ':') && (cnt0 < readsize); cnt0++) cw++;
	if (cnt0 == readsize){
	    scePrintf("Sorry: File name not Collect\n");
	    lpcnt++;
            continue;
	}
	cw++; 
	sprintf( rfilename, "cdrom0:\\%s", cw);

	uiarg= 0;
	sceDevctl("cdrom0:", CDIOC_DISKRDY, &uiarg, 4, rbuf, 4);

	uiarg= 0;
	sceDevctl("cdrom0:", CDIOC_TRYCNT, &uiarg, 4, NULL, 0);
	    /* エラー発生時は256トライ		 */
	sceDevctl("cdrom0:", CDIOC_SPINNOM, NULL, 0, NULL, 0);
	    /* エラー発生時は回転速度を落してリード	 */

	/* 標準入出力キャッシュ付きでファイルを読む */
        rfd = sceOpen(rfilename,SCE_RDONLY | SCE_CdCACHE);
        if ( rfd < 0 ){
	    /* キャッシュ付きオープンに失敗
			キャッシュ無しでファイルオープン */
            scePrintf("Fio Cache isn't used.\n",rfilename);
            rfd = sceOpen(rfilename,SCE_RDONLY);
            if ( rfd < 0 ){
                scePrintf("Can't open %s\n",rfilename);
                lpcnt++;
                continue;
	    }
        }

	/* ファイルの読み込みセクタ数を算出する */
        readsize = sceLseek(rfd, 0, SCE_SEEK_END);
    	sceLseek(rfd, 0, SCE_SEEK_SET);
	readsize= ((2048 * 1024) < readsize)? (2048 * 1024): readsize; 

	uiarg= 0;
	sceDevctl("cdrom0:", CDIOC_DISKRDY, &uiarg, 4, rbuf, 4);

	/* ファイルの読み込み開始時刻の取得 */
        sceDevctl("cdrom0:", CDIOC_READCLOCK, NULL, 0, &srtc, sizeof(sceCdCLOCK));
	/* ファイルの読み込みを開始する */
	for (ret= sceRead(rfd, bf, readsize);
			 ret != readsize;
	    ret= sceRead(rfd, bf, readsize)){
	    sceDevctl("cdrom0:", CDIOC_GETERROR, NULL, 0, &ret, 4);
	    scePrintf("Disk error code= 0x%08x\n", ret);
    	    sceLseek(rfd, 0, SCE_SEEK_SET);
	    DelayThread(WAIT_16MSEC);
	}
	/* ファイルの読み込み終了時刻の取得 */
        sceDevctl("cdrom0:", CDIOC_READCLOCK, NULL, 0, &ertc, sizeof(sceCdCLOCK));
        stime= (btoi(srtc.hour) * 60 * 60) +
	       (btoi(srtc.minute) * 60)    +
	        btoi(srtc.second);
        etime= (btoi(ertc.hour) * 60 * 60) +
	       (btoi(ertc.minute) * 60)    +
		btoi(ertc.second);
        scePrintf("Read Time= %d(sec)\n",etime - stime);

	sceClose(rfd);

	/* スタンドバイテスト */
	scePrintf("CdStandby Test\n");
	while ( sceDevctl("cdrom0:", CDIOC_STANDBY, NULL, 0, NULL, 0) < 0){
	    DelayThread(WAIT_16MSEC);
 	    scePrintf("Standby cmd fail\n");
	}

	/* ポーズテスト */
	scePrintf("CdPause Test\n");
	while ( sceDevctl("cdrom0:", CDIOC_PAUSE, NULL, 0, NULL, 0) < 0){
	    DelayThread(WAIT_16MSEC);
	    scePrintf("Pause cmd fail\n");
	}

	/* テストのためディスクを止めてみる */
	scePrintf("CdStop Test\n");
	while (sceDevctl("cdrom0:", CDIOC_STOP, NULL, 0, NULL, 0) < 0){
	    DelayThread(WAIT_16MSEC);
	    scePrintf("Stop cmd fail\n");
	}

        /* 読み込みファイルをホストに保存する。*/
	sprintf(wfilename,"host0:data%d.dat",lpcnt);
	wfd = sceOpen(wfilename, SCE_WRONLY|SCE_TRUNC|SCE_CREAT);
	if (wfd < 0 ) {
	    scePrintf("Can't open file data.dat\n");
	    lpcnt++;
            continue;
	}
	sceWrite(wfd, bf, readsize);
	sceClose(wfd);

	lpcnt++;
    }
}

/* start_main()関数スレッドを作成するための関数 */
int main()
{
    static struct ThreadParam thp;
    int thid,modid,res;
    char argp[16];
    char apaarg[] = "-o" "\0" "4";
    /* -o  同時にオープンできるパーテーションの数 */
    char pfsarg[] = "-m" "\0" "4" "\0" "-o" "\0" "10" "\0" "-n" "\0" "28";
    /* -m  同時にマウントできる数 */
    /* -o  同時にオープンできる数 */
    /* -n  キャッシュの数         */
    struct SemaParam    sparam;
    int poff_semid;


#ifdef MDLCHG_RESET
    /* IOP側のモジュール入れ換えを行う。*/
    sceSifInitRpc(0);
    while (sceSifRebootIop("host0:/usr/local/sce/iop/modules/"IOP_IMAGE_file) == 0){
       scePrintf("iop reset error\n");
    }
    while (sceSifSyncIop() == 0);
#endif

    sceSifInitRpc(0);

    /* EE側にて、標準入出力のみを使用する場合は"cdvd_ee_driver"をUnloadできる。*/
    scePrintf("sceSifSearchModuleByName: %d\n",
            modid= sceSifSearchModuleByName("cdvd_ee_driver"));
    scePrintf("sceSifStopModule: %d\n",sceSifStopModule(modid,0,argp,&res));
    scePrintf("sceSifUnloadModule: %d\n",sceSifUnloadModule(modid));

    /* PS２パワーオフのサンプルに使用する為、HDD関係のドライバをロード
	する。 べイモデルのみ						 */

    if (sceSifLoadModule("host0:/usr/local/sce/iop/modules/dev9.irx",
							   0, NULL) < 0){
        scePrintf("Can't load module dev9\n");
    }
    if (sceSifLoadModule("host0:/usr/local/sce/iop/modules/atad.irx",
							   0, NULL) < 0){
        scePrintf("Can't load module atad\n");
    }
    if (sceSifLoadModule("host0:/usr/local/sce/iop/modules/hdd.irx",
							  5, apaarg) < 0){
        scePrintf("Can't load module hdd\n");
    }
    if (sceSifLoadModule("host0:/usr/local/sce/iop/modules/pfs.irx",
							 18, pfsarg) < 0){
        scePrintf("Can't load module pfs\n");
    }

    sparam.initCount = 0;
    sparam.maxCount = 1;
    sparam.option = 0;
    poff_semid = CreateSema(&sparam);

    /* パワーオフ処理スレッドの生成 */
    thp.stackSize = POFF_STACK_SIZE;
    thp.gpReg = &_gp;
    thp.entry = PowerOff;
    thp.stack = (void *)poff_stack;
    thp.initPriority = 2;
    thid= CreateThread( &thp );
    StartThread( thid,(void *)&poff_semid);

    /* メイン処理スレッドの生成 */
    thp.stackSize = MAIN_STACK_SIZE;
    thp.gpReg = &_gp;
    thp.entry = start_main;
    thp.stack = (void *)main_stack;
    thp.initPriority = 3;
    thid= CreateThread( &thp );
    StartThread( thid,(void *)&poff_semid);

    SleepThread();
    return (0);
}

/* end of file.*/
