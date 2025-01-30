/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
*/
/*
 *                      Emotion Engine Library
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         libhip - libhip.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *                      Sep,19,2000     kaneko
 */
/*	$Id: libhip.h,v 1.15 2001/10/29 05:40:55 kaneko Exp $	*/

#ifndef __HIP_H__
#define __HIP_H__
#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************
 *				CONSTANTS
 *********************************************************************************/

/* Micro Code ID for micro plug*/
typedef enum {
    SCE_HIP_MICRO_ATTR_NONE	= 0,
    SCE_HIP_MICRO_ATTR_FGE	= (1 << 0),
    SCE_HIP_MICRO_ATTR_ANTI	= (1 << 1)
} sceHiPlugMicroAttr_t;

/*********************************************************************************
 *				STRUCTURES
 *********************************************************************************/

typedef struct {
    u_int	*micro;
    u_int	attr;
} sceHiPlugMicroTbl_t;

typedef struct {
    sceHiPlugMicroTbl_t	*tbl;
    u_int		tblnum;
} sceHiPlugMicroInitArg_t;

typedef struct {
    int		micro;	/* The microcode which you want to activate */
    float	anticutoff;	/* anti-alias parameter */
    float	fogbegin;     	/* fog begin */
    float	fogend;		/* fog end */
} sceHiPlugMicroPreCalcArg_t;

typedef struct {
    int			resident;	/* for resident */
    sceHiGsMemTbl	*tbl;		/* for tbp & cbp */
}sceHiPlugTex2dInitArg_t;

typedef struct {
    int			resident;	/* for resident */
    sceHiGsMemTbl	*tbl;		/* for tbp & cbp */
}sceHiPlugTim2InitArg_t;

typedef struct {
    u_int	setframe;		/* force frame count */
    int		setframe_enable;	/* enable setframe value */
    u_int	currentframe;		/* return current frame cocunt */
} sceHiPlugAnimePreCalcArg_t;

typedef struct {
    sceVu0FMATRIX *root;
} sceHiPlugHrchyPreCalcArg_t;

/*		ClutBump		*/
typedef struct {
    sceVu0FVECTOR               light_dir;   
    sceVu0FVECTOR		shading;
} sceHiPlugClutBumpPreArg_t;

/*		ShadowMap		*/
typedef struct {
    int			width,height;
    u_int		*box;		/* shadowobj bounding box */
}sceHiPlugShadowMapInitArg_t;

/*		LightMap		*/
typedef struct {
    int			width,height;
    int			fov;
}sceHiPlugLightMapInitArg_t;

/*		FishEye			*/
typedef struct {
    u_int zdepth;
    float rmin;
    float rmax;
} sceHiPlugFishEyeInitArg_t;

typedef struct {
    sceVu0FVECTOR               *camera_pos;
    sceVu0FVECTOR               *camera_zdir;
    sceVu0FVECTOR               *camera_up;
    float tex_size;
} sceHiPlugFishEyePreArg_t;

/*		Reflect			*/
typedef struct {
    sceVu0FVECTOR               *camera_pos;
    sceVu0FVECTOR               *camera_zdir;
    sceVu0FVECTOR               *camera_up;
    float                       zoom;
    float                       z_shift;
} sceHiPlugReflectPreArg_t;

/*		Refract			*/
typedef struct {
    sceVu0FVECTOR               *camera_pos;
    sceVu0FVECTOR               *camera_zdir;
    sceVu0FVECTOR               *camera_up;
    float                       refract_index;
    float                       zoom;
    float                       z_shift;
} sceHiPlugRefractPreArg_t;

/*********************************************************************************
 *				FUNCTIONS
 *********************************************************************************/

/*	extern plugins functions	*/
extern sceHiErr sceHiPlugMicro(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugTex2D(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugTim2 (sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugShape(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugHrchy(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugAnime(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugShare(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugClutBump(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugShadowMap(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugShadowBox(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugLightMap(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugFishEye(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugReflect(sceHiPlug *plug, int process);
extern sceHiErr sceHiPlugRefract(sceHiPlug *plug, int process);

extern size_t sceHiPlugTex2DSize(sceHiPlug *plug);

extern int	sceHiPlugTim2Num(sceHiPlug *plug);
extern char	*sceHiPlugTim2GetName(sceHiPlug *plug, int idx);
extern sceHiErr sceHiPlugTim2SetData(sceHiPlug *plug, int idx, u_int *fdata);
extern sceHiErr sceHiPlugTim2GetNPictures(sceHiPlug *plug, int n, int *num);
extern sceHiErr sceHiPlugTim2SetPicture(sceHiPlug *plug, int n, int num);

extern sceHiErr sceHiPlugShapeInvisible(sceHiPlug *plug, int matidx, int flag);
extern sceHiErr sceHiPlugShapeMasterChainSetting(sceHiPlug *, int flag);
enum { /* the bellow use as flag in sceHiPlugShapeMasterChainSetting() args */
    SCE_HIP_SHAPE_MASTER_CHAIN_IN_STATIC_O = (1 << 0)
};

/*********************************************************************************
 *				MACROS
 *********************************************************************************/
/*	Repository	*/
#define SCE_HIP_COMMON	1

/*	Project		*/
#define	SCE_HIP_FRAMEWORK	1

/*	Category	*/
#define SCE_HIP_MICRO	1
#define SCE_HIP_TEX2D	2
#define SCE_HIP_SHAPE	3
#define SCE_HIP_HRCHY	4
#define SCE_HIP_ANIME	5
#define SCE_HIP_SHARE	6
#define SCE_HIP_CAMERA	7
#define SCE_HIP_LIGHT	8
#define SCE_HIP_TIM2	9
#define SCE_HIP_FRAME	16

#define	SCE_HIP_BUMP	10
#define	SCE_HIP_REFLECT	11
#define SCE_HIP_SHADOW	12

/*	PluginID	*/
#define SCE_HIP_MICRO_PLUG	1
#define SCE_HIP_TEX2D_PLUG	1
#define SCE_HIP_SHAPE_PLUG	1
#define SCE_HIP_HRCHY_PLUG	1
#define SCE_HIP_ANIME_PLUG	1
#define SCE_HIP_SHARE_PLUG	1
#define SCE_HIP_CAMERA_PLUG	1
#define SCE_HIP_TIM2_PLUG	1
#define SCE_HIP_FRAME_PLUG	1

#define	SCE_HIP_CLUTBUMP_PLUG	1
#define SCE_HIP_FISHEYE_PLUG	1
#define SCE_HIP_REFLECT_PLUG	2
#define SCE_HIP_REFRACT_PLUG	3
#define	SCE_HIP_LIGHTMAP_PLUG	2
#define SCE_HIP_SHADOWMAP_PLUG	1
#define	SCE_HIP_SHADOWBOX_PLUG	2

/*	DataID		*/

/*	MICRO		*/
#define SCE_HIP_MICRO_DATA	1

/*	TEX2D		*/
#define SCE_HIP_TEX2D_DATA	1
#define SCE_HIP_TEX2D_ENV	2

/* 	TIM2		*/
#define SCE_HIP_TIM2_DATA	1

/*	SHAPE		*/
#define SCE_HIP_SHAPE_DATA	1
#define SCE_HIP_BASEMATRIX	2

/*	HRCHY		*/
#define SCE_HIP_HRCHY_DATA	1
#define SCE_HIP_PIVOT_DATA	2

/*	ANIME		*/
#define SCE_HIP_ANIME_DATA	1
#define SCE_HIP_KEYFRAME	2
#define SCE_HIP_KEYVALUE	3

/*	SHARE		*/

#define SCE_HIP_SHARE_DATA	1
#define SCE_HIP_SRCDSTVERTEX	2
#define SCE_HIP_SRCDSTNORMAL	3
#define SCE_HIP_VERTEXINDEX	4
#define SCE_HIP_NORMALINDEX	5
#define SCE_HIP_SHAREVERTEX	6
#define SCE_HIP_SHARENORMAL	7

/*	CAMERA		*/
#define SCE_HIP_CAMERA_DATA	1

/*	REVISON		*/
#define SCE_HIP_REVISION	1

/*	CLUTBUMP	*/
#define	SCE_HIP_CLUTBUMP_DATA		1
#define SCE_HIP_CLUTBUMP_NORMAL		2

/*	SHADOWBOX	*/
#define	SCE_HIP_SHADOWBOX_DATA		2

#ifdef __cplusplus
}
#endif
#endif /* !__HIP_H__ */
