/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *               HiG Sample Program
 *                  
 * 
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : def.h
 *
 */
#define	SCE_HIG_VERSION			0x000001
#define SCE_HIG_HEADER_STATUS		0
#define SCE_HIG_PLUGIN_STATUS		0
#define SCE_HIG_DATA_STATUS		0

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

#define SCE_HIP_OBJCLIP 13     
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

#define SCE_HIP_OBJCLIP_PLUG 1 
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

/*  OBJCLIP */
#define SCE_HIP_BBOX_DATA 1      
/*********************************************************************************
 *				MACROS (old)
 *********************************************************************************/
#define	HiG_VERSION		0x000001	/* build 2000/10/01 */

/* default converter variables */
#define HEADER_STATUS		0
#define PLUGIN_STATUS		0
#define DATA_STATUS		0

/*	Repository	*/
#define COMMON	1

/*	Project		*/
#define	FRAMEWORK	1

/*	Category	*/
#define	MICRO	1
#define	TEX2D	2
#define	SHAPE	3
#define	HRCHY	4
#define	ANIME	5
#define	SHARE	6
#define	CAMERA	7
#define	LIGHT	8
#define	FRAME	16

/*	PluginID	*/
#define MICRO_PLUG	1
#define TEX2D_PLUG	1
#define SHAPE_PLUG	1
#define HRCHY_PLUG	1
#define	ANIME_PLUG	1
#define	SHARE_PLUG	1
#define CAMERA_PLUG	1
#define	FRAME_PLUG	1

/*	DataID		*/

/*	MICRO		*/
#define	MICRO_DATA	1

/*	TEX2D		*/
#define	TEX2D_DATA	1
#define TEX2D_ENV	2

/*	SHAPE		*/
#define	SHAPE_DATA	1
#define	BASEMATRIX	2

/*	HRCHY		*/
#define	HRCHY_DATA	1
#define PIVOT_DATA	2

/*	ANIME		*/
#define	ANIME_DATA	1
#define	KEYFRAME	2
#define	KEYVALUE	3

/*	SHARE		*/

#define	SHARE_DATA	1
#define	SRCDSTVERTEX	2
#define	SRCDSTNORMAL	3
#define	VERTEXINDEX	4
#define	NORMALINDEX	5
#define	SHAREVERTEX	6
#define	SHARENORMAL	7

/*	CAMERA		*/
#define	CAMERA_DATA	1


#define REVISION	1
