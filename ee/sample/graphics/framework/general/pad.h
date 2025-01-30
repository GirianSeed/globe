/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 *
 *
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : pad.h
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000      
 *
 */

#ifndef _PAD_H_
#define _PAD_H_

/*******************************************************
 * PAD Type Defines
 *******************************************************/
typedef struct {
    /* Switch : the moment that pushed, 1 */
    unsigned	L2Switch	: 1;
    unsigned	R2Switch	: 1;
    unsigned	L1Switch	: 1;
    unsigned	R1Switch	: 1;
    unsigned	RupSwitch	: 1;
    unsigned	RrightSwitch    : 1;
    unsigned	RdownSwitch	: 1;
    unsigned	RleftSwitch	: 1;
    unsigned	SelectSwitch	: 1;
    unsigned	reserve3	: 1;
    unsigned	reserve4	: 1;
    unsigned	StartSwitch	: 1;
    unsigned	LupSwitch	: 1;
    unsigned	LrightSwitch	: 1;
    unsigned	LdownSwitch	: 1;
    unsigned	LleftSwitch	: 1;

    /* On : while pushed, 1 */
    unsigned	L2On		: 1;
    unsigned	R2On		: 1;
    unsigned	L1On		: 1;
    unsigned	R1On		: 1;
    unsigned	RupOn		: 1;
    unsigned	RrightOn     	: 1;
    unsigned	RdownOn		: 1;
    unsigned	RleftOn		: 1;
    unsigned	SelectOn	: 1;
    unsigned	reserve1	: 1;
    unsigned	reserve2	: 1;
    unsigned	StartOn		: 1;
    unsigned	LupOn		: 1;
    unsigned	LrightOn	: 1;
    unsigned	LdownOn		: 1;
    unsigned	LleftOn		: 1;
} fwPadState_t;

/*******************************************************
 * Pad Variable Defines
 *******************************************************/
extern fwPadState_t fwPadStateVal;

/*******************************************************
 * Pad Function/Macro Defines
 *******************************************************/
extern void fwPadInit(void);
extern void fwPadCheck(void);

#endif /* !_PAD_H_ */
