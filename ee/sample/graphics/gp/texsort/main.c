/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *                         - libgp: texsort -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            main.c
 *			  material sort with libgp 
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Sep,20,2001      aoki
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
#include <libhig.h>
#include <math.h>

#define POPULATION_MAX 10000
#define POPULATION_INIT 3000

#define BASESPEED 5.0f
#define RANDSPEED 3.0f
#define SEQSPEED 0.15f
#define ZSPEED 0.2f
#define TURNSPEED 0.3f

#define TRANSFER_MODE_SELECT SCE_GP_PATH2
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 224
#define TBP 6720
#define CTXT 0

#define ZFAR 10000
#define ZNEAR 100

#define XMAX (5000.0f)
#define XMIN (-5000.0f)

#define ZBUF_MAX ((1<<20)-1)


#define PI 3.14159


#define TEXNUM 4
#define TEXLVL 3
struct _texture {
    char *file;
    int width;
    int height;
    int psm;
    int texnum;
    u_long128 *addr;
    int size;
} texture[TEXLVL]={
    {
	"host0:laura.raw",
	128,
	64,
	SCE_GS_PSMCT32,
	TEXNUM, 0, 0
    },
    {
	"host0:laura-s.raw",
	64,
	32,
	SCE_GS_PSMCT32,
	TEXNUM, 0, 0
    },
    {
	"host0:laura-t.raw",
	32,
	16,
	SCE_GS_PSMCT32,
	TEXNUM, 0, 0
    },
};

static char seq[]={1, 0, 3, 2, -1};


static sceVu0FMATRIX view_screen;
static sceVu0FMATRIX local_screen;
static sceVu0FVECTOR obj_rot;
static sceVu0FVECTOR obj_trans;

#define random() rand()
    
#define MAN_WIDTH 256
#define MAN_HEIGHT 256

#define SPRITE_FTU1_P_SIZE 10

#define GROUND_SPRIT_H 32
#define GROUND_SPRIT_W 8
#define GROUND_PACKET_P_LEN 22

/* volume clipping area */
static sceVu0FVECTOR clipv[2]={
    {2048.0f-(320.0f*3.0f), 2048.0f-(112.0f*3.0f), 0.0f, 0.0f},	/* min  */
    {2048.0f+(320.0f*3.0f), 2048.0f+(112.0f*3.0f), ((1<<24)-1.0f), ((1<<24)-1.0f)} /* max  */
};


/* packet area for alpha-test enable */
static u_long128 aTestEnArea[2][3];
/* packet handler for alpha-test enable */
static sceGpAd *aTestEn[2]={(sceGpAd *)aTestEnArea[0],(sceGpAd *)aTestEnArea[0]};

/* to change texture */
sceGpTexEnv texenv[TEXLVL][TEXNUM];
/* to load texture */
sceGpLoadImage textrans[TEXLVL][TEXNUM];

/* packet handler for the ground */
sceGpPrimP *ground[2][GROUND_SPRIT_H];
/* packet area for the ground */
u_long128 GroundBuf[2][GROUND_SPRIT_H][GROUND_PACKET_P_LEN]  __attribute__((aligned (16))); 
/* ground vertex list (generate at ground_init() */
static sceVu0FVECTOR ground_vx[GROUND_SPRIT_H+1][GROUND_SPRIT_W+1];


typedef struct {
    u_long128 sprite[2][SPRITE_FTU1_P_SIZE];
    float x, z;
    float xspeed;
    float zspeed;
    int texlvl;
    char direction;
    char seq;
    char texid;
    char disp;
    char seqspeed;
    char timetoturn;
    char timetoseq;
} MAN __attribute__((aligned (64)));

static MAN man[POPULATION_MAX] __attribute__((aligned (64)));

static float basespeed=BASESPEED;
static float randspeed=RANDSPEED;
static float seqspeed=SEQSPEED;
static float turnspeed=TURNSPEED;
static float zspeed=ZSPEED;
static int population=0;



/* for libpad controller */
u_long128 pad_dma_buf[scePadDmaBufferMax]  __attribute__((aligned (64)));
/* controller input */
static int pad_current;
static int pad_trig;

/* for libgraph GS double buffer */
static sceGsDBuff db;



/* functions  */
static void error_handling(const char *file, int line, const char *msg);
static void dev_init();
static void chain_init(sceGpChain *chain, int resolution);
static void texture_load(const char *file, u_long128 **addr, int *filesize);
static void texture_init();
static u_int pad_read();
static void sprite_init(sceGpPrimP *p);
static void ground_init();
static void geom_init();
static void ground_proceed(int side);
static void man_init();
static inline void man_turn(MAN *m, int direction);
static inline void man_seq(MAN *m);
static inline int man_find();
static int man_create(int isRandomZ);
static void geom_proceed();
static void man_geom(MAN *m, int side);
static inline void man_erase(MAN *m);
static inline void man_disappear();
static void man_proceed(sceGpChain *chain, int move, int side);
static void Vu0RotTransPersN_clip(sceVu0IVECTOR *v0, sceVu0FMATRIX m0, sceVu0FVECTOR *v1, int n, int mode, sceVu0FVECTOR *clipv);

static void error_handling(const char *file, int line, const char *msg)
{
    printf("Error: %s:%d: %s\n", file, line, msg);

    while (1);
}


static void dev_init()
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

    sceGsResetPath();
    sceDmaReset(1);
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);


    // --- set double buffer ---
    sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		     SCE_GS_ZGREATER, SCE_GS_PSMZ24, SCE_GS_CLEAR);
    *(u_long *)&db.clear0.rgbaq
      = SCE_GS_SET_RGBAQ(0x40, 0x40, 0x40, 0x80, 0x3f800000);
    *(u_long *)&db.clear1.rgbaq
      = SCE_GS_SET_RGBAQ(0x40, 0x40, 0x40, 0x80, 0x3f800000);

    sceGsSyncV(0);

    scePadInit(0);
    scePadPortOpen(0, 0, pad_dma_buf);

}


/* init libgp dma chain */
static void chain_init(sceGpChain *chain, int resolution)
{
    u_long128 *ot;
    int size;

    size=sceGpChkChainOtSize(resolution);
    ot=memalign(16, size*sizeof(u_long128));
    if (ot==NULL) error_handling( __FILE__, __LINE__, "malloc failed\n");

    sceGpInitChain(chain, ot, resolution);
}

/* load texture file to main memory */
static void texture_load(const char *file, u_long128 **addr, int *filesize)
{
    size_t size;
    u_int *dptr;
    int	fd;

    if ((fd = sceOpen(file, SCE_RDONLY)) < 0) {
	error_handling(file, 0, "file open error");
    }
    size = sceLseek(fd, 0, SCE_SEEK_END);
    sceLseek(fd, 0, SCE_SEEK_SET);

    dptr = (u_int *)memalign(64, size);
    if (dptr == NULL) error_handling(file, size, "malloc failed");

    FlushCache(0);
    if (sceRead(fd, dptr, size) < 0) {
	error_handling(file, size, "file read failed");
    }
    sceClose(fd);

    *filesize=size;
    *addr=(u_long128 *)dptr;

    return;
}

/* init loadimage/texenv packets */
static void texture_init()
{
    sceGpTextureArg texarg;  /* for set-functions  */
    int i,j;
    char *addr;

    texarg.tbp = TBP;
    texarg.tx = texarg.ty = 0;

    for (i=0; i<TEXLVL; i++){
	texture_load(texture[i].file, &texture[i].addr, &texture[i].size);
	texture[i].size/=texture[i].texnum;

	texarg.tbw = (texture[i].width+63)/64;
	texarg.tpsm = texture[i].psm;

	texarg.tw = texture[i].width;
	texarg.th = texture[i].height;

	
	addr=(char *)texture[i].addr;
	for (j=0; j<texture[i].texnum; j++){
	    /* init packet */
	    sceGpInitPacket(&texenv[i][j], SCE_GP_TEXENV, CTXT);
	    /* set values */
	    sceGpSetTexEnv(&texenv[i][j], &texarg, SCE_GS_DECAL, SCE_GS_LINEAR);

	    /* init packet */
	    sceGpInitPacket(&textrans[i][j], SCE_GP_LOADIMAGE, 0);
	    /* set values */
	    sceGpSetLoadImage(&textrans[i][j], &texarg, addr, 0);

	    addr+=texture[i].size;
	}
    }

}

static u_int pad_read()
{
    u_char rdata[32];
    u_int paddata;

    if(scePadRead(0, 0, rdata) > 0){
	paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
    }else{
	paddata = 0;
    }

    return paddata;
}

/* packet for alpha test enable */
static void alpha_init()
{
    union {
	sceGsTest reg;
	u_long ul;
    } test;
    int i;

    for (i=0; i<2; i++){
	sceGpInitAd(aTestEn[i], 1);

	test.reg=db.clear0.testb; /* copy from original setting */
	test.reg.ATE=1;
	test.reg.ATST=SCE_GS_ALPHA_GREATER;
	test.reg.AFAIL=SCE_GS_AFAIL_KEEP;
	test.reg.AREF=0x80;
	sceGpSetAd(aTestEn[i], 0, SCE_GS_TEST_1, test.ul);
    }
}

static int pad_ctrl()
{
    static u_int oldpaddata=0;
    float dt=50.0f;
    float dr=1.0f * 3.14159 / 180.0f;
    int i;
    int move;

    move=1;
    oldpaddata= pad_current;
    pad_current=pad_read();
    pad_trig= pad_current& (~oldpaddata);


    if (pad_trig&SCE_PADselect) geom_init();	/* reset camera  */

    if (pad_current&SCE_PADR1) man_create(0);	/* increase population  */
    if (pad_current&SCE_PADR2) man_disappear();	/* decrease population  */

    if (pad_current&SCE_PADRup){		/* increase more population  */
	for (i=0; i<100; i++) man_create(1);
	move=0;
    }

    if (pad_current&SCE_PADRdown){		/* decrease more population  */
	for (i=0; i<100; i++) man_disappear();
	move=0;
    }

    if(pad_current & SCE_PADLdown){
	obj_rot[0] += dr; 
	if(obj_rot[0]>0.8f) obj_rot[0] = 0.8f; else obj_trans[1] += dt/3;

    }
    if(pad_current & SCE_PADLup){
	obj_rot[0] -= dr; 
	if(obj_rot[0]<0.2f) obj_rot[0] = 0.2f; else obj_trans[1] -= dt/3;

    }

    if(pad_current & SCE_PADL1){
	obj_trans[2] -= dt;
	if(obj_trans[2]<0.0f) obj_trans[2] = 0.0f;
    }
    if(pad_current & SCE_PADL2){
	obj_trans[2] += dt;
    }

    if (pad_current & SCE_PADstart) move= 0;	/* pause */
    
    return move;
}


int main( void )
{
    sceGpChain chain[2];
    int odev;
    int fr=0;
    int side; /* double buffering flag */
    int i,j;
    int move;

    dev_init();
    
    /* init loadimage/texenv packets */
    texture_init();
    
    /* init variables for geometrical calc. */
    geom_init();

    /* init alpha test packet */
    alpha_init();

    /* init libgp dma-chain */
    for (i=0; i<2; i++){
	chain_init(&chain[i], TEXNUM*TEXLVL);  /* resolution = total texture num */
    }

    /* init packets for people */
    man_init();
    
    /* init packets for the ground */
    ground_init();

    population=0; /* population counter */
    /* create people */
    for (i=0; i<POPULATION_INIT; i++){
	man_create(1);
    }

    while (1){
	side=fr&1; /* which side of the double buffer */

	sceGpResetChain(&chain[side]); /* reset the ordering table */

	move=pad_ctrl();

	if (pad_trig&SCE_PADRleft) printf("population: %d\n", population);

	/* update camera matrix */
	geom_proceed();

	/* update people,  add packet to chain */
	man_proceed(&chain[side], move, fr&1);

	/* update ground */
	ground_proceed(side);

	/* add texture-related packets to each OT entry */
	for (i=0; i<TEXLVL; i++){
	    for (j=0; j<TEXNUM; j++){
		sceGpAddPacket(&chain[side], i*TEXNUM+j, &texenv[i][j]);
		sceGpAddPacket(&chain[side], i*TEXNUM+j, &textrans[i][j]);
	    }
	}
	/* alpha test enable, place between people and ground */
	sceGpAddPacket(&chain[side], 0, aTestEn[side]);

	/* add gound packet */
	for (i=0; i<GROUND_SPRIT_H; i++){
	    sceGpAddPacket(&chain[side], 0, ground[side][i]);
	}

	/* print chain/packet status */
	if (pad_trig&SCE_PADRright) sceGpPrintChain(&chain[side], 0, 0, 0);

	/* wait to the previous transfer done */
	sceGsSyncPath(0, 0);

	/* wait vsync, switch GS double buffering */
	odev = !sceGsSyncV(0);
	sceGsSetHalfOffset((fr&1) ? &db.draw1 : &db.draw0, 2048, 2048, odev);
        FlushCache(0);
        sceGsSwapDBuff(&db, fr);

	/* start transfer */
        while (sceGpKickChain(&chain[side], TRANSFER_MODE_SELECT)<0){
	    volatile int counter;
	    for (counter=0; counter<1000; counter++) ;
	}

	fr++;

    }
}

/* set sprite packet */
static void sprite_init(sceGpPrimP *p)
{
    int index;
    u_int type=SCE_GP_PRIM_P|SCE_GP_SPRITE_FTS;
    
    sceGpInitPacket(p, type, 1);

    index=sceGpIndexRgba(type, 0);
    sceGpSetRgba((sceGpPrimP *)p, index, 0x80, 0x80, 0x80, 0x80);

    /* use STQ register,  not to change UV values depending texture size */
    index=sceGpIndexStq_P(type, 0);
    sceGpSetStq_P((sceGpPrimP *)p, index, 0.0f, 0.0f, 1.0f);

    index=sceGpIndexStq_P(type, 1);
    sceGpSetStq_P((sceGpPrimP *)p, index, 1.0f, 1.0f, 1.0f);

}

/* init packets for the ground */
static void ground_init()
{
    int i,j;
    int size;
    int type=SCE_GP_PRIM_P|SCE_GP_TRISTRIP_FM; /* packet type */
    int pnum= 2*GROUND_SPRIT_W;			/* polygon num for each packet */

    float  dx=(XMAX-XMIN)/GROUND_SPRIT_W;
    float  dz=(ZFAR-ZNEAR)/GROUND_SPRIT_H;
    float zoffset;
    float r=1.2f;
    dz=((float)ZFAR-ZNEAR)*(1-r)/(1-powf(r, GROUND_SPRIT_H));

    /* initialize packets for ground */
    for (i=0; i<2; i++){
	for (j=0; j<GROUND_SPRIT_H; j++){
	    ground[i][j]=(sceGpPrimP *)GroundBuf[i][j];

	    size = sceGpChkPacketSize(type, pnum);
	
	    if (GROUND_PACKET_P_LEN < size){
		printf("size=%d\n", size);
		error_handling( __FILE__, __LINE__, "short buffer\n");
	    }
	    sceGpInitPacket(ground[i][j], type, pnum);

	    sceGpSetRgbaFM(ground[i][j], 0x80, 0x80, 0x80, 0x80);

	}

    }

    /* ground ground polygons' vertex */
    zoffset=0.0f;
    for (i=0; i<GROUND_SPRIT_H+1; i++){
	for (j=0; j<GROUND_SPRIT_W+1; j++){
	    ground_vx[i][j][0]=XMIN+(dx*j);
	    ground_vx[i][j][1]=0;
	    ground_vx[i][j][2]=ZNEAR+zoffset;
	    ground_vx[i][j][3]=1.0f;
	}
	zoffset+=dz;
	dz*=r;
    }


}

static void geom_init()
{
    int i;
    
   sceVu0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 0.47f,
                             2048.0f, 2048.0f, 50.0f, (float)ZBUF_MAX,
			   ZNEAR, ZFAR*10);

    for (i=0; i<3; i++){
	obj_rot[i]=0.0f;
	obj_trans[i]=0.0f;
    }
    obj_rot[0]=0.4f;
    obj_trans[1]=400.0f;

    obj_rot[3]=1.0f;
    obj_trans[2]= 50.0f;
    obj_trans[3]=0.0f;
}


/* update ground */
static void ground_proceed(int side)
{
    sceVu0IVECTOR sx[GROUND_SPRIT_H+1][GROUND_SPRIT_W+1];
    int i, j;
    int clip[3]={1,1,1};


    /* calculate all vertexes */
    Vu0RotTransPersN_clip(&sx[0][0], local_screen, &ground_vx[0][0], 
			((GROUND_SPRIT_W+1)*(GROUND_SPRIT_H+1)), 0, clipv);


    /* set vertex to the ground packets */
    for (i=0; i<GROUND_SPRIT_H; i++){
	for (j=0; j<GROUND_SPRIT_W+1; j++){
	    int index;

	    clip[2]=clip[1];
	    clip[1]=clip[0];
	    if (sx[i][j][3]) clip[0]=1;
	    else clip[0]=0;
	    
	    index=sceGpIndexXyzfTriStripFM(j*2);
	    ground[side][i]->reg[index].xyzf.X= sx[i][j][0];
	    ground[side][i]->reg[index].xyzf.Y= sx[i][j][1];
	    ground[side][i]->reg[index].xyzf.Z= 16;
	    ground[side][i]->reg[index].xyzf.ADC= clip[0]&&clip[1]&&clip[2];


	    clip[2]=clip[1];
	    clip[1]=clip[0];
	    if (sx[i][j][3]) clip[0]=1;
	    else clip[0]=0;

	    index=sceGpIndexXyzfTriStripFM(j*2+1);
	    ground[side][i]->reg[index].xyzf.X= sx[i+1][j][0];
	    ground[side][i]->reg[index].xyzf.Y= sx[i+1][j][1];
	    ground[side][i]->reg[index].xyzf.Z= 16;
	    ground[side][i]->reg[index].xyzf.ADC= clip[0]&&clip[1]&&clip[2];
	}

    }
}

/* init packets for people */
static void man_init()
{
    int i,j;


    sceGpSetDefaultAbe(0);

    /* set the first one packet */
    sprite_init((sceGpPrimP *)&man[0].sprite[0]);

    /* copy the first one to all the rest */
    sceGpCopyPacket(&man[0].sprite[1], &man[0].sprite[0]);

    for (i=1; i<POPULATION_MAX; i++){
	man[i].disp=0;
	for (j=0; j<2; j++){
	    sceGpCopyPacket(&man[i].sprite[j], &man[0].sprite[0]);
	}
    }


}

/* turn direction */
static inline void man_turn(MAN *m, int direction)
{
    m->direction=direction;
    m->timetoturn=(random()&0xff)*turnspeed;
}

/* to the next texture */
static inline void man_seq(MAN *m)
{
    m->timetoseq=(int)(m->xspeed*seqspeed);

    m->seq++;
    if (seq[(int)m->seq]<0) m->seq=0;
}

/* find the place to create */
static inline int man_find()
{
    int i;

    for (i=0; i<POPULATION_MAX; i++){
	if (man[i].disp==0) return i;
    }
    return -1;
}

static int man_create(int isRandomZ)
{
    int id;
    MAN *m;
    u_long r;

    id=man_find();				/* find the place to create  */

    if (id<0) return -1;				/* all men running  */

    m= &man[id];
    
    if (isRandomZ) m->z=ZNEAR+(random()%(ZFAR-ZNEAR)); /* random Z position  */
    else m->z= ZFAR;				/* the farest Z */

    r=random();

    m->xspeed= basespeed+(randspeed*(float)(r&0x1ff)/(float)(0x100)); /* set the speed */

    r=random();

    man_turn(m, r&1);				/* set the direction  */
    r>>=1;
    m->timetoturn=3;				/* turn soon */

    m->x= ((XMAX-XMIN)*(r&0x1ff)/0x1ff) + XMIN;	/* random X position */


    m->disp=1;
    m->seq=r%TEXNUM;				/* random texture sequence */

    population++;

    return id;
}

/* update camera matrix */
static void geom_proceed()
{
    sceVu0UnitMatrix(local_screen);
    sceVu0RotMatrix(local_screen, local_screen, obj_rot);
    sceVu0TransMatrix(local_screen, local_screen, obj_trans);

    sceVu0MulMatrix(local_screen, view_screen, local_screen);
}

/* "leading zsero count" to check polygon size at log scale; very fast function */
static inline u_long lzc(u_long src)
{
    register u_long dst;

    __asm__ __volatile__("\n\
	plzcw %0,%1\n\
        ": "=r" (dst): "r" (src));

    return dst;
}

/* rotation/transpers/volume clipping */
static void Vu0RotTransPersN_clip(sceVu0IVECTOR *v0, sceVu0FMATRIX m0, sceVu0FVECTOR *v1, int n, int mode, sceVu0FVECTOR *clipv)
{

    __asm__ __volatile__("\n\
	lqc2	vf4,0x0(%1)\n\
	lqc2	vf5,0x10(%1)\n\
	lqc2	vf6,0x20(%1)\n\
	lqc2	vf7,0x30(%1)\n\
	\n\
	lqc2	vf29,0x10(%5)\n\
	lqc2	vf31,0x0(%5)\n\
_rotTPN_loop:\n\
	lqc2	vf8,0x0(%2)\n\
	vmulax.xyzw     ACC, vf4,vf8\n\
	vmadday.xyzw    ACC, vf5,vf8\n\
	vmaddaz.xyzw    ACC, vf6,vf8\n\
	vmaddw.xyzw      vf9,vf7,vf8\n\
	vdiv    Q,vf0w,vf9w\n\
	vwaitq\n\
	vmulq.xyz	vf9,vf9,Q\n\
	ctc2    $0,$vi16\n\
	vsub.xyz        vf28,vf9,vf31  # check min\n\
	vsub.xyz        vf28,vf29,vf9  # check max\n\
	vftoi4.xyzw	vf10,vf9\n\
	beqz	%4,_rotTPN\n\
	vftoi0.zw	vf10,vf9	\n\
_rotTPN:\n\
	vnop\n\
	vnop\n\
	vnop\n\
	cfc2    $2,$vi16\n\
	\n\
	sqc2	vf10,0x0(%0)\n\
	and	$2, $2, 0xc0\n\
	sw	$2, 0xc(%0)\n\
	\n\
	addi	%3,-1\n\
	addi	%2,0x10\n\
	addi	%0,0x10\n\
	bne	$0,%3,_rotTPN_loop\n\
	":: "r" (v0) , "r" (m0) ,"r" (v1), "r" (n) ,"r" (mode), "r" (clipv) : "$2", "memory");
}


/* vertex caluculation for people */
static void man_geom(MAN *m, int side)
{
    sceVu0FVECTOR vx[2]; /* left-upper/right-bottom vertex at local-matrix coordinate */
    sceVu0IVECTOR sx[2]; /* at primitive coordinate */
    int i;
    int size;

    vx[0][0]=m->x+(MAN_WIDTH/2);
    vx[0][1]=0.0f-(0.47f*MAN_HEIGHT);
    vx[0][2]=m->z;
    vx[0][3]=1.0f;


    vx[1][0]=m->x-(MAN_WIDTH/2);
    vx[1][1]=0.0f;
    vx[1][2]=m->z;
    vx[1][3]=1.0f;

    Vu0RotTransPersN_clip(sx, local_screen, vx, 2, 1, clipv);

    /* texlvl: texture size, which depends display size */
    size=(sx[1][1] - sx[0][1])/16;
    m->texlvl= lzc(size) - 32 + 7;    /* use small texture for small polygon */
    if (m->texlvl<0) m->texlvl=0;
    if (m->texlvl>=TEXLVL) m->texlvl=TEXLVL-1;

    /* set values to packets */
    if ((sx[0][3]|sx[1][3])==0){

	for (i=0; i<2; i++){
	    int index;

	    index=sceGpIndexXyzfSpriteFTS(i);

	    if (m->direction) ((sceGpPrimP*)m->sprite[side])->reg[index].xyzf.X= sx[i][0];
	    else  ((sceGpPrimP*)m->sprite[side])->reg[index].xyzf.X= sx[1-i][0];
	    ((sceGpPrimP*)m->sprite[side])->reg[index].xyzf.Y= sx[i][1];
	    ((sceGpPrimP*)m->sprite[side])->reg[index].xyzf.Z= sx[0][2];
	    ((sceGpPrimP*)m->sprite[side])->reg[index].xyzf.F= 0;
	}
    } else{
	/* outside of volume clip */
	m->texlvl= -1;
    }
    
}

static inline void man_erase(MAN *m)
{
    m->disp=0;

    population--;
}

/* erase the first-found man; not so smart strategy, though */
static inline void man_disappear()
{
    int i;

    if (population>1){
	for (i=0; i<POPULATION_MAX; i++){
	    if (man[i].disp){
		man_erase(&man[i]);
		return;
	    }
	}
    }
    return;
}

/* update people, add packet to chain */
static void man_proceed(sceGpChain *chain, int move, int side)
{
    int i;
    MAN *m;
    int disappear=0;

    for (i=0; i<POPULATION_MAX; i++){
	if (man[i].disp){
	    m= &man[i];

	    if (move){
		m->z -= m->xspeed*zspeed;	/* advance along z */
		if (m->z < ZNEAR){		/* acrossed the nearest line */
		    man_erase(m);
		    disappear++;
		} else {
		    if (m->timetoturn--<0){	
			man_turn(m, random()&1);  /* change direction  */
		    } else{
						/* advance along x  */
			if (m->direction){
			    if (m->x>= XMAX) man_turn(m, 0);
			    else m->x+= m->xspeed;
			} else{
			    if (m->x<= XMIN) man_turn(m, 1);
			    else m->x-=m->xspeed;
			}
		    }
		    
		    if (m->timetoseq--<0) man_seq(m); /* to the next posture */
		    m->texid=seq[(int)m->seq];  /* convert: posture to texture id */
		}
	    }

	    /* vertex caluculation */
	    man_geom(m, side);

	    if (m->disp){ /* existing person */
		if (m->texlvl>=0){ /* not clipped person */
		    /* add packet */
		    sceGpAddPacket(chain, (m->texlvl*TEXNUM)+m->texid, m->sprite[side]);
		}
	    }
	}
    }

    /* fill vacancy */
    for (i=0; i<disappear; i++){
	man_create(0);
    }

}
