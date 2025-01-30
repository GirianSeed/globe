/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: zsort -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            zsort.c
 *			zsort with libgp
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Sep,20,2001      ogiso
 */

#include <stdio.h>
#include <stdlib.h>
#include <libdma.h>
#include <libpad.h>
#include <libgraph.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <malloc.h>
#include <libgp.h>
#include <libvu0.h>
#include <math.h>
#include <libdev.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 224
#define OFFX          (((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY          (((4096-SCREEN_HEIGHT)/2)<<4)
#define PI            (3.141592f)
#define IMAGE_SIZE    256

#define Z_NEAR        10
#define Z_FAR         100

#define OTLENGTH      1024

extern  u_int SamplePyramidDataHead[];

typedef struct{
    u_int  *pDataHead;
    u_int  magic;
    u_int  blockNum;
    u_int  prim;
    u_int  dataNum;
    u_int  *vertexNum;
    float  **pData;
}ObjData;


sceVu0FVECTOR camera_p   = { 0, 0, -15, 0 };
sceVu0FVECTOR camera_zd  = { 0, 0, 1, 1 };
sceVu0FVECTOR camera_yd  = { 0, 1, 0, 1 };
sceVu0FVECTOR camera_rot = { 0, 0, 0, 0 };

sceVu0FVECTOR light0 = { 0.0, 1.5, 0.5, 0 };
sceVu0FVECTOR light1 = { 1.5, -0.5, 0.5, 0 };
sceVu0FVECTOR light2 = { -1.5, -0.5, 0.5, 0 };

sceVu0FVECTOR color0 = { 0.8, 0.3, 0.3, 1 };
sceVu0FVECTOR color1 = { 0.3, 0.8, 0.3, 1 };
sceVu0FVECTOR color2 = { 0.3, 0.3, 0.8, 1 };

sceVu0FVECTOR ambient = { 0.3, 0.3, 0.3, 0 };

sceVu0FVECTOR obj_trans = { 0, 0, 0, 0 };
sceVu0FVECTOR obj_rot   = { -0.5f, 1.27f, 0, 0 };

sceVu0FMATRIX local_world;
sceVu0FMATRIX world_view;
sceVu0FMATRIX view_screen;
sceVu0FMATRIX local_screen;

sceVu0FMATRIX normal_light;
sceVu0FMATRIX light_color;
sceVu0FMATRIX local_light;
sceVu0FMATRIX local_color;

sceVu0FMATRIX work;

u_long128 pad_dma_buf[scePadDmaBufferMax]  __attribute__((aligned (64)));

// vu0.c functions
void sceVu0RotTransPersNClipCol(u_long128 *v0, sceVu0FMATRIX m0, 
                                sceVu0FVECTOR *v1, sceVu0FVECTOR *nv,
                                sceVu0FVECTOR *st, sceVu0FVECTOR *c, 
                                sceVu0FMATRIX ll, sceVu0FMATRIX lc, int n);
void sceVu0RotCameraMatrix(sceVu0FMATRIX m, sceVu0FVECTOR p, sceVu0FVECTOR zd,
                           sceVu0FVECTOR yd, sceVu0FVECTOR rot);

// local functions
void InitDevice(sceGsDBuff *pdb);
void LoadObj(ObjData *o, u_int *Head);
void ReleaseObj(ObjData *o);
float getZAverage(void* vtxaddr, int n);
void *readfile(char *fname);


/*******************************************************************/
int main( void )
{    
    sceGpChain chain1;
    sceGpPrimR **primpac, *sprpac;
    
    static sceGpLoadTexelClut loadtexelclut;
    static sceGpLoadImage loadimage;
    static sceGpTexEnv texenv[2];
    static sceGpAlphaEnv alphaenv;
    
    sceGpTextureArg texarg[2];

    static sceGsDBuff db;
    u_int fr;
    int   i;
    float delta = 1.0f * PI / 180.0f;
    int consId, odev, sel_toggle = 0, zsort_switch = 1;
    int pacsqwsize;
    char *pacaddr, *p;
    void *ptim2[2];
    ObjData  obj;
    static u_long128 OT[sceGpChkChainOtSize(OTLENGTH)];
    
    u_int paddata;
    u_char rdata[32];

    InitDevice(&db);

    LoadObj(&obj, SamplePyramidDataHead);
    
    ptim2[0] = readfile("host0:inuneko.tm2");
    ptim2[1] = readfile("host0:plus.tm2");
    
    sceGpInitChain(&chain1, OT, OTLENGTH);
    
    // inspect packets total size 
    pacsqwsize = 0;    
    for(i = 0; i < obj.blockNum; i++){
        pacsqwsize += sceGpChkPacketSize(SCE_GP_PRIM_R | SCE_GP_TRISTRIP_GTS,
                                         obj.vertexNum[i] - 2);
    }
    
    // Create pyramid packet
    p = pacaddr = (char*)malloc(pacsqwsize * sizeof(qword));
    primpac = (sceGpPrimR**)malloc(sizeof(sceGpPrimR*) * obj.blockNum);
    
    sceGpSetDefaultAbe(1);    // alpha blend ON for "SamplePyramidData"
    
    for(i=0; i<obj.blockNum; i++){
        primpac[i] = (sceGpPrimR*)p;
        sceGpInitPacket((void*)primpac[i],
            SCE_GP_PRIM_R | SCE_GP_TRISTRIP_GTS, obj.vertexNum[i] - 2);
        p += sizeof(qword)* sceGpChkPacketSize(
            SCE_GP_PRIM_R | SCE_GP_TRISTRIP_GTS, obj.vertexNum[i] - 2);
    }

    // Create BG sprite packet
    sprpac = (sceGpPrimR*)malloc(sizeof(qword) * sceGpChkPacketSize(
                                  SCE_GP_PRIM_R | SCE_GP_SPRITE_FTU, 1));

    sceGpSetDefaultAbe(0);    // alpha blend OFF (default)    

    sceGpInitPacket(sprpac, SCE_GP_PRIM_R | SCE_GP_SPRITE_FTU, 1);

    sceGpSetRgba(sprpac, sceGpIndexRgbaSpriteFTU(0), 255, 255, 255, 128);
    sceGpSetUv(sprpac, sceGpIndexUvSpriteFTU(0), 0.0f*16.0f, 0.0f*16.0f);
    sceGpSetXyzf(sprpac, sceGpIndexXyzfSpriteFTU(0), 
                 (-200+2048)<<4, (-100+2048)<<4, 0x8000001, 0x7f);
    sceGpSetUv(sprpac, sceGpIndexUvSpriteFTU(1), 128.0f*16.0f, 128.0f*16.0f);
    sceGpSetXyzf(sprpac, sceGpIndexXyzfSpriteFTU(1), 
                 (200+2048)<<4, (100+2048)<<4, 0x8000001, 0x7f);
    
    // Init LoadImage, TexEnv/AlphaEnv
    texarg[0].tbp = 6720;
    texarg[0].cbp = 0;
    texarg[0].tx = 0;
    texarg[0].ty = 0;
    texarg[0].tbw = texarg[0].tw = texarg[0].th
        = texarg[0].tpsm = texarg[0].cpsm = -1;

    texarg[1] = texarg[0];
    texarg[1].tbp = 7756;
    texarg[1].cbp = 7000;

    sceGpInitPacket(&loadtexelclut, SCE_GP_LOADTEXELCLUT, 0);
    sceGpInitPacket(&loadimage, SCE_GP_LOADIMAGE, 0);
    sceGpInitPacket(&texenv[0], SCE_GP_TEXENV, 0);
    sceGpInitPacket(&texenv[1], SCE_GP_TEXENV, 0);
    sceGpInitPacket(&alphaenv, SCE_GP_ALPHAENV, 0);
    
    // Set LoadImage, TexEnv/AlphaEnv
    sceGpSetLoadTexelClutByArgTim2(&loadtexelclut, &texarg[1], ptim2[1], 0, 0);
    sceGpSetLoadImageByArgTim2(&loadimage, &texarg[0], ptim2[0], 0, 0, 0);

    sceGpSetTexEnvByArgTim2(&texenv[0], &texarg[0], ptim2[0], 
                            SCE_GS_MODULATE, SCE_GS_LINEAR);
    sceGpSetTexEnvByArgTim2(&texenv[1], &texarg[1], ptim2[1], 
                            SCE_GS_MODULATE, SCE_GS_NEAREST);

    sceGpSetAlphaEnv(&alphaenv, SCE_GP_ALPHA_INTER_AS, 0);
        
    // Add LoadImage, AlphaEnv
    sceGpAddPacket(&chain1, 1, &loadtexelclut);
    sceGpAddPacket(&chain1, 1, &loadimage);
    sceGpAddPacket(&chain1, 1, &alphaenv);
    sceGpTermChain(&chain1, 2, 0);
    
    // Kick Chain
    FlushCache(0);
    sceGpKickChain(&chain1, SCE_GP_PATH3);    

    sceVu0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 0.47f,
                2048.0f, 2048.0f, 1.0f, 16777215.0f, 1.0f, 65536.0f);

    consId = sceDevConsOpen(OFFX, OFFY, 79, 40);

    fr = 0;
    odev = !sceGsSyncV(0);

    scePadInit(0);
    scePadPortOpen(0, 0, pad_dma_buf);
    
    while(1){
        sceGsSwapDBuff(&db, fr);
	
        if(scePadRead(0, 0, rdata) > 0){
            paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
        }else{
            paddata = 0;
        }
        
        if(paddata & SCE_PADLdown){
             obj_rot[0] += delta; if(obj_rot[0]>PI) obj_rot[0] -= 2.0f*PI; }
        if(paddata & SCE_PADLup){
             obj_rot[0] -= delta; if(obj_rot[0]<-PI) obj_rot[0] += 2.0f*PI; }
        if(paddata & SCE_PADLright){
             obj_rot[1] += delta; if(obj_rot[1]>PI) obj_rot[1] -= 2.0f*PI; }
        if(paddata & SCE_PADLleft){
             obj_rot[1] -= delta; if(obj_rot[1]<-PI) obj_rot[1] += 2.0f*PI; }
        if(paddata & SCE_PADL1){
             obj_rot[2] += delta; if(obj_rot[2]>PI) obj_rot[2] -= 2.0f*PI; }
        if(paddata & SCE_PADL2){
             obj_rot[2] -= delta; if(obj_rot[2]<-PI) obj_rot[2] += 2.0f*PI; }
        if(paddata & SCE_PADRdown){
             camera_rot[0] += delta; 
             if(camera_rot[0]>PI*0.4) camera_rot[0] = 0.4f*PI;
        }
        if(paddata & SCE_PADRup){
             camera_rot[0] -= delta;
             if(camera_rot[0]<-PI*0.4) camera_rot[0] = -0.4f*PI;
        }
        if(paddata & SCE_PADRright){
             camera_rot[1] += delta;
             if(camera_rot[1]>PI) camera_rot[1] -= 2.0f*PI;
        }
        if(paddata & SCE_PADRleft){
             camera_rot[1] -= delta;
             if(camera_rot[1]<-PI) camera_rot[1] += 2.0f*PI;
        }
        if(paddata & SCE_PADR1){
             camera_p[2] -= delta*5;
             if(camera_p[2]<-Z_FAR) camera_p[2] = -Z_FAR;
        }
        if(paddata & SCE_PADR2){
             camera_p[2] += delta*5;
             if(camera_p[2]>-Z_NEAR) camera_p[2] = -Z_NEAR;
        }
        if(!sel_toggle && (paddata & SCE_PADselect)){
             ++zsort_switch; zsort_switch &= 0x01; sel_toggle = 1;
        }else if(!(paddata & SCE_PADselect)){sel_toggle = 0;}

        
        // local -> world (rotate)matrix
        sceVu0UnitMatrix(work);
        sceVu0RotMatrix(local_world, work, obj_rot);

        // color&normal matrix setting
        sceVu0NormalLightMatrix(normal_light, light0, light1, light2);
        sceVu0LightColorMatrix(light_color, color0, color1, color2, ambient);

        // light(normal) -> local_light matrix
        sceVu0MulMatrix(local_light, normal_light, local_world);

        // local -> world (rotate&translate)matrix
        sceVu0TransMatrix(local_world, local_world, obj_trans);

        // world -> view matrix
        sceVu0RotCameraMatrix(world_view, camera_p, camera_zd, camera_yd,camera_rot);

        // local -> screen matrix
        sceVu0MulMatrix(work, world_view, local_world);
        sceVu0MulMatrix(local_screen, view_screen, work);

        sceGpResetChain(&chain1);
        sceGpAddPacket(&chain1, 1, sprpac);      //send 2nd
        sceGpAddPacket(&chain1, 1, &texenv[0]);  //send 1st
        sceGpAddPacket(&chain1, 2, &texenv[1]);  //send 3rd
        
        for(i=0; i<obj.blockNum; i++){
            float z;
            u_int inslevel;
            sceVu0FVECTOR *vertex, *normal, *texUV, *color;
            vertex  = (sceVu0FVECTOR *)obj.pData[i];
            normal  = (sceVu0FVECTOR *)(obj.pData[i]+obj.vertexNum[i]*4);
            texUV   = (sceVu0FVECTOR *)(obj.pData[i]+obj.vertexNum[i]*4*2);
            color   = (sceVu0FVECTOR *)(obj.pData[i]+obj.vertexNum[i]*4*3);

            sceVu0RotTransPersNClipCol((u_long128*)&(primpac[i]->reg[0]), local_screen, vertex,
                normal, texUV,color, local_light,light_color, obj.vertexNum[i]);

            z = getZAverage(&primpac[i]->reg[0], obj.vertexNum[i]);
            
            inslevel = OTLENGTH * (Z_FAR + 5 - z) / (Z_FAR + 10);
            
            sceGpAddPacket(&chain1, zsort_switch ? inslevel : 5, primpac[i]);
        }

        FlushCache(0);
        sceGpKickChain(&chain1, SCE_GP_PATH3);

        sceDevConsClear(consId);
        sceDevConsLocate(consId, 1, 1);
        sceDevConsPrintf(consId, "Z sort %3s [SELECT to change]\n", 
                         zsort_switch ? "ON" : "OFF");
        sceDevConsDraw(consId);

        sceGsSetHalfOffset((fr&1) ? &db.draw1 : &db.draw0, 2048, 2048, odev);
        fr++;
        
        odev = !sceGsSyncV(0);
        FlushCache(0);
    }
    
    // unreachable code
    free(ptim2[0]);  // allocated in readfile
    free(ptim2[1]);  // allocated in readfile

    free(sprpac);
    free(primpac);
    free(pacaddr);

    ReleaseObj(&obj);
}
/*******************************************************************/
void InitDevice(sceGsDBuff *pdb)
{
    sceSifInitRpc(0);

    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx",
                            0, NULL) < 0){
        printf("Can't load module sio2man\n");
    }
    while (sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx",
                            0, NULL) < 0){
        printf("Can't load module padman\n");
    }
    
    // set double buffer
    sceGsSetDefDBuff(pdb, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		     SCE_GS_ZGREATER, SCE_GS_PSMZ24, SCE_GS_CLEAR);
    *(u_long *)&pdb->clear0.rgbaq
        = SCE_GS_SET_RGBAQ(0x10, 0x60, 0x60, 0x80, 0x3f800000);
    *(u_long *)&pdb->clear1.rgbaq
        = SCE_GS_SET_RGBAQ(0x10, 0x60, 0x60, 0x80, 0x3f800000);
    
    sceGsResetPath();
    sceDmaReset(1);
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
    
    sceDevVif1Reset();
}

/*******************************************************************/
void LoadObj(ObjData *o, u_int *Head)
{
    int    i;

    o->pDataHead  = Head;
    o->magic      = *o->pDataHead;
    o->blockNum   = *(o->pDataHead+2)>>16;
    o->prim       = *(o->pDataHead+2)&0x0000ffff;
    o->dataNum    = *(o->pDataHead+3);

    o->pData      = (float**)malloc(sizeof(float*)*o->blockNum);
    o->vertexNum  = (u_int*)malloc(sizeof(u_int)*o->blockNum);

    o->pData[0]     = (float *)(o->pDataHead+8);
    o->vertexNum[0] = *(o->pDataHead+4);

    for(i=0; i<o->blockNum-1; i++){
        o->pData[i+1] = (o->pData[i]+o->vertexNum[i]*4*4+4);
        o->vertexNum[i+1] = *(u_int *)(o->pData[i]+o->vertexNum[i]*4*4);
    }
}

/*******************************************************************/
void ReleaseObj(ObjData *o)
{
    free(o->pData);
    free(o->vertexNum);
}

/*******************************************************************/
float getZAverage(void* vtxaddr, int n)
{
    int i;
    float z = 0, *faddr = (float*)vtxaddr;
    faddr -= 3;
    
    for(i=0; i<n; i++){
        faddr += 6;
        z += 1.0 / *faddr;
    }
    
    return z / (float)n;
}

/*******************************************************************/
void *readfile(char *fname)
{
    size_t  size;
    void    *dptr;
    int     fd;

    if ((fd = sceOpen(fname, SCE_RDONLY)) < 0) {
        printf("Can't open file %s\n", fname);
        while (1)
          ;
    }
    size = sceLseek(fd, 0, SCE_SEEK_END);
    sceLseek(fd, 0, SCE_SEEK_SET);

    dptr = memalign(64, size);
    if (dptr == NULL) {
        printf("Can't allocate memory for file reading %s\n", fname);
        while (1)
          ;
    }
    if (sceRead(fd, (u_int *)dptr, size) < 0) {
        printf("Can't read file %s\n", fname);
        while (1)
          ;
    }
    FlushCache(0);
    sceClose(fd);

    return dptr;
}

/*******************************************************************/
