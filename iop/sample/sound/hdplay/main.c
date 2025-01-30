/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: main.c,v 1.51 2003/05/21 07:22:27 kaol Exp $
 */
/* 
 *              I/O Processor Library Sample Program
 * 
 *                            
 *                          Version 1.0 
 * 
 *       Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 * 
 *                             main.c
 *
 *       Play the BD data with HD information by SdHd fuctions
 */

#include <kernel.h>
#include <stdio.h>
#include <sys/file.h>
#include <sceerrno.h>
#include <stdlib.h>
#include <libsd.h>
#include <sdhd.h>


/* 転送先の SPU2 ローカルメモリ内の先頭アドレス */
#define  SPU2_ADDR	0x15010

/* IOP メモリ → SPU2 ローカルメモリに転送する時の DMA チャンネル */
#define  DMA_CH         0
#define  BASE_priority  60

/* サウンドデータ */
#define  BD_NAME       "host1:/usr/local/sce/data/sound/wave/ivory.bd"
#define  HD_NAME       "host1:/usr/local/sce/data/sound/wave/ivory.hd"

unsigned char *HD_ADDR;         
unsigned char *BD_ADDR;         

/* 割り込みハンドラに渡されるデータ：転送終了フラグ( > 0 転送終了 ) */
volatile int gEndFlag = 0;           

/* 発音パラメータ構造体 */
typedef struct {
    int   core;              /* コア番号             */
    int   voiceNumber;       /* ボイス番号           */
    int   velocity;          /* べロシティ           */
    int   baseNote;          /* ベースノート         */
    int   note;              /* ノート               */
    int   detune;            /* ディチューン         */
    int   ADSR1;             /* ADSR1                */
    int   ADSR2;             /* ADSR2                */
    int   panpot;            /* パンポット           */
    int   vagOffsetAddr;     /* 波形データオフセット */
    int   vagSampleRate;     /* サンプリングレート   */
} KeyOnParam; 

static int core_voice[2];

#define NO_CINDEX    (-1)    /* ノイズ音源使用 */
/* ---------------------------------------------------------------
 * データ読み込み
 * ---------------------------------------------------------------- */
char *set_data (char *FILENAME, int *size)
{
    char *buffer;
    int  oldstat;
    int  fd;
    
    if ((fd = open (FILENAME, O_RDONLY)) < 0){
	printf("Can not open file : %s.\n", FILENAME);
	return NULL;
    }
    
    *size = lseek (fd, 0, SEEK_END);	/* ファイルサイズの取得 */
    printf("FILE SIZE : %d\n", *size);
    if (*size <= 0){
	printf("\nCan't load  file to iop heap\n");
	return NULL;
    }    
    lseek(fd, 0, SEEK_SET);		/* 読み込み位置を先頭に */
   
    CpuSuspendIntr(&oldstat);
    
    buffer = AllocSysMemory(SMEM_Low, *size, NULL); /* データ領域 */
    
    CpuResumeIntr(oldstat);
    
    Kprintf("allocate IOP heap memory - ");    

    if (buffer == NULL){
	printf("Can't alloc heap \n");
	return NULL;
    }
    printf("alloced 0x%x  \n", (int)buffer);
    
    /* ファイルの内容を読み込み */	
    if (read(fd, buffer, *size) != *size) return NULL;
    close(fd);  
    
    return buffer; 
    
}
/* ----------------------------------------------------------------
 * 割り込みハンドラ
 * ---------------------------------------------------------------- */
int IntTrans (int ch, void * common)
{
    int *c =( int *)common;

    (*c)++;
    Kprintf("##### interrupt detected. count: %d CORE ch: %d #####\n", *c, ch);
    Kprintf("##### DMA transfer finished                     #####\n");
    
    return 0;	/* 現在未使用。必ず 0 を返す。 */
}

/* ----------------------------------------------------------------
 * IOPのBDデータをSPU2に転送
 * ---------------------------------------------------------------- */
void spu2_transfer (int size)
{
    int   ret;
       
    /* DMA 転送終了割り込みハンドラを設定 */
    sceSdSetTransIntrHandler(DMA_CH, (sceSdSpu2IntrHandler)IntTrans, 
			      (void *) &gEndFlag);
    gEndFlag = 0;
    /* 転送開始 */
    ret = sceSdVoiceTrans(DMA_CH, SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA,
			   (u_char *)BD_ADDR, SPU2_ADDR, size);
    /* DMA_CB_TEST */
    while (gEndFlag == 0){   
        /* 割り込みハンドラが呼ばれるまで待つ : インターバル: 0.1 秒 */
	DelayThread(100 * 1000);
    } 
    
    return;
}
/* ----------------------------------------------------------------
 * 初期化
 * ---------------------------------------------------------------- */
int initialize (void)
{
    int core, voice;
    
    /* 低レベルサウンドライブラリの初期化 */
    if (sceSdInit(0) != SCE_OK) return -1;

    for (core = 0; core < 2; core++){

	 /* マスターボリュームを オフ */
	sceSdSetParam(core | SD_P_MVOLL, 0);
	sceSdSetParam(core | SD_P_MVOLR, 0);
	
	for (voice = 0; voice < 24; voice++){
	    /* 各ボイスのボリューム、ピッチの初期化 */
	    sceSdSetParam(core | (voice << 1) | SD_VP_VOLL,  0);
	    sceSdSetParam(core | (voice << 1) | SD_VP_VOLR,  0);
	    sceSdSetParam(core | (voice << 1) | SD_VP_PITCH, 0x1000);
	}
	 /* マスターボリュームをセット */
	sceSdSetParam(core | SD_P_MVOLL, 0x3fff);
	sceSdSetParam(core | SD_P_MVOLR, 0x3fff);
    }
  
    return 0;
}

/* ----------------------------------------------------------------
 * panpot からVolume値への計算
 * ---------------------------------------------------------------- */
void set_pan (KeyOnParam *voice)
{      
    int  vol_L, vol_R;
    int  panpot;

    /* パンポットの代入 :  逆相ならば符号反転 */
    if (voice->panpot < 0)panpot = - voice->panpot;
    else panpot = voice->panpot;    
  
    if (panpot == 64){ 
	/* panpot = 64 */
	vol_R = vol_L = (0x3fff * voice->velocity) / 128;
    } else if (panpot > 64){
	/* panpot : 64 --- 127 */
	vol_L = ((0x3fff * (128 - panpot) * voice->velocity) / 64) / 128;
	vol_R = (0x3fff * voice->velocity) / 128;
    } else if (panpot < 64){
	/* panpot : 0  ---  64 */
	vol_L = (0x3fff * voice->velocity) / 128;
	vol_R = ((0x3fff * panpot * voice->velocity) / 64) / 128;
    }
      
    if (voice->panpot < 0){
	/* パンポットが逆相の時、15bit有符号で負の値を代入 */
	vol_L = (- vol_L) & 0x7fff;
	vol_R = (- vol_R) & 0x7fff;
    }
    
    /* ボイスボリューム L/R の設定 */
    sceSdSetParam(voice->core | (voice->voiceNumber << 1) | SD_VP_VOLL, vol_L);
    sceSdSetParam(voice->core | (voice->voiceNumber << 1) | SD_VP_VOLR, vol_R);
    
    return;
}
/* ----------------------------------------------------------------
 * べロシティレンジのクロスフェイドの計算 
 * ---------------------------------------------------------------- */
int cal_velRangeEffect (int velocity, SceSdHdSampleParam *sample)
{
    int value;

    /* べロシティが 0 ならば終了 */
    if(velocity <= 0) return 0;
    value = velocity;

    if (((sample->velRange).low & 0x80) == 0x80){
	/*  べロシティレンジ Low の ビット7 が 1 */
	if (((sample->velRange).low & 0x7f) > velocity){
	    /* べロシティはべロシティレンジの範囲外 */
	    return 0;
	}
	if ((sample->velRange).crossFade > velocity){
	    /* べロシティのクロスフェイド効果量を算出 */
	    value = ((velocity - ((sample->velRange).low & 0x7f))* velocity)/(
		(sample->velRange).crossFade - ((sample->velRange).low & 0x7f));
	}
    }
    if (((sample->velRange).high & 0x80) == 0x80){
	/* べロシティレンジ High の ビット7 が 1 */
	if (((sample->velRange).high & 0x7f) < velocity){
	  /* べロシティはべロシティレンジの範囲外 */
	    return 0;
	}
	if ((sample->velRange).crossFade < velocity){
	    /* べロシティのクロスフェイド効果量を算出 */
	    value = ((((sample->velRange).high & 0x7f) - velocity)* velocity)/
		(((sample->velRange).high & 0x7f) - (sample->velRange).crossFade);
	}
    }
    return  value;
}
/* ----------------------------------------------------------------
 * スプリットのクロスフェイドの計算 
 * ---------------------------------------------------------------- */
int cal_splitRangeEffect (int note, int velocity, SceSdHdSplitBlock *split)
{
    int value;

    /* べロシティが 0 ならば終了 */
    if (velocity <= 0) return 0;
    value = velocity;

    if (((split->range.low) & 0x80) == 0x80){
	/* スプリットレンジ Low の ビット7 が 1 */
	if ((((split->range).low) & 0x7f) > note){
	    /* ノートはスプリットレンジの範囲外 */
	    return 0;
	}
	if ((split->range).crossFade > note){
	    /* ノートによるベロシティのクロスフェイド効果量を算出 */
	    value = (((note - ((split->range).low & 0x7f)) * velocity)/
		     (((split->range).crossFade) - (((split->range).low) & 0x7f)));
	}
    }
   
    if ((((split->range).high) & 0x80) == 0x80){
	/* スプリットレンジ High の ビット7 が 1 */
	if ((((split->range).high) & 0x7f) < note){
	    /* ノートはスプリットレンジの範囲外 */
	    return 0;
	} 
	if ((split->range).crossFade < note){
	    /* ノートによるベロシティのクロスフェイド効果量を算出 */
	    value = (((((split->range).high & 0x7f) - note)* velocity)/
		     (((split->range).high & 0x7f) - (split->range).crossFade));
	}
    }
    return value;
}
/* ----------------------------------------------------------------
 * コア毎のボイスナンバー管理
 * ---------------------------------------------------------------- */
void set_voiceNumber (KeyOnParam *voice, int voicecount)
{
    int i;
    
    for(i = 0; i < voicecount; i++){
	/* 指定したコアのボイスナンバーを割り当てる : 0 - 23 */
	voice[i].voiceNumber = core_voice[voice[i].core]++;
	if(core_voice[voice[i].core] >= 24) core_voice[voice[i].core] = 0;
    }
      
    return;
}
/* ----------------------------------------------------------------
 * 発音処理
 * ---------------------------------------------------------------- */
void  key_on (KeyOnParam *voice, int voicecount)
{
    int voiceBit[2];
    int i;
    int pitch;
    
    /* コア毎のボイスナンバーを計算するボイスビット配列の初期化 */
    voiceBit[0] = voiceBit[1] = 0;
    for(i = 0; i < voicecount; i++){
	/* コア毎に発音するボイスのビットを１にする */
	voiceBit[(voice[i].core)] |=  1 << voice[i].voiceNumber;
	/* パンポットの設定 */
	set_pan(&voice[i]); 

	/* ピッチの設定 */
	pitch = sceSdNote2Pitch(voice[i].baseNote, voice[i].detune, voice[i].note, 0);
	sceSdSetParam(voice[i].core | (voice[i].voiceNumber << 1) | SD_VP_PITCH,      
		       (0x3fff & ((pitch * voice[i].vagSampleRate) / 48000)));
	
        /* ADSRの設定 */
	sceSdSetParam(voice[i].core | (voice[i].voiceNumber << 1) | SD_VP_ADSR1,
		      voice[i].ADSR1);
	sceSdSetParam(voice[i].core | (voice[i].voiceNumber << 1) | SD_VP_ADSR2,
		      voice[i].ADSR2);
        /* _SSA: 波形データの先頭アドレス = ( BD_ADDR + vagi->vagOffsetAddr ) */
	sceSdSetAddr(voice[i].core | (voice[i].voiceNumber << 1) | SD_VA_SSA, 
		     SPU2_ADDR + voice[i].vagOffsetAddr);
    }
    /* 発音 */
    sceSdSetSwitch(SD_CORE_0 | SD_S_KON, voiceBit[0]);
    sceSdSetSwitch(SD_CORE_1 | SD_S_KON, voiceBit[1]);
    
    return;
}
/* ----------------------------------------------------------------
 * 消音処理
 * ---------------------------------------------------------------- */
void  key_off (KeyOnParam *voice, int voicecount)
{
   
	int voiceBit[2];
	int i;

	voiceBit[0] = 0;
	voiceBit[1] = 0;
	
 	for(i = 0; i < voicecount; i++){
	    /* コア毎に発音するボイスのビットを１にする */
	    voiceBit[voice[i].core] |=  1 << voice[i].voiceNumber;
	}
	/* 消音 */
 	sceSdSetSwitch(SD_CORE_0 | SD_S_KOFF, voiceBit[0]);	
 	sceSdSetSwitch(SD_CORE_1 | SD_S_KOFF, voiceBit[1]);
	
	return;
}
/* ----------------------------------------------------------------
 * 情報取得方法 その１
 *  プログラムに登録されている波形を順番に検索し発音する。 
 * プログラム情報から順にHDの階層構造をたどり、最終的に５つの
 * 情報を取得する。この情報を基に波形の発音を行う。    
 * ---------------------------------------------------------------- */
int GetHDdata_case1(void)
{
    int   program, split, sample; /* ループ変数 */
    int   maxnum, ret, sampleNumber;
      
    /*  SdHd */
    SceSdHdProgramParam      programParam;
    SceSdHdSplitBlock        splitBlock;
    SceSdHdSampleSetParam    sampleSetParam; 
    SceSdHdSampleParam       sampleParam;
    SceSdHdVAGInfoParam      vagInfoParam;
    KeyOnParam               voice; 
   
    maxnum = sceSdHdGetMaxProgramNumber(HD_ADDR);
    if (maxnum <= (-1)) return -1;  

    for (program = 0; program <= maxnum; program++){ 
	/* プログラム情報の取得 */
	ret = sceSdHdGetProgramParam(HD_ADDR, program, &programParam);
	if (ret != SCE_OK){
	    printf("Can not GetProgramParam %x.\n", program);
	    continue;
	}
	for (split = 0; split < programParam.nSplit; split++){
	    /* スプリットブロック情報の取得 */
	    ret = sceSdHdGetSplitBlock(HD_ADDR, program, split, &splitBlock);
	    if (ret != SCE_OK){
		printf("Can not GetSplitBlock %x.\n", split);
		continue;
	    }
	    /* サンプルセット情報の取得 */
	    ret = sceSdHdGetSampleSetParam 
		(HD_ADDR, splitBlock.sampleSetIndex, &sampleSetParam);
	    if (ret != SCE_OK){
		printf("Can not GetSampleSetParam %x.\n", 
		       splitBlock.sampleSetIndex);
		continue;
	    }
	    for (sample = 0; sample < sampleSetParam.nSample; sample++){
                /* サンプルセットナンバーとサンプルインデックスナンバーから
		   サンプルナンバーを取得 */
		sampleNumber = sceSdHdGetSampleNumberBySampleIndex
		    (HD_ADDR, splitBlock.sampleSetIndex, sample);
		if (sampleNumber <= (-1)){
		    printf("Can not GetSampleNumber.\n");
		    continue;
                }
                /* サンプル情報の取得 */
		ret = sceSdHdGetSampleParam
		    (HD_ADDR, sampleNumber, &sampleParam);
		if (ret != SCE_OK){
		    printf("Can not GetSampleParam %x.\n", sampleNumber);
		    continue;
		}		
		/* sampleParam.vagIndex が NO_CINDEX( -1): ノイズ音源を使用 */
		if (sampleParam.vagIndex == NO_CINDEX){
		    printf("Sound source is NOISE.\n");
		    continue;
		}
		/* VAGInfo情報の取得 */
		ret = sceSdHdGetVAGInfoParam
		    (HD_ADDR, sampleParam.vagIndex, &vagInfoParam);		
		if (ret != SCE_OK){
		    printf("Can not GetVAGInfoParam %x.\n", 
			   sampleParam.vagIndex);
		    continue;
		}
		
                /* 発音パラメータ構造体に値を設定 */
                /* 発音を行うコア指定 */
		switch (sampleParam.spuAttr & 0xf0){/* spuAttr のコアビット判定 */
		case SCEHD_SPU_CORE_0: /* コア 0 使用 */
		    voice.core = SD_CORE_0;
		    break;
		case SCEHD_SPU_CORE_1: /* コア 1 使用 */
		    voice.core = SD_CORE_1;
		    break;
		default:
		    voice.core = SD_CORE_0;
		    break;
		}
		voice.baseNote       = sampleParam.common.baseNote;
		voice.note           = sampleParam.common.baseNote;
		voice.detune         = sampleParam.common.detune;
		voice.panpot         = sampleParam.common.panpot;
		/* べロシティはサンプルボリュームにプログラムボリュームを加味した値を採用*/
		voice.velocity       = (sampleParam.common.volume * programParam.common.volume) / 128;
		voice.ADSR1          = sampleParam.ADSR.ADSR1;
		voice.ADSR2          = sampleParam.ADSR.ADSR2;
		voice.vagOffsetAddr  = vagInfoParam.vagOffsetAddr;
		voice.vagSampleRate  = vagInfoParam.vagSampleRate;
		printf("Program #%2d, Split #%2d, Sample #%2d ,vagInfo #%2d : offset %5x\n",
		       program, split, sampleNumber, sampleParam.vagIndex, vagInfoParam.vagOffsetAddr);

		/* ボイスナンバーの設定 */
		set_voiceNumber(&voice, 1);
                /* 発音処理 */
		key_on(&voice, 1);
		DelayThread(500 * 1000); /* 0.5秒待つ */
		/* 消音 */
		key_off(&voice, 1);
	    }
	}
    }
    return 0;
}
/* ----------------------------------------------------------------
 * 情報取得方法 その２
 *
 * sceSdHdGetSampleParamByNoteVelocity, sceSdHdGetVAGInfoParamByNoteVelocity 
 * を使った波形情報の取得と取得した波形の発音する例。 
 *
 *  ノートとべロシティとプログラムナンバーから発音のための
 * 最低限の情報を取得し、最も少ない工数で波形情報を取得、発音する。
 *
 *   発音に関する基本情報はサンプル情報とVAGInfo情報の値を採用。
 * ---------------------------------------------------------------- */
int  GetHDdata_case2(void)
{
    int   program, sample, vag, voice; /* ループ変数 */
    int   ret;
    int   note, velocity;
    int   programcount, samplecount, vagcount;
    int   oldstat;
  

    /* SdHd */
    SceSdHdProgramParam      programParam;
    SceSdHdSplitBlock        *splitPtr      = (SceSdHdSplitBlock *)NULL;
    SceSdHdSampleSetParam    *sampleSetPtr  = (SceSdHdSampleSetParam *)NULL; 
    SceSdHdSampleParam       *samplePtr     = (SceSdHdSampleParam *)NULL;
    SceSdHdVAGInfoParam      *vagInfoPtr    = (SceSdHdVAGInfoParam *)NULL;
    KeyOnParam               *voicePtr      = (KeyOnParam *)NULL;
    int                      *programNumber = (int *)NULL;


    /* 有効なプログラムナンバーの数を取得 */
    ret = sceSdHdGetValidProgramNumberCount(HD_ADDR);
    if (ret < 1){
	printf("Can not Find ValidProgramNumber\n");
	 return -1; 
    } else {
	/* 有効なプログラムナンバーを格納する領域を確保 */
	CpuSuspendIntr(&oldstat);
	programNumber = AllocSysMemory
	    (SMEM_Low, sizeof ( int )*ret, NULL);
	CpuResumeIntr(oldstat);                 
	if (programNumber == NULL){
	    printf("Can not allocate!\n"); 
	     return -1;
	}
    }
    /* 有効なプログラムナンバーを個数分取得 */
    programcount = sceSdHdGetValidProgramNumber(HD_ADDR, programNumber);    
    if (programcount < 1){
	printf("Can not get ValidProgramNumber\n");
	 goto FreeParam;
    }
    
    /* 基本情報の組合せから考えられるHD内の各種情報の候補数を取得 */
    /* スプリットブロック情報の候補数を取得 */
    ret = sceSdHdGetMaxSplitBlockCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SplitBlock\n");
	 goto FreeParam;
    } else {
	/* データを格納する領域を確保 */
	CpuSuspendIntr(&oldstat);
	splitPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSplitBlock)*ret, NULL);
	CpuResumeIntr(oldstat);                 
	if (splitPtr == NULL){
	    printf("Can not allocate!\n"); 
	    goto FreeParam;
	}
    }	
    /* サンプルセット情報の候補数を取得 */
    ret = sceSdHdGetMaxSampleSetParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleSetParam\n");
	goto FreeParam;
    } else {
	/* データを格納する領域を確保 */
	CpuSuspendIntr(&oldstat);
	sampleSetPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSampleSetParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (sampleSetPtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }
    /* サンプル情報の候補数を取得 */
    ret = sceSdHdGetMaxSampleParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleParam\n");
	goto FreeParam;
    } else {
	/* データを格納する領域を確保 */
	CpuSuspendIntr(&oldstat);
	samplePtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSampleParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (samplePtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
	/* 発音パラメータ構造体をサンプル情報の候補数分確保 */
	CpuSuspendIntr(&oldstat);
	voicePtr = AllocSysMemory
	    (SMEM_Low, sizeof (KeyOnParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (voicePtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }
    /* VAGInfo情報の候補数を取得 */
    ret = sceSdHdGetMaxVAGInfoParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleSetParam\n");
	goto FreeParam;
    } else {
	/* データを格納する領域を確保 */
	CpuSuspendIntr(&oldstat);
	vagInfoPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdVAGInfoParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (vagInfoPtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }

    /* HD情報の取得 */
    for (program = 0; program < programcount; program++){
	/* コア毎の発音ボイスを管理するボイスナンバーカウンタを初期化 */
	core_voice[0] = core_voice[1] = 0;
	/* プログラム情報の取得 */
	ret = sceSdHdGetProgramParam(HD_ADDR, programNumber[program], &programParam);
	if (ret != SCE_OK){
	    printf("Can not GetProgramParam %x.\n", programNumber[program]);
	    continue;
	}
	for (note = 0; note < 128; note++){ /* ノートレンジ：0 - 127 */
	    for (velocity = 5; velocity < 128; velocity += 60){ /* ベロシティレンジ：0 - 127 (0 = Key off) */
		/* ボイスカウンタ（発音する波形の数をカウント）の初期化 */
		voice = 0;
		/* プログラムナンバー、ノート、べロシティに該当する
		   サンプル情報を取得 */
		samplecount = sceSdHdGetSampleParamByNoteVelocity
		    (HD_ADDR, programNumber[program], note,
		     velocity, SCESDHD_IGNORE, samplePtr);
		
		/* ナンバー、べロシティに該当するVAGInfo情報を取得 */
		vagcount = sceSdHdGetVAGInfoParamByNoteVelocity
		    (HD_ADDR, programNumber[program], note, velocity, 
		     SCESDHD_IGNORE, vagInfoPtr);
		
		/* 発音パラメータ構造体に値を設定 */
		if (vagcount > 0){
		    for (vag = 0, sample = 0; sample < samplecount; sample++){
		
			/* サンプル情報のsampleParam.vagIndex が NO_CINDEX(ノイズ音源を使用)
			   ならば、サンプル情報の配列を進ませる */
			if (samplePtr[sample].vagIndex == NO_CINDEX){
			    printf("Sound source is NOISE.\n");
			    continue;
			}		
			/* 発音を行うコア指定 */
			switch (samplePtr[sample].spuAttr & 0xf0){
			    /* spuAttr のコアビット判定 */
			 case SCEHD_SPU_CORE_0: /* コア 0 使用 */
			     voicePtr[voice].core = SD_CORE_0;
			     break;
			case SCEHD_SPU_CORE_1: /* コア 1 使用 */
			    voicePtr[voice].core = SD_CORE_1;
			    break;
			default:
			    voicePtr[voice].core = SD_CORE_0;
			    break;
			}
			/* べロシティはサンプルボリュームとプログラムボリュームを加味 */
			voicePtr[voice].velocity      = 
			    (velocity * samplePtr[sample].common.volume * programParam.common.volume)/(128 * 128);
			voicePtr[voice].baseNote      = samplePtr[sample].common.baseNote;
			voicePtr[voice].note          = note;
			/* ディチューンはサンプル情報の値を採用 */
			voicePtr[voice].detune        = samplePtr[sample].common.detune; 
			/* パンポットは サンプル情報の値を採用 */
			voicePtr[voice].panpot        = samplePtr[sample].common.panpot; 
			voicePtr[voice].ADSR1         = samplePtr[sample].ADSR.ADSR1; 
			voicePtr[voice].ADSR2         = samplePtr[sample].ADSR.ADSR2; 
			voicePtr[voice].vagOffsetAddr = vagInfoPtr[vag].vagOffsetAddr;
			voicePtr[voice].vagSampleRate = vagInfoPtr[vag].vagSampleRate;
			vag++;
			voice++;
		    }
		    
		    /* 発音波形の数が > 0 ならば 発音 */
		    if (voice >0){
			printf("Program #%3d note %3d velocity %3d  : ", 
			       programNumber[program], note, velocity);
			printf("%2d Sample(s) / %2d vag(s)\n", samplecount, voice);

			/* ボイスナンバーの設定 */
			set_voiceNumber(voicePtr, voice);
			/* 発音 */			    
			key_on(voicePtr, voice);
			DelayThread(400 * 1000); /* 0.4秒待つ */
			/* 消音 */			    
			key_off(voicePtr, voice);
		    }
		}  
	    }
	}
    }

  FreeParam:
    /* 確保した情報領域の解放 */
    if (programNumber !=( int *) NULL) { 
	FreeSysMemory(programNumber);
    }
    if (splitPtr !=(SceSdHdSplitBlock *) NULL) { 
	FreeSysMemory(splitPtr);
    }
    if (sampleSetPtr !=(SceSdHdSampleSetParam *) NULL) { 
	FreeSysMemory(sampleSetPtr);
    }
    if (samplePtr !=(SceSdHdSampleParam *) NULL) { 
	FreeSysMemory(samplePtr);
    }
    if (vagInfoPtr !=(SceSdHdVAGInfoParam *) NULL) { 
	FreeSysMemory(vagInfoPtr);
    }


    return 0;
}
/* ----------------------------------------------------------------
 * 情報取得方法 その３
 *
 *   sceSdHdGetSplitBlockByNote, sceSdHdGetSampleSetParamByNote
 *   sceSdHdGetSampleParamByVelocity, sceSdHdGetVAGInfoParamByVelocity
 * を使って波形情報を取得。取得した情報を活用しノートとボリューム
 * のクロスフェイドの効果を計算して発音する例。
 *
 *   ノートとプログラムナンバーから、スプリットブロック情報と
 * サンプルセット情報。スプリットブロック情報内のサンプルセットナンバーへの
 * インデックスとべロシティから、サンプル情報とVAGInfo情報の取得。
 *   スプリットブロック情報、サンプル情報に内のノートとボリューム
 * のレンジからクロスフェイドの効果を計算し、発音するボリュームを設定。
 *
 *   その他の発音に関する基本情報はサンプル情報とVAGInfo情報の値を採用した。
 * ---------------------------------------------------------------- */
int GetHDdata_case3(void)
{
    int   program, split, sample, vag, voice; /* ループ変数 */
    int   ret;
    int   note, velocity;
    int   programcount, splitcount, samplesetcount, samplecount, vagcount;
    int   oldstat;
  
    
    /* SdHd */
    SceSdHdProgramParam      programParam;
    SceSdHdSplitBlock        *splitPtr      = (SceSdHdSplitBlock *)NULL;
    SceSdHdSampleSetParam    *sampleSetPtr  = (SceSdHdSampleSetParam *)NULL; 
    SceSdHdSampleParam       *samplePtr     = (SceSdHdSampleParam *)NULL;
    SceSdHdVAGInfoParam      *vagInfoPtr    = (SceSdHdVAGInfoParam *)NULL;
    KeyOnParam               *voicePtr      = (KeyOnParam *)NULL;
    int                      *programNumber = (int *)NULL;

    /* 有効なプログラムナンバーの数を取得 */
    ret = sceSdHdGetValidProgramNumberCount(HD_ADDR);
    if (ret < 1){
	printf("Can not Find ValidProgramNumber\n");
	return -1; 
    } else {
	/* 有効なプログラムナンバーを格納する領域を確保 */
	CpuSuspendIntr(&oldstat);
	programNumber = AllocSysMemory
	    (SMEM_Low, sizeof ( int )*ret, NULL);
	CpuResumeIntr(oldstat);                 
	if (programNumber == NULL){
	    printf("Can not allocate!\n"); 
	    goto FreeParam;
	}
    }
    /* 有効なプログラムナンバーを個数分取得 */
    programcount = sceSdHdGetValidProgramNumber(HD_ADDR, programNumber);    
    if (programcount < 1){
	printf("Can not get ValidProgramNumber\n");
	goto FreeParam;
    }
    
    /* 基本情報の組合せから考えられるHD内の各種情報の候補数を取得 */
    /* スプリットブロック情報の候補数を取得 */
    ret = sceSdHdGetMaxSplitBlockCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SplitBlock\n");
	goto FreeParam;
    } else {
	/* データを格納する領域を確保 */
	CpuSuspendIntr(&oldstat);
	splitPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSplitBlock)*ret, NULL);
	CpuResumeIntr(oldstat);                 
	if (splitPtr == NULL){
	    printf("Can not allocate!\n"); 
	    goto FreeParam;
	}
    }	
    /* サンプルセット情報の候補数を取得 */
    ret = sceSdHdGetMaxSampleSetParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleSetParam\n");
	goto FreeParam;
    } else {
	/* データを格納する領域を確保 */
	CpuSuspendIntr(&oldstat);
	sampleSetPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSampleSetParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (sampleSetPtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }
    /* サンプル情報の候補数を取得 */
    ret = sceSdHdGetMaxSampleParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleParam\n");
	goto FreeParam;
    } else {
	/* データを格納する領域を確保 */
	CpuSuspendIntr(&oldstat);
	samplePtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdSampleParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (samplePtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
	/* 発音パラメータ構造体をサンプル情報の候補数分確保 */
	CpuSuspendIntr(&oldstat);
	voicePtr = AllocSysMemory
	    (SMEM_Low, sizeof (KeyOnParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (voicePtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }
    /* VAGInfo情報の候補数を取得 */
    ret = sceSdHdGetMaxVAGInfoParamCount(HD_ADDR);
    if (ret < 1){
	printf("Can not count SampleSetParam\n");
	goto FreeParam;
    } else {
	/* データを格納する領域を確保 */
	CpuSuspendIntr(&oldstat);
	vagInfoPtr = AllocSysMemory
	    (SMEM_Low, sizeof (SceSdHdVAGInfoParam)*ret, NULL);
	CpuResumeIntr(oldstat); 
	if (vagInfoPtr == NULL){
	    printf("Can not allocate!\n");
	    goto FreeParam;
	}
    }

    /* HD情報取得 */
    for (program = 0; program < programcount; program++){  
	/* コア毎の発音ボイスを管理するボイスナンバーカウンタを初期化 */
	core_voice[0] = core_voice[1] = 0;
	ret = sceSdHdGetProgramParam(HD_ADDR, programNumber[program], &programParam);
	if (ret != SCE_OK){
	    printf("Can not GetProgramParam %x.\n", program);
	    continue;
	}
	for (note = 0; note < 128; note++){/* ノートレンジ：0 - 127 */
	    /* プログラムナンバー、ノートに該当するスプリットブロック情報を取得 */
	    splitcount = sceSdHdGetSplitBlockByNote
		(HD_ADDR, programNumber[program], note, splitPtr);
	    /* プログラムナンバー、ノートに該当する
	       サンプルセット情報を取得 */
	    samplesetcount = sceSdHdGetSampleSetParamByNote
		(HD_ADDR, programNumber[program], note, sampleSetPtr);
	    /* ベロシティレンジ：0 - 127 (0 = Key off) */
	    for (velocity = 5; velocity < 128; velocity += 60){
		/* ボイスカウンタ（発音する波形の数をカウント）の初期化 */
		voice = 0;
		for (split = 0; split < splitcount; split++){
		    /* サンプルセットナンバー、べロシティに該当する
		       サンプル情報を取得 */
		    samplecount = sceSdHdGetSampleParamByVelocity
			(HD_ADDR, splitPtr[split].sampleSetIndex, velocity, 
			 SCESDHD_IGNORE, samplePtr);
		    /* サンプルセットナンバー、べロシティに該当する
		       VAGInfo情報を取得 */
		    vagcount = sceSdHdGetVAGInfoParamByVelocity
			(HD_ADDR, splitPtr[split].sampleSetIndex, velocity, 
			 SCESDHD_IGNORE, vagInfoPtr);
		 
		    /* 発音パラメータ構造体に値を設定 */
		    if (vagcount > 0){
			for (vag = 0, sample = 0; sample < samplecount; sample++){
			  
			    /* サンプル情報のsampleParam.vagIndex が NO_CINDEX(ノイズ音源を使用)
			       ならば、サンプル情報の配列を進ませる */
			    if (samplePtr[sample].vagIndex == NO_CINDEX){
				printf("%d sampleParam data Sound source is NOISE.\n", sample);
				continue;
			    }
			    
			    /* 発音を行うコア指定 */
			    switch (samplePtr[sample].spuAttr & 0xf0){
				/* spuAttr のコアビット判定 */
			    case SCEHD_SPU_CORE_0: /* コア 0 使用 */
				voicePtr[voice].core = SD_CORE_0;
				break;
			    case SCEHD_SPU_CORE_1: /* コア 1 使用 */
				voicePtr[voice].core = SD_CORE_1;
				break;
			    default:
				voicePtr[voice].core = SD_CORE_0;
				break;
			    }

			    /* ノートとべロシティのレンジに対するクロスフェイド効果の計算 */

			    /* べロシティレンジのクロスフェイドの計算 */
			    ret = cal_velRangeEffect(velocity, &samplePtr[sample]);

			    if (ret <= 0) continue;
			    /* べロシティはサンプルボリュームとプログラムボリュームを加味 */
			    voicePtr[voice].velocity = 
				(ret * samplePtr[sample].common.volume * programParam.common.volume)/(128 * 128);
			    /* スプリットのクロスフェイドの計算 */
			    ret = cal_splitRangeEffect(note, velocity, &splitPtr[split]);
			    if (ret <= 0) continue;
                             /* べロシティはサンプルボリュームとプログラムボリュームを加味 */
			     voicePtr[voice].velocity = 
				(ret * samplePtr[sample].common.volume * programParam.common.volume)/(128 * 128);

				
			
			    voicePtr[voice].baseNote      = samplePtr[sample].common.baseNote;
			    voicePtr[voice].note          = note;
			    /* ディチューンはサンプル情報の値を採用 */
			    voicePtr[voice].detune        = samplePtr[sample].common.detune; 
                            /* パンポットは サンプル情報の値を採用 */
			    voicePtr[voice].panpot        = samplePtr[sample].common.panpot; 
			    voicePtr[voice].ADSR1         = samplePtr[sample].ADSR.ADSR1; 
			    voicePtr[voice].ADSR2         = samplePtr[sample].ADSR.ADSR2; 
			    voicePtr[voice].vagOffsetAddr = vagInfoPtr[vag].vagOffsetAddr;
			    voicePtr[voice].vagSampleRate = vagInfoPtr[vag].vagSampleRate;
			    vag++;
			    voice++;
			}
		    }
		}
		/* 発音波形の数が > 0 ならば 発音 */
		if (voice > 0){
		    printf("Program #%3d note %3d velocity %3d  : ",
			   programNumber[program], note, velocity);
		    printf("%2d Split(s) / %2d vag(s)\n", splitcount, voice);

		    /* ボイスナンバーの設定 */
		    set_voiceNumber(voicePtr, voice);
		    /* 発音 */			    
		    key_on(voicePtr, voice);
		    DelayThread(400 * 1000); /* 0.4秒待つ */
		    /* 消音 */			    
		    key_off(voicePtr, voice);
		}
	    }
	}
    }

  FreeParam:
    /* 確保した情報領域の解放 */
    if (programNumber !=( int *) NULL) { 
	FreeSysMemory(programNumber);
    }
    if (splitPtr !=(SceSdHdSplitBlock *) NULL) { 
	FreeSysMemory(splitPtr);
    }
    if (sampleSetPtr !=(SceSdHdSampleSetParam *) NULL) { 
	FreeSysMemory(sampleSetPtr);
    }
    if (samplePtr !=(SceSdHdSampleParam *) NULL) { 
	FreeSysMemory(samplePtr);
    }
    if (vagInfoPtr !=(SceSdHdVAGInfoParam *) NULL) { 
	FreeSysMemory(vagInfoPtr);
    }

    return 0;
}
/* ----------------------------------------------------------------
 * メイン処理
 * ---------------------------------------------------------------- */
int sound_start (void)
{
    int   hd_size, bd_size;

    printf ("sound_start...\n");

    /* ファイルの内容を IOP メモリ内に読み込み */
    if ((HD_ADDR = set_data (HD_NAME, &hd_size)) == NULL) return -1;
    printf("IOP load : HD_ADDR %p ", HD_ADDR);
    printf(".... HD data SIZE %d\n", hd_size);
    if ((BD_ADDR = set_data (BD_NAME, &bd_size)) == NULL) goto finalize;

    printf("IOP load : BD_ADDR %p ", BD_ADDR);
    printf(".... BD data SIZE %d\n", bd_size);

    /* 初期化 */
    if (initialize() != 0){
	printf("Can not Initialize!\n");
	goto finalize;
    }  

    /* IOPのBDデータをSPU2に転送 */
    spu2_transfer(bd_size);
      
    /* sdhd を使った HD情報の取得 */
    if (GetHDdata_case1() != 0) goto finalize; /* その１ */  
    if (GetHDdata_case2() != 0) goto finalize; /* その２ */
    if (GetHDdata_case3() != 0) goto finalize; /* その３ */

  finalize:
    if (HD_ADDR !=( unsigned char *) NULL) { 
	FreeSysMemory(HD_ADDR);
    }
    if (BD_ADDR !=( unsigned char *) NULL) { 
	FreeSysMemory(BD_ADDR);
    }
    printf("sound_start completed...\n");
     
    return 0;
    
}
/* ----------------------------------------------------------------
 * スタート関数
 * ---------------------------------------------------------------- */
int start (int argc, char *argv [])
{
    struct ThreadParam     param;
    int                    thid;
    int                    ret;
  
    /* Create thread */
    param.attr         = TH_C;
    param.entry        = sound_start;
    param.initPriority = BASE_priority;
    param.stackSize    = 0x800;
    param.option       = 0;

    thid = CreateThread(&param);
  
    printf("thid ID %x\n", thid);
  
    if (thid > 0){
	printf("Start Thread\n");
   
	ret = StartThread(thid, 0);	/* スレッドの起動 */
   
	printf(".....Fine!\n");
    
	return RESIDENT_END;	/* program is resident */
    
    } else {
	printf("Program is deleted\n");
    
	return NO_RESIDENT_END;	/* program is deleted */
    }
}
/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
