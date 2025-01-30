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

#include <eekernel.h>
#include <libpad.h>
#include "pad.h"

/* dma buffer for pad */
static u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));
static sceSamplePad_t pad_state;

/*
 * Padの初期化
 */
void pad_init(void)
{
    scePadInit(0);
    scePadPortOpen(0, 0, pad_dma_buf);
}

/*
 * Pad情報の更新＆取得
 * このままではthread safeではないので注意
 */
sceSamplePad_t *pad_update(void)
{
    static u_short old = 0;
    u_short pad;
    u_char rdata[32];

    if (scePadRead(0, 0, rdata) > 0)
        pad = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
    else
        pad = 0;

    // pad_state.〜Onは pad & SCE_PAD〜のコピーに等しい
    // (押されている間1になり続ける)
    pad_state.LleftOn   = pad & SCE_PADLleft ? 1 : 0;
    pad_state.LupOn     = pad & SCE_PADLup ? 1 : 0;
    pad_state.LrightOn  = pad & SCE_PADLright ? 1 : 0;
    pad_state.LdownOn   = pad & SCE_PADLdown ? 1 : 0;
    pad_state.L1On      = pad & SCE_PADL1 ? 1 : 0;
    pad_state.L2On      = pad & SCE_PADL2 ? 1 : 0;
    pad_state.RleftOn   = pad & SCE_PADRleft ? 1 : 0;
    pad_state.RupOn     = pad & SCE_PADRup ? 1 : 0;
    pad_state.RrightOn  = pad & SCE_PADRright ? 1 : 0;
    pad_state.RdownOn   = pad & SCE_PADRdown ? 1 : 0;
    pad_state.R1On      = pad & SCE_PADR1 ? 1 : 0;
    pad_state.R2On      = pad & SCE_PADR2 ? 1 : 0;
    pad_state.SelectOn  = pad & SCE_PADselect ? 1 : 0;
    pad_state.StartOn   = pad & SCE_PADstart ? 1 : 0;

    // pad_state.〜Switchは、「そのボタンが押された時」に1になる
    pad_state.LleftSw   = pad_state.LleftOn && (!(old & SCE_PADLleft)) ? 1 : 0;
    pad_state.LupSw     = pad_state.LupOn && (!(old & SCE_PADLup)) ? 1 : 0;
    pad_state.LrightSw  = pad_state.LrightOn && (!(old & SCE_PADLright)) ? 1 : 0;
    pad_state.LdownSw   = pad_state.LdownOn && (!(old & SCE_PADLdown)) ? 1 : 0;
    pad_state.L1Sw      = pad_state.L1On && (!(old & SCE_PADL1)) ? 1 : 0;
    pad_state.L2Sw      = pad_state.L2On && (!(old & SCE_PADL2)) ? 1 : 0;
    pad_state.RleftSw   = pad_state.RleftOn && (!(old & SCE_PADRleft)) ? 1 : 0;
    pad_state.RupSw     = pad_state.RupOn && (!(old & SCE_PADRup)) ? 1 : 0;
    pad_state.RrightSw  = pad_state.RrightOn && (!(old & SCE_PADRright)) ? 1 : 0;
    pad_state.RdownSw   = pad_state.RdownOn && (!(old & SCE_PADRdown)) ? 1 : 0;
    pad_state.R1Sw      = pad_state.R1On && (!(old & SCE_PADR1)) ? 1 : 0;
    pad_state.R2Sw      = pad_state.R2On && (!(old & SCE_PADR2)) ? 1 : 0;
    pad_state.SelectSw  = pad_state.SelectOn && (!(old & SCE_PADselect)) ? 1 : 0;
    pad_state.StartSw   = pad_state.StartOn && (!(old & SCE_PADstart)) ? 1 : 0;

    old = pad;
    return &pad_state;
}

/* updateせずに状態だけ得る */
sceSamplePad_t *pad_get(void)
{
    return &pad_state;
}

void pad_debug(sceSamplePad_t *pad)
{
#define PRINT(x)        { scePrintf("%s ", x); count++; }
    int count = 0;

    if (pad->LleftOn)   PRINT("LLeft On");
    if (pad->LupOn)     PRINT("Lup On");
    if (pad->LrightOn)  PRINT("Lright On");
    if (pad->LdownOn)   PRINT("Ldown On");
    if (pad->L1On)      PRINT("L1 On");
    if (pad->L2On)      PRINT("L2 On");
    if (pad->RleftOn)   PRINT("Rleft On");
    if (pad->RupOn)     PRINT("Rup On");
    if (pad->RrightOn)  PRINT("Rright On");
    if (pad->RdownOn)   PRINT("Rdown On");
    if (pad->R1On)      PRINT("R1 On");
    if (pad->R2On)      PRINT("R2 On");
    if (pad->SelectOn)  PRINT("Select On");
    if (pad->StartOn)   PRINT("Start On");

    if (pad->LleftSw)   PRINT("Lleft Sw");
    if (pad->LupSw)     PRINT("Lup Sw");
    if (pad->LrightSw)  PRINT("Lright Sw");
    if (pad->LdownSw)   PRINT("Ldown Sw");
    if (pad->L1Sw)      PRINT("L1 Sw");
    if (pad->L2Sw)      PRINT("L2 Sw");
    if (pad->RleftSw)   PRINT("Rleft Sw");
    if (pad->RupSw)     PRINT("Rup Sw");
    if (pad->RrightSw)  PRINT("Rright Sw");
    if (pad->RdownSw)   PRINT("Rdown Sw");
    if (pad->R1Sw)      PRINT("R1 Sw");
    if (pad->R2Sw)      PRINT("R2 Sw");
    if (pad->SelectSw)  PRINT("Select Sw");
    if (pad->StartSw)   PRINT("Start Sw");

    if (count > 0)      scePrintf("\n");
}
