/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 */
/*	$Id: micro.c,v 1.19.6.1 2002/02/22 15:09:30 aoki Exp $	*/
#include <eekernel.h>
#include <stdlib.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libvu0.h>
#include <libhig.h>
#include <libhip.h>

#define PADDR_MASK 0x0fffffff
#define Paddr(x) ((u_int)(x)&PADDR_MASK)

/***************************************************
 * local errs
 ***************************************************/
typedef enum {
    _NO_HEAP,
    _CODE_PACKET_MAKING_ERR,
    _DATA_PACKET_MAKING_ERR,
    _PLGBLK_STACK_BROKEN,
    _ILLEAGAL_MICRO_ID,
} _local_err;

static const sceHiErr _local2higerr[] = {
    SCE_HIG_NO_HEAP,
    SCE_HIG_FAILURE,
    SCE_HIG_FAILURE,
    SCE_HIG_INVALID_DATA,
    SCE_HIG_INVALID_VALUE
};

static const char *_micro_err_mes[] = {
    "HiP Micro : can't allocate memory from HiG Heap\n",
    "HiP Micro : fail to make micro code packet.\n",
    "HiP Micro : fail to make micro data packet.\n",
    "HiP Micro : the plugin's stack is broken(maybe didnt do INIT_PROCESS)\n",
    "HiP Micro : illeagal micro id\n",
};

/* error handling */
static sceHiErr _hip_micro_err(_local_err err)
{
    sceHiErrState.mes = _micro_err_mes[err];
    return _local2higerr[err];
}

/*******************************************************
 *	Micro Code Address
 *******************************************************/
extern u_int sce_micro_base[] __attribute__((section(".vudata")));

sceHiPlugMicroTbl_t sce_default_micro_tbl[] = {
    { sce_micro_base, SCE_HIP_MICRO_ATTR_NONE }
};

typedef union {
    u_long	ul[2];
    u_int	ui[4];
} gtag_t; /* _giftags[2] __attribute__((section(".vudata"))); */

/*******************************************************
 *	Structure		       
 *******************************************************/

/* Data Structure for micro data */
typedef struct {
    sceVu0FMATRIX	vector;
    sceVu0FMATRIX	point;
    sceVu0FMATRIX	color;
} MicroLight_t;

typedef struct {
    sceVu0FMATRIX	wscr;
    sceVu0FMATRIX	wclp;
    sceVu0FMATRIX	cscr;
    sceVu0FMATRIX	init_data;
    MicroLight_t	lightgrp[3];
} MicroData_t;

typedef struct{
    gtag_t		giftags[2];
    int			mic_id;			/* micro code id */
    int			*code_id;	/* micro code packet id */
    u_int		*attr;	/* attribute data */
    int			data_id;		/* micro data packet id */

    sceHiPlugMicroTbl_t	*tbl;
    u_int		tblnum;
}MICRO_FRAME __attribute__((aligned(64)));

/*******************************************************
 *	Micro Packet		       
 *******************************************************/

static u_int MicroMakeCODEPacket(MICRO_FRAME *mic, u_int *codeadr, int worksize)
{
	int	id;

	if (sceHiDMAMake_ChainStart())			return 0xffffffff;
	if (sceHiDMAMake_CallPtr(codeadr))		return 0xffffffff;
	if (sceHiDMAMake_LoadPtr((u_int *)8, (u_int *)mic->giftags, 2))	return 0xffffffff;
	if (sceHiDMAInit_DBuf(120, 1024-worksize))	return 0xffffffff;
	if (sceHiDMAMake_ChainEnd(&id))			return 0xffffffff;
	return id;
}

static u_int MicroMakeDATAPacket(u_int *micdata)
{
    int	id;

    if (sceHiDMAMake_ChainStart())			return 0xffffffff;
    if (sceHiDMAMake_LoadPtr((u_int *)16, micdata, 52))	return 0xffffffff;
    /* �\�ߍ쐬���Ă���giftag�f�[�^�̓]�� */
    if (sceHiDMAMake_ChainEnd(&id))			return 0xffffffff;
    return id;
}

/*******************************************************
 *	set giftag params (== fwMicroChange)
 *******************************************************/
static void micro_set_data(sceHiPlug *plug, MICRO_FRAME	*mf, u_int *daddr)
{
    union {
	sceVu0FMATRIX	mat;
	u_int		ui[4][4];
    } data;
    sceHiPlugMicroPreCalcArg_t	*arg;
    u_int			nreg, tag2, tag3, flg, prim;
    sceHiGsGeneral	*gen;

    gen = sceHiGsGeneralStatus();

    /*
      data�̒��g
      { cameraX, cameraY, cameraZ, anti-cutoff },
      { fogA,    fogB,    PRIM     ----------  },
      { fSn/sw, fSn/sh, fPn/2.0f, fSn/2.0f },
      { ------- --------  -------  ----------  }
      
      fogA = 1 / (fog_begin - fog_end)
      fogB = fog_end
    */

    arg = (sceHiPlugMicroPreCalcArg_t *)plug->args;
    if (arg != NULL) {
	data.mat[0][3] = arg->anticutoff;
	data.mat[1][0] = 1 / (arg->fogbegin - arg->fogend);
	data.mat[1][1] = arg->fogend;
    }

    /* make gif tag */
    /* �����ł� giftag��
       PRE	: PRIM field�𖳎������邽�� 0��
       FLG	: PACKED���[�h��
       NREG	: ���ꂼ��̃}�C�N���R�[�h�ˑ���
       REGS	: ���ꂼ��̃}�C�N���R�[�h�ˑ���
       �̕�����ݒ肵�AInit Data�����ɒu��
       �܂��AInit Data��PRIM����([2][1])�� Primitive atrtibute��
       �ݒ肷��悤�ɂ���i�����̒l�͒���GS��PRMODE���W�X�^�ɏ������j
       �����Őݒ肵��attribute�͂��̂܂܂ł͓��삵�Ȃ��\��������B
       Shape Plug�Őݒ肵�����̂�OR�������̂����ۂ�GS Kick�Ŏg����
       Primitive�����ɂȂ鎖�ɒ��Ӂisee me. shape.c�j

       01.01.16
       gs service functions�̐ݒ��L���ɂ���悤�ɕύX
    */

    /* default value */
    nreg = 3;
    tag2 = (SCE_GS_ST << (4 * 0)) | (SCE_GS_RGBAQ << (4 * 1)) | (SCE_GS_XYZF2 << (4 * 2));
    tag3 = 0;
    flg = SCE_GIF_PACKED;
    prim = 0;

    if (mf->mic_id != -1) {
	/* GS�ݒ�� Fog on���� Micro Code Table�� on�Ȃ� FOG */
	if (gen->prmode.FGE && (mf->attr[mf->mic_id] & SCE_HIP_MICRO_ATTR_FGE)) {
	    prim |= GS_PRIM_FGE_M;
	}
	/* GS�ݒ�ɂ�Anti���Ȃ� */
	if (mf->attr[mf->mic_id] & SCE_HIP_MICRO_ATTR_ANTI) {
	    nreg = 4;
	    tag2 = (SCE_GIF_PACKED_AD << (4 * 0)) | (SCE_GS_ST << (4 * 1)) | (SCE_GS_RGBAQ << (4 * 2)) | (SCE_GS_XYZF2 << (4 * 3));
	}
	/* make tag */
	mf->giftags[1].ui[3] = tag3;
	mf->giftags[1].ui[2] = tag2;
	mf->giftags[1].ui[1] = (nreg << GIF_TAG1_NREG_O) | (flg << GIF_TAG1_FLG_O);
	mf->giftags[1].ui[0] = 0;
    }
				/* GIF_TAG1 */

    /* GS�ݒ�� Alpha on�Ȃ� on */
    if (gen->prmode.ABE) {
	prim |= GS_PRIM_ABE_M;
    }
#if 0
    data.ui[2][0] = 0;		/* GIF_TAG0 */
#endif
    data.ui[1][2] = prim;

    /* set initial data */
#if 0
    sceVu0CopyMatrix(((MicroData_t *)daddr)->init_data, data.mat);
#else
    /* dont over-write other data */
    ((MicroData_t *)daddr)->init_data[0][3] = data.mat[0][3];
    ((MicroData_t *)daddr)->init_data[1][0] = data.mat[1][0];
    ((MicroData_t *)daddr)->init_data[1][1] = data.mat[1][1];
    ((MicroData_t *)daddr)->init_data[1][2] = data.mat[1][2];
#endif
}

/*******************************************************
 *	Micro Plug		       
 *******************************************************/
sceHiErr sceHiPlugMicro(sceHiPlug *plug, int process)
{
    sceHiType	type;
    u_int	*data;
    sceHiErr	err;
    MICRO_FRAME	*micro_frame;
    int		i;

    switch(process){
      case SCE_HIG_INIT_PROCESS:
	type.repository = SCE_HIP_COMMON;
	type.project = SCE_HIP_FRAMEWORK;
	type.category = SCE_HIP_MICRO;
	type.status = SCE_HIG_DATA_STATUS;
	type.id = SCE_HIP_MICRO_DATA;
	type.revision = SCE_HIP_REVISION;

	err = sceHiGetData(plug, &data, type);	if(err != SCE_HIG_NO_ERR) return err;

	micro_frame = (MICRO_FRAME *)sceHiMemAlign(16, sizeof(MICRO_FRAME) *1);
	if(micro_frame == NULL) return _hip_micro_err(_NO_HEAP);

/*	plug->args = (u_int) &IARG;*/
/*	printf("%d\n", plug->args);*/
	
	/* Micro Code Packet Making */
	if (plug->args == (int)NULL) {
	    /* default setting */
	    micro_frame->tblnum = 1;
	    micro_frame->tbl = sce_default_micro_tbl;
	} else {
	    micro_frame->tblnum = ((sceHiPlugMicroInitArg_t *)plug->args)->tblnum;
	    micro_frame->tbl    = ((sceHiPlugMicroInitArg_t *)plug->args)->tbl;
	}

	micro_frame->code_id = (int *) sceHiMemAlloc(sizeof(int) * micro_frame->tblnum);
	micro_frame->attr = (u_int *) sceHiMemAlloc(sizeof(u_int) * micro_frame->tblnum);
	for (i = 0; i < micro_frame->tblnum; i++) {
	    micro_frame->attr[i] = micro_frame->tbl[i].attr;
	}
	for (i = 0; i < micro_frame->tblnum; i++) {
	    micro_frame->code_id[i] = MicroMakeCODEPacket(micro_frame, micro_frame->tbl[i].micro, 0);
	    if (micro_frame->code_id[i] == 0xffffffff)
		return _hip_micro_err(_CODE_PACKET_MAKING_ERR);
	}

	micro_frame->data_id = MicroMakeDATAPacket(data);
	if(micro_frame->data_id == 0xffffffff)
	    return _hip_micro_err(_DATA_PACKET_MAKING_ERR);

	/* giftag making (common) */
	/*				nloop, eop, pre, prim, flg, nreg */
	micro_frame->giftags[0].ul[0] = SCE_GIF_SET_TAG(1, 0, 0, 0, 0, 2);
	micro_frame->giftags[0].ul[1] = (SCE_GIF_PACKED_AD << (4 * 0)) | (SCE_GIF_PACKED_AD << (4 * 1));

	plug->args = 0;
	plug->stack = (u_int)micro_frame;
	break;

      case SCE_HIG_PRE_PROCESS:
	micro_frame = (MICRO_FRAME *)plug->stack;
	if (micro_frame == NULL) return _hip_micro_err(_PLGBLK_STACK_BROKEN);

	type.repository = SCE_HIP_COMMON;
	type.project = SCE_HIP_FRAMEWORK;
	type.category = SCE_HIP_MICRO;
	type.id = SCE_HIP_MICRO_DATA;

	err = sceHiGetData(plug, &data, type);
	if(err != SCE_HIG_NO_ERR) return err;

	if (plug->args == NULL)
	    micro_frame->mic_id = 0;
	else {
	    micro_frame->mic_id = ((sceHiPlugMicroPreCalcArg_t *)plug->args)->micro;
	    if (micro_frame->mic_id >= (int)micro_frame->tblnum)
		micro_frame->mic_id = 0;
	}

	/* set Initial Data */
	micro_set_data(plug, micro_frame, data);

	break;

      case SCE_HIG_POST_PROCESS:
	micro_frame = (MICRO_FRAME *)plug->stack;
	if(micro_frame == NULL) return _hip_micro_err(_PLGBLK_STACK_BROKEN);
	if (micro_frame->mic_id != -1) {
	    /* id check */
	    if (micro_frame->mic_id < 0)
		return _hip_micro_err(_ILLEAGAL_MICRO_ID);
	    if (micro_frame->mic_id >= (int)micro_frame->tblnum)
		micro_frame->mic_id = 0;
		/* Micro Code�]���v�� */
	    sceHiDMARegist(micro_frame->code_id[micro_frame->mic_id]);
	}
	/* Micro Data�]�� */
	sceHiDMARegist(micro_frame->data_id);
	break;

      case SCE_HIG_END_PROCESS:
	micro_frame = (MICRO_FRAME *)plug->stack;
	if(micro_frame == NULL) return _hip_micro_err(_PLGBLK_STACK_BROKEN);

	for (i = 0; i < micro_frame->tblnum; i++)
	    sceHiDMADel_Chain(micro_frame->code_id[i]);
	sceHiDMADel_Chain(micro_frame->data_id);
	sceHiMemFree((u_int *)Paddr(micro_frame->code_id));
	sceHiMemFree((u_int *)Paddr(micro_frame->attr));
	sceHiMemFree((u_int *)Paddr(micro_frame));

	plug->stack = NULL;
	plug->args = NULL;
	break;
      default:
	break;
    }

    return SCE_HIG_NO_ERR;
}
