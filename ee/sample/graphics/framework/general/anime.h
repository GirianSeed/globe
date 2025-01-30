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
 *                          Name : anime.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            2.1       Nov,17,2000  
*/

#ifndef _ANIM_H_
#define _ANIM_H_

/*
 * external function declaration
 */
extern fwAnimeFrame_t* fwAnimeInit(fwDataHead_t *);
extern void fwAnimePreCalc(fwAnimeFrame_t *);
extern void fwAnimePostCalc(fwAnimeFrame_t *);
extern void fwAnimeFinish(fwAnimeFrame_t *);

#endif /* !_ANIM_H_ */
