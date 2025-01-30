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
 *                          Name : mode.h
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000   
 *
 */

#ifndef _MODE_H_
#define _MODE_H_

#define DEFMODE(name)	name##MODE,
typedef enum {
#include "mode_tbl.h"
MODE_MAX
} fwModeType_t;
#undef DEFMODE

extern void fwModeInit(fwModeType_t);
extern void fwModeExec(void);
extern void fwModeExit(void);
extern void fwModeChange(fwModeType_t);

#endif /* !_MODE_H_ */
