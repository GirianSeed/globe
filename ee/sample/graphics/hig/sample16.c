/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
*/
/*
 *
 *	Copyright (C) 2002 Sony Computer Entertainment Inc.
 *							All Right Reserved
 *
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "camera.h"
#include "light.h"

#define	NUM	 2			/* num of Scene */
enum { EMBOSS0, EMBOSS1, EMBOSS2 };	/* PASS ID */

/****************************************/
/*	Micro Codes			*/
/****************************************/
extern u_int sce_micro_vertex_point[];
extern u_int sce_micro_embossbump[];

/****************************************/
/*	Drawport			*/
/****************************************/
static sceHiGsCtx *port[NUM+1];		/* for Emboss,Emboss1,Emboss2 Render */

static void drawport_init(void)
{
    sceHiErr err;

    port[EMBOSS0]=sceHiGsCtxCreate(1);
    if (port[EMBOSS0]==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxCopy(port[EMBOSS0], sceHiGsStdCtx);
    err = sceHiGsCtxSetRegAlpha(port[EMBOSS0], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_ZERO, 128);	/* NONE */
    err = sceHiGsCtxSetRegTest(port[EMBOSS0], SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GREATER);
    err = sceHiGsCtxUpdate(port[EMBOSS0]);

    port[EMBOSS1]=sceHiGsCtxCreate(1);
    if (port[EMBOSS1]==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxCopy(port[EMBOSS1], sceHiGsStdCtx);
    err = sceHiGsCtxSetDefault(port[EMBOSS1]);
    err = sceHiGsCtxSetRegAlpha(port[EMBOSS1], SCE_GS_ALPHA_ZERO,SCE_GS_ALPHA_CS,SCE_GS_ALPHA_FIX,SCE_GS_ALPHA_CD, 0x47);	/* SUB */
    err = sceHiGsCtxSetRegTest(port[EMBOSS1], SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);    
    err = sceHiGsCtxUpdate(port[EMBOSS1]);

    port[EMBOSS2]=sceHiGsCtxCreate(1);
    if (port[EMBOSS2]==NULL) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiGsCtxCopy(port[EMBOSS2], sceHiGsStdCtx);
    err = sceHiGsCtxSetDefault(port[EMBOSS2]);
    err = sceHiGsCtxSetRegAlpha(port[EMBOSS2], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, 0x47);	/* ADD */
    err = sceHiGsCtxSetRegTest(port[EMBOSS2], SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL); 
    err = sceHiGsCtxUpdate(port[EMBOSS2]);
}


static void drawport_exit(void)
{
    sceHiErr	err;
    int		i;

    for(i=0;i<NUM+1;i++){
	err = sceHiGsCtxDelete(port[i]);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
}


static void alpha_ctrl(void)
{
    sceHiErr err;
    static int	base_alpha=128;	/* base alpha */
    static int	bump_alpha=70;	/* bump alpha */

    if (gPad.RleftOn) {
	base_alpha-=2;
	if (base_alpha<=0) base_alpha=0;
    }
    if (gPad.RrightOn) {
	base_alpha+=2;
	if (base_alpha>=255) base_alpha=255;
    }

    if (gPad.RdownOn) {
	bump_alpha-=2;
	if (bump_alpha<=0) bump_alpha=0;
    }
    if (gPad.RupOn) {
	bump_alpha+=2;
	if (bump_alpha>=255) bump_alpha=255;
    }

    err = sceHiGsCtxSetRegAlpha(port[EMBOSS0], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_ZERO, base_alpha);	/* NONE */
    err = sceHiGsCtxUpdate(port[EMBOSS0]);

    err = sceHiGsCtxSetRegAlpha(port[EMBOSS1], SCE_GS_ALPHA_ZERO,SCE_GS_ALPHA_CS,SCE_GS_ALPHA_FIX,SCE_GS_ALPHA_CD, bump_alpha);	/* SUB */
    err = sceHiGsCtxUpdate(port[EMBOSS1]);

    err = sceHiGsCtxSetRegAlpha(port[EMBOSS2], SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, bump_alpha);	/* ADD */
    err = sceHiGsCtxUpdate(port[EMBOSS2]);

    draw_debug_print(1, 4, "MODE : ALPHA CTRL base_alpha_fix: %d, bump_alpha_fix: %d",base_alpha,bump_alpha);
}

/****************************************/
/*	EmobssBump Mapping Sample	*/
/****************************************/
static void		*data_addr[NUM];
static sceHiPlug	*frameP[NUM];
static sceHiPlug	*microP[NUM];
static sceHiPlugMicroData_t *microD[NUM];

void SAMPLE16_init(void)
{
    sceHiErr	err;
    char	filename[NUM][20] = {"data/emboss0.bin","data/emboss1.bin"};
    char	framename[NUM][12] = {"Emboss0","Emboss1"};
    int		i;
    static sceHiType type = {SCE_HIP_COMMON,SCE_HIP_FRAMEWORK,SCE_HIP_MICRO,SCE_HIG_PLUGIN_STATUS,SCE_HIP_MICRO_PLUG,SCE_HIP_REVISION};

    sceHiPlugMicroTbl_t	micro_tbl[NUM+1] = {
	{sce_micro_vertex_point, SCE_HIP_MICRO_ATTR_NONE},		/*	Emboss0		*/
	{sce_micro_vertex_point, SCE_HIP_MICRO_ATTR_NONE},		/*	Emboss1		*/
	{sce_micro_embossbump, SCE_HIP_MICRO_ATTR_NONE},		/*	Emboss2		*/
    };
    sceHiPlugMicroInitArg_t	micro_arg[NUM] = {
	{&micro_tbl[EMBOSS0], 1},{&micro_tbl[EMBOSS1], 2}
    };

    /*	Get Micro Plug & Data	*/
    for(i=0;i<NUM;i++){
	data_addr[i] = file_read(filename[i]);
	err = sceHiParseHeader((u_int *)data_addr[i]);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	err = sceHiGetPlug((u_int *)data_addr[i], framename[i], &frameP[i]);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

	err = sceHiGetInsPlug(frameP[i], &microP[i], type);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	microP[i]->args = (u_int)&micro_arg[i];		/* exec micro id default 0 */

	if((microD[i] = sceHiPlugMicroGetData(microP[i])) == NULL)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    /*	INIT PROCESS	*/
    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_INIT_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    /*	Init Etc	*/
    for(i=0;i<NUM;i++){
	camerainit(microD[i]);
	lightinit(microD[i]);
    }
    drawport_init();
    lgpini();		/* create light gp */
}

int SAMPLE16_main(void)
{
    sceHiErr	err;
    static int	mode = 0;	/* mode select */
    static float	bump_shift=0.05;
    static sceHiPlugMicroPreCalcArg_t       arg;
    int		i;

    if (gPad.SelectSwitch)	mode = (++mode) > 3 ? 0 : mode;

    switch (mode) {
      case 0:
          draw_debug_print(1, 4, "MODE : LIGHT CTRL");
          lightctrl(microD[EMBOSS0],POINT_LIGHT);
	  lightcopy(microD[EMBOSS1],microD[EMBOSS0],POINT_LIGHT);
	  /* light info */
	  draw_debug_print(50, 1, "Rup/Rdown    : Light Up/Down");
	  draw_debug_print(50, 2, "Rleft/Rright : Light Left/Right");
	  draw_debug_print(50, 3, "R1/R2        : Light Back/Front");
	  draw_debug_print(50, 4, "Lup/Ldown    : ITEM CHANGE");
	  draw_debug_print(50, 5, "Lleft/Lright : LIGHT CHANGE");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
	  draw_debug_print(50, 8, "START        : EXIT");	    
          break;
      case 1:
          draw_debug_print(1, 4, "MODE : CAMERA MOVE");
          cameractrl();
	  cameraview(microD[EMBOSS0]);
	  cameracopy(microD[EMBOSS1], microD[EMBOSS0]);
	  /* camera mode info */
	  draw_debug_print(50, 1, "Rup/Rdown    : Cam Up/Down");
	  draw_debug_print(50, 2, "Rleft/Rright : Cam Left/Right");
	  draw_debug_print(50, 3, "R1/R2        : Cam Back/Front");
	  draw_debug_print(50, 4, "Lup/Ldown    : Cam Rot X");
	  draw_debug_print(50, 5, "Lleft/Lright : Cam Rot Y");
	  draw_debug_print(50, 6, "L1/L2        : Cam Rot Z");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
	  draw_debug_print(50, 8, "START        : EXIT");
          break;
      case 2:
	  alpha_ctrl();
	  draw_debug_print(50, 1, "Rleft/Rright : Bump Parameter");
	  draw_debug_print(50, 2, "Rup/Rdown    : Base Parameter");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
	  draw_debug_print(50, 8, "START        : EXIT");
	  break;
      case 3:
	  if (gPad.RleftOn) {
	      bump_shift-=0.001;
	  }
	  if (gPad.RrightOn) {
	      bump_shift+=0.001;
	  }
	  draw_debug_print(1, 4, "MODE : BUMP CTRL %f",bump_shift);
	  draw_debug_print(50, 1, "Rleft/Rright : Bump Parameter");
	  draw_debug_print(50, 7, "SELECT       : MODE CHANGE");	    
	  draw_debug_print(50, 8, "START        : EXIT");
	  break;
    }

    /* set bump paramter */
    microD[EMBOSS1]->shift = bump_shift;

    /* set micro for pass1 */
    arg.micro=0;
    microP[EMBOSS1]->args = (u_int)&arg;

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_PRE_PROCESS);
	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    err = sceHiGsCtxRegist(port[EMBOSS0], 0);
    err = sceHiCallPlug(frameP[EMBOSS0], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    err = sceHiGsCtxRegist(port[EMBOSS1], 0);
    err = sceHiCallPlug(frameP[EMBOSS1], SCE_HIG_POST_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /* change micro for pass 2 */
    arg.micro=1; 
    err = sceHiCallPlug(microP[EMBOSS1], SCE_HIG_PRE_PROCESS); 
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiCallPlug(microP[EMBOSS1], SCE_HIG_POST_PROCESS); 
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);


    err = sceHiGsCtxRegist(port[EMBOSS2], 0);       
    err = sceHiCallPlug(frameP[EMBOSS1], SCE_HIG_POST_PROCESS);      
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    err = sceHiGsCtxRegist(sceHiGsStdCtx, 0);
    if(mode==0) lgpreg();	/* render light gp */

    utilSetDMATimer();
    err = sceHiDMASend();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    err = sceHiDMAWait();	if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if (gPad.StartSwitch)
	return 1;
    return 0;
}


void SAMPLE16_exit(void)
{
    sceHiErr	err;
    int		i;

    for(i=0;i<NUM;i++){
	err = sceHiCallPlug(frameP[i], SCE_HIG_END_PROCESS);
    if(err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }
    for(i=0;i<NUM;i++){
	free(data_addr[i]);
    }

    drawport_exit();
    lgpdel();		/* delete light gp */
}
