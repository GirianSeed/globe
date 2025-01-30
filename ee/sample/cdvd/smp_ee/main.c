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
 *      0.01            Oct,13,1999     kashiwabara  first   Version
 *      0.02            Dec,04,1999     kashiwabara  rel_13  Version
 *      0.03            Jan,12,2000     kashiwabara  rel_134 Version
 *      0.04            Feb,02,2000     kashiwabara  rel_142 Version
 *      0.05            Mer,21,2000     kashiwabara  rel_16  Version
 *      0.06            Nov,29,2000     kumagae      rel_21  Version
 */


#include <eekernel.h>
#include <eeregs.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <stdio.h>
#include <libcdvd.h>

#define MEDIA_CD	/* 読み込みメディアは CD */
// #define MEDIA_DVD_DUAL  /* 読み込みメディアは DVD_Dual */

/* ================================================================
 *
 *      Program
 *
 * ================================================================ */

unsigned char  bf[2048 * 1024] __attribute__ ((aligned(64)));
    /* 読み込みバッファ読み込み速度向上の為(aligned(64))がお勧めです。*/
unsigned char  toc[1024];	/* TOC読み込みバッファ */

static u_char main_stack[512 * 16] __attribute__ ((aligned(16)));
				/* start_main()関数のスタック */
#define MAIN_STACK_SIZE (512 * 16) /* start_main()関数のスタックサイズ */
static u_char cb_stack[512 * 16] __attribute__ ((aligned(16)));
				/* test_callback()関数のスタック */
#define CB_STACK_SIZE (512 * 16) /* test_callback()関数のスタックサイズ */
#define WAIT_16MSEC	(16 * 1000)

/* メディア読みとり関数等の、終了コールバック関数 */
static void test_callback( int cb_reason )
{
        switch(cb_reason){
            case SCECdFuncRead:
                scePrintf("SCECdFuncRead Ended\n"); break;
            case SCECdFuncSeek:
                scePrintf("SCECdFuncSeek Ended\n"); break;
            case SCECdFuncStandby:
                scePrintf("SCECdFuncStandby Ended\n"); break;
            case SCECdFuncStop:
                scePrintf("SCECdFuncStop Ended\n"); break;
            case SCECdFuncPause:
                scePrintf("SCECdFuncPause Ended\n"); break;
            default:
                scePrintf("Other Ended \n"); break;
        }
        scePrintf("error code= 0x%08x\n",sceCdGetError());
}

/* メイン処理スレッド関数 */
static void start_main(void *dummy)
{
	int	 rfd, wfd, rsec, rs_flg, cnt0, ret, *old_func, lpcnt,
		 disk_type, readsize, trayflg, traycnt;
#ifdef MEDIA_CD
	int	last_track;
	unsigned char *cp;
#endif
	char *cw, *filename,wfilename[32];
	sceCdlFILE fp;
	sceCdRMode mode;
	sceCdlLOCCD cl;
#ifdef MEDIA_DVD_DUAL
        int Dualflg;
	unsigned int Layer1Start;
#endif
	scePrintf("sample start.\n");
	sceSifInitRpc(0);

	scePrintf(" sceCdInit\n");
	sceCdInit(SCECdINIT);
#ifdef MEDIA_CD
	sceCdMmode(SCECdCD);
#else
	sceCdMmode(SCECdDVD);
#ifdef MEDIA_DVD_DUAL
	sceCdDiskReady(0);
	while(!(sceCdReadDvdDualInfo(&Dualflg,&Layer1Start))){
	    DelayThread(WAIT_16MSEC);
	    scePrintf("sceCdReadDvdDualInfo function fail\n");
	}
	scePrintf("Disk error code= 0x%08x\n",sceCdGetError());
	scePrintf("DvdDual %d\n",Dualflg);
#endif
#endif

        sceCdInitEeCB(1, (void *)cb_stack, CB_STACK_SIZE);

        old_func= sceCdCallback(test_callback);

	scePrintf(" sceCdDiskReady\n");
	sceCdDiskReady(0);

	lpcnt= 0;
	while(1){	
	    if(lpcnt){
	        /* メディア交換 */
	        traycnt= 0;
	        sceCdTrayReq(2,&trayflg);
	        scePrintf("Change your PlayStation CD/DVD-ROM\n");
	        do{
                    /* 他スレッドへの影響を考えリソース返却 */
		    DelayThread(WAIT_16MSEC * 15);

		    while(!sceCdTrayReq(2,&trayflg)){
	                /* 他スレッドへの影響を考えリソース返却 */
		        DelayThread(WAIT_16MSEC);
		    }
		    traycnt+= trayflg;
                    disk_type= sceCdGetDiskType();
	        }while(!traycnt ||
		       (disk_type == SCECdNODISC) ||
		       (disk_type == SCECdDETCT )    );
	        sceCdDiskReady(0);
	    }

	    scePrintf(" sceCdGetDiskType   ");
	    disk_type= sceCdGetDiskType();
	    switch(disk_type){
	        case SCECdIllegalMedia:
		    scePrintf("Disk Type= IllegalMedia\n"); break;
	        case SCECdPS2DVD:
		    scePrintf("Disk Type= PlayStation2 DVD\n"); break;
	        case SCECdPS2CD:
		    scePrintf("Disk Type= PlayStation2 CD\n"); break;
	        case SCECdPS2CDDA:
		    scePrintf("Disk Type= PlayStation2 CD with CDDA\n"); break;
	        case SCECdPSCD:
		    scePrintf("Disk Type= PlayStation CD\n"); break;
	        case SCECdPSCDDA:
		    scePrintf("Disk Type= PlayStation CD with CDDA\n"); break;
	        case SCECdDVDV:
		    scePrintf("Disk Type= DVD video\n"); break;
	        case SCECdCDDA:
		    scePrintf("Disk Type= CD-DA\n"); break;
	        case SCECdDETCT:
		    scePrintf("Working\n"); break;
	        case SCECdNODISC: 
		    scePrintf("Disk Type= No Disc\n"); break;
	        default:
		    scePrintf("Disk Type= OTHER DISK\n"); break;
	    }

#ifdef MEDIA_DVD_DUAL
            sceCdDiskReady(0);
            while(!(sceCdReadDvdDualInfo(&Dualflg,&Layer1Start))){
        	DelayThread(WAIT_16MSEC);
        	scePrintf("sceCdReadDvdDualInfo function fail\n");
            }
       	    scePrintf("Disk error code= 0x%08x\n",sceCdGetError());
            scePrintf("DvdDual %d\n",Dualflg);
#endif

	    sceCdDiskReady(0);
#ifdef MEDIA_CD
	    scePrintf("Get TOC ");
	    sceCdGetToc(toc);

	    cp= (u_char *)toc;
            last_track= (int)btoi(*(cp + 17));
            scePrintf("track max= %d\n",last_track);
            for(cnt0= 0,cp+= 30; cnt0 < last_track; cnt0+= 1){
                scePrintf("track No= %d abs Minute= %d Second= %d Frame= %d\n\n",
                  cnt0 + 1, btoi(*(cp + 7)), btoi(*(cp + 8)), btoi(*(cp + 9)));
                cp+= 10;
            }
#endif

	    /* 標準入出力でファイルを読む */
            rfd = sceOpen("cdrom0:\\SYSTEM.CNF;1",SCE_RDONLY);
            if ( rfd < 0 ){
                scePrintf("Can't open SYSTEM.CNF\n");
		lpcnt++;
                continue;
            }
            readsize = sceLseek(rfd, 0, SCE_SEEK_END);
    	    sceLseek(rfd, 0, SCE_SEEK_SET);
            sceRead(rfd, (u_int *)bf, readsize);
            sceClose(rfd);


	    /* PlayStation実行のファイル名を取り出す。*/
	    cw= (char *)bf;
	    for(cnt0= 0;
	        (*cw != '\n') && (*cw != '\r') && (cnt0 < readsize);
		     cnt0++) cw++;
	    if(cnt0 == readsize){
		 scePrintf("Sorry: File name not Correct\n");
		 lpcnt++;
                 continue;
	    }
	    *cw= 0;
	    cw= (char *)bf;
	    for(cnt0= 0; (*cw != ':') && (cnt0 < readsize); cnt0++) cw++;
	    if(cnt0 == readsize){
		scePrintf("Sorry: File name not Collect\n");
		lpcnt++;
                continue;
	    }
	    cw++; filename= cw;

	    sceCdDiskReady(0);

	    /* ライブラリ関数でファイルを読む */
	    mode.trycount= 0; /* エラー発生時は255リトライ		 */
#ifdef MEDIA_DVD_DUAL
	    mode.spindlctrl= SCECdSpinDvdDL0;
		/* DVD_Dual_Layer_0をアクセスする場合は必ずSCECdSpinDvdDL0を使用する*/
#else
	    mode.spindlctrl= SCECdSpinNom;
		/* エラー発生時は回転速度を落してリード	 */
#endif

	    mode.datapattern= SCECdSecS2048; /* データサイズは2048byte   */
 
	    /* ファイルの格納位置を検索する */
	    scePrintf("Search Filename: %s Layer 0\n",filename);
	    ret= sceCdLayerSearchFile(&fp, filename, 0);
	    if(!ret){
		scePrintf("sceCdLayerSearchFile cmd fail :%d\n",ret);
		lpcnt++;
                continue;
	    }

	    /* 分秒セクタに変換して表示してみる。DVDでは意味が無い */
	    sceCdIntToPos(fp.lsn, &cl);
	    scePrintf("File pos Minute %d Second %d Sector %d\n",
				cl.minute,cl.second,cl.sector);
	    /* ブロックナンバに変換して表示する。 */
	    scePrintf("File pos lbn= %d\n",sceCdPosToInt(&cl));

	    /* ファイルの読み込みセクタ数を算出する */
	    rsec= fp.size / 2048; if(fp.size % 2048) rsec++;
            /* 読み込みバッファの都合により読み込みセクタ数を制限する。*/
	    if(rsec > 1024) rsec= 1024;
	    scePrintf("CD_READ LSN= %d sectors= %d\n", fp.lsn,rsec);
	    sceCdDiskReady(0);

	    /* ファイルの読み込みを開始する */
	    while(!sceCdRead(fp.lsn, rsec, (u_int *)bf, &mode)){
		DelayThread(WAIT_16MSEC);
	        scePrintf("Read cmd fail\n");
	    }
	    scePrintf("ReadSync ");
            for( rs_flg= 1; rs_flg; ){
                rs_flg= sceCdSync(1); 
			/* ファイルの読み込みステータスを得る 0:終了 */
	        scePrintf("Cur Read Position= %d\n",sceCdGetReadPos());
		DelayThread(WAIT_16MSEC);
	    }
	    scePrintf("Disk error code= 0x%08x\n",sceCdGetError());

	    /* シークテスト */
//          scePrintf("CdSeek Test\n");
//          sceCdDiskReady(0);
//          while(!sceCdSeek(fp.lsn)){
//		DelayThread(WAIT_16MSEC);
//		scePrintf("Seek cmd fail\n");
//	    }
//          sceCdSync(0); 

	    /* スタンドバイテスト */
	    scePrintf("CdStandby Test\n");
	    sceCdDiskReady(0);
	    while(!sceCdStandby()){
		DelayThread(WAIT_16MSEC);
 		scePrintf("Standby cmd fail\n");
	    }
            sceCdSync(0); 

	    /* ポーズテスト */
	    scePrintf("CdSPause Test\n");
	    sceCdDiskReady(0);
	    while(!sceCdPause()){
		DelayThread(WAIT_16MSEC);
		scePrintf("Pause cmd fail\n");
	    }
            sceCdSync(0); 

	    /* テストのためディスクを止めてみる */
	    scePrintf("CdStop Test\n");
	    sceCdDiskReady(0);
	    while(!sceCdStop()){
		DelayThread(WAIT_16MSEC);
		scePrintf("Stop cmd fail\n");
	    }
            sceCdSync(0); 

            /* 読み込みファイルをホストに保存する。*/
	    sprintf(wfilename,"host0:data%d.dat",lpcnt);
	    wfd = sceOpen(wfilename, SCE_WRONLY|SCE_TRUNC|SCE_CREAT);
	    if (wfd < 0 ) {
		scePrintf("Can't open file data.dat\n");
		lpcnt++;
                continue;
	    }
	    sceWrite(wfd, (u_int *)bf, 2048 * rsec);
	    sceClose(wfd);

	    lpcnt++;
	}
}

/* start_main()関数スレッドを作成するための関数 */
int main()
{
static struct ThreadParam thp;
int thid;

#ifdef MDLCHG_RESET
	/* IOP側のモジュール入れ換えを行う。*/
	sceSifInitRpc(0);
	while(sceSifRebootIop("host0:/usr/local/sce/iop/modules/"IOP_IMAGE_file) == 0){
	    scePrintf("iop reset error\n");
        }
	while(sceSifSyncIop() == 0);
#endif
        sceSifInitRpc(0);

	thp.stackSize = MAIN_STACK_SIZE;
	thp.gpReg = &_gp;
	thp.entry = start_main;
        thp.stack = (void *)main_stack;
        thp.initPriority = 2;
        thid= CreateThread( &thp );
        StartThread( thid, NULL);

	SleepThread();
	return(0);
}

/* end of file.*/
