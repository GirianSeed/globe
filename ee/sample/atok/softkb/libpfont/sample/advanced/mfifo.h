/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#ifndef __mfifo__
#define __mfifo__

#include <malloc.h>

// MFIFO �ȈՃ��C�u����
// MFIFO���C�u�����Ŏg����^�O�� cnt, ref, end �̂݁B
// end �������pause��ԂɁB
// MFIFO �ł� ref �̎��ɕK�� cnt �� end ��u�����ƁB
// �����łȂ��ꍇ ref ���������]������Ȃ��ꍇ�����邩��
// cycle stearing ���[�h�̂ق����������ǂ�����


// bit : MFIFO�̑傫���w�� �S���傫���Ȃ���΂Ȃ�Ȃ�
void* _mfifo_alloc(int bit);
void _mfifo_free(void* ptr);

// MFIFO ������
// ch : �o��DMA�`�����l�� (SCE_DMA_VIF1 | SCE_DMA_GIF)
// buffer : MFIFO�o�b�t�@(�A���C�������g�ɒ��ӁI)
// size : �o�b�t�@�̑傫��(2^n)
// return : 0=success 0>error
int _mfifo_init(int ch, void* buffer, size_t size);

// MFIFO �I��
void _mfifo_close(void);

// MFIFO �X�^�[�g
// tte : �^�O�]������ vif1 �Ȃ�ʏ�P
// tie : 0=IRQ bit ���� 1=IRQ bit �L��
void _mfifo_start(int tte, int tie);

// MFIFO ��~
void _mfifo_stop(void);

// �ꎞ��~
void _mfifo_pause(void);

// �ꎞ��~��Ԃ���̍ĊJ
void _mfifo_continue(void);

// MFIFO �]���I������(�󂫂��ł���܂Ń|�[�����O)
// ���j�|�[�Y��Ԃł̓u���b�N���Ȃ��B
// async : 0=�]���I����҂� 1=�������^�[��
// return : 0=�f�[�^���� 1=�]����
int _mfifo_sync(int async);

// end tag �ɂ��]���I�������o(Semaphore)
int _mfifo_sync2(int async);

// MFIFO �̋󂫗e�ʂ�Ԃ��B(�ő�T�C�Y=((1 << (bit - 4)) - 1))
// return : �o�͂ł���p�P�b�g�̐� (qwc)
u_int _mfifo_getspace(void);

// MFIFO �փL�b�N
// spr_packet : �X�N���b�`�p�b�h��ɂ���p�P�b�g�̃A�h���X
// qwc : �o�̓p�P�b�g�T�C�Y
// mode : 0=mfifo�ɋ󂫂��ł��ďo�͂ɐ�������܂Ńu���b�N�B1:�ʖڂȂ炷���ɔ�����B
// return : 0=���� 1=�󂫂��Ȃ�
int _mfifo_kick(u_long128 const* spr_packet, u_int qwc, int mode);

// ���荞�݂ƃZ�}�t�H�g����MFIFO�󂫂̓����҂�������B
// �f�o�b�O�ł����g�����Ⴂ���Ȃ��炵�� T-T
// return : 0=�󂢂� 1=�f�[�^���邯�Ǔ����Ė��� 0>�G���[
int _mfifo_sync_dbg(void);



extern __inline__ void* _mfifo_alloc(int bit){
	size_t size = 1 << bit;
	return memalign(size, size);
}

extern __inline__ void _mfifo_free(void* ptr){
	free(ptr);
}

#endif /* __mfifo__ */
