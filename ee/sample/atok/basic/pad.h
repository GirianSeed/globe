/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */

#ifndef __PAD_H__
#define __PAD_H__

typedef struct {
    /* On/Off : 押しているか、いないか */
    unsigned    LleftOn         : 1;
    unsigned    LupOn           : 1;
    unsigned    LrightOn        : 1;
    unsigned    LdownOn         : 1;
    unsigned    L1On            : 1;
    unsigned    L2On            : 1;
    
    unsigned    RleftOn         : 1;
    unsigned    RupOn           : 1;
    unsigned    RrightOn        : 1;
    unsigned    RdownOn         : 1;
    unsigned    R1On            : 1;
    unsigned    R2On            : 1;

    unsigned    SelectOn        : 1;
    unsigned    StartOn         : 1;

    /* Switch : 押された瞬間 1だけ1になる */
    unsigned    LleftSw         : 1;
    unsigned    LupSw           : 1;
    unsigned    LrightSw        : 1;
    unsigned    LdownSw         : 1;
    unsigned    L1Sw            : 1;
    unsigned    L2Sw            : 1;

    unsigned    RleftSw         : 1;
    unsigned    RupSw           : 1;
    unsigned    RrightSw        : 1;
    unsigned    RdownSw         : 1;
    unsigned    R1Sw            : 1;
    unsigned    R2Sw            : 1;

    unsigned    SelectSw        : 1;
    unsigned    StartSw         : 1;
} sceSamplePad_t;

extern void pad_init(void);
extern sceSamplePad_t *pad_update(void);
extern sceSamplePad_t *pad_get(void);
extern void pad_debug(sceSamplePad_t *);

#endif /* __PAD_H__ */
