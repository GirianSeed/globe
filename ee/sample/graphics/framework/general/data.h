/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 *                  
 *              ---- Scene Viewer Version 0.40 ----
 * 
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : data.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000  
 */

#ifndef _DATA_H_
#define _DATA_H_

/*******************************************************
 * type declarations
 *******************************************************/

/*
 * fwDataHead_t
 *	es2rawで吐かれたデータのヘッダ部分のデータの構造を
 *	示します。
 *	各データの構造自体は es2rawのドキュメントを参照してください
 */
typedef struct {
    /* SHAPE */
    u_int	*shape_data;
    u_int	*basematrix;

    /* TEX2D */
    u_int	*tex2d_data;

    /* HRCHY */
    u_int	*hrchy_data;

    /* MICRO */
    u_int	*micro_data;

    /* ANIME */
    u_int	*anime_data;
    u_int	*keyframe;
    u_int	*keyvalue;

    /* SHARE */
    u_int	*share_data;
    u_int	*srcdstvertex;
    u_int	*srcdstnormal;
    u_int	*vertexindex;
    u_int	*normalindex;
    u_int	*sharevertex;
    u_int	*sharenormal;
} fwDataHead_t;

#endif /* _DATA_H_ */
