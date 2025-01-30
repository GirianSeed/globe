/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
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
#include <eeregs.h>
#include <libvu0.h>
#include <libgp.h>
#include <libhig.h>
#include <libhip.h>
#include "util.h"

/************************************************/
/*	constant values				*/
/************************************************/
enum {X, Y, Z, W};

/************************************************/
/*	screen & clip & view			*/
/************************************************/
static sceVu0FVECTOR position = { 0.0f, 0.0f, -30.0f, 0.0f };
static sceVu0FVECTOR rotation = { 0.0f, 0.0f, 0.0f, 0.0f };
static sceVu0FVECTOR interest = { 0.0f, 0.0f, 0.0f, 0.0f };
static sceVu0FVECTOR upvector = { 0.0f, 1.0f, 0.0f, 0.0f };
static sceVu0FMATRIX sm = {{512, 0, 0, 0}, {0, 240.639999, 0, 0}, {2048, 2048, -251.000534, 1}, {0, 0, 16777250, 0}};	/* screen matrix */
static sceVu0FVECTOR cv = {69.7180862, 87.6136322, 32768.5, 32767.5};	/* clip volume */

/*
  screen [width, height, distance, clipwide]
  window [aspectx, aspecty, centerx, centery]
  depth  [min, max, near, far]
*/
void perspective(sceHiPlugMicroData_t *md, sceVu0FVECTOR scr, sceVu0FVECTOR win, sceVu0FVECTOR dep)
{
    sceVu0ViewScreenMatrix(sm, scr[Z], win[X], win[Y], win[Z], win[W], dep[X], dep[Y], dep[Z], dep[W]);
    cv[0] = (dep[W]-dep[Z])/(scr[X]+scr[W]);	/* (f-n)/(w+) */
    cv[1] = (dep[W]-dep[Z])/(scr[Y]+scr[W]);	/* (f-n)/(h+) */
    cv[2] = (dep[W]+dep[Z])/2.0f;		/* (f+n)/2 */
    cv[3] = (dep[W]-dep[Z])/2.0f;		/* (f-n)/2 */
    sceVu0CopyVector(md->clp.clip, cv);
}

void viewset(sceVu0FVECTOR pos, sceVu0FVECTOR rot, sceVu0FVECTOR tag, sceVu0FVECTOR up)
{
    sceVu0CopyVector(position, pos);
    sceVu0CopyVector(rotation, rot);
    sceVu0CopyVector(interest, tag);
    sceVu0CopyVector(upvector, up);
}

void viewget(sceVu0FVECTOR pos, sceVu0FVECTOR rot, sceVu0FVECTOR tag, sceVu0FVECTOR up)
{
    sceVu0CopyVector(pos, position);
    sceVu0CopyVector(rot, rotation);
    sceVu0CopyVector(tag, interest);
    sceVu0CopyVector(up, upvector);
}

/************************************************/
/*	camera (util.c)				*/
/************************************************/
void cameractrl(void)
{
    if(gPad.RrightOn){	position[X] += 1.0f; }
    if(gPad.RleftOn){	position[X] -= 1.0f; }
    if(gPad.RdownOn){	position[Y] += 1.0f; }
    if(gPad.RupOn){	position[Y] -= 1.0f; }
    if(gPad.R1On){	position[Z] += 1.0f; }
    if(gPad.R2On){	position[Z] -= 1.0f; }

    if(gPad.LupOn){	rotation[X] -= 0.1f; if(rotation[X]<-PI) rotation[X]+=2.0f*PI; }
    if(gPad.LdownOn){	rotation[X] += 0.1f; if(rotation[X]>PI)  rotation[X]-=2.0f*PI; }
    if(gPad.LleftOn){	rotation[Y] -= 0.1f; if(rotation[Y]<-PI) rotation[Y]+=2.0f*PI; }
    if(gPad.LrightOn){	rotation[Y] += 0.1f; if(rotation[Y]>PI)  rotation[Y]-=2.0f*PI; }
    if(gPad.L2On){	rotation[Z] -= 0.1f; if(rotation[Z]<-PI) rotation[Z]+=2.0f*PI; }
    if(gPad.L1On){	rotation[Z] += 0.1f; if(rotation[Z]>PI)  rotation[Z]-=2.0f*PI; }

    draw_debug_print(1, 10, "  POS  %5.2f %5.2f %5.2f", position[X],position[Y],position[Z]);
    draw_debug_print(1, 11, "  ROT  %5.2f %5.2f %5.2f", rotation[X],rotation[Y],rotation[Z]);
}

void cameraview(sceHiPlugMicroData_t *md)
{
    sceVu0FVECTOR xd,zd={0.0f,0.0f,1.0f,0.0f},yd={0.0f,1.0f,0.0f,0.0f};
    sceVu0FMATRIX m;

    sceVu0UnitMatrix(m);
    sceVu0RotMatrix(m, m, rotation);
    sceVu0ApplyMatrix(zd, m, zd);
    sceVu0ApplyMatrix(yd, m, yd);
    sceVu0ApplyMatrix(xd, m, position);
    sceVu0CameraMatrix(m, xd, zd, yd);

    sceVu0MulMatrix(md->wscreen, sm, m);

    /*	for specific micro code	*/
    md->camx = xd[X];
    md->camy = xd[Y];
    md->camz = xd[Z];
    sceVu0CopyMatrix(md->mtx.wview, m);
}

/*
  default screen clip & view
*/
void camerainit(sceHiPlugMicroData_t *md)
{
    static sceVu0FVECTOR s = {640.0f, 448.0f, 512.0f, 300.0f};
    static sceVu0FVECTOR w = {1.0f, 0.47f, 2048.0f, 2048.0f};
    static sceVu0FVECTOR d = {5.0f, 16777000.0f, 1.0f, 65536.0f};
    static sceVu0FVECTOR p = { 0.0f, 0.0f, -30.0f, 0.0f };
    static sceVu0FVECTOR r = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR i = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR u = { 0.0f, 1.0f, 0.0f, 0.0f };

    perspective(md,s,w,d);
    viewset(p,r,i,u);
    cameraview(md);
}

/************************************************/
/*	lookat					*/
/************************************************/
void lookatview(sceHiPlugMicroData_t *md)
{
    sceVu0FVECTOR xd,yd,zd;
    sceVu0FMATRIX m;

    sceVu0UnitMatrix(m);
    sceVu0RotMatrix(m, m, rotation);
    sceVu0ApplyMatrix(zd, m, interest);
    sceVu0ApplyMatrix(yd, m, upvector);
    sceVu0ApplyMatrix(xd, m, position);

    sceVu0SubVector(zd, zd, xd);
    sceVu0CameraMatrix(m, xd, zd, yd);

    sceVu0MulMatrix(md->wscreen, sm, m);
}

/************************************************/
/*	pilot					*/
/************************************************/
void pilotview(sceHiPlugMicroData_t *md)
{
    sceVu0FMATRIX m;
    sceVu0FVECTOR pos;

    sceVu0UnitMatrix(m);
    sceVu0ScaleVector(pos, pos, -1.0f);
    sceVu0TransMatrix(m, m, pos);		/* plane */
    sceVu0RotMatrixX(m, m, rotation[X]);	/* heading */
    sceVu0RotMatrixY(m, m, rotation[Y]);	/* pitch */
    sceVu0RotMatrixZ(m, m, rotation[Z]);	/* roll */

    sceVu0MulMatrix(md->wscreen, sm, m);
}
/************************************************/
/*	polar					*/
/************************************************/
void polarview(sceHiPlugMicroData_t *md)
{
    sceVu0FMATRIX m;
    sceVu0FVECTOR pos;

    sceVu0UnitMatrix(m);
    sceVu0RotMatrixZ(m, m,  rotation[X]);	/* azimuth */
    sceVu0RotMatrixX(m, m, -rotation[Y]);	/* elevation */
    sceVu0RotMatrixZ(m, m, -rotation[Z]);	/* twist */
    sceVu0ScaleVector(pos, pos, -1.0f);
    sceVu0TransMatrix(m, m, pos);		/* distance */

    sceVu0MulMatrix(md->wscreen, sm, m);
}
