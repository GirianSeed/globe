/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                     Emotion Engine Library
 *                          Version 1.2.0
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                        sqsoft - main.c
 *                   EE Software Synthesizer Sample
 *
 *     Version   Date          Design   Log
 *  --------------------------------------------------------------------
 *     1.2.0     Nov.29.1999   morita   use LoadModule service
 *     0.6.0     Oct.12.1999   morita   first checked in.
 */

#include <eekernel.h>
#include <eeregs.h>
#include <sifdev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sif.h>
#include <libgraph.h>
#include <csl.h>
#include <cslpcm.h>
#include <liblout.h>
#include <libssyn.h>
#include <sifrpc.h>
#include <libsdr.h>
#include <sdrcmd.h>

#define ERR_STOP while(1)
#define SSB_FILE "host0:../../../../data/sound/ssyn/sce.ssb"
#define PAD_SIZE(x) ((64-((x)&(64-1)))&(64-1))
#define N_STREAM 2

//SSynth���P��ɏ�������ő又���P�ʁB�t���[�����[�g�Ɉˑ��B
#define N_LO_UNIT 4
#define LO_BUFF_SIZE (N_LO_UNIT*sceLoutInputUnit) 

static struct {
    sceCslPcmStream	hdr;
    char		data [LO_BUFF_SIZE];
    char		pad [PAD_SIZE(sizeof(sceCslPcmStream)+LO_BUFF_SIZE)];
} pcmBf[N_STREAM] __attribute__ ((aligned(64)));

static sceCslCtx loCtx;
static sceCslBuffGrp loBuffGrp [1];
static sceCslBuffCtx loBuffCtx [N_STREAM];
static struct {
#define N_DMA_BUF	4
    sceLoutConf		conf;
    sceSifDmaData	dma[N_DMA_BUF];
} loConf = {
    sceLoutDmaPreWait,
    {0, 1,  0,  1},
    (LO_BUFF_SIZE * 2) * 2,
    {NULL, NULL},
    N_DMA_BUF
};


#define N_VOICES 32
#define RPC_BUF_SIZE (16*50)

static sceCslCtx ssCtx;
static sceCslBuffGrp ssBuffGrp [2];
static sceCslBuffCtx ssBuffCtx0;
static char voiceBuf [N_VOICES*sceSSynVoiceBufferSize] __attribute__ ((aligned(4)));
static char inBuf [sceSSynInputBufferSize] __attribute__ ((aligned(4)));

static sceSSynConf ssConf = {
    sceLoutInputUnit/sizeof(short),
    48000,
    N_VOICES,
    voiceBuf,	sizeof (voiceBuf),
    1,
    inBuf,	sizeof(inBuf)
};

static sceSSynEnv ssEnv __attribute__ ((aligned(64)));

struct {
    char d [RPC_BUF_SIZE];
    char pad [PAD_SIZE(RPC_BUF_SIZE)];
} ssRpcBuf __attribute__ ((aligned(64)));


/*------------------------------------------
  �g�p���郂�W���[����IOP���ɓǂݍ���ŋN��
  ------------------------------------------*/
void
load_modules (void)
{
    sceSifInitRpc (0);

    // libsd�̓T�E���h�����ɂ͕K�{
    while (sceSifLoadModule("host0:../../../../iop/modules/libsd.irx",0, NULL) < 0){
	scePrintf("loading libsd.irx failed\n");
    }
    while (sceSifLoadModule("host0:../../../../iop/modules/sdrdrv.irx",0, NULL) < 0){
	scePrintf("loading sdrdrv.irx failed\n");
    }
    while (sceSifLoadModule("host0:../../../../iop/modules/modmidi.irx",0, NULL) < 0){
	scePrintf("loading modmidi.irx failed\n");
    }
    while (sceSifLoadModule("host0:../../../../iop/modules/modssyn.irx",0, NULL) < 0){
	scePrintf("loading modssyn.irx failed\n");
    }

    return;
}

/*-------------------
  ���f�f�[�^��ǂ�
  -------------------*/
static void *
loadSSB (const char *file)
{
    int sfd, flen;
    void *p;
    while ((sfd = sceOpen (file, SCE_RDONLY)) < 0) {
	perror (file);
    }
    while ((flen = sceLseek (sfd, 0, SCE_SEEK_END)) < 0) {
	perror (file); sceClose (sfd);
    }
    while (sceLseek (sfd, 0, SCE_SEEK_SET) < 0) {
	perror (file); sceClose (sfd);
    }
    scePrintf ("file length = %d\n", flen);
    while ((p = (void *)malloc (flen)) == NULL) {
	perror ("malloc"); sceClose (sfd);
    }
    while (sceRead (sfd, p, flen) != flen) {
	perror (file); sceClose(sfd); free(p);
    }
    sceClose (sfd);
    if (sceSSyn_PrepareParameter (p, flen) != sceSSynNoError) {
	scePrintf("sceSSyn_PrepareParameter Error\n");
	ERR_STOP;
    }
    scePrintf ("Data = %x\n", (unsigned int)p);

    return p;
}

/*-------------------------------
  LineOut���W���[���̃Z�b�g�A�b�v

  -------------------------------*/
/* 
    ��LineOut���W���[���̎d�l��
    ���݂̂Ƃ���A���̓o�b�t�@��0�Ԃ��E�A1�Ԃ����Ƃ��āASPU2���̓u���b�N��
    �`�����l���P�ɏo�͂��܂��B
    ���ݒ�p�o�b�t�@�͂���܂���B
    ����͎b��d�l�ŁA���o�[�W�����ł͕ύX����܂��B
*/
int
setLineOut(void)
{
    int i;

    // allocate IOP Buffer
    loConf.conf.iopBuf[0] = NULL;
    loConf.conf.iopBuf[1] = (void *)sceSifAllocSysMemory (0, loConf.conf.iopBufSize, NULL);
    if (loConf.conf.iopBuf[1] == NULL) {
	scePrintf("sceSifAllocSysMemory Error\n"); return 1;
    }

    // initialize buffer context
    loCtx.extmod = NULL;
    loCtx.callBack = NULL;
    loCtx.conf = &loConf;
    loCtx.buffGrpNum = 1;
    loCtx.buffGrp = loBuffGrp;
      loBuffGrp [0].buffNum = N_STREAM;
        /* �ʏ�A���W���[���Ԃ̃f�[�^���L��buffer�P�ʂōs�Ȃ����A
	   SoftSynth��LineOut��BuffCtx�̍\�����S�������Ȃ̂ŁABuffCtx
	   �܂邲�Ƌ��L���Ă���B*/
      loBuffGrp [0].buffCtx = loBuffCtx;
    for (i = 0; i < N_STREAM; i++) {
	loBuffCtx [i].sema = 0;
	loBuffCtx [i].buff = &(pcmBf [i].hdr);
	if (PAD_SIZE ((unsigned int)loBuffCtx [i].buff)) {
	    scePrintf ("port %d need %d bytes pad.\n", i,
		    PAD_SIZE((unsigned int)loBuffCtx [i].buff));
	    return 1;
	}
	pcmBf [i].hdr.pcmbuf_size = LO_BUFF_SIZE;
	pcmBf [i].hdr.validsize   = 0;
	pcmBf [i].hdr.pcmbuf      = pcmBf [i].data;
    }

    // EE����������SPU2�ւ�PCM�]���J�n
    if (sceLout_Init (&loCtx, (1000*1000)/60) != sceLoutNoError) {
	scePrintf("sceLout_Init Error\n"); return 1;
    }

    return 0;
}

/*---------------------------------
  SoftSynth���W���[���̃Z�b�g�A�b�v
  ---------------------------------*/
int
setSoftSynth (void)
{
    int i, k;
    void *param;

    // initialize buffer context
    ssCtx.extmod = NULL;
    ssCtx.callBack = NULL;
    ssCtx.conf = &ssConf; //test
    ssCtx.buffGrpNum = 2;
    ssCtx.buffGrp = ssBuffGrp;
      ssBuffGrp [0].buffNum = 1;
      ssBuffGrp [0].buffCtx =  &ssBuffCtx0;
        ssBuffCtx0.sema = 0;
        ssBuffCtx0.buff = &ssEnv;
          ssEnv.input_buff_len = RPC_BUF_SIZE;
	  ssEnv.input_buff = ssRpcBuf.d;
	  ssEnv.tone_param = NULL;

      ssBuffGrp [1].buffNum = N_STREAM;
      ssBuffGrp [1].buffCtx = loBuffCtx;  // LineOut�Ƌ��L

    if (sceSSyn_Init (&ssCtx, 0) != sceSSynNoError) {
	scePrintf("sceSSyn_Init Error\n"); return 1;
    }

    // ���f�f�[�^�t�@�C���ǂ݂���
    param = loadSSB (SSB_FILE); 

    // ���f�f�[�^�Z�b�g
    if (sceSSyn_Load (&ssCtx, 0, param) != sceSSynNoError) {
	free (param);
	scePrintf("sceSSyn_Load Error\n"); return 1;
    }

    for (i = 0; i < 16; i++) {
	sceSSynChOutAttrib oatt;
	for (k = 0; k < sceSynthesizerChOutNum; k++) {
	    oatt.ch = i;
	    oatt.ch_output = k;
	    oatt.mode = (k & 1) ? sceSSynROut : sceSSynLOut;
	    oatt.output_line = k;
	    oatt.att = SSYN_VOLUME_0DB;
	    sceSSyn_SetOutputAssign(&ssCtx, 0, &oatt);
	}
    }

    return 0;
}

/*-------------------
  main
  -------------------*/
int
main (int argc, char **argv)
{
    char argstring[64] = "host0:/usr/local/sce/data/sound/seq/sample.sq";

    load_modules ();

    // initialize hadrware
    sceSdRemoteInit ();
    sceSdRemote (1, rSdInit , SD_INIT_COLD);

    sceSdRemote (1, rSdSetParam, 1 | SD_P_BVOLL, 0x3fff);
    sceSdRemote (1, rSdSetParam, 1 | SD_P_BVOLR, 0x3fff);

    sceSifInitIopHeap ();

    // initialize modules
    if (setLineOut ()   != 0) return 1;
    if (setSoftSynth () != 0) return 1;

    // communicate IOP modules
    ChangeThreadPriority (GetThreadId (), 20);
    if (sceSSyn_RegisterRpc (&ssCtx, 10) != sceSSynNoError) {
	scePrintf("sceSSyn_RegisterRpc Error\n"); return 1;
    }

    scePrintf("Ready to run IOP module.......\n");
    // �����ŁAEE����IOP������̃f�[�^�҂��ɂȂ�

    // IOP�����W���[�������[�h
    while (sceSifLoadModule ("host0:../../../../iop/sample/sound/sqsoft/main.irx",
			     sizeof(argstring), argstring ) < 0){
	scePrintf ("loading ezbgm.irx failed\n"); 
    }
    scePrintf ("main.irx loaded\n");

    // main loop
    for (;;) {
	if (sceSSyn_ATick (&ssCtx) != sceSSynNoError) {
	    scePrintf("sceSSyn_ATick Error\n"); return 1;
	}
	if (sceLout_ATick (&loCtx) != sceLoutNoError) {
	    scePrintf("sceLout_ATick Error\n"); return 1;
	}
	sceGsSyncV (0);
    }

    return 0;
}
/* $Id: main.c,v 1.18 2003/04/30 17:17:12 kaol Exp $ */

