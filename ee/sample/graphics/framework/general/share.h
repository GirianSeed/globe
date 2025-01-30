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
 *                          Name : share.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *             2.1     Nov,17,2000
 */

#ifndef _SHARE_H_
#define _SHARE_H_

extern fwShareFrame_t* fwShareInit(fwDataHead_t *);
extern void fwSharePreCalc(fwShareFrame_t *);
extern void fwSharePostCalc(fwShareFrame_t *);
extern void fwShareFinish(fwShareFrame_t *);

#endif /* !_SHARE_H_ */
