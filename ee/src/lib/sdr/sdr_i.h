/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * Emotion Engine Library
 * Copyright (C) 1998-2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

/* ----------------------------------------------
   moduke ID number
   �ŏ�ʃr�b�g��1��ID��SCE R&D��p�B
   �J�X�^�}�C�Y�����ꍇ�͕ύX���邱�ƁB
  -----------------------------------------------*/
#define sce_SDR_DEV   0x80000701
#define sce_SDRST_CB  0x80000704

#ifdef SCE_OBSOLETE
extern int (*_sce_sdr_gDMA0CB)(void);
extern int (*_sce_sdr_gDMA1CB)(void);
extern int (*_sce_sdr_gIRQCB)(void);
extern void *_sce_sdr_gDMA0Gp;
extern void *_sce_sdr_gDMA1Gp;
extern void *_sce_sdr_gIRQGp;
#endif

extern sceSdTransIntrHandler _sce_sdr_transIntr0Hdr;
extern sceSdTransIntrHandler _sce_sdr_transIntr1Hdr;
extern sceSdSpu2IntrHandler  _sce_sdr_spu2IntrHdr;

extern void *_sce_sdr_transIntr0Arg;
extern void *_sce_sdr_transIntr1Arg;
extern void *_sce_sdr_spu2IntrArg;

extern void *_sce_sdr_transIntr0Gp;
extern void *_sce_sdr_transIntr1Gp;
extern void *_sce_sdr_spu2IntrGp;

//--- �R�[���o�b�N�̃X�e�[�^�X�󂯓n���p
typedef struct {
    int		mode;	
    int		voice_bit;
    int		status;	
    int		opt;
    int     pad [12];  // �L���b�V�����C���T�C�Y(64Byte)�ɂȂ�悤��
} SdrEECBData;

#ifdef __DEBUG
#define PRINTF(x) printf x
#else
#define PRINTF(x) 
#endif
