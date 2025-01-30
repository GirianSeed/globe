/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
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

#ifndef __TEXTFIELD_H__
#define __TEXTFIELD_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include "shareddef.h"

#if CDBOOT
#define TF_TEXTURE_PATH		"cdrom0:\\RESOURCE\\TEX\\SKB"
#else
#define TF_TEXTURE_PATH		LOCALPATH"/tex/skb"
#endif

void TextF_Init(gImport_t *pImport);
void TextF_Destroy(void);
void TextF_Create(textField_t *pTf, iRect_t *rc);
void TextF_Delete(textField_t *pTf);
void TextF_Run(sceVif1Packet *pk, textField_t *pTf);
void TextF_Move(textField_t *pTf, iRect_t *rc);
void TextF_ChangeAlpha(textField_t *pTf, int index, u_char alpha);
void TextF_SetDispMode( textField_t *pTf, int mode );



#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__TEXTFIELD_H__
