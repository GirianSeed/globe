/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                    - scissoring CORE&VU0  -
 *
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     Name : thatake
 *
 *       Version      Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.5.0        Sep.22.1999     thatake     1st Version
 */

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <libdev.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
//#include <libpkt.h>
#include <libvu0.h>
#include <sifdev.h>

#include "defines.h"

extern sceVu0FMATRIX local_clip;
extern sceVu0FMATRIX local_screen;
extern sceVu0FMATRIX local_light;
extern sceVu0FMATRIX light_color;
extern sceVu0FMATRIX clip_screen;

extern void sceVu0NormalColorVector(sceVu0IVECTOR c0, 
             sceVu0FMATRIX local_light, sceVu0FMATRIX light_color, 
                                    sceVu0FVECTOR v0, sceVu0FVECTOR c1);

// copy vertex information
void CopyNode(Node *nod0, Node *nod1)
{
    sceVu0CopyVector(nod0->vertex, nod1->vertex);
    sceVu0CopyVector(nod0->normal, nod1->normal);
    sceVu0CopyVector(nod0->color, nod1->color);
    sceVu0CopyVector(nod0->texUV, nod1->texUV);
    sceVu0CopyVector(nod0->clipV, nod1->clipV);
};

void InitNodeArraySet(NodeArraySet *scissorflip)
{
    scissorflip->rotflag = 0;
    scissorflip->flipflag = 0;
    scissorflip->in = &scissorflip->narray[scissorflip->flipflag];
    scissorflip->out = &scissorflip->narray[!scissorflip->flipflag];
    scissorflip->in->nodeNum = 0;
    scissorflip->out->nodeNum = 0;
    scissorflip->triangle.nodeNum = 3;
};

void ResetNodeArraySet(NodeArraySet *scissorflip)
{
    scissorflip->flipflag = 0;
    scissorflip->in = &scissorflip->narray[scissorflip->flipflag];
    scissorflip->out = &scissorflip->narray[!scissorflip->flipflag];
    scissorflip->in->nodeNum = 0;
    scissorflip->out->nodeNum = 0;
};

void FlipNodeArray(NodeArraySet *scissorflip)
{
    scissorflip->flipflag = !scissorflip->flipflag;
    scissorflip->in = &scissorflip->narray[scissorflip->flipflag];
    scissorflip->out = &scissorflip->narray[!scissorflip->flipflag];
    scissorflip->out->nodeNum = 0;
};

void PushTriangleNodeArray(NodeArraySet *scissorflip, Node *nod, int mode)
{
    if(mode!=0){
      sceVu0CopyVector(scissorflip->triangle.node[0].normal, nod->normal);
      sceVu0CopyVector(scissorflip->triangle.node[1].normal, nod->normal);
      sceVu0CopyVector(scissorflip->triangle.node[2].normal, nod->normal);
      sceVu0CopyVector(scissorflip->triangle.node[0].color, nod->color);
      sceVu0CopyVector(scissorflip->triangle.node[1].color, nod->color);
      sceVu0CopyVector(scissorflip->triangle.node[2].color, nod->color);
    }
    CopyNode(&scissorflip->triangle.node[scissorflip->rotflag++], nod);
    scissorflip->rotflag %= 3;
};

void InitScissorPlane(ScissorPlaneArray *sp)
{
    sp->planeNum = 6;
    sp->plane[0].clipmask = 0x820;
    sp->plane[0].xyzflag = 2 | 0x10;    // -z plane
    sp->plane[1].clipmask = 0x410;
    sp->plane[1].xyzflag = 2;           // +z plane
    sp->plane[2].clipmask = 0x208;
    sp->plane[2].xyzflag = 1 | 0x10;    // -y plane
    sp->plane[3].clipmask = 0x104;
    sp->plane[3].xyzflag = 1;           // +y plane
    sp->plane[4].clipmask = 0x82;
    sp->plane[4].xyzflag = 0 | 0x10;    // -x plane
    sp->plane[5].clipmask = 0x41;
    sp->plane[5].xyzflag = 0;           // +x plane
};


int ClipCheck(Node *a, Node*b)
{
    register int ret;

    __asm__ __volatile__(
    "lqc2  vf10, 0x40(%1) #load clipV\n"
    "lqc2  vf11, 0x40(%2) #load clipV\n"
    "vclipw.xyz  vf10, vf10w\n"
    "vclipw.xyz  vf11, vf11w\n"
    "vnop\n"
    "vnop\n"
    "vnop\n"
    "vnop\n"
    "vnop\n"
    "cfc2  %0, $vi18\n"
    :"=&r"(ret) : "r"(a), "r"(b));

    return ret;
};

void InterpNode(Node* inter, Node* inside, Node* outside, 
                float sgn, int xyzflag)
{
  /*
  len01 = (out->clipV[flag]- sgn*out->clipV[3])
         - (in->clipV[flag] - sgn*in->clipV[3]);
  ans = (inside->clipV[flag] - sgn*inside->clipV[3]);
  ans01 = fabs(ans/len01);
  */
    __asm__ __volatile__(
    "lqc2  vf10, 0x40(%1) #clipV\n"
    "lqc2  vf11, 0x40(%2)\n"
    "lqc2  vf21, 0x00(%1) #vertex\n"
    "lqc2  vf22, 0x10(%1) #normal\n"
    "lqc2  vf23, 0x20(%1) #color\n"
    "lqc2  vf24, 0x30(%1) #texUV\n"
    "lqc2  vf25, 0x00(%2)\n"
    "lqc2  vf26, 0x10(%2)\n"
    "lqc2  vf27, 0x20(%2)\n"
    "lqc2  vf28, 0x30(%2)\n"
    "vsub.xyzw  vf15, vf11, vf10 #dclip\n"
    "vsub.xyzw  vf25, vf25, vf21 #dvector\n"
    "vsub.xyzw  vf26, vf26, vf22 #dnormal\n"
    "vsub.xyzw  vf27, vf27, vf23 #dcolor\n"
    "vsub.xyzw  vf28, vf28, vf24 #dtexUV\n"
    "mfc1     $8, %3\n"
    "qmtc2    $8, vf09             #load sgn\n"
    "vmulx.w  vf12w, vf10w, vf09x  #sgn*clipV[3]\n"
    "vmulx.w  vf13w, vf11w, vf09x  \n"
    "vsubw.xyzw  vf16, vf10, vf12w #ans=clip[xyz]-sgn*clipV[3]\n"
    "vsubw.xyzw  vf17, vf11, vf13w  \n"
	"\n"
    "vabs.xyzw vf16, vf16\n"
    "vabs.xyzw vf17, vf17\n"
	"\n"
    "qmfc2    $9,  vf16            #load (clip[xyz]-clip[w]) to core\n"
    "qmfc2    $10, vf17\n"
  "LOOP:                           #rotate clip[xyz] field x<-y<-z \n"
    "beq      %4, $0, LOOP_END\n"
    "prot3w   $9, $9\n"
    "prot3w   $10, $10\n"
    "addi     %4, %4, -1\n"
    "j        LOOP\n"
    "nop\n"
  "LOOP_END:                     #X field is clip plane() \n"
    "qmtc2  $9, vf16 \n"
    "qmtc2  $10, vf17            #enable x field to solve answer \n"
//  "vsub.xyz  vf18, vf17, vf16  #len01 = (out.clipX-sgn*W)-(in.clipX-sgn*W)\n"
    "vadd.xyz  vf18, vf17, vf16  #len01 = (out.clipX-sgn*W)-(in.clipX-sgn*W)\n"
    "vdiv   Q, vf16x, vf18x      #ans01 = fabs(ans/len01);\n"
    "vwaitq\n"
    "vaddq.xyzw  vf20, vf00, Q\n"
    "vabs.x     vf20x, vf20x\n"
    "vmulx.xyzw  vf15, vf15, vf20x #ans*dclip\n"
    "vmulx.xyzw  vf25, vf25, vf20x #ans*dvector\n"
    "vmulx.xyzw  vf26, vf26, vf20x #ans*dnormal\n"
    "vmulx.xyzw  vf27, vf27, vf20x #ans*dcolor\n"
    "vmulx.xyzw  vf28, vf28, vf20x #ans*dtexUV\n"
	"\n"
    "vadd.xyzw vf19, vf10, vf15 #clip+ans*dclip\n"
    "vadd.xyzw vf25, vf25, vf21 #vector+ans*dvector\n"
    "vadd.xyzw vf26, vf26, vf22 #normal+ans*dnormal\n"
    "vadd.xyzw vf27, vf27, vf23 #color+ans*dcolor\n"
    "vadd.xyzw vf28, vf28, vf24 #texUV+ans*dtexUV\n"
	"\n"
    "sqc2  vf25, 0x00(%0)\n"
    "sqc2  vf26, 0x10(%0)\n"
    "sqc2  vf27, 0x20(%0)\n"
    "sqc2  vf28, 0x30(%0)\n"
    "sqc2  vf19, 0x40(%0)\n"
	"\n"
    ::"r"(inter), "r"(inside), "r"(outside), 
                     "f"(sgn), "r"(xyzflag) : "$8", "$9", "$10", "memory");

};

void ScissorTriangle(NodeArraySet *scissorflip, 
                     ScissorPlaneArray *planeset)
{
    int i, j;
    int clip;
    int mask;
    int xyz;
    float sgn;
    ScissorPlane *plane;
    NodeArray *inarray = &scissorflip->triangle;
    NodeArray *outarray = scissorflip->out;
    Node *currN, *nextN;
    Node interN;

    for(i=0; i<planeset->planeNum; i++){
      plane = &planeset->plane[i];
      sgn = (plane->xyzflag&0x10)? -1.0 : 1.0;
      mask = plane->clipmask;
      xyz = plane->xyzflag&0xf;

      for(j=0; j<inarray->nodeNum; j++){
        currN = &inarray->node[j];
        nextN = &inarray->node[(j+1)%inarray->nodeNum];
       
        clip = mask&ClipCheck(currN, nextN);

        if(clip==0){ // each vertex is inside
          CopyNode(&outarray->node[outarray->nodeNum++], currN);
        }else if((clip&0x3f) && (clip&0xfc0)){ // each vertex is outside
          //do nothing
        }else if((clip&0x3f) && (clip&0xfc0)==0){ // current is inside
          InterpNode(&interN, currN, nextN, sgn, xyz);
          CopyNode(&outarray->node[outarray->nodeNum++], currN);
          CopyNode(&outarray->node[outarray->nodeNum++], &interN);
        }else if((clip&0x3f)==0 && (clip&0xfc0)){ // next is inside
          InterpNode(&interN, nextN, currN, sgn, xyz);
          CopyNode(&outarray->node[outarray->nodeNum++], &interN);
        }
      }
      FlipNodeArray(scissorflip);
      inarray = scissorflip->in;
      outarray = scissorflip->out;
    }
};

void DrawScissorPolygon(NodeArraySet *scissorflip, ObjData *obj, int flag)
{
    int j;
    float Q;
    NodeArray *in = scissorflip->in;
    GifDmaPacket *pack = obj->packbuf;
    sceVu0IVECTOR v01, c01;
    sceVu0FVECTOR tex;
    int  primitive = obj->prim;

    if(flag==0) primitive = (obj->prim&0xfff8) | 0x5; // triangle fan
    else  primitive = (obj->prim&0xffe8) | 0x02; // triangle fan

//  make packet(PACKED mode)
    pack->size = 0;
    pack->buf[pack->size].ul128 = (u_long128)0;
    pack->buf[pack->size++].ui32[0] 
      = 0x70000000|(in->nodeNum*3+1);
//  add vertex info(GIFtag,STQ&RGBA&XYZ)
    pack->buf[pack->size].ul64[0] 
      = SCE_GIF_SET_TAG(in->nodeNum, 1, 1, primitive, SCE_GIF_PACKED, 3);
    pack->buf[pack->size++].ul64[1] = 0x0412L;

    for(j=0; j<in->nodeNum; j++){
      sceVu0RotTransPers(v01, local_screen, in->node[j].vertex, 0);
      sceVu0ApplyMatrix(tex, local_screen, in->node[j].vertex);
      Q = 1.0/tex[3];
      sceVu0ScaleVector(tex, in->node[j].texUV, Q); 
      sceVu0FTOI0Vector(c01, in->node[j].color);
      pack->buf[pack->size++].ul128 = *((u_long128*)tex);
      pack->buf[pack->size++].ul128 = *((u_long128*)c01);
      pack->buf[pack->size++].ul128 = *((u_long128*)v01);
    }

    // Kick TriangleFan data to GS
    if(in->nodeNum!=0){
      FlushCache(0);
      sceGsSyncPath(0, 0);
      *D2_QWC  = 0x00;
      *D2_TADR = (u_int)&(obj->packbuf->buf)&0x7fffffff;
      *D_STAT = 4;
      *D2_CHCR = 1 | (1<<2) | (0<<4) | (1<<6) | (0<<7) | (1<<8);
      sceGsSyncPath(0, 0);
    }

};

