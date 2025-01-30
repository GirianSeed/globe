/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                    silhouette anti-alias sample program
 *                        Version 1.0
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.00      Nov,07,2001    aoki   	first version
 */
#include <libgraph.h>
#include <libgp.h>

typedef struct {
    void *pPacket;
    sceGpChain *chain;
    int packetsize;
} AA_PACKET;

typedef struct {
    sceGsDrawEnv1 work;
    int wrect[2];
} AA_WB;

enum {
    AA_CROSS_HV,
    AA_HV,
    AA_OFF,
    AA_MODE_MAX,
};


extern void fba_change(int on);
extern void aa_init(int aa_mode, int hblur, int vblur);
extern void aa_set_wb(AA_WB *wb, int wbp, int wpsm, int wbbw, int wbw, int wbh);
extern void init_bg_texture();
extern void load_bg_texture(int fbp, int field);
extern int aa_ctrl(u_int paddata, u_int padtrig);
extern void aa_draw(AA_PACKET *aa_packet);
extern void aa_mkpacket(AA_PACKET *aa_packet, sceGsDrawEnv1 *draw, AA_WB *wb, void *pPacket);
