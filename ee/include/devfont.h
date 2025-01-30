/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/* 
 *                      Emotion Engine Library
 *                          Version 0.01
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        libdev - devfont.h
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,29,1999     shibuya
 *       0.02           Jun,11,1999     shibuya
 */

#ifndef __devfont__
#define __devfont__

#include <eekernel.h>
#include <eetypes.h>
#include <eestruct.h>

#include <libdma.h>
#include <libgraph.h>
#include <libgifpk.h>

#ifdef __cplusplus
extern "C" {
#endif

// �R���\�[�����C�u������������
void sceDevConsInit();

// �R���\�[�����I�[�v���B�i�P�����Ȃ��B�T�C�Y�̍ő��80 x 64�L�����N�^�j
int sceDevConsOpen(u_int gs_x, u_int gs_y, u_int chr_w, u_int chr_h);

// �R���\�[�����N���[�Y
void sceDevConsClose(int cd);


// �R���\�[���̃C���[�W���p�P�b�g�ɂ���B
void sceDevConsRef(int cd, sceGifPacket* pPacket);

// �R���\�[���̃C���[�W��`��
void sceDevConsDraw(int cd);

// �R���\�[���̃C���[�W��`��i�X�N���b�`�p�b�h�g�p�j
void sceDevConsDrawS(int cd);

// �R���\�[���N���A
void sceDevConsClear(int cd);

// �R���\�[���֕����\��
u_int sceDevConsPrintf(int cd, const u_char* str, ...);


// �����C�u�����T�|�[�g
u_long128* sceDevFont(u_long128*, int, int, int, char *, int);
  /* packet, x, y, z(����), str, ctxt */


#ifdef __cplusplus
}
#endif

#endif /* __devfont__ */
