/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : sample0.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            1.0     Oct 27, 2000     kazama      Initial
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include <math.h>
#include "util.h"
#include "light.h"
#include "camera.h"

/***************************************************
 * sample program 0
 *
 *     A viewing sample of 1 object by the data which used es2hig
 ***************************************************/

static void	*DATA_ADDR;

void SAMPLE29_init();
int SAMPLE29_main();
void SAMPLE29_exit();

static sceHiPlug	*frameP;
static sceHiPlug        *shapeP;
static sceHiPlugMicroData_t *microD;
static sceHiPlug	*microP;
static sceHiPlug	*tex2dP;

static sceHiPlugTex2dInitArg_t	tex2d_arg;
static sceHiPlugMicroPreCalcArg_t	prearg;

extern u_int sce_micro_clone[];  

static sceHiPlugMicroTbl_t my_micro[] = {
    /* backface culling */
    { sce_micro_clone,    SCE_HIP_MICRO_ATTR_NONE }
    };




#define NUM 30 
 
typedef struct {
    sceVu0FMATRIX       m;      /* world matrix */
    sceHiDMAChainID_t   id;     /* dma chain id */
}matrix_packet_t;
 
static matrix_packet_t mp[NUM] __attribute__((aligned(16)));    /* align qword */
 
/*      Create DMA packet       */
static void mp_init(void)
{
    sceHiErr    err;
    int i;
 
    for(i=0;i<NUM;i++){
        err = sceHiDMAMake_ChainStart();
        err = sceHiDMAMake_LoadPtr((u_int *)68, (u_int *)mp[i].m, 4);   /* vu1 address:16 , size 4 qword */
        err = sceHiDMAMake_ChainEnd(&mp[i].id);
    }
}
 
#define PI      3.14159265f
static void mp_calc(void)
{
    sceVu0FMATRIX m;
    sceVu0FVECTOR t = {0.0f, 0.0f, 0.0f, 0.0f};

    float n;
    int i;
    float a = 2.0f * PI / (NUM-1);         /* angle for trans */
    float r = 10.0f;                    /* radius for trans */
 
    sceVu0UnitMatrix(mp[0].m);
    for(i=1, n=0.0f; i<NUM; i++, n++){

    if (i%2==0)
        t[0] = cosf(a * n+2) * r;
    else  if (i%3==0) t[0] = sinf(a * n+2) * r; 
        else t[0] = sinf(0.6f*a * n) * r;
    if(i==1) t[2]=i*6; else    
        t[2]=i*3;

     sceVu0UnitMatrix(m);
     sceVu0TransMatrix(mp[i].m, m, t);
    }
 
}
 
static void mp_exit(void)
{
    sceHiErr err;
    int i;
 
    for(i=0;i<NUM;i++){
        err = sceHiDMADel_Chain(mp[i].id);
    }
}                                                              

static sceHiGsCtx *port;        

void SAMPLE29_init(void)
{
	sceHiErr	err;
        sceHiPlugMicroInitArg_t arg; 

        /* create ctx for alpha test setting */
        port=sceHiGsCtxCreate(0);
        if(port == NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
        err = sceHiGsCtxSetRegTest(port,SCE_GS_TRUE, SCE_GS_ALPHA_LESS, 0xFF, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GREATER);  
        err = sceHiGsCtxUpdate(port); 

	DATA_ADDR = file_read("data/tree.bin");

	if ((err = sceHiParseHeader((u_int *)DATA_ADDR)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Frame", &frameP)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


        if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Shape", &shapeP)) != SCE_HIG_NO_ERR)
            error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Tex2d", &tex2dP)) == SCE_HIG_NO_ERR) {
	    tex2d_arg.resident = TRUE;
	    tex2d_arg.tbl = NULL;
	    tex2dP->args = (u_int)&tex2d_arg;
	} else
	    tex2dP = NULL;

	if ((err = sceHiGetPlug((u_int *)DATA_ADDR, "Micro", &microP)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
        arg.tbl = my_micro;
        arg.tblnum = 1;
        microP->args = (u_int)&arg;  

        if ((err = sceHiCallPlug(frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
            error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);        
        sceHiPlugShapeMasterChainSetting(shapeP,1); 

        if((microD = sceHiPlugMicroGetData(microP)) == NULL)
        error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);  

        camerainit(microD);
        lightinit(microD);  

	prearg.micro = 0;      
        mp_init();mp_calc();
}

int SAMPLE29_main(void)
{
	sceHiErr	err;
        int i;

        /* camear moving/set */
        cameractrl();
        cameraview(microD);  

	/* micro code change? */
	microP->args = (u_int)&prearg;

        /* set test register for alpha test */
        err = sceHiGsCtxRegist(port, 0); 

        
	if ((err = sceHiCallPlug(frameP, SCE_HIG_PRE_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

        /* original */
        err = sceHiDMARegist(mp[0].id);
	if ((err = sceHiCallPlug(frameP, SCE_HIG_POST_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

        /* clone */
        for(i=1;i<NUM;i++){
          err = sceHiDMARegist(mp[i].id);
          err = sceHiCallPlug(shapeP, SCE_HIG_POST_PROCESS);
        }           


        err = sceHiGsCtxRegist(sceHiGsStdCtx, 0); 

	utilSetDMATimer();
	if ((err = sceHiDMASend()) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	if ((err = sceHiDMAWait()) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


        /* transmit only the first */
        prearg.micro = -1;        

	if (gPad.StartSwitch)
		return 1;
	return 0;
}

void SAMPLE29_exit(void)
{
	sceHiErr	err;

	if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
        mp_exit();
	free(DATA_ADDR);
}
