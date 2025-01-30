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
 *                          Name : hrchy.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.40      Jun,21,2000    kaneko
 *            2.1       Nov,17,2000
 */
#ifndef _HIER_H_
#define _HIER_H_

/***************************************************
 * external function declarations
 ***************************************************/
extern fwHrchyFrame_t* fwHrchyInit(fwDataHead_t *);
extern void fwHrchyPreCalc(fwHrchyFrame_t *);
extern void fwHrchyPostCalc(fwHrchyFrame_t *);
extern void fwHrchyFinish(fwHrchyFrame_t *);

#endif /* !_HIER_H_ */
