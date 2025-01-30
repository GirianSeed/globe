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

/***************************************************
 * sample program 8
 *
 *	using HiG data-access service functions
 ***************************************************/

static char *data_file[]={
    "data/APEWALK39.bin", 
    "data/dino.bin"
};

#define MIN_SPEED (0.1f)
#define MAX_SPEED (10.0f)
#define SPEED_CHANGE_RATE (1.05f)

#define MAX_MATERIAL 16
#define MAX_ANIMATION 24

/* object position */
static sceVu0FVECTOR root_rot0={0.0f, 0.0f, 0.0f, 0.0f};
static sceVu0FVECTOR root_trans0={-5.0f, 13.0f, 0.0f, 0.0f};

static sceVu0FVECTOR root_rot1={0.0f, -0.7f, 0.0f, 0.0f};
static sceVu0FVECTOR root_trans1={3.0f, 5.0f, 0.0f, 0.0f};


/* for HiG data handling */
typedef struct {
    void 	*addr;
    sceHiPlug	*frameP;
    sceHiPlugMicroData_t	*microD;
    int  	tim2datanum;
    u_int  	**tim2data;
    sceVu0FMATRIX root_mat;
    sceHiPlugHrchyPreCalcArg_t hrchy_arg;
    sceHiPlugAnimePreCalcArg_t anime_arg;
} DATAHDL;

static DATAHDL object[2];

typedef struct {
    int shape;
    int material;
    int tmestatus;
    sceHiPlugShapeHead_t *attribute;
} MATERIAL_STATUS;

static MATERIAL_STATUS mst[MAX_MATERIAL];

static int mnum;

typedef struct {
    sceHiPlugAnimeHead_t *keyhead;
    int keynum;
    u_int *keyframe;
    u_int *keyframe_org;
    float speed;
} ANIMATION_STATUS;

static ANIMATION_STATUS ast[MAX_ANIMATION];
static int anum;

static sceHiPlugTex2dInitArg_t		tex2d_arg;
static sceHiPlugTim2InitArg_t		tim2_arg;
static sceHiPlugMicroPreCalcArg_t	micro_arg;


static int get_animations(sceHiPlug *frame, ANIMATION_STATUS *pAst);
static int get_materials(sceHiPlug *frame, MATERIAL_STATUS *pMst);
static void and_prim(sceHiPlugShapeHead_t *mh, int prim);
static void or_prim(sceHiPlugShapeHead_t *mh, int prim);
static void init_model(char *file, DATAHDL *obj);
static void anime_ctrl();
static void exit_model(sceHiPlug *frameP);
static void finish_model(DATAHDL *obj);
static void turn_off_tme(MATERIAL_STATUS *pmst);
static void turn_on_tme(MATERIAL_STATUS *pmst);
static int attrib_ctrl(void);
static void obj_setpos(DATAHDL *obj);
static void obj_setanimearg(DATAHDL *obj);



static int get_animations(sceHiPlug *frame, ANIMATION_STATUS *pAst)
{
    sceHiPlugAnimeHead_t *head;
    sceHiPlug *anime_plug;
    int num;
    int i;

    sceHiType type1={
	SCE_HIP_COMMON,				/* repository */
	SCE_HIP_FRAMEWORK,			/* project */
	SCE_HIP_ANIME,				/* category */
	SCE_HIG_PLUGIN_STATUS,			/* status */
	SCE_HIP_ANIME_PLUG,			/* id */
	SCE_HIP_REVISION			/* revision */
    };
    sceHiType type2={
	SCE_HIP_COMMON,				/* repository */
	SCE_HIP_FRAMEWORK,			/* project */
	SCE_HIP_ANIME,				/* category */
	SCE_HIG_DATA_STATUS,			/* status */
	SCE_HIP_KEYFRAME,			/* id */
	SCE_HIP_REVISION			/* revision */
    };

    if (sceHiGetInsPlug(frame, &anime_plug, type1) != SCE_HIG_NO_ERR){
	printf("ANIME_PLUG not found\n");
	return 0;
    }

    head=sceHiPlugAnimeGetHead(anime_plug, type2);

    if (head==NULL) return -1;
    

    num=0;
    for (i=0; i<head->top.num; i++){
	pAst[num].keyhead=sceHiPlugAnimeGetKeyHead(head, i);
	if (pAst[num].keyhead==NULL) return -1;
	pAst[num].keynum=pAst[num].keyhead->key.num;
	pAst[num].keyframe=sceHiPlugAnimeGetFrame(pAst[num].keyhead, 0);

	num++;
	if (num==MAX_ANIMATION){
	    printf("too many animation data has been found\n");
	    return num;
	}
    }
    return num;
}


static int get_materials(sceHiPlug *frame, MATERIAL_STATUS *pMst)
{
    sceHiPlugShapeHead_t *head, *sh, *mh;
    sceHiPlug *shape_plug;
    int s;
    int m;
    int num;
    sceHiType type1={			/* SHAPE_PLUG */
	SCE_HIP_COMMON,				/* repository */
	SCE_HIP_FRAMEWORK,			/* project */
	SCE_HIP_SHAPE,				/* category */
	SCE_HIG_PLUGIN_STATUS,			/* status */
	SCE_HIP_SHAPE_PLUG,			/* id */
	SCE_HIP_REVISION			/* revision */
    };
    sceHiType type2={			/* SHAPE_DATA */
	SCE_HIP_COMMON,				/* repository */
	SCE_HIP_FRAMEWORK,			/* project */
	SCE_HIP_SHAPE,				/* category */
	SCE_HIG_PLUGIN_STATUS,			/* status */
	SCE_HIP_SHAPE_DATA,			/* id */
	SCE_HIP_REVISION			/* revision */
    };

    num=0;
    if (sceHiGetInsPlug(frame, &shape_plug, type1) != SCE_HIG_NO_ERR) return -1;

    head=sceHiPlugShapeGetHead(shape_plug, type2);
    if (head==NULL) return -1;


    for (s=0; s<head->top.num; s++){
	sh=sceHiPlugShapeGetDataHead(head, s);
	if (sh==NULL) return -1;

	for (m=0; m<sh->dat.num; m++){
	    mh=sceHiPlugShapeGetMaterialHead(sh, m);
	    if (mh==NULL) return -1;
	    pMst[num].shape=s;
	    pMst[num].material=m;
	    pMst[num].attribute=mh;
	    num++;
	    if (num==MAX_MATERIAL){
		printf("find too many materials");
		return num;
	    }
	}
    }
    return num;
}




static void and_prim(sceHiPlugShapeHead_t *mh, int prim)
{
    int i;
    sceHiPlugShapeHead_t *gh;

    for (i=0; i<mh->mat.num; i++){
	gh=sceHiPlugShapeGetGeometryHead(mh, i);
	if (gh==NULL) error_handling( __FILE__, __LINE__, "geometry scan failed");
	gh->geo.prim&=prim;
    }

}

static void or_prim(sceHiPlugShapeHead_t *mh, int prim)
{
    int i;
    sceHiPlugShapeHead_t *gh;

    for (i=0; i<mh->mat.num; i++){
	gh=sceHiPlugShapeGetGeometryHead(mh, i);
	if (gh==NULL) error_handling( __FILE__, __LINE__, "geometry scan failed");
	gh->geo.prim|=prim;
    }

}

static void rottransmat(sceVu0FMATRIX mat, sceVu0FVECTOR rot, sceVu0FVECTOR trans)
{
    sceVu0UnitMatrix(mat);
    sceVu0RotMatrixX(mat, mat, rot[0]);
    sceVu0RotMatrixY(mat, mat, rot[1]);
    sceVu0RotMatrixZ(mat, mat, rot[2]);
    sceVu0TransMatrix(mat, mat, trans);
}

static void init_model(char *file, DATAHDL *obj)
{
    int j;
    sceHiErr	err;
    sceHiType	type;
    char *name;
    sceHiPlug	*tim2P;
    sceHiPlug	*microP;
    sceHiPlug	*tex2dP;

    if (file){
	obj->addr = file_read(file);

	if ((err = sceHiParseHeader((u_int *)obj->addr)) != SCE_HIG_NO_ERR)
	    error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    if ((err = sceHiGetPlug((u_int *)obj->addr, "Frame", &obj->frameP)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    tex2d_arg.resident = TRUE;
    tex2d_arg.tbl = NULL;
    if(sceHiGetPlug((u_int *)obj->addr, "Tex2d", &tex2dP) == SCE_HIG_NO_ERR) {
	tex2dP->args = (u_int)&tex2d_arg;
    }

    type.repository = SCE_HIP_COMMON;
    type.project = SCE_HIP_FRAMEWORK;
    type.revision = SCE_HIP_REVISION;
    type.category = SCE_HIP_TIM2;
    type.id = SCE_HIP_TIM2_PLUG; 
    type.status = SCE_HIG_PLUGIN_STATUS;
    obj->tim2data=NULL;
    if (sceHiGetInsPlug(obj->frameP, &tim2P, type) == SCE_HIG_NO_ERR){
	tim2P->args = (u_int)&tim2_arg;

	obj->tim2datanum = sceHiPlugTim2Num(tim2P);
	obj->tim2data= malloc(obj->tim2datanum*sizeof(u_int *));
	if (obj->tim2data==NULL){
	    tim2P=NULL;
	    error_handling(__FILE__, __LINE__, "malloc error");
	}
	for (j=0; j < obj->tim2datanum; j++) {
	    name=sceHiPlugTim2GetName(tim2P, j);
	    obj->tim2data[j]=file_read(name);
	    if (obj->tim2data[j]==NULL){
		error_handling(__FILE__, __LINE__, "texture file not found");
	    }
	    sceHiPlugTim2SetData(tim2P, j, obj->tim2data[j]);
	}
    }

    if ((err = sceHiCallPlug(obj->frameP, SCE_HIG_INIT_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    /*	camera & light	*/
    type.category = SCE_HIP_MICRO;
    type.id = SCE_HIP_MICRO_PLUG;
    if (sceHiGetInsPlug(obj->frameP, &microP, type) != SCE_HIG_NO_ERR) 
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    if((obj->microD = sceHiPlugMicroGetData(microP)) == NULL)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    camerainit(obj->microD);
    lightinit(obj->microD);

    micro_arg.micro = 0;
    microP->args = (u_int)&micro_arg;
}

static void copy_animations(ANIMATION_STATUS *pAst, int num)
{
    int i,j;

    for (i=0; i<num; i++){

	pAst[i].keyframe_org=(u_int *)malloc(pAst[i].keynum*sizeof(u_int));
	if (pAst[i].keyframe_org==NULL) error_handling(__FILE__, __LINE__, "malloc failed");
	pAst[i].speed=1.0f;
	for (j=0; j<pAst[i].keynum; j++){
	    pAst[i].keyframe_org[j]=pAst[i].keyframe[j];
	}
    }
}

static void tempo_animations(ANIMATION_STATUS *pAst)
{
    int j;

    for (j=0; j<pAst->keynum; j++){
	pAst->keyframe[j]=(u_int)(pAst->keyframe_org[j]/pAst->speed + 0.5f);
	
	if (pAst->keyframe[j-1]==pAst->keyframe[j])
	    pAst->keyframe[j]=pAst->keyframe[j-1] + 1;
    }
}


static void anime_ctrl()
{
    static int a= -1;
    int i;
    int tempo_changed=0;

    if (gPad.RdownSwitch){
	if (a<anum-1) a++;
    }
    if (gPad.RupSwitch){
	if (a>=0) a--;
    }

    if (gPad.RrightOn){
	tempo_changed=1;

	if (a>=0){
	    ast[a].speed*=SPEED_CHANGE_RATE;
	    if (ast[a].speed>MAX_SPEED) ast[a].speed=MAX_SPEED;
	    tempo_animations(&ast[a]);
	} else{
	    for (i=0; i<anum; i++){
		ast[i].speed*=SPEED_CHANGE_RATE;
		if (ast[i].speed>MAX_SPEED) ast[i].speed=MAX_SPEED;
		tempo_animations(&ast[i]);
	    }
	}
    }
    if (gPad.RleftOn){
	tempo_changed=1;

	if (a>=0){
	    ast[a].speed/=SPEED_CHANGE_RATE;
	    if (ast[a].speed<MIN_SPEED) ast[a].speed=MIN_SPEED;
	    tempo_animations(&ast[a]);
	} else{
	    for (i=0; i<anum; i++){
		ast[i].speed/=SPEED_CHANGE_RATE;
		if (ast[i].speed<MIN_SPEED) ast[i].speed=MIN_SPEED;
		tempo_animations(&ast[i]);
	    }
	}
    }

    if (tempo_changed) object[0].anime_arg.setframe_enable=1;
    else object[0].anime_arg.setframe_enable=0;

    draw_debug_print(61, 1, "anim|speed");
    draw_debug_print(61, 2, " (all)");

    for (i=0; i<anum && i<24; i++){
	draw_debug_print(62, 3+i, "%2d |%4.1f ", i, ast[i].speed);
    }
    draw_debug_print(47, 3+a, "[R left/right]>");

}


static void exit_model(sceHiPlug *frameP)
{
    sceHiErr	err;

    if ((err = sceHiCallPlug(frameP, SCE_HIG_END_PROCESS)) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
}

static void finish_model(DATAHDL *obj)
{    
    int i;

    exit_model(obj->frameP);


    if (obj->tim2data){
	for (i=0; i<obj->tim2datanum; i++){
	    free(obj->tim2data[i]);
	}
	free(obj->tim2data);
	obj->tim2data=NULL;
    }

    if (obj->addr){
	free(obj->addr);
	obj->addr=NULL;
    }

}


static void turn_off_tme(MATERIAL_STATUS *pmst)
{
    and_prim(pmst->attribute, ~(1<<GS_PRIM_TME_O));
    pmst->tmestatus=0;
}

static void turn_on_tme(MATERIAL_STATUS *pmst)
{
    or_prim(pmst->attribute, (1<<GS_PRIM_TME_O));
    pmst->tmestatus=1;
}


static int attrib_ctrl(void)
{
    static int m= -1;
    int data_change=0;
    int i;

    if (m<0){				/* first time  */
	m=0;

	turn_off_tme(&mst[m]);			/* for demonstration */
	data_change=1;
	return data_change;
    }

    if (gPad.LdownSwitch){
	if (m<mnum-1) m++;
    }
    if (gPad.LupSwitch){
	if (m>0) m--;

    }

    if (gPad.LrightSwitch){
	turn_off_tme(&mst[m]);
	data_change=1;				/* needs restart frameP  */
    } else if (gPad.LleftSwitch){
	turn_on_tme(&mst[m]);
	data_change=1;				/* needs restart frameP  */
    }

    draw_debug_print(1, 5, "  s  m | TME");

    for (i=0; i<mnum; i++){
	draw_debug_print(2, 6+i, "%2d %2d : ", mst[i].shape, mst[i].material);
	if (mst[i].tmestatus) draw_debug_print(10, 6+i, "ON");
	else draw_debug_print(10, 6+i, "OFF");
    }
    draw_debug_print(1, 6+m, ">");
    draw_debug_print(15, 6+m, "[L left/right]");

    return data_change;

}

static void obj_setpos(DATAHDL *obj)
{
    sceHiPlug *hrchyP;
    sceHiType type={
	SCE_HIP_COMMON,				/* repository */
	SCE_HIP_FRAMEWORK,			/* project */
	SCE_HIP_HRCHY,				/* category */
	SCE_HIG_PLUGIN_STATUS,			/* status */
	SCE_HIP_HRCHY_PLUG,			/* id */
	SCE_HIP_REVISION			/* revision */
    };

    /* get hrchy-plugin */
    if (sceHiGetInsPlug(obj->frameP, &hrchyP, type) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    obj->hrchy_arg.root= &obj->root_mat;
    hrchyP->args = (u_int)&obj->hrchy_arg;
}


static void obj_setanimearg(DATAHDL *obj)
{
    sceHiPlug *animeP;
    sceHiType type={
	SCE_HIP_COMMON,				/* repository */
	SCE_HIP_FRAMEWORK,			/* project */
	SCE_HIP_ANIME,				/* category */
	SCE_HIG_PLUGIN_STATUS,			/* status */
	SCE_HIP_ANIME_PLUG,			/* id */
	SCE_HIP_REVISION			/* revision */
    };

    /* get anime-plugin  */
    if (sceHiGetInsPlug(obj->frameP, &animeP, type) != SCE_HIG_NO_ERR)
	error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);

    obj->anime_arg.setframe=0;
    obj->anime_arg.setframe_enable=0;
    animeP->args = (u_int)&obj->anime_arg;
}



void SAMPLE8_init(void)
{
    int i;

    tim2_arg.resident = TRUE;

    rottransmat(object[0].root_mat, root_rot0, root_trans0);
    rottransmat(object[1].root_mat, root_rot1, root_trans1);

    /* HiG data read and initialiaze */
    for (i=0; i<2; i++){
	init_model(data_file[i], &object[i]);
	obj_setpos( &object[i]);
    }

    /* for material data access */
    mnum=get_materials(object[1].frameP, mst);
    if (mnum<0) error_handling(__FILE__, __LINE__, "unexpected error");

    for (i=0; i<mnum; i++){
	mst[i].tmestatus=1;	/* this sample data, every geom has texture */
    }

    /* for animation data access */
    obj_setanimearg(&object[0]);
    anum=get_animations(object[0].frameP, ast);
    if (anum<0) error_handling(__FILE__, __LINE__, "unexpected error");
    copy_animations(ast, anum);
}

int SAMPLE8_main(void)
{
    sceHiErr	err;
    int i;

    if (attrib_ctrl()){
	exit_model(object[1].frameP);
	init_model(NULL, &object[1]);
	obj_setpos(&object[1]);
    }

    anime_ctrl();

    for (i=0; i<2; i++){
	err = sceHiCallPlug(object[i].frameP, SCE_HIG_PRE_PROCESS);
	if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
	    
	err = sceHiCallPlug(object[i].frameP, SCE_HIG_POST_PROCESS);
	if (err != SCE_HIG_NO_ERR) error_handling(__FILE__,__LINE__,sceHiGetErrStatePtr()->mes);
    }

    utilSetDMATimer();
    sceHiDMASend();
    sceHiDMAWait();


    if (gPad.StartSwitch)
	return 1;

    micro_arg.micro = -1;

    return 0;
}



void SAMPLE8_exit(void)
{
    int i;

    for (i=0; i<2; i++){
	finish_model(&object[i]);
    }


    for (i=0; i<anum; i++){
	free(ast[i].keyframe_org);
	ast[i].keyframe_org=NULL;
    }
}
