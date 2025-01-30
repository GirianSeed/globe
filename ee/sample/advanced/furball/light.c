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
#include <malloc.h>
#include <math.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libgp.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"
#include "light.h"

/************************************************/
/*	light gp init/regist/delete		*/
/************************************************/
static sceHiDMAChainID_t id=0;
static sceGpPrimR	*lgp=NULL;	/* light gp packet */
#define NLINE	4

void lgpini(void)
{
    const u_int type = SCE_GP_PRIM_R | SCE_GP_LINE_G;

    /*	libgp packet	*/
    lgp = memalign(16, sizeof(u_long128) * sceGpChkPacketSize(type, NLINE));
    sceGpInitPacket(lgp, type, NLINE);
/*    sceGpSetAa1(lgp, 1);*/
    sceGpSetAbe(lgp, 1);

    /*	packet in hidma	*/
    sceHiDMAMake_ChainStart();
    sceHiDMAMake_WaitMicro();
    sceHiDMAMake_LoadGS((u_int*)(&lgp->giftag1), lgp->dmanext.qwc);
    sceHiDMAMake_ChainEnd(&id);
}

void lgpreg(void)
{
    sceHiDMARegist(id);
}

void lgpdel(void)
{
    free(lgp);
    sceHiDMADel_Chain(id);
    lgp=NULL;
    id=0;
}

/************************************************/
/*	light gp data set			*/
/************************************************/
static sceVu0FVECTOR lgpv[NLINE*2] = {		/* vertices */
    {0.0f,0.0f,0.0f,1.0f},{1.0f,0.0f,1.0f,1.0f},
    {0.0f,0.0f,0.0f,1.0f},{-1.0f,0.0f,1.0f,1.0f},
    {0.0f,0.0f,0.0f,1.0f},{0.0f,1.0f,1.0f,1.0f},
    {0.0f,0.0f,0.0f,1.0f},{0.0f,-1.0f,1.0f,1.0f},
};

static sceVu0IVECTOR lgpc[NLINE*2] = {		/* colors */
    {255,255,255,128},{255,255,255,0},
    {255,255,255,128},{255,255,255,0},
    {255,255,255,128},{255,255,255,0},
    {255,255,255,128},{255,255,255,0},
};

static void lgpcal(sceVu0FMATRIX m, sceVu0FVECTOR dir, sceVu0FVECTOR pos, sceVu0FVECTOR col)
{
    sceVu0FVECTOR yd = {0.0f, 1.0f, 0.0f, 1.0f};
    sceVu0FVECTOR xd,zd;
    float angle;
    int i;
    sceVu0FVECTOR c;

    /*	direction	*/
    sceVu0CopyVector(zd, dir);
    sceVu0OuterProduct(xd, yd, zd);
    sceVu0OuterProduct(yd, zd, xd);
    sceVu0UnitMatrix(m);
    sceVu0Normalize(m[0],xd);
    sceVu0Normalize(m[1],yd);
    sceVu0Normalize(m[2],zd);

    /*	position	*/
    sceVu0TransMatrix(m, m, pos);

    /*	spot angle	*/
    angle = acosf(sqrtf(pos[3]));
    lgpv[1][0] = tanf(angle);
    lgpv[3][0] = -tanf(angle);
    lgpv[5][1] = tanf(angle);
    lgpv[7][1] = -tanf(angle);

    /*	color	*/
    for(i=0;i<NLINE;i++){
	sceVu0ScaleVectorXYZ(c, col, 255.0f);
	c[3]=128.0f;
	sceVu0FTOI0Vector(lgpc[i*2], c);
	c[3]=0.0f;
	sceVu0FTOI0Vector(lgpc[i*2+1],c);
    }
}
/*
  md:	micro data
  dir:	light direction
  pos:	light position
  col:	light color
  lt:	light type
  ln:	light number
*/
static void lgpset(sceHiPlugMicroData_t *md, sceVu0FVECTOR dir, sceVu0FVECTOR pos, sceVu0FVECTOR col, int lt, int ln)
{
    int index;
    int i;
    sceVu0IVECTOR v;
    sceVu0FMATRIX m;

    lgpcal(m, dir, pos, col);
    sceVu0MulMatrix(m, md->wscreen, m);

    for(i=0;i<NLINE;i++){
	sceVu0RotTransPers(v, m, lgpv[i*2], 1);
	index = sceGpIndexXyzfLineG(i*2);
	sceGpSetXyz(lgp, index, v[0], v[1], v[2]);

	sceVu0RotTransPers(v, m, lgpv[i*2+1], 1);
	index = sceGpIndexXyzfLineG(i*2+1);
	sceGpSetXyz(lgp, index, v[0], v[1], v[2]);

	index = sceGpIndexRgbaLineG(i*2);
	sceGpSetRgba(lgp, index, lgpc[i*2][0], lgpc[i*2][1], lgpc[i*2][2], lgpc[i*2][3]);
	index = sceGpIndexRgbaLineG(i*2+1);
	sceGpSetRgba(lgp, index, lgpc[i*2+1][0], lgpc[i*2+1][1], lgpc[i*2+1][2], lgpc[i*2+1][3]);
    }
}

/************************************************/
/*	light load/store/copy			*/
/************************************************/
/*
  md:	micro data with light parameter
  dir:	light direction
  pos:	light position
  col:	light color
  lt:	light type
  ln:	light number
*/

/*	md -> dir,pos,col	*/
void lightload(sceHiPlugMicroData_t *md, sceVu0FVECTOR dir, sceVu0FVECTOR pos, sceVu0FVECTOR col, int lt, int ln)
{
    dir[0] = -md->light[lt].dir[0][ln];
    dir[1] = -md->light[lt].dir[1][ln];
    dir[2] = -md->light[lt].dir[2][ln];
    dir[3] =  md->light[lt].dir[3][ln];
    sceVu0CopyVector(pos, md->light[lt].pos[ln]);
    sceVu0CopyVector(col, md->light[lt].col[ln]);
}

/*	md <- dir,pos,col	*/
void lightstore(sceHiPlugMicroData_t *md, sceVu0FVECTOR dir, sceVu0FVECTOR pos, sceVu0FVECTOR col, int lt, int ln)
{
    sceVu0Normalize(dir,dir);
    md->light[lt].dir[0][ln] = -dir[0];
    md->light[lt].dir[1][ln] = -dir[1];
    md->light[lt].dir[2][ln] = -dir[2];
    md->light[lt].dir[3][ln] =  dir[3];
    sceVu0CopyVector(md->light[lt].pos[ln], pos);
    sceVu0CopyVector(md->light[lt].col[ln], col);
}

/*	dst <- src	*/
void lightcopy(sceHiPlugMicroData_t *dst, sceHiPlugMicroData_t *src, int lt)
{
    sceVu0FVECTOR dir,pos,col;
    int i;

    for(i=0;i<3;i++){
	lightload(src, dir,pos,col,lt,i);
	lightstore(dst, dir,pos,col,lt,i);
    }
}

/************************************************/
/*	constant values				*/
/************************************************/
#define DELTA01		0.1f
#define DELTA001	0.01f
enum {DIR, POS, COL, MAX};	/* 3 light parameter */

/************************************************/
/*	light ctrl				*/
/************************************************/
void lightctrl(sceHiPlugMicroData_t *md, int lt)
{
    static const char *pname[] = {"DIRECTION","POS/ANGLE","COL/INTENS"};	/* param name */
    static u_int ln = 0;	/* light number < MAX */
    static u_int pn = 0;	/* param number < MAX */
    sceVu0FVECTOR dir,pos,col;
    float intensity;    
    /*	inc & dec light number	*/
    if(gPad.LleftSwitch)	ln--;
    if(gPad.LrightSwitch)	ln++;
    ln = ln >= MAX ? 0 : ln;

    /*	inc & dec param number	*/
    if(gPad.LupSwitch)		pn--;
    if(gPad.LdownSwitch)	pn++;
    pn = pn >= MAX ? 0 : pn;

    /* from micro data */
    lightload(md, dir, pos, col, lt, ln);

    if (lt==SPOT_LIGHT) intensity=col[3]*(1.0f-pos[3]);
        else intensity=col[3];      
    
    /*	change light parameters	*/
    switch(pn){
      case DIR:
	  /* direction */
	  if(gPad.RleftOn)	dir[0] -= DELTA001;
	  if(gPad.RrightOn)	dir[0] += DELTA001;
	  if(gPad.RupOn)	dir[1] -= DELTA001;
	  if(gPad.RdownOn)	dir[1] += DELTA001;
	  if(gPad.R1On)		dir[2] += DELTA001;
	  if(gPad.R2On)		dir[2] -= DELTA001;
	  sceVu0Normalize(dir, dir);
	  break;

      case POS:
	  /* position */
	  if(gPad.RleftOn)	pos[0] -= DELTA01;
	  if(gPad.RrightOn)	pos[0] += DELTA01;
	  if(gPad.RupOn)	pos[1] -= DELTA01;
	  if(gPad.RdownOn)	pos[1] += DELTA01;
	  if(gPad.R1On)		pos[2] += DELTA01;
	  if(gPad.R2On)		pos[2] -= DELTA01;

	  /* spotangle */
	  if(gPad.L1On)	      	pos[3] += DELTA001;
	  if(gPad.L2On)		pos[3] -= DELTA001;

	  pos[3]=pos[3]>0.99f ? 0.99f : pos[3];
	  pos[3]=pos[3]<0.01f ? 0.01f : pos[3];
          if (lt==SPOT_LIGHT) col[3]=intensity/(1.0f-pos[3]);  

	  break;

      case COL:
	  /* color */
	  if(gPad.RleftOn)	col[0] -= DELTA001;
	  if(gPad.RrightOn)	col[0] += DELTA001;
	  if(gPad.RupOn)	col[1] -= DELTA001;
	  if(gPad.RdownOn)	col[1] += DELTA001;
	  if(gPad.R1On)		col[2] += DELTA001;
	  if(gPad.R2On)		col[2] -= DELTA001;

	  col[0]=col[0]>1.0f ? 1.0f : col[0];
	  col[0]=col[0]<0.0f ? 0.0f : col[0];
	  col[1]=col[1]>1.0f ? 1.0f : col[1];
	  col[1]=col[1]<0.0f ? 0.0f : col[1];
	  col[2]=col[2]>1.0f ? 1.0f : col[2];
	  col[2]=col[2]<0.0f ? 0.0f : col[2];

	  /* intensity */
	  if(gPad.L1On)		col[3] += DELTA01;
	  if(gPad.L2On)		col[3] -= DELTA01;

	  col[3]=col[3]<0.0f ? 0.0f : col[3];
	  break;
    }

    /* to micro data */
    lightstore(md, dir, pos, col, lt, ln);

    /* when there is light gp, set parameter & transform */
    if(lgp != NULL) lgpset(md, dir, pos, col, lt, ln);

    /*	print prameters	*/
    draw_debug_print(1,  9, "LIGHT %d", ln);
    draw_debug_print(1, 10, "PARAM %s", pname[pn]);
    draw_debug_print(1, 11, "DIR %5.2f %5.2f %5.2f", dir[0],dir[1],dir[2]);
    draw_debug_print(1, 12, "POS %5.2f %5.2f %5.2f", pos[0],pos[1],pos[2]);
    draw_debug_print(1, 13, "COL %5.2f %5.2f %5.2f", col[0],col[1],col[2]);
    draw_debug_print(1, 14, "ANGLE %5.2f",pos[3]);
    draw_debug_print(1, 15, "INTEN %5.2f",intensity);
}

/*
  default light parameter
*/
void lightinit(sceHiPlugMicroData_t *md)
{
    int i,j;
    static sceVu0FVECTOR dir[4]={{0.86f, 0.00f, 0.51f, 0.0f},{-0.56f, 0.62f, 0.55f, 0.0f},{-0.86f,0.0f, 0.51f, 0.0f},{0.0f, 0.0f, 0.0f, 1.0f}};
    static sceVu0FVECTOR pos[4]={{-8.0f, 0.0f, -6.0f, 0.7f},{0.0f, -8.0f, -6.0f, 0.7f},{8.0f, 0.0f, -6.0f, 0.7f},{0.0f, 0.0f, 0.0f, 1.0f}};
    static sceVu0FVECTOR col[4]={{0.23f, 0.54f, 0.09f, 30.0f},{0.59f, 0.55f, 0.31f, 30.0f},{0.64f, 0.4f, 0.4f, 30.0f},{0.2f, 0.2f, 0.2f, 0.0f}};

    for(i=0;i<MAX;i++){		/* light type 3 */
	for(j=0;j<MAX+1;j++){	/* light number 3+1 */
	    lightstore(md, dir[j], pos[j], col[j], i, j);	/* to micro data */
	}
    }
}
